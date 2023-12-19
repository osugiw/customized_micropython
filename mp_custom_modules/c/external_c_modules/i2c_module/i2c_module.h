// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

// ESP-IDF APIs
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

// Macros
#define DATA_LENGTH 512                                         /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 128                                      /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000                        /*!< delay time between different test items */

#define I2C_MASTER_TX_BUF_DISABLE 0                             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                             /*!< I2C master doesn't need buffer */
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH)                  /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)                  /*!< I2C slave rx buffer size */

#define WRITE_BIT I2C_MASTER_WRITE                              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                                             /*!< I2C ack value */
#define NACK_VAL 0x1                                            /*!< I2C nack value */

// Function Declaration
/************************************I2C MASTER****************************************/
extern mp_obj_t master_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _freq_obj);
extern mp_obj_t master_write(mp_obj_t _slave_addr_obj, mp_obj_t _data_obj);

/************************************I2C SLAVE****************************************/
extern mp_obj_t slave_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _slaveAddr_obj);
extern mp_obj_t slave_read();
