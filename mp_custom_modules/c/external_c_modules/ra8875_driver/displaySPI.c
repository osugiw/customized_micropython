/*********************
 *      INCLUDES
 *********************/
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "displaySPI.h"

/*********************
 *      DEFINES
 *********************/

#define TAG "disp_spi"
#define SPI_TRANSACTION_POOL_SIZE 50	/* maximum number of DMA transactions simultaneously in-flight */

/* DMA Transactions to reserve before queueing additional DMA transactions. A 1/10th seems to be a good balance. Too many (or all) and it will increase latency. */
#define SPI_TRANSACTION_POOL_RESERVE_PERCENTAGE 10
#if SPI_TRANSACTION_POOL_SIZE >= SPI_TRANSACTION_POOL_RESERVE_PERCENTAGE
#define SPI_TRANSACTION_POOL_RESERVE (SPI_TRANSACTION_POOL_SIZE / SPI_TRANSACTION_POOL_RESERVE_PERCENTAGE)	
#else
#define SPI_TRANSACTION_POOL_RESERVE 1	/* defines minimum size */
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static spi_host_device_t    spi_host;
static spi_bus_config_t     *_busConfig;
static spi_device_interface_config_t *_displayConfig;
static spi_device_handle_t  spi;
static QueueHandle_t        TransactionPool = NULL;
static transaction_cb_t     chained_post_cb;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void disp_spi_add_device_config(spi_host_device_t host, spi_device_interface_config_t *devcfg, spi_bus_config_t *buscfg)
{
    esp_err_t ret;

    // Assign to global variables
    spi_host        = host;
    chained_post_cb = devcfg->post_cb;
    _busConfig      = buscfg;
    _displayConfig  = devcfg;  

    // Initialize bus
    ret = spi_bus_initialize(host, buscfg, SPI_DMA_CH_AUTO);       
    ESP_ERROR_CHECK(ret);
    // Attach Display to the bus
    ret=spi_bus_add_device(host, devcfg, &spi);
    ESP_ERROR_CHECK(ret);
}

void disp_spi_change_device_speed(int clock_speed_hz)
{
    esp_err_t ret;
    ESP_LOGI(TAG, "Changing SPI device clock speed: %d", clock_speed_hz);
    disp_spi_remove_device();
    
    // Change the SPI Bus Clock
    _displayConfig->clock_speed_hz = clock_speed_hz;
    ret=spi_bus_add_device(spi_host, _displayconfig, &spi);
    ESP_ERROR_CHECK(ret);

	/* create the transaction pool and fill it with ptrs to spi_transaction_ext_t to reuse */
	if(TransactionPool == NULL) {
		TransactionPool = xQueueCreate(SPI_TRANSACTION_POOL_SIZE, sizeof(spi_transaction_ext_t*));
		assert(TransactionPool != NULL);
		for (size_t i = 0; i < SPI_TRANSACTION_POOL_SIZE; i++)
		{
			spi_transaction_ext_t* pTransaction = (spi_transaction_ext_t*)heap_caps_malloc(sizeof(spi_transaction_ext_t), MALLOC_CAP_DMA);
			assert(pTransaction != NULL);
			memset(pTransaction, 0, sizeof(spi_transaction_ext_t));
			xQueueSend(TransactionPool, &pTransaction, portMAX_DELAY);
		}
	}
}

void disp_spi_transaction(const uint8_t *data, size_t length,
    disp_spi_send_flag_t flags, uint8_t *out,
    uint64_t addr, uint8_t dummy_bits)
{
    if (0 == length) {
        return;
    }

    spi_transaction_ext_t t = {0};

    /* transaction length is in bits */
    t.base.length = length * 8;

    if (length <= 4 && data != NULL) {
        t.base.flags = SPI_TRANS_USE_TXDATA;
        memcpy(t.base.tx_data, data, length);
    } else {
        t.base.tx_buffer = data;
    }

    if (flags & DISP_SPI_RECEIVE) {
        assert(out != NULL && (flags & (DISP_SPI_SEND_POLLING | DISP_SPI_SEND_SYNCHRONOUS)));
        t.base.rx_buffer = out;
    }

    if (flags & DISP_SPI_ADDRESS_8) {
        t.address_bits = 8;
    } else if (flags & DISP_SPI_ADDRESS_16) {
        t.address_bits = 16;
    } else if (flags & DISP_SPI_ADDRESS_24) {
        t.address_bits = 24;
    } else if (flags & DISP_SPI_ADDRESS_32) {
        t.address_bits = 32;
    }
    if (t.address_bits) {
        t.base.addr = addr;
        t.base.flags |= SPI_TRANS_VARIABLE_ADDR;
    }

    /* Save flags for pre/post transaction processing */
    t.base.user = (void *) flags;

    /* Poll/Complete/Queue transaction */
    if (flags & DISP_SPI_SEND_POLLING) {
		disp_wait_for_pending_transactions();	/* before polling, all previous pending transactions need to be serviced */
        spi_device_polling_transmit(spi, (spi_transaction_t *) &t);
    } else if (flags & DISP_SPI_SEND_SYNCHRONOUS) {
		disp_wait_for_pending_transactions();	/* before synchronous queueing, all previous pending transactions need to be serviced */
        spi_device_transmit(spi, (spi_transaction_t *) &t);
    } else {
		
		/* if necessary, ensure we can queue new transactions by servicing some previous transactions */
		if(uxQueueMessagesWaiting(TransactionPool) == 0) {
			spi_transaction_t *presult;
			while(uxQueueMessagesWaiting(TransactionPool) < SPI_TRANSACTION_POOL_RESERVE) {
				if (spi_device_get_trans_result(spi, &presult, 1) == ESP_OK) {
					xQueueSend(TransactionPool, &presult, portMAX_DELAY);	/* back to the pool to be reused */
				}
			}
		}

		spi_transaction_ext_t *pTransaction = NULL;
		xQueueReceive(TransactionPool, &pTransaction, portMAX_DELAY);
        memcpy(pTransaction, &t, sizeof(t));
        if (spi_device_queue_trans(spi, (spi_transaction_t *) pTransaction, portMAX_DELAY) != ESP_OK) {
			xQueueSend(TransactionPool, &pTransaction, portMAX_DELAY);	/* send failed transaction back to the pool to be reused */
        }
    }
}

void disp_spi_remove_device()
{
    /* Wait for previous pending transaction results */
    disp_wait_for_pending_transactions();

    esp_err_t ret=spi_bus_remove_device(spi);
    assert(ret==ESP_OK);
}

void disp_wait_for_pending_transactions(void)
{
    spi_transaction_t *presult;

	while(uxQueueMessagesWaiting(TransactionPool) < SPI_TRANSACTION_POOL_SIZE) {	/* service until the transaction reuse pool is full again */
        if (spi_device_get_trans_result(spi, &presult, 1) == ESP_OK) {
			xQueueSend(TransactionPool, &presult, portMAX_DELAY);
        }
    }
}

void disp_spi_acquire(void)
{
    esp_err_t ret = spi_device_acquire_bus(spi, portMAX_DELAY);
    assert(ret == ESP_OK);
}

void disp_spi_release(void)
{
    spi_device_release_bus(spi);
}