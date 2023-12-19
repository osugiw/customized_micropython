// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

// ESP-IDF APIs
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "ssd1306.h"

#define I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */

static ssd1306_handle_t ssd1306_dev = NULL;

STATIC mp_obj_t initDisplay(mp_obj_t _sda_obj, mp_obj_t _scl_obj)
{
    mp_int_t _sda = mp_obj_get_int(_sda_obj);
    mp_int_t _scl = mp_obj_get_int(_scl_obj);

    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)_sda;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)_scl;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_refresh_gram(ssd1306_dev);
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    
    char data_str[30] = {0};
    sprintf(data_str, "Training Micropython");
    ssd1306_draw_string(ssd1306_dev, 5, 15, (const uint8_t *)data_str, 12, 1);
    sprintf(data_str, "by Sugiarto Wibowo");
    ssd1306_draw_string(ssd1306_dev, 5, 40, (const uint8_t *)data_str, 12, 1);
    ssd1306_refresh_gram(ssd1306_dev);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(initDisplay_obj, initDisplay);

STATIC mp_obj_t drawString(size_t n_args, const mp_obj_t *args) {
    mp_int_t xPos = mp_obj_get_int(args[0]);
    mp_int_t yPos = mp_obj_get_int(args[1]);
    mp_int_t fontSize = mp_obj_get_int(args[2]);
    const char* strData = mp_obj_str_get_str(args[3]);

    ssd1306_draw_string(ssd1306_dev, xPos, yPos, (const uint8_t *)strData, fontSize, 1);
    ssd1306_refresh_gram(ssd1306_dev);

     return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawString_obj, 4, 4, drawString);

STATIC mp_obj_t clearScreen(){
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(clearScreen_obj, clearScreen);

// Define all attributes of the module (the MicroPython object reference.)
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2c_driver) },
    { MP_ROM_QSTR(MP_QSTR_initDisplay), MP_ROM_PTR(&initDisplay_obj) },
    { MP_ROM_QSTR(MP_QSTR_drawString), MP_ROM_PTR(&drawString_obj) },
    { MP_ROM_QSTR(MP_QSTR_clearScreen), MP_ROM_PTR(&clearScreen_obj) },
};
STATIC MP_DEFINE_CONST_DICT(module_globals, module_globals_table);

// Define module object.
const mp_obj_module_t i2c_ssd1306 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_i2c_ssd1306, i2c_ssd1306);