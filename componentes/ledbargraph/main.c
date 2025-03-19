#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_COUNT 10  // Número de LEDs na barra
int led_pins[LED_COUNT] = {1, 2, 42, 41, 40, 39, 38, 37, 36, 35};  // GPIOs usados

void app_main() {
    // Configura os pinos como saída
    for (int i = 0; i < LED_COUNT; i++) {
        gpio_reset_pin(led_pins[i]);
        gpio_set_direction(led_pins[i], GPIO_MODE_OUTPUT);
    }

    while (1) {
        // Acende os LEDs progressivamente
        for (int i = 0; i < LED_COUNT; i++) {
            gpio_set_level(led_pins[i], 1);
            vTaskDelay(pdMS_TO_TICKS(200));  // Espera 200ms
        }
        // Apaga todos os LEDs
        for (int i = 0; i < LED_COUNT; i++) {
            gpio_set_level(led_pins[i], 0);
        }
        vTaskDelay(pdMS_TO_TICKS(500));  // Espera 500ms antes de repetir
    }
}
