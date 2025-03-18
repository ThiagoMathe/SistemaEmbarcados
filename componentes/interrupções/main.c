#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define LED_GPIO_NUM      GPIO_NUM_2
#define BUTTON_GPIO_NUM   GPIO_NUM_13
#define DEBOUNCE_DELAY    200  // Delay para evitar múltiplos triggers (debouncing)

static QueueHandle_t gpio_queue = NULL;  // Fila para comunicação entre a ISR e a task principal

// Função de interrupção do botão
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Envia o número do GPIO para a fila dentro da ISR
    xQueueSendFromISR(gpio_queue, &gpio_num, &xHigherPriorityTaskWoken);

    // Se uma task de maior prioridade foi despertada, notifica o scheduler para execução imediata
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Configura o botão com interrupção de borda positiva
void button_config() {
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);  // Instala serviço de interrupção
    gpio_reset_pin(BUTTON_GPIO_NUM);
    gpio_set_direction(BUTTON_GPIO_NUM, GPIO_MODE_INPUT);
    gpio_pullup_en(BUTTON_GPIO_NUM);  // Ativa o pull-up interno
    gpio_set_intr_type(BUTTON_GPIO_NUM, GPIO_INTR_POSEDGE);  // Interrupção na borda de subida
    gpio_isr_handler_add(BUTTON_GPIO_NUM, gpio_isr_handler, (void*) BUTTON_GPIO_NUM);  // Adiciona o handler de interrupção
}

// Configura o LED para saída
void led_config() {
    gpio_reset_pin(LED_GPIO_NUM);
    gpio_set_direction(LED_GPIO_NUM, GPIO_MODE_OUTPUT);
}

// Task principal para controlar o LED com base na interrupção
void led_task(void* arg) {
    uint8_t led_state = 0;  // Estado do LED (OFF: 0, ON: 1)
    uint32_t gpio_num;

    while (1) {
        // Aguarda uma mensagem na fila (quando a interrupção é acionada)
        if (xQueueReceive(gpio_queue, &gpio_num, portMAX_DELAY)) {
            // Alterna o estado do LED quando o botão for pressionado
            led_state = !led_state;
            gpio_set_level(LED_GPIO_NUM, led_state);
            printf("Botão pressionado! LED: %s\n", led_state ? "ON" : "OFF");
        }
    }
}

void app_main() {
    // Inicializa o sistema de GPIO
    button_config();
    led_config();

    // Cria a fila para comunicar a ISR com a task principal
    gpio_queue = xQueueCreate(10, sizeof(uint32_t));  // Fila para até 10 interrupções
    
    if (gpio_queue != NULL) {
        // Cria a task para controle do LED
        xTaskCreate(led_task, "led_task", 2048, NULL, 10, NULL);
    } else {
        printf("Erro ao criar a fila.\n");
    }
}
