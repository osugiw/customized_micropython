#include "main.h"

// ESP-IDF APIs
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "soc/soc_caps.h"

// Other library
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ADC_DIVIDER                  8
#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_11

static int adc_raw[2][10];
static int voltage[2][10];

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
static bool init_adc_calibration(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        printf( "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        printf( "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        printf( " - Calibration Success\n");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        printf("- eFuse not burnt, skip software calibration\n");
    } else {
        printf("- Invalid arg or no memory\n");
    }
    return calibrated;
}

/**
*   @brief DeInitialize calibration
*   @param handle      ADC Calibration handle 
**/
static void uninit_adc_calibration(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    printf( "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    printf( "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
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

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = EXAMPLE_ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, channel_number, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_handle = NULL;
    bool do_calibration1 = init_adc_calibration(ADC_UNIT_1, EXAMPLE_ADC_ATTEN, &adc1_cali_handle);
    uint8_t readTemperature;

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel_number, &adc_raw[0][0]));
    readTemperature = temperature_conversion_ntc_5k(adc_raw[0][0]);
    // printf("ADC%d Channel[%d] - Temperature: %d, Raw Data: %d, ", ADC_UNIT_1 + 1, channel_number, readTemperature, adc_raw[0][0]);
    if (do_calibration1) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw[0][0], &voltage[0][0]));
        // printf(", Cali Voltage: %d mV\n", voltage[0][0]);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    return mp_obj_new_int(readTemperature);
}