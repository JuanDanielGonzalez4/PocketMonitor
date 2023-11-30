#ifndef CONFIG_ISR
#define CONFIG_ISR

#include <stdio.h>
#include "driver/adc.h" //Librería para configurar y leer por medio de ADC
#include "esp_log.h"    //librería para poder imprimir texto con colores diferentes
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h" //Librería para las colas

#define button_isr 27 // GPIO del bottón
#define button_isr_b 34 // GPIO del bottón

static const char *tag_ISR = "ISR"; // Etiqueta para función ESP_LOG()

// Prototipado de funciones
void isr_handler(void *args);
void isr_handler_B(void *args);
esp_err_t init_isr(void);

#endif