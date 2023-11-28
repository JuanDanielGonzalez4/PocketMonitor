#include "adc.h"

adc_oneshot_unit_handle_t adc1_handle;

QueueHandle_t ADC_QUEUE;

QueueHandle_t BPM_QUEUE;


static int64_t beat_old = 0;
static float beats[BPM_ARRAY_SIZE];
static int beatIndex = 0;
static int BPM = 0;

void adc_config(void)
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = EXAMPLE_ADC_ATTEN,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));

    // Create a queue to handle ADC data
    ADC_QUEUE = xQueueCreate(10, sizeof(double));

    // Create a task to read ADC
    xTaskCreate(adc_read_task, "adc_read_task", 2048, NULL, 5, NULL);
}

void calculate_bpm(){

    BPM_QUEUE = xQueueCreate(10, sizeof(double));

    int64_t beat_new = esp_timer_get_time() / 1000; // Convert to milliseconds
    int64_t diff = beat_new - beat_old;
    if (diff > 0) { // Prevent division by zero
        float currentBPM = 60000.0 / diff; // Convert to BPM
        beats[beatIndex] = currentBPM;

        float total = 0.0;
        for (int i = 0; i < BPM_ARRAY_SIZE; i++) {
            total += beats[i];
        }
        BPM = (int)(total / BPM_ARRAY_SIZE);

        printf("BPM: %d\n", BPM);
        
        xQueueSend(BPM_QUEUE, &BPM, portMAX_DELAY);

        beat_old = beat_new;
        beatIndex = (beatIndex + 1) % BPM_ARRAY_SIZE;
    }
}
void adc_read_task(void *pvParameters)
{
    int data;
    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_4, &data));

        printf("Value adc: %d\n", data);
        double voltage_adc = ((double)data / ADC_MAX_VALUE) * VOLTAGE_REFERENCE;

        if (voltage_adc>BPM_TRESHOLD){
            calculate_bpm();
        } 

        printf("Voltage adc: %f\n", voltage_adc);

        xQueueSend(ADC_QUEUE, &voltage_adc, portMAX_DELAY);
        // if (xQueueReceive(ADC_QUEUE, &voltage_adc, portMAX_DELAY))
        // {
        //     printf("Received: %f\n", voltage_adc);
        // }
        vTaskDelay(pdMS_TO_TICKS(DELAY));
    }
}