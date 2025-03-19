#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_err.h"

// Configuração dos pinos (ADC2)
#define JOYSTICK_X_PIN 26  // ADC2_CH9
#define JOYSTICK_Y_PIN 27  // ADC2_CH7

void app_main() {
    // Configuração do ADC
    adc2_config_channel_atten(ADC2_CHANNEL_9, ADC_ATTEN_DB_11);  // X (GPIO26)
    adc2_config_channel_atten(ADC2_CHANNEL_7, ADC_ATTEN_DB_11);  // Y (GPIO27)

    int x_raw, y_raw;
    esp_err_t ret;

    while(1) {
        // Leitura do eixo X (GPIO26 - ADC2_CH9)
        ret = adc2_get_raw(ADC2_CHANNEL_9, ADC_WIDTH_BIT_12, &x_raw);
        if (ret != ESP_OK) {
            printf("Erro na leitura do X! Código: %d\n", ret);
            continue;
        }

        // Leitura do eixo Y (GPIO27 - ADC2_CH7)
        ret = adc2_get_raw(ADC2_CHANNEL_7, ADC_WIDTH_BIT_12, &y_raw);
        if (ret != ESP_OK) {
            printf("Erro na leitura do Y! Código: %d\n", ret);
            continue;
        }

        // Exibir valores brutos
        printf("X: %d, Y: %d -> ", x_raw, y_raw);

        // Determinar direção do joystick
        if (x_raw < 1200) {
            printf("Direita\n");
        } else if (x_raw > 2800) {
            printf("Esquerda\n");
        } else if (y_raw < 1200) {
            printf("Baixo\n");
        } else if (y_raw > 2800) {
            printf("Cima\n");
        } else {
            printf("Centro\n");
        }

        vTaskDelay(pdMS_TO_TICKS(500)); // Delay de 500ms
    }
}