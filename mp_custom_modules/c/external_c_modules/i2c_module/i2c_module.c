// Include Header
#include <i2c_module.h>

/************************************************************************************************/
/*                                      I2C SLAVE                                               */
/************************************************************************************************/
MP_DEFINE_CONST_FUN_OBJ_3(slave_init_obj, slave_init);
MP_DEFINE_CONST_FUN_OBJ_0(slave_read_obj, slave_read);

/************************************************************************************************/
/*                                      I2C MASTER                                              */
/************************************************************************************************/
MP_DEFINE_CONST_FUN_OBJ_3(master_init_obj, master_init);
MP_DEFINE_CONST_FUN_OBJ_2(master_write_obj, master_write);

// Define all attributes of the module (the MicroPython object reference.)
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2c_driver) },
    { MP_ROM_QSTR(MP_QSTR_master_init), MP_ROM_PTR(&master_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_master_write), MP_ROM_PTR(&master_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_slave_init), MP_ROM_PTR(&slave_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_slave_read), MP_ROM_PTR(&slave_read_obj) },
};
STATIC MP_DEFINE_CONST_DICT(module_globals, module_globals_table);

// Define module object.
const mp_obj_module_t i2c_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_i2c_module, i2c_module);