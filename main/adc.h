#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "math.h"
#include "rgb_led.h"

#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_11
#define EXAMPLE_ADC1_CHAN0 ADC_CHANNEL_4
#define EXAMPLE_ADC1_CHAN1 ADC_CHANNEL_5

#define DELAY 300
#define DELAY_TEMP 1000

#define VOLTAGE_REFERENCE 3300
#define VOLTAGE_REFERENCE_TEMP 5000
#define ADC_MAX_VALUE 4096

#define CELCIUS_RATE 10

#define BPM_TRESHOLD 3000
#define BPM_ARRAY_SIZE 500
void adc_config(void);

void adc_read_task();

void adc_temp_read_task();
