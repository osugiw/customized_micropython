#include "py/dynruntime.h"
#include "py/obj.h"

// Helper function to compute multiplication
STATIC mp_int_t multiplication_helper(mp_int_t x, mp_int_t y) {
    if (x == 0 || y ==0)
	    return 1;
    return x * y;
}

// This is the function which will be called from Python, as multiplication(x)
STATIC mp_obj_t multiplication(mp_obj_t x_obj, mp_obj_t y_obj) {
    // Extract the integer from the MicroPython input object
    mp_int_t x = mp_obj_get_int(x_obj);
    mp_int_t y = mp_obj_get_int(y_obj);
    mp_int_t result = multiplication_helper(x, y);
    return mp_obj_new_int(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(multiplication_obj, multiplication);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY
    
    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_multiplication, MP_OBJ_FROM_PTR(&multiplication_obj));
    
    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
