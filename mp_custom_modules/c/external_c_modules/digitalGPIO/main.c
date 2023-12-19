#include "py/runtime.h"
#include "py/obj.h"
#include "driver/gpio.h"

/************************************************************************************************/
/*                                      Digital GPIO                                            */
/************************************************************************************************/
STATIC mp_obj_t digitalWrite(mp_obj_t outputPin_obj, mp_obj_t _value_obj) {
    mp_int_t _pin = mp_obj_get_int(outputPin_obj);
    mp_int_t _value = mp_obj_get_int(_value_obj);
    
    gpio_config_t io_conf     = {};
    io_conf.intr_type       = GPIO_INTR_DISABLE;        // Disable interrupt
    io_conf.mode            = GPIO_MODE_OUTPUT;         // Set as output mode
    io_conf.pin_bit_mask    = (1ULL<<_pin);
    io_conf.pull_down_en    = 0;                        // Disable pull-down mode
    io_conf.pull_up_en      = 0;                        // Disable pull-up mode
    gpio_config(&io_conf);                              // Configure GPIO with the given settings

    gpio_set_level(_pin, _value);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(digitalWrite_obj, digitalWrite);

STATIC mp_obj_t digitalRead(mp_obj_t inputPin_obj) {
    mp_int_t _pin = mp_obj_get_int(inputPin_obj);
    
    gpio_config_t io_conf     = {};
    io_conf.intr_type       = GPIO_INTR_DISABLE;       // Disable interrupt
    io_conf.mode            = GPIO_MODE_INPUT;         // Set as output mode
    io_conf.pin_bit_mask    = (1ULL<<_pin);
    io_conf.pull_down_en    = 0;                        // Disable pull-down mode
    io_conf.pull_up_en      = 1;                        // Enable pull-up mode
    gpio_config(&io_conf);                              // Configure GPIO with the given settings

    bool readValue = gpio_get_level(_pin);
    return mp_obj_new_bool(readValue);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(digitalRead_obj, digitalRead);

STATIC const mp_rom_map_elem_t digitalGPIO_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR__name__), MP_ROM_QSTR(MP_QSTR_digitalGPIO) },
    { MP_ROM_QSTR(MP_QSTR_digitalWrite), MP_ROM_PTR(&digitalWrite_obj) },
    { MP_ROM_QSTR(MP_QSTR_digitalRead),    MP_ROM_PTR(&digitalRead_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalGPIO_globals, digitalGPIO_globals_table);

// Define module object.
const mp_obj_module_t digitalGPIO_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&digitalGPIO_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_digitalGPIO, digitalGPIO_module);
