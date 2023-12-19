#include "XL9535_driver.h"
#include "pin_config.h"
#include "img.h"

uint8_t _address;
i2c_cmd_handle_t _wire;
bool is_found;
static bool touch_pin_get_int = false;

static void IRAM_ATTR touch_handler(void* arg)
{
    touch_pin_get_int = true;
}

void XL9535_begin(bool A0, bool A1, bool A2) {
  esp_err_t err;
  _address = XL9535_IIC_ADDRESS | (A2 << 3) | (A1 << 2) | (A0 << 1);
  
  // I2C Configuration
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = LCD_SDA_PIN,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = LCD_CLK_PIN,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = 100000,
      // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
  };
  i2c_param_config(I2C_MASTER_NUM, &conf); 
  i2c_set_timeout(I2C_MASTER_NUM, I2C_LL_MAX_TIMEOUT);
  err = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

  _wire = i2c_cmd_link_create();
  is_found = true;
  i2c_master_start(_wire);
  i2c_master_write_byte(_wire, (_address << 1) | WRITE_BIT, ACK_CHECK_EN);      // Send a byte to slave
  i2c_master_stop(_wire);
  err = i2c_master_cmd_begin(1, _wire, DELAY_TIME_BETWEEN_ITEMS_MS / portTICK_PERIOD_MS);    // Send All queued commands
  i2c_cmd_link_delete(_wire);                                                   // Finished I2C Transaction

  if(err == ESP_OK){
    mp_print_str(&mp_plat_print, "Found XL9535\n");
  }
  else{
    mp_print_str(&mp_plat_print, "XL9535 not found\n");
    is_found = false;
  }

  uint8_t pin = (1 << PWR_EN_PIN) | (1 << LCD_CS_PIN) | (1 << TP_RES_PIN) | (1 << LCD_SDA_PIN) | (1 << LCD_CLK_PIN) |
                  (1 << LCD_RST_PIN) | (1 << SD_CS_PIN);
  esp_err_t ret = ESP_OK;

  XL9535_pinMode8(0, pin, GPIO_MODE_OUTPUT);
  XL9535_digitalWrite(PWR_EN_PIN, 1);
  
  XL9535_digitalWrite(LCD_CS_PIN, 1);
  XL9535_digitalWrite(LCD_SDA_PIN, 1);
  XL9535_digitalWrite(LCD_CLK_PIN, 1);

  // Reset the display
  XL9535_digitalWrite(LCD_RST_PIN, 1);
  XL9535_digitalWrite(TP_RES_PIN, 0);
  vTaskDelay(100 / portTICK_PERIOD_MS);
  XL9535_digitalWrite(LCD_RST_PIN, 1);
  XL9535_digitalWrite(TP_RES_PIN, 1);
  vTaskDelay(100 / portTICK_PERIOD_MS);

  // Initialize touch driver
  ft3267_init(_wire);

  // Switch on the backlight
  gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_OUTPUT,
      .pin_bit_mask = (1ULL << EXAMPLE_PIN_NUM_BK_LIGHT),
      .pull_down_en = 0,
      .pull_up_en = 0
  };
  gpio_config(&io_conf);
  gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

  int cmd = 0;
  while (st_init_cmds[cmd].databytes != 0xff) {
      lcd_cmd(st_init_cmds[cmd].cmd);
      lcd_data(st_init_cmds[cmd].data, st_init_cmds[cmd].databytes & 0x1F);
      if (st_init_cmds[cmd].databytes & 0x80) {
          vTaskDelay(100 / portTICK_PERIOD_MS);
      }
      cmd++;
  }
  gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
  mp_print_str(&mp_plat_print, "XL9535 configuration complete!\n");

  // Test Drawing
  esp_lcd_panel_handle_t panel_handle = XL9535_register_tft();  // Register XL9535 driver to esp_lcd 
  ret = esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 480, 480, logo_img); 
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  mp_printf(&mp_plat_print, "Drawing bitmap status is %s\n", ret == ESP_OK ? "Success!" : "Fail!");
  
  // // Touch Interrupt Pin
  // io_conf.mode = GPIO_MODE_INPUT;
  // io_conf.pin_bit_mask = (1ULL << TP_INT_PIN);
  // io_conf.pull_up_en = 1;
  // io_conf.intr_type = GPIO_INTR_NEGEDGE;
  // gpio_config(&io_conf);
  // gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);                                  // Install gpio isr service
  // gpio_isr_handler_add(TP_INT_PIN, touch_handler, (void*) touch_pin_get_int); //hook isr handler for specific gpio pin

  // // LVGL
  // lv_disp_draw_buf_t disp_buf;  // Contains internal graphic buffer(s) called draw buffer(s)
  // lv_disp_drv_t disp_drv; 
  // lv_indev_drv_t indev_drv;

  // // Initialize LVGL
  // lv_init();

  // // alloc draw buffers used by LVGL from PSRAM
	// lv_color_t *buf1 = (lv_color_t*) heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	// assert(buf1);
	// lv_color_t *buf2 = (lv_color_t*) heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
	// assert(buf2);
	// lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES);

  // // Register the display to LVGL
	// lv_disp_drv_init(&disp_buf);   // Contains display driver callback
	// disp_drv.hor_res = EXAMPLE_LCD_H_RES;
	// disp_drv.ver_res = EXAMPLE_LCD_V_RES;
	// disp_drv.flush_cb = lvgl_flush_cb;
	// disp_drv.draw_buf = &disp_buf;
	// disp_drv.user_data = XL9535_register_tft();
	// lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
  // mp_print_str(&mp_plat_print,"Successfully register the display driver to LVGL!\n");

  // // Register touch driver to LVGL
  // lv_indev_drv_init(&indev_drv); // Contains touch driver callback
	// indev_drv.type = LV_INDEV_TYPE_POINTER;
	// indev_drv.read_cb = lv_touchpad_read;
	// lv_indev_drv_register(&indev_drv);

  // mp_printf(&mp_plat_print, "1-Drawing buffer frame size %d, buf-1: %d, buf-2: %d\n", disp_buf, buf1, buf2);
  // lv_obj_t *label = lv_label_create( lv_scr_act() );
  // lv_label_set_text( label, "Hello Ardino and LVGL!");
  // lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
  // mp_printf(&mp_plat_print, "2-Drawing buffer frame size %d, buf-1: %d, buf-2: %d\n", disp_buf, buf1, buf2);
}

/*****************************************************************************************/
/*                                LVGL Functions                                         */
/*****************************************************************************************/
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
  esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
  int offsetx1 = area->x1;
  int offsetx2 = area->x2;
  int offsety1 = area->y1;
  int offsety2 = area->y2;
  esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
  lv_disp_flush_ready(drv);
}

static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) 
{
  if (touch_pin_get_int) {
    uint8_t touch_points_num;
    uint16_t x, y;
    ft3267_read_pos(&touch_points_num, &x, &y);
    if (touch_points_num > 0) {
    	data->point.x = x;
    	data->point.y = y;
    	data->state = LV_INDEV_STATE_PRESSED;
    } else {
    	data->state =  LV_INDEV_STATE_RELEASED;
    }
    touch_pin_get_int = false;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}
/*****************************************************************************************/

/*****************************************************************************************/
/*                         Display Configuration Functions                               */
/*****************************************************************************************/
esp_lcd_panel_handle_t XL9535_register_tft(void)
{
  esp_lcd_panel_handle_t panel_handle = NULL;
  esp_err_t ret = ESP_OK;
  esp_lcd_rgb_panel_config_t panel_config = {
      .clk_src = LCD_CLK_SRC_PLL160M,
      .timings =
      {
          .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
          .h_res = EXAMPLE_LCD_H_RES,
          .v_res = EXAMPLE_LCD_V_RES,
          // The following parameters should refer to LCD spec
          .hsync_pulse_width = 1,
          .hsync_back_porch = 30,
          .hsync_front_porch = 50,
          .vsync_pulse_width = 1,
          .vsync_back_porch = 30,
          .vsync_front_porch = 20,
          .flags =
          {
              .pclk_active_neg = 1,
          },
      },
      .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
      .psram_trans_align = 64,
      .hsync_gpio_num = EXAMPLE_PIN_NUM_HSYNC,
      .vsync_gpio_num = EXAMPLE_PIN_NUM_VSYNC,
      .de_gpio_num = EXAMPLE_PIN_NUM_DE,
      .pclk_gpio_num = EXAMPLE_PIN_NUM_PCLK,
      .data_gpio_nums =
      {
          // EXAMPLE_PIN_NUM_DATA0,
          EXAMPLE_PIN_NUM_DATA13,
          EXAMPLE_PIN_NUM_DATA14,
          EXAMPLE_PIN_NUM_DATA15,
          EXAMPLE_PIN_NUM_DATA16,
          EXAMPLE_PIN_NUM_DATA17,

          EXAMPLE_PIN_NUM_DATA6,
          EXAMPLE_PIN_NUM_DATA7,
          EXAMPLE_PIN_NUM_DATA8,
          EXAMPLE_PIN_NUM_DATA9,
          EXAMPLE_PIN_NUM_DATA10,
          EXAMPLE_PIN_NUM_DATA11,
          // EXAMPLE_PIN_NUM_DATA12,

          EXAMPLE_PIN_NUM_DATA1,
          EXAMPLE_PIN_NUM_DATA2,
          EXAMPLE_PIN_NUM_DATA3,
          EXAMPLE_PIN_NUM_DATA4,
          EXAMPLE_PIN_NUM_DATA5,
      },
      .disp_gpio_num = EXAMPLE_PIN_NUM_DISP_EN, 
      .flags =
      {
          .fb_in_psram = 1, // allocate frame buffer in PSRAM
      },
  };
  ret = esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
  mp_printf(&mp_plat_print, "Adding new panel configuration status is %s\n", ret == ESP_OK ? "Success!" : "Fail!");
  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  mp_printf(&mp_plat_print, "Resetting panel status is %s\n", ret == ESP_OK ? "Success!" : "Fail!");
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
  mp_printf(&mp_plat_print, "Initialize panel status is %s\n", ret == ESP_OK ? "Success!" : "Fail!");
  return panel_handle;
}

void lcd_send_data(uint8_t data)
{
    uint8_t n;
    for (n = 0; n < 8; n++) {
        XL9535_digitalWrite(LCD_CLK_PIN, 0);
        if (data & 0x80)
            XL9535_digitalWrite(LCD_SDA_PIN, 1);
        else
            XL9535_digitalWrite(LCD_SDA_PIN, 0);

        data <<= 1;
        XL9535_digitalWrite(LCD_CLK_PIN, 1);
    }
}

void lcd_cmd(const uint8_t cmd)
{
    XL9535_digitalWrite(LCD_CS_PIN, 0);
    XL9535_digitalWrite(LCD_SDA_PIN, 0);
    XL9535_digitalWrite(LCD_CLK_PIN, 0);
    XL9535_digitalWrite(LCD_CLK_PIN, 1);
    lcd_send_data(cmd);
    XL9535_digitalWrite(LCD_CS_PIN, 1);
}

void lcd_data(const uint8_t *data, int len)
{
    uint32_t i = 0;
    XL9535_digitalWrite(LCD_CS_PIN, 0);
    if (len == 0)
        return; // no need to send anything
    do {
        XL9535_digitalWrite(LCD_SDA_PIN, 1);
        XL9535_digitalWrite(LCD_CLK_PIN, 0);
        XL9535_digitalWrite(LCD_CLK_PIN, 1);
        lcd_send_data(*(data + i));
        i++;
    } while (len--);
    XL9535_digitalWrite(LCD_CS_PIN, 1);
}
/*****************************************************************************************/

/*****************************************************************************************/
/*                         Display Low-level Function helper                             */
/*****************************************************************************************/
void XL9535_pinMode(uint8_t pin, uint8_t mode) {
  if (is_found) {
    uint8_t port = 0;
    if (pin > 7) {
      XL9535_readRegister(XL9535_CONFIG_PORT_1_REG, &port, 1);
      if (mode == GPIO_MODE_OUTPUT) {
        port = port & (~(1 << (pin - 10)));
      } else {
        port = port | (1 << (pin - 10));
      }
      XL9535_writeRegister(XL9535_CONFIG_PORT_1_REG, &port, 1);

    } else {
      XL9535_readRegister(XL9535_CONFIG_PORT_0_REG, &port, 1);
      if (mode == GPIO_MODE_OUTPUT) {
        port = port & (~(1 << pin));
      } else {
        port = port | (1 << pin);
      }
      XL9535_writeRegister(XL9535_CONFIG_PORT_0_REG, &port, 1);
    }
  } else {
     mp_print_str(&mp_plat_print, "XL9535 not found\n");
  }
}

void XL9535_pinMode8(uint8_t port, uint8_t pin, uint8_t mode) {
  if (is_found) {
    uint8_t _pin = (mode != GPIO_MODE_OUTPUT) ? pin : ~pin;
    if (port) {
      XL9535_writeRegister(XL9535_CONFIG_PORT_1_REG, &_pin, 1);
    } else {
      XL9535_writeRegister(XL9535_CONFIG_PORT_0_REG, &_pin, 1);
    }
  } else {
    mp_print_str(&mp_plat_print, "XL9535 not found\n");
  }
}

void XL9535_digitalWrite(uint8_t pin, uint8_t val) {
  if (is_found) {
    uint8_t port = 0;
    uint8_t reg_data = 0;
    if (pin > 7) {
      XL9535_readRegister(XL9535_OUTPUT_PORT_1_REG, &reg_data, 1);
      reg_data = reg_data & (~(1 << (pin - 10)));
      port = reg_data | val << (pin - 10);
      XL9535_writeRegister(XL9535_OUTPUT_PORT_1_REG, &port, 1);
    } else {
      XL9535_readRegister(XL9535_OUTPUT_PORT_0_REG, &reg_data, 1);
      reg_data = reg_data & (~(1 << pin));
      port = reg_data | val << pin;
      XL9535_writeRegister(XL9535_OUTPUT_PORT_0_REG, &port, 1);
    }
  } else {
    mp_print_str(&mp_plat_print, "XL9535 not found\n");
  }
}

int XL9535_digitalRead(uint8_t pin) {
  if (is_found) {
    int state = 0;
    uint8_t port = 0;
    if (pin > 7) {
      XL9535_readRegister(XL9535_INPUT_PORT_1_REG, &port, 1);
      state = port & (pin - 10) ? 1 : 0;
    } else {
      XL9535_readRegister(XL9535_INPUT_PORT_0_REG, &port, 1);
      state = port & pin ? 1 : 0;
    }
    return state;
  } else {
    mp_print_str(&mp_plat_print,"XL9535 not found\n");
  }
  return 0;
}

void XL9535_read_all_reg() {
  uint8_t data;
  for (uint8_t i = 0; i < 8; i++) {
    XL9535_readRegister(i, &data, 1);
    mp_printf(&mp_plat_print, "0x%02x : 0x%02X \r\n", i, data);
  }
}

void XL9535_writeRegister(uint8_t reg, uint8_t *data, uint8_t len) {
  _wire = i2c_cmd_link_create();
  i2c_master_start(_wire);
  i2c_master_write_byte(_wire, (_address << 1) | WRITE_BIT, ACK_CHECK_EN);    // Send a byte of data address
  i2c_master_write_byte(_wire, reg, ACK_CHECK_EN);                            // Send a byte of register destination
  i2c_master_write(_wire, data, len, ACK_CHECK_EN);                           // Queue data
  i2c_master_stop(_wire);
  esp_err_t ret = i2c_master_cmd_begin(1, _wire, DELAY_TIME_BETWEEN_ITEMS_MS / portTICK_PERIOD_MS);                  // Send All queued commands
  i2c_cmd_link_delete(_wire); 
  if (ret != ESP_OK){
    mp_printf(&mp_plat_print, "XL9535 Error - Write Register status is 0x%x\n", ret);
  }
}

void XL9535_readRegister(uint8_t reg, uint8_t *data, uint8_t len) {
  _wire = i2c_cmd_link_create();
  i2c_master_start(_wire);
  i2c_master_write_byte(_wire, (_address << 1) | READ_BIT, ACK_CHECK_EN);
  i2c_master_write_byte(_wire, reg, ACK_CHECK_EN);                            // Send a byte of register destination

  // Read multiple bytes
  if(len > 1){
    i2c_master_read(_wire, data, len - 1, ACK_VAL);
  }

  // Read a byte
  i2c_master_read_byte(_wire, data + len - 1, NACK_VAL);
  i2c_master_stop(_wire);
  esp_err_t ret = i2c_master_cmd_begin(1, _wire, DELAY_TIME_BETWEEN_ITEMS_MS / portTICK_PERIOD_MS);          // Send All queued commands
  i2c_cmd_link_delete(_wire); 
  if (ret != ESP_OK){
    mp_printf(&mp_plat_print, "XL9535 Error - Read Register status is 0x%x\n", ret);
  }
}
/*****************************************************************************************/