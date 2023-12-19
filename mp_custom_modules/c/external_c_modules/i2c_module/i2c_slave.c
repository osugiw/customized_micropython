// Include Header
#include <i2c_module.h>

/**
*   @brief Iniialize I2C Slave
*   @param _sda_obj     Slave SDA Pin
*   @param _scl_obj     Slave SCL Pin 
*   @param _freq_obj    I2C Frequency 
**/
mp_obj_t slave_init(mp_obj_t _sda_obj, mp_obj_t _scl_obj, mp_obj_t _slaveAddr_obj) {
    mp_int_t _sda = mp_obj_get_int(_sda_obj);
    mp_int_t _scl = mp_obj_get_int(_scl_obj);
    mp_int_t _slaveAddr = mp_obj_get_int(_slaveAddr_obj);

    // I2C Port Number
    int i2c_slave_port = 0;   

    // I2C Configuration
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = _sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = _scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = _slaveAddr,
    };

    // Error checking on the I2C Configuration
    esp_err_t err = i2c_param_config(i2c_slave_port, &conf);
    if(err != ESP_OK){
        printf("Error slave configuration - Error Code %d\n", err);
        return mp_obj_new_int(err);
    }

    return mp_obj_new_int(i2c_driver_install(i2c_slave_port, conf.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0));
}

/**
*   @brief I2C Slave Read
**/
mp_obj_t slave_read() {
    uint8_t *data = (uint8_t *)malloc(DATA_LENGTH);
    int size = i2c_slave_read_buffer(0, data, RW_TEST_LENGTH, 1000 / portTICK_PERIOD_MS);

    // Print recieved data
    for(int i=0; i<size; i++){
        printf("%02x ", data[i]);
        if((i+1) % 16 == 0){
            printf("\n");
        }
    }
    printf("\n");
    return mp_const_none;
}


