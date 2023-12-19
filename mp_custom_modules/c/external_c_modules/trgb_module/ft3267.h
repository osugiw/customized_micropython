/**
 * @file ft3267.h
 * @brief ft3267 driver header file.
 * @version 0.1
 * @date 2021-03-07
 *
 * @copyright Copyright 2021 Espressif Systems (Shanghai) Co. Ltd.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#pragma once

#include "main.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @brief FT5x06 register map and function codes */
#define FT5x06_ADDR                    (0x38)

#define FT5x06_DEVICE_MODE             (0x00)
#define FT5x06_GESTURE_ID              (0x01)
#define FT5x06_TOUCH_POINTS            (0x02)

#define FT5x06_TOUCH1_EV_FLAG          (0x03)
#define FT5x06_TOUCH1_XH               (0x03)
#define FT5x06_TOUCH1_XL               (0x04)
#define FT5x06_TOUCH1_YH               (0x05)
#define FT5x06_TOUCH1_YL               (0x06)

#define FT5x06_TOUCH2_EV_FLAG          (0x09)
#define FT5x06_TOUCH2_XH               (0x09)
#define FT5x06_TOUCH2_XL               (0x0A)
#define FT5x06_TOUCH2_YH               (0x0B)
#define FT5x06_TOUCH2_YL               (0x0C)

#define FT5x06_TOUCH3_EV_FLAG          (0x0F)
#define FT5x06_TOUCH3_XH               (0x0F)
#define FT5x06_TOUCH3_XL               (0x10)
#define FT5x06_TOUCH3_YH               (0x11)
#define FT5x06_TOUCH3_YL               (0x12)

#define FT5x06_TOUCH4_EV_FLAG          (0x15)
#define FT5x06_TOUCH4_XH               (0x15)
#define FT5x06_TOUCH4_XL               (0x16)
#define FT5x06_TOUCH4_YH               (0x17)
#define FT5x06_TOUCH4_YL               (0x18)

#define FT5x06_TOUCH5_EV_FLAG          (0x1B)
#define FT5x06_TOUCH5_XH               (0x1B)
#define FT5x06_TOUCH5_XL               (0x1C)
#define FT5x06_TOUCH5_YH               (0x1D)
#define FT5x06_TOUCH5_YL               (0x1E)

#define FT5x06_ID_G_THGROUP            (0x80)
#define FT5x06_ID_G_THPEAK             (0x81)
#define FT5x06_ID_G_THCAL              (0x82)
#define FT5x06_ID_G_THWATER            (0x83)
#define FT5x06_ID_G_THTEMP             (0x84)
#define FT5x06_ID_G_THDIFF             (0x85)
#define FT5x06_ID_G_CTRL               (0x86)
#define FT5x06_ID_G_TIME_ENTER_MONITOR (0x87)
#define FT5x06_ID_G_PERIODACTIVE       (0x88)
#define FT5x06_ID_G_PERIODMONITOR      (0x89)
#define FT5x06_ID_G_AUTO_CLB_MODE      (0xA0)
#define FT5x06_ID_G_LIB_VERSION_H      (0xA1)
#define FT5x06_ID_G_LIB_VERSION_L      (0xA2)
#define FT5x06_ID_G_CIPHER             (0xA3)
#define FT5x06_ID_G_MODE               (0xA4)
#define FT5x06_ID_G_PMODE              (0xA5)
#define FT5x06_ID_G_FIRMID             (0xA6)
#define FT5x06_ID_G_STATE              (0xA7)
#define FT5x06_ID_G_FT5201ID           (0xA8)
#define FT5x06_ID_G_ERR                (0xA9)

// ESP-IDF I2C
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

typedef enum {
  ft3267_gesture_none       = 0x00,
  ft3267_gesture_move_up    = 0x10,
  ft3267_gesture_move_left  = 0x14,
  ft3267_gesture_move_down  = 0x18,
  ft3267_gesture_move_right = 0x1c,
  ft3267_gesture_zoom_in    = 0x48,
  ft3267_gesture_zoom_out   = 0x49,
} ft3267_gesture_t;

/**
 * @brief Init ft3267 series touch panel
 *
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t ft3267_init(i2c_cmd_handle_t i2c_handle);

/**
 * @brief Read touch point from ft3267
 *
 * @param touch_points_num Touch point number
 * @param x X coordinate
 * @param y Y coordinate
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t ft3267_read_pos(uint8_t *touch_points_num, uint16_t *x, uint16_t *y);

/**
 * @brief Read guesture from ft3267
 *
 * @param gesture Gesture read from ft3267
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t fx5x06_read_gesture(ft3267_gesture_t *gesture);

#ifdef __cplusplus
}
#endif