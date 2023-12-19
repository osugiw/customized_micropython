// Include Header
#include <i2c_module.h>

/**
*   @brief Iniialize I2C Master
*   @param  _sda_obj     Master SDA Pin
*   @param  _scl_obj     Master SCL Pin 
*   @param  _freq_obj    I2C Frequency 
**/
mp_obj_t master_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _freq_obj) {
    mp_int_t _sda = mp_obj_get_int(_sda_obj);
    mp_int_t _scl = mp_obj_get_int(_scl_obj);
    mp_int_t _freq = mp_obj_get_int(_freq_obj);

    // I2C Port Number
    int i2c_master_port = 1;    //  0(ESP32C3, ESP32C2, ESP32H2) , 1(ESP32/ESP32S2/ESP32S3)

    // I2C Configuration
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = _sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = _scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = _freq,
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };

    // Error checking on the I2C Configuration
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if(err != ESP_OK){
        printf("Error master configuration - Error Code %d\n", err);
        return mp_obj_new_int(err);
    }
    return mp_obj_new_int(i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
}

/**
*   @brief I2C Master Write
*   @param  _i2c_port_num_obj        I2C Port Number
*   @param  _data_obj                Data to write 
*   @param  _size_obj                Data Length 
**/
mp_obj_t master_write(mp_obj_t _slave_addr_obj, mp_obj_t _data_obj){
    mp_int_t _slave_addr = mp_obj_get_int(_slave_addr_obj);
    mp_obj_t *_data = NULL;
    size_t _data_len = 0;
    mp_obj_get_array(_data_obj, &_data_len, &_data);

    // Save the received data into the buffer
    uint8_t* data_wr = (uint8_t *) malloc(DATA_LENGTH);
    for(int i=0; i<_data_len; i++){
        data_wr[i] = mp_obj_get_int(_data[i]);
    }

    // Your code here!
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();                               // Create and initialize an I2C commands list with a given buffer.
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_slave_addr << 1) | WRITE_BIT, ACK_CHECK_EN);   // Send a byte to slave
    i2c_master_write(cmd, data_wr, _data_len, ACK_CHECK_EN);                    // Queue data
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(1, cmd, 1000 / portTICK_PERIOD_MS);    // Send All queued commands
    i2c_cmd_link_delete(cmd);                                                   // Finished I2C Transaction

    return mp_obj_new_int(err);
}



