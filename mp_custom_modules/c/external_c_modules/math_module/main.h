// Include MicroPython API.
#include "py/runtime.h"
#include "py/obj.h"

// Math Class
typedef struct _mp_mathclass_obj_t {
    mp_obj_base_t base;
    int32_t currentValue;
} mp_mathclass_obj_t;
extern const mp_obj_type_t mp_mathclass_type_obj;

// Class methods
extern mp_obj_t mathclass_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
extern void mathclass_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
extern mp_obj_t mathclass_add(mp_obj_t self_in, mp_obj_t input_value);
extern mp_obj_t mathclass_subtract(mp_obj_t self_in, mp_obj_t input_value);
extern mp_obj_t mathclass_multiply(mp_obj_t self_in, mp_obj_t input_value);
extern mp_obj_t mathclass_divide(mp_obj_t self_in, mp_obj_t input_value);
