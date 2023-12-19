#include "main.h"

/*************************************Class methods*****************************************/
// __init__
mp_obj_t mathclass_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 256, true);
    mp_mathclass_obj_t *self = m_new_obj(mp_mathclass_obj_t);
    self->base.type     = &mp_mathclass_type_obj;
    self->currentValue  = mp_obj_get_int(args[0]);
    return MP_OBJ_FROM_PTR(self);
}

// __repr__
void mathclass_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "mathclass(");
    mp_obj_print_helper(print, mp_obj_new_int(self->currentValue), PRINT_REPR);
    mp_print_str(print, ")");
}

// Sum method
mp_obj_t mathclass_add(mp_obj_t self_in, mp_obj_t input_value) {
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue += mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue); 
}

// Subtract method
mp_obj_t mathclass_subtract(mp_obj_t self_in, mp_obj_t input_value) {
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue -= mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue);
}

// Multiplication method
mp_obj_t mathclass_multiply(mp_obj_t self_in, mp_obj_t input_value){
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue *= mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue);
}

// Division method
mp_obj_t mathclass_divide(mp_obj_t self_in, mp_obj_t input_value){
    mp_mathclass_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t *arg = NULL;
    size_t dataLength = 0;
    mp_obj_get_array(input_value, &dataLength, &arg);

    for(uint8_t i=0; i<dataLength; i++){
        self->currentValue /= mp_obj_get_int(arg[i]);
    }
    return mp_obj_new_int(self->currentValue);
}
/**********************************EOF Class methods*****************************************/