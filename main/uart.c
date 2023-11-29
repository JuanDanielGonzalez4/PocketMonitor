#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "freertos/task.h"
#include "adc.h"



static const char *mensaje_1  = "¡usted se encuentra bien de salud!";
static const char *mensaje_2  = "¡Le recomendamos visitar su medico de confianza!";
static const char *mensaje_3 = "¡Los datos ingresados son invalidos!";


extern QueueHandle_t BPM_QUEUE;
extern QueueHandle_t UART_QUEUE;

void read_uart_task(void *pvParameters) {
    const int uart_buffer_size = 10;
    uint8_t data[uart_buffer_size];

    int BPM = 0;

    while (1) {
        int value = uart_read_bytes(UART_PORT, data, uart_buffer_size, 20 / portTICK_PERIOD_MS);

        if (value > 0) {
            data[value] = '\0'; // Ensure null-termination
            int age = atoi((char *)data);
            printf("Integer value: %d\n", age);

            if (xQueueReceive(BPM_QUEUE, &BPM, portMAX_DELAY) == pdPASS){
                if ((age>=3 &&  age<=4) && (BPM >=80 && BPM<=120)){
                    uart_write_bytes(UART_PORT, mensaje_1,strlen(mensaje_1));
                }
            else{
                uart_write_bytes(UART_PORT,mensaje_2,strlen(mensaje_2));
                }     
            }
        }

        // Delay to prevent constant CPU occupation
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void init_uart(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    uart_driver_install(UART_PORT, 1024 * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    xTaskCreate(read_uart_task, "read_uart_task", 2048, NULL, 5, NULL);
}
