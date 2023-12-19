/*********************
 *      INCLUDES
 *********************/
#include "py/runtime.h"
#include "py/obj.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/**
*   @brief Inialize SPI Peripehral
*   @param args[0]         CS Pin
*   @param args[1]         MISO Pin
*   @param args[2]         MOSI Pin
*   @param args[3]         SCLK Pin
**/
STATIC mp_obj_t init_spi(size_t n_args, const mp_obj_t *args) {
    mp_int_t cs_pin                 = mp_obj_get_int(args[0]);
    mp_int_t miso_pin               = mp_obj_get_int(args[1]);
    mp_int_t mosi_pin               = mp_obj_get_int(args[2]);
    mp_int_t sclk_pin               = mp_obj_get_int(args[3]);
    esp_err_t ret;
    spi_device_handle_t handle;

    // SPI Bus configuration
    spi_bus_config_t buscfg = {
        .miso_io_num = miso_pin,
        .mosi_io_num = mosi_pin,
        .sclk_io_num = sclk_pin,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz = 32,
    };

    // Initialize the SPI
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,      // 1 MHz 
        .mode           = 0,            // Mode 0
        .spics_io_num   = cs_pin,
        .queue_size     = 1,
        .pre_cb         = NULL,
        .post_cb        = NULL
    };

    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &handle);
    ESP_ERROR_CHECK(ret);

    uint8_t buffer[1] = {0x10};
    spi_transaction_t t = {
        .tx_buffer  = &buffer,
        .length     = 8
    };
    ret = spi_device_polling_transmit(handle, &t); // Transmit
    ESP_ERROR_CHECK(ret);

    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(init_spi_obj, 4, 4, init_spi);


// Copy the uncommented line into your map table
STATIC const mp_rom_map_elem_t spi_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_spi) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&init_spi_obj) },
};
STATIC MP_DEFINE_CONST_DICT(spi_globals, spi_globals_table);

// Define module object
const mp_obj_module_t spi_module = {
    .base       = {&mp_type_module},
    .globals    = (mp_obj_dict_t *)&spi_globals,
};

// Register the module to make available in Python
MP_REGISTER_MODULE(MP_QSTR_spi_module, spi_module);