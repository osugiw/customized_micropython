/*********************
 *      INCLUDES
 *********************/
#include <py/mpconfig.h>
#include <py/misc.h>
#include <py/gc.h>
#include <py/mpstate.h>
#include "lvgl/lvgl.h"

typedef struct lvgl_root_pointers_t {
  LV_ROOTS
} lvgl_root_pointers_t;