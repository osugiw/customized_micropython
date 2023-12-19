// extern "C" {

#include "ft3267.h"

i2c_cmd_handle_t ft3267_wire;

static inline esp_err_t ft3267_read_bytes(uint8_t reg_addr, size_t len, uint8_t *data) {
    ft3267_wire = i2c_cmd_link_create();
    i2c_master_start(ft3267_wire);
    i2c_master_write_byte(ft3267_wire, (FT5x06_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(ft3267_wire, reg_addr, ACK_CHECK_EN);                            // Send a byte of register destination
    
    // Read multiple bytes
    if(len > 1){
        i2c_master_read(ft3267_wire, data, len - 1, ACK_VAL);
    }

    // Read a byte
    i2c_master_read_byte(ft3267_wire, data, NACK_VAL);
    i2c_master_stop(ft3267_wire);
    esp_err_t ret = i2c_master_cmd_begin(1, ft3267_wire, DELAY_TIME_BETWEEN_ITEMS_MS / portTICK_PERIOD_MS);          // Send All queued commands
    i2c_cmd_link_delete(ft3267_wire); 
    if (ret != ESP_OK){
        mp_printf(&mp_plat_print, "FT3267 Error - Read Register status is 0x%x\n", ret);
    }
    return 0;
}

static inline esp_err_t ft3267_write_byte(uint8_t reg_addr, uint8_t data) {
    ft3267_wire = i2c_cmd_link_create();
    i2c_master_start(ft3267_wire);
    i2c_master_write_byte(ft3267_wire, (FT5x06_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);    // Send a byte of data address
    i2c_master_write_byte(ft3267_wire, reg_addr, ACK_CHECK_EN);                          // Send a byte of register destination
    i2c_master_write_byte(ft3267_wire, data, ACK_CHECK_EN);                              // Queue data
    i2c_master_stop(ft3267_wire);
    esp_err_t ret = i2c_master_cmd_begin(1, ft3267_wire, DELAY_TIME_BETWEEN_ITEMS_MS / portTICK_PERIOD_MS);                  // Send All queued commands
    i2c_cmd_link_delete(ft3267_wire); 
    if (ret != ESP_OK){
    mp_printf(&mp_plat_print, "FT3267 Error - Write Register status is 0x%x\n", ret);
    }
  return 0;
}

static esp_err_t ft3267_get_touch_points_num(uint8_t *touch_points_num) {
  return ft3267_read_bytes(FT5x06_TOUCH_POINTS, 1, touch_points_num);
}

esp_err_t ft3267_read_pos(uint8_t *touch_points_num, uint16_t *x, uint16_t *y) {
  esp_err_t ret_val = ESP_OK;
  static uint8_t data[4];

  ret_val |= ft3267_get_touch_points_num(touch_points_num);
  *touch_points_num = (*touch_points_num) & 0x0f;
  if (0 == *touch_points_num) {
  } else {
    ret_val |= ft3267_read_bytes(FT5x06_TOUCH1_XH, 4, data);

    *x = ((data[0] & 0x0f) << 8) + data[1];
    *y = ((data[2] & 0x0f) << 8) + data[3];
  }

  return ret_val;
}

esp_err_t fx5x06_read_gesture(ft3267_gesture_t *gesture) {
  return ft3267_read_bytes(FT5x06_GESTURE_ID, 1, (uint8_t *)gesture);
}

esp_err_t ft3267_init(i2c_cmd_handle_t _i2cHandle)
{
    // Assign the initialized I2C
    if(_i2cHandle != NULL){
        ft3267_wire = _i2cHandle;
    }
    else{
        return ESP_FAIL;
    }

    esp_err_t ret = ESP_OK;
    ft3267_write_byte(FT5x06_ID_G_THGROUP, 70);             // Valid touching detect threshold
    ft3267_write_byte(FT5x06_ID_G_THPEAK, 60);              // valid touching peak detect threshold 
    ft3267_write_byte(FT5x06_ID_G_THCAL, 16);               // Touch focus threshold
    ft3267_write_byte(FT5x06_ID_G_THWATER, 60);             // Threshold when there is surface water
    ft3267_write_byte(FT5x06_ID_G_THTEMP, 10);              // Threshold of temperature compensation
    ft3267_write_byte(FT5x06_ID_G_THDIFF, 20);              // Touch difference threshold
    ft3267_write_byte(FT5x06_ID_G_TIME_ENTER_MONITOR, 2);   // Delay to enter 'Monitor' status (s)
    ft3267_write_byte(FT5x06_ID_G_PERIODACTIVE, 12);        // Period of 'Active' status (ms)
    ft3267_write_byte(FT5x06_ID_G_PERIODMONITOR, 40);       // Timer to enter 'idle' when in 'Monitor' (ms)
    return ESP_OK;
}

// /*EOF Externing C++*/
// }