#include "main.h"

// Simple Math Class methods
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_add_obj, mathclass_add);
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_subtract_obj, mathclass_subtract);
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_multiply_obj, mathclass_multiply);
MP_DEFINE_CONST_FUN_OBJ_2(mathclass_divide_obj, mathclass_divide);

// Define class methods
STATIC const mp_rom_map_elem_t mathclass_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_add), MP_ROM_PTR(&mathclass_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_subtract), MP_ROM_PTR(&mathclass_subtract_obj) },
    { MP_ROM_QSTR(MP_QSTR_multiply), MP_ROM_PTR(&mathclass_multiply_obj) },
    { MP_ROM_QSTR(MP_QSTR_divide), MP_ROM_PTR(&mathclass_divide_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mathclass_locals_dict, mathclass_locals_dict_table);

// Define Class as the object
MP_DEFINE_CONST_OBJ_TYPE(
    mp_mathclass_type_obj,                  // Name of the object
    MP_QSTR_mathclass,                      // Name of the class
    MP_TYPE_FLAG_NONE,                  
    make_new, mathclass_make_new,           // __init__
    print, mathclass_print,                 // __print__
    locals_dict, &mathclass_locals_dict     // methods
);

// Map the class reference
STATIC const mp_map_elem_t mp_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_math_module) },
    { MP_ROM_QSTR(MP_QSTR_mathclass), (mp_obj_t)&mp_mathclass_type_obj },
};
STATIC MP_DEFINE_CONST_DICT (
    mp_module_mp_globals,
    mp_globals_table
);
const mp_obj_module_t mp_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_mp_globals,
};
// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_math_module, mp_user_cmodule);