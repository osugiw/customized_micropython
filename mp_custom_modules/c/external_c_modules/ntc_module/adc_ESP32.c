#include "main.h"

// ESP-IDF APIs
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

// Other library
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ADC_DIVIDER                  8
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_VREF
#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_11

static int adc_raw[2][10];
static esp_adc_cal_characteristics_t adc1_chars;

// NTC Threshold after Divided
enum{
    ADC_MIN40		= 483,
    ADC_MIN20       = 424,
	ADC_0			= 316,
	ADC_PLUS20		= 195,
	ADC_PLUS40		= 107,
	ADC_PLUS60		= 56,
	ADC_PLUS80		= 30,
	ADC_PLUS100		= 17,
	ADC_PLUS120		= 10,
	ADC_PLUS150		= 5,
};

/**
*   @brief Initialize calibration on ADC pin
*   @param adc_unit        ADC Channel Number
*   @param atten           Level of attenuation on target adc_unit
*   @param out_handle      ADC Calibration handle 
**/
static bool init_adc_calibration(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        printf("Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        printf("eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, EXAMPLE_ADC_ATTEN, ADC_BITWIDTH_12, 0, &adc1_chars);
    } else {
        printf("Invalid arg");
    }

    return cali_enable;
}

/**
*   @brief Convert raw value to temperature (Celsius)
*   @param raw_data     ADC Raw data
**/
int8_t temperature_conversion_ntc_5k(uint16_t raw_data)
{
    uint16_t divided = raw_data / ADC_DIVIDER;
    uint8_t _celsius = 0;

    if(divided <= ADC_MIN40 && divided >= ADC_MIN20)
		_celsius =  ((-0.3336 * divided) + 122.61);
	else if(divided < ADC_MIN20 && divided >= ADC_0)
		_celsius = ((-0.1825 * divided) + 58.187);
	else if(divided < ADC_0 && divided >= ADC_PLUS20)
		_celsius = ((-0.1644 * divided) + 51.868);
	else if(divided < ADC_PLUS20 && divided >= ADC_PLUS40)
		_celsius = ((-0.2274 * divided) + 63.628);
    else if(divided < ADC_PLUS40 && divided >= ADC_PLUS60)
		_celsius = ((-0.3992 * divided) + 81.552);
    else if(divided < ADC_PLUS60 && divided >= ADC_PLUS80)
		_celsius = ((-0.7771 * divided) + 102.49);
    else if(divided < ADC_PLUS80 && divided >= ADC_PLUS100)
		_celsius = ((-1.5101 * divided) + 124.3);
    else if(divided < ADC_PLUS100 && divided >= ADC_PLUS120)
		_celsius = ((-2.9107 * divided) + 147.44);
	else if(divided < ADC_PLUS120 && divided >= ADC_PLUS150)
		_celsius = ((0.5588 * pow(divided, 2)) - (14.089 * divided) + 204.67);
	return _celsius;
}


/**
*   @brief Read and inialize data from ADC Pin
*   @param channel_number        ADC Channel Number
**/
mp_obj_t ntc_5k(mp_obj_t _channel_number_obj) {
    mp_int_t _channel_number = mp_obj_get_int(_channel_number_obj);
    uint8_t channel_number = (uint8_t)_channel_number;

    uint32_t voltage = 0;
    bool cali_enable = init_adc_calibration();
    uint8_t readTemperature;

    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(ADC_BITWIDTH_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(channel_number, EXAMPLE_ADC_ATTEN));

    adc_raw[0][0] = adc1_get_raw(channel_number);
    readTemperature = temperature_conversion_ntc_5k(adc_raw[0][0]);
    // printf("ADC%d Channel[%d] - Temperature: %d, Raw Data: %d, ", ADC_UNIT_1 + 1, channel_number, readTemperature, adc_raw[0][0]);
    if (cali_enable) {
        voltage = esp_adc_cal_raw_to_voltage(adc_raw[0][0], &adc1_chars);
        // printf(" cali data: %ld mV\n", voltage);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    return mp_obj_new_int(readTemperature);
}