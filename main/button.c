#include "button.h"

QueueHandle_t Button_queue_A = 0;
QueueHandle_t Button_queue_B = 0;

esp_err_t init_isr(void) // Configuración e inicialización de las interrupciones
{
    gpio_config_t pGPIOConfig; // Prototipado de variable de tipo gpio_config_t (lo exigue la librería) utilizada para la posterior configuración
    pGPIOConfig.pin_bit_mask = (1ULL << button_isr);
    pGPIOConfig.mode = GPIO_MODE_INPUT;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.intr_type = GPIO_INTR_NEGEDGE;

    gpio_config(&pGPIOConfig); // Se carga a la función gpio_config(), la variable pGPIOConfig que fue prototipada anterioremente

    gpio_config_t pGPIOConfig_B; // Prototipado de variable de tipo gpio_config_t (lo exigue la librería) utilizada para la posterior configuración
    pGPIOConfig_B.pin_bit_mask = (1ULL << button_isr_b);
    pGPIOConfig_B.mode = GPIO_MODE_INPUT;
    pGPIOConfig_B.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig_B.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig_B.intr_type = GPIO_INTR_NEGEDGE;

    gpio_config(&pGPIOConfig_B); // Se carga a la función gpio_config(), la variable pGPIOConfig que fue prototipada anterioremente

    gpio_install_isr_service(0);                         // Se inicia la interrupción 0
    gpio_isr_handler_add(button_isr, isr_handler, NULL); // Se define cómo va a trabajar la interrupción
    // gpio_isr_handler_add(pin a utilizar, función que va a ejecutar, NULL)

    gpio_isr_handler_add(button_isr_b, isr_handler_B, NULL); // Se define cómo va a trabajar la interrupción
    // gpio_isr_handler_add(pin a utilizar, función que va a ejecutar, NULL)

    Button_queue_A = xQueueCreate(1, sizeof(int));
    Button_queue_B = xQueueCreate(1, sizeof(int));

    ESP_LOGI(tag_ISR, "Init isr completed");
    return ESP_OK;
}

void isr_handler(void *args)
{
    static int count_button = 0; // Se inicializa la variable en 1000 ya que el mínimo es 1seg

    count_button++;
    xQueueOverwrite(Button_queue_A, &count_button); // Este valor se almacena en la cola Button_queue_A para ser utilizado en las tareas
    
}
void isr_handler_B(void *args)
{
    static int count_buttonB = 0; // Se inicializa la variable en 1000 ya que el mínimo es 1seg

    count_buttonB++;
    xQueueOverwrite(Button_queue_B, &count_buttonB); // Este valor se almacena en la cola Button_queue_A para ser utilizado en las tareas

}