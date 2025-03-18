#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_err.h"
#include "driver/gpio.h"

// Configuração dos pinos (ADC2)
#define JOYSTICK_X_PIN 26  // ADC2_CH9
#define JOYSTICK_Y_PIN 27  // ADC2_CH7

// Pinos dos LEDs conforme sua tabela
#define LED0_PIN 23
#define LED1_PIN 22
#define LED2_PIN 21
#define LED3_PIN 19
#define LED4_PIN 18
#define LED5_PIN 5
#define LED6_PIN 17
#define LED7_PIN 16
#define LED8_PIN 4
#define LED9_PIN 0

// Configurar pinos dos LEDs como saída
void configure_leds() {
    gpio_set_direction(LED0_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED3_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED4_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED5_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED6_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED7_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED8_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED9_PIN, GPIO_MODE_OUTPUT);
}

// Função para acionar os LEDs de acordo com o índice
void set_leds(int led_level) {
    gpio_set_level(LED9_PIN, led_level >= 1);
    gpio_set_level(LED8_PIN, led_level >= 2);
    gpio_set_level(LED7_PIN, led_level >= 3);
    gpio_set_level(LED6_PIN, led_level >= 4);
    gpio_set_level(LED5_PIN, led_level >= 5);
    gpio_set_level(LED4_PIN, led_level >= 6);
    gpio_set_level(LED3_PIN, led_level >= 7);
    gpio_set_level(LED2_PIN, led_level >= 8);
    gpio_set_level(LED1_PIN, led_level >= 9);
    gpio_set_level(LED0_PIN, led_level >= 10);
}

void app_main(void) {
    // Configuração do ADC para os eixos X e Y do joystick
    adc2_config_channel_atten(ADC2_CHANNEL_9, ADC_ATTEN_DB_11);  // X (GPIO26)
    adc2_config_channel_atten(ADC2_CHANNEL_7, ADC_ATTEN_DB_11);  // Y (GPIO27)

    int x_raw, y_raw;
    esp_err_t ret;

    // Configuração dos LEDs
    configure_leds();

    int led_level = 0; // Nível de LEDs acesos (0 a 10)
    
    while (1) {
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

        // Determinar direção do joystick e atualizar LEDs
        if (x_raw < 1200) {
            printf("Direita\n");
            if (led_level < 10) led_level++;  // Incrementa o nível dos LEDs
        } else if (x_raw > 2800) {
            printf("Esquerda\n");
            if (led_level > 0) led_level--;  // Decrementa o nível dos LEDs
        } else if (y_raw < 1200) {
            printf("Baixo\n");
            if (led_level > 0) led_level--;  // Decrementa o nível dos LEDs
        } else if (y_raw > 2800) {
            printf("Cima\n");
            if (led_level < 10) led_level++;  // Incrementa o nível dos LEDs
        } else {
            printf("Centro\n");
        }

        // Acionar LEDs conforme o nível
        set_leds(led_level);

        vTaskDelay(100 / portTICK_PERIOD_MS); // Delay de 100ms para detectar o movimento
    }
}