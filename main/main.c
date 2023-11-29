#include <stdio.h>
#include "esp_log.h"

#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "http_server.h"
#include "adc.h"
#include "wifi_app.h"
#include "uart.h"
#include "rgb_led.h"
#include "button.h"

QueueHandle_t BPM_QUEUE;

QueueHandle_t UART_QUEUE;

void app_main(void)
{

    esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	BPM_QUEUE = xQueueCreate(10, sizeof(int));
	UART_QUEUE = xQueueCreate(10, sizeof(int));

	// Start Wifi
	wifi_app_start();
	// rgb_led_bpm_received();
	xTaskCreate(rgb_led_bpm, "rgb_led_bpm", 2048, NULL, 5, NULL);

    adc_config();
	init_uart();
	init_isr();
}   
