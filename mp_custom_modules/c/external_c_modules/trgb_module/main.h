/*********************
 *      INCLUDES
 *********************/

// Micropython APIs
#include "py/runtime.h"
#include "py/obj.h"

// ESP-IDF APIs
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "hal/i2c_ll.h"
#include "esp_system.h"

// class TRGBSuppport {
//   private:
//     XL9535 xl;		// I/O Extender
//     TouchLib touch;

//     lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
//     lv_disp_drv_t disp_drv;      // contains callback functions
//     lv_indev_drv_t indev_drv;
//     void tft_init(void);
//     void lcd_cmd(const uint8_t cmd);
//     void lcd_data(const uint8_t *data, int len);
//     void lcd_send_data(uint8_t data);
//     static esp_lcd_panel_handle_t register_tft();

//   public:
//     TRGBSuppport();
//     TouchLib& getTouch(){return touch;};
//     void deepSleep();
//     void restart();
//     void init();
//     void SD_init(); //could be static, but I guess it is too confusing.

//     float getBatVoltage() const {return (analogRead(BAT_VOLT_PIN) * 2 * 3.3) / 4096;}
//     bool isBatCharging() const {return getBatVoltage() > 4.1;}  // LiIon end-of-charge voltage ~ 4.2V, so charger voltage is approx this voltage.
//     uint16_t getBootCount();

//     // **** functions from official example that give some useful information ****
//     static void print_chip_info();  // ESP32 hardware details

//     //     Scan I2C (Groove connector) for devices.
//     //         0x20: I/O extension (XL9535)
//     //         0x38: Touchpad-controller (FT3267)
//     static void scan_iic();

// };

// extern TRGBSuppport trgb;

void lcd_send_data(uint8_t data);
void lcd_cmd(const uint8_t cmd);
void lcd_data(const uint8_t *data, int len);
void print_chip_info(void);
// static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);