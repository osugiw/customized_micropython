#include "main.h"

MP_DEFINE_CONST_FUN_OBJ_1(ntc_5k_obj, ntc_5k);

// Copy the uncommented line into your map table
STATIC const mp_rom_map_elem_t ntc_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ntc) },
    { MP_ROM_QSTR(MP_QSTR_ntc_5k), MP_ROM_PTR(&ntc_5k_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ntc_globals, ntc_globals_table);

// Define module object
const mp_obj_module_t ntc_module = {
    .base       = {&mp_type_module},
    .globals    = (mp_obj_dict_t *)&ntc_globals,
};

// Register the module to make available in Python
MP_REGISTER_MODULE(MP_QSTR_ntc_module, ntc_module);