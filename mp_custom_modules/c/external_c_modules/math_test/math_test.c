// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

/************************************************************************************************/
/*                                      Function                                                */
/************************************************************************************************/
// This is the function which will be called from Python as math_test.subtract(a, b).
STATIC mp_obj_t subtract(mp_obj_t a_obj, mp_obj_t b_obj) {
    // Extract the ints from the micropython input objects.
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);
    
    return mp_obj_new_int(a - b);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(subtract_obj, subtract);

// Define all attributes of the module (the MicroPython object reference.)
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_math_test) },
    { MP_ROM_QSTR(MP_QSTR_subtract), MP_ROM_PTR(&subtract_obj) },
};
STATIC MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t math_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_math_test, math_module);
