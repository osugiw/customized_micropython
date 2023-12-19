#include "main.h"

// ESP-IDF APIs
#include "pin_config.h"
#include "soc/efuse_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/esp_clk.h"
#include "esp_chip_info.h"
#include "esp_rom_spiflash.h"
#include "esp_flash.h"
#include "esp_psram.h"
#include "spi_flash_mmap.h"
#include "esp_lcd_panel_rgb.h"

// Driver
#include "XL9535_driver.h"

// LVGL
#include "lvgl/lvgl.h"
#include "ui/ui.h"

void print_chip_info(void)
{
    esp_chip_info_t chip_info;
    uint32_t size_flash_chip = 0;

    esp_chip_info(&chip_info);
    uint32_t chip_id = g_rom_flashchip.device_id;
    chip_id = ((chip_id & 0xff) << 16) | ((chip_id >> 16) & 0xff) | (chip_id & 0xff00);

    mp_print_str(&mp_plat_print, "\n\n------------------------------------------\n");
    mp_printf(&mp_plat_print, "Chip: %d\n", chip_info.model);
    mp_printf(&mp_plat_print, "ChipRevision: %d\n", chip_info.revision);
    mp_printf(&mp_plat_print, "Chip ID: %d\n", chip_id);
    mp_printf(&mp_plat_print, "Psram size: %d MB\n", esp_psram_get_size() / 1024 / 1024);
    esp_flash_get_size(NULL, &size_flash_chip);
    mp_printf(&mp_plat_print, "Flash size: %d MB\n", size_flash_chip / 1024 / 1024);
    mp_printf(&mp_plat_print, "CPU frequency: %d MHz\n", esp_clk_cpu_freq() / 1000000);
    mp_print_str(&mp_plat_print, "------------------------------------------\n\n");
}

/**
*   @brief Inialize SPI Peripehral
*   @param args[0]         CS Pin
*   @param args[1]         MISO Pin
*   @param args[2]         MOSI Pin
*   @param args[3]         SCLK Pin
**/
STATIC mp_obj_t XL9535_init() {
    esp_err_t ret = ESP_OK;
    
    print_chip_info();      // Print Chip configuration
    mp_print_str(&mp_plat_print, "Initializing XL9535...\n");
    XL9535_begin(0, 0, 0);  // Initialize display

    // // load UI
    // ui_init();
    // mp_print_str(&mp_plat_print,"Successfully load the UI!\n");

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(XL9535_init_obj, XL9535_init);

// Copy the uncommented line into your map table
STATIC const mp_rom_map_elem_t trgb_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_trgb) },
    { MP_ROM_QSTR(MP_QSTR_XL9535_init), MP_ROM_PTR(&XL9535_init_obj) },
};
STATIC MP_DEFINE_CONST_DICT(trgb_globals, trgb_globals_table);

// Define module object
const mp_obj_module_t trgb_module = {
    .base       = {&mp_type_module},
    .globals    = (mp_obj_dict_t *)&trgb_globals,
};

// Register the module to make available in Python
MP_REGISTER_MODULE(MP_QSTR_trgb_module, trgb_module);