#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Definições dos pinos
#define PIR_PIN GPIO_NUM_40  // Sensor PIR
#define LED_PIN GPIO_NUM_39  // LED indicador

// Fila para comunicação entre a interrupção e a tarefa
static QueueHandle_t pir_event_queue = NULL;

// Função de interrupção para o sensor PIR
static void IRAM_ATTR pir_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // Envia o número do pino para a fila
    xQueueSendFromISR(pir_event_queue, &gpio_num, &xHigherPriorityTaskWoken);

    // Se necessário, força uma troca de contexto
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// Tarefa para piscar o LED
static void led_task(void* arg)
{
    while (true) {
        uint32_t gpio_num;
        if (xQueueReceive(pir_event_queue, &gpio_num, portMAX_DELAY)) {
            printf("ALARME ACIONADO!\n");

            // Pisca o LED 50 vezes
            for (int i = 0; i < 30; i++) {
                gpio_set_level(LED_PIN, 1); // Liga o LED
                vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 100 ms
                gpio_set_level(LED_PIN, 0); // Desliga o LED
                vTaskDelay(pdMS_TO_TICKS(100)); // Delay de 100 ms
            }
        }
    }
}

void app_main()
{
    // Configura pino como entrada para o sensor PIR
    gpio_config_t pir_config = {
        .pin_bit_mask = (1ULL << PIR_PIN),  // Apenas o pino do sensor PIR
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLDOWN_ENABLE,  // Habilita pull-down
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,  // Interrupção na borda de subida
    };
    gpio_config(&pir_config);

    // Configura pino de saída para o LED
    gpio_config_t led_config = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&led_config);
    gpio_set_level(LED_PIN, 0); // Inicializa o LED desligado

    // Cria a fila para eventos de interrupção
    pir_event_queue = xQueueCreate(10, sizeof(uint32_t));

    // Instala o serviço de interrupção
    gpio_install_isr_service(0);

    // Adiciona o handler de interrupção para o sensor PIR
    gpio_isr_handler_add(PIR_PIN, pir_handler, (void*) PIR_PIN);

    // Cria a tarefa para piscar o LED
    xTaskCreate(led_task, "led_task", 4096, NULL, 10, NULL);

    printf("Sistema de detecção de movimento inicializado.\n");

    // Loop infinito
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Mantém o programa rodando
    }
}
