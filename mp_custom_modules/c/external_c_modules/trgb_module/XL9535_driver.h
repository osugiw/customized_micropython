#pragma once

#include "main.h"
#include "ft3267.h"

// ESP-IDF APIs
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"

// LVGL
#include "lvgl/lvgl.h"
#include "ui/ui.h"

/*              MACROS                 */
#define XL9535_IIC_ADDRESS          0x20

#define XL9535_INPUT_PORT_0_REG     0x00
#define XL9535_INPUT_PORT_1_REG     0x01
#define XL9535_OUTPUT_PORT_0_REG    0x02
#define XL9535_OUTPUT_PORT_1_REG    0x03
#define XL9535_INVERSION_PORT_0_REG 0x04
#define XL9535_INVERSION_PORT_1_REG 0x05
#define XL9535_CONFIG_PORT_0_REG    0x06
#define XL9535_CONFIG_PORT_1_REG    0x07

// ESP-IDF I2C
#define I2C_MASTER_NUM              1                           /*!< I2C master i2c port number,  0(ESP32C3, ESP32C2, ESP32H2) , 1(ESP32/ESP32S2/ESP32S3) */
#define I2C_MASTER_FREQ_HZ          400000                      /*!< I2C master clock frequency */

#define DATA_LENGTH                 512                         /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH              128                         /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000                       /*!< delay time between different test items */

#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_SLAVE_TX_BUF_LEN        (2 * DATA_LENGTH)           /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN        (2 * DATA_LENGTH)           /*!< I2C slave rx buffer size */

#define WRITE_BIT                   I2C_MASTER_WRITE            /*!< I2C master write */
#define READ_BIT                    I2C_MASTER_READ             /*!< I2C master read */
#define ACK_CHECK_EN                0x1                         /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS               0x0                         /*!< I2C master will not check ack from slave */
#define ACK_VAL                     0x0                         /*!< I2C ack value */
#define NACK_VAL                    0x1                         /*!< I2C nack value */

// CPU Interrupt number
#define ESP_INTR_FLAG_DEFAULT 0

/**************************************************
 *          Display Initialization Commands
 **************************************************/
typedef struct {
  uint8_t cmd;
  uint8_t data[16];
  uint8_t databytes; // No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

DRAM_ATTR static const lcd_init_cmd_t st_init_cmds[] = {
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x10}, 0x05},
    {0xC0, {0x3b, 0x00}, 0x02},
    {0xC1, {0x0b, 0x02}, 0x02},
    {0xC2, {0x07, 0x02}, 0x02},
    {0xCC, {0x10}, 0x01},
    {0xCD, {0x08}, 0x01}, // 用565时屏蔽    666打开
    {0xb0, {0x00, 0x11, 0x16, 0x0e, 0x11, 0x06, 0x05, 0x09, 0x08, 0x21, 0x06, 0x13, 0x10, 0x29, 0x31, 0x18}, 0x10},
    {0xb1, {0x00, 0x11, 0x16, 0x0e, 0x11, 0x07, 0x05, 0x09, 0x09, 0x21, 0x05, 0x13, 0x11, 0x2a, 0x31, 0x18}, 0x10},
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x11}, 0x05},
    {0xb0, {0x6d}, 0x01},
    {0xb1, {0x37}, 0x01},
    {0xb2, {0x81}, 0x01},
    {0xb3, {0x80}, 0x01},
    {0xb5, {0x43}, 0x01},
    {0xb7, {0x85}, 0x01},
    {0xb8, {0x20}, 0x01},
    {0xc1, {0x78}, 0x01},
    {0xc2, {0x78}, 0x01},
    {0xc3, {0x8c}, 0x01},
    {0xd0, {0x88}, 0x01},
    {0xe0, {0x00, 0x00, 0x02}, 0x03},
    {0xe1, {0x03, 0xa0, 0x00, 0x00, 0x04, 0xa0, 0x00, 0x00, 0x00, 0x20, 0x20}, 0x0b},
    {0xe2, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x0d},
    {0xe3, {0x00, 0x00, 0x11, 0x00}, 0x04},
    {0xe4, {0x22, 0x00}, 0x02},
    {0xe5, {0x05, 0xec, 0xa0, 0xa0, 0x07, 0xee, 0xa0, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x10},
    {0xe6, {0x00, 0x00, 0x11, 0x00}, 0x04},
    {0xe7, {0x22, 0x00}, 0x02},
    {0xe8, {0x06, 0xed, 0xa0, 0xa0, 0x08, 0xef, 0xa0, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x10},
    {0xeb, {0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00}, 0x07},
    {0xed, {0xff, 0xff, 0xff, 0xba, 0x0a, 0xbf, 0x45, 0xff, 0xff, 0x54, 0xfb, 0xa0, 0xab, 0xff, 0xff, 0xff}, 0x10},
    {0xef, {0x10, 0x0d, 0x04, 0x08, 0x3f, 0x1f}, 0x06},
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x13}, 0x05},
    {0xef, {0x08}, 0x01},
    {0xFF, {0x77, 0x01, 0x00, 0x00, 0x00}, 0x05},
    {0x36, {0x08}, 0x01},
    {0x3a, {0x66}, 0x01},
    {0x11, {0x00}, 0x80},
    // {0xFF, {0x77, 0x01, 0x00, 0x00, 0x12}, 0x05},
    // {0xd1, {0x81}, 0x01},
    // {0xd2, {0x06}, 0x01},
    {0x29, {0x00}, 0x80},
    {0, {0}, 0xff}
};

// class XL9535 {
//     public:
//         XL9535(){};
//         ~XL9535(){};

//         void begin(bool A0 = 0, bool A1 = 0, bool A2 = 0);
//         void pinMode(uint8_t pin, uint8_t mode);
//         void pinMode8(uint8_t port, uint8_t pin, uint8_t mode);

//         void digitalWrite(uint8_t pin, uint8_t val);
//         int digitalRead(uint8_t pin);
//         void read_all_reg();

//     protected:
//         void writeRegister(uint8_t reg, uint8_t *data, uint8_t len);
//         uint8_t readRegister(uint8_t reg, uint8_t *data, uint8_t len);

//         uint8_t _address;
//         i2c_cmd_handle_t _wire;
//         bool is_found;
// };

// extern XL9535 xl;

// Initialize display and touch drivers
void XL9535_begin(bool A0, bool A1, bool A2);

// Display Configuration functions
void lcd_send_data(uint8_t data);
void lcd_cmd(const uint8_t cmd);
void lcd_data(const uint8_t *data, int len);
esp_lcd_panel_handle_t XL9535_register_tft(void);

// Display low level functions
void XL9535_pinMode(uint8_t pin, uint8_t mode);
void XL9535_pinMode8(uint8_t port, uint8_t pin, uint8_t mode);
void XL9535_digitalWrite(uint8_t pin, uint8_t val);
int XL9535_digitalRead(uint8_t pin);
void XL9535_read_all_reg();
void XL9535_writeRegister(uint8_t reg, uint8_t *data, uint8_t len);
void XL9535_readRegister(uint8_t reg, uint8_t *data, uint8_t len);
// static void IRAM_ATTR touch_handler(void* arg);

// // LVGL Function
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);