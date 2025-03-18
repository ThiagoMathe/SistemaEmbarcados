#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"

// Definições de constantes
#define UART_NUM UART_NUM_0       // Número da porta UART
#define BUF_SIZE (1024)           // Tamanho do buffer de leitura da UART
#define LED_RED_PIN 18            // Pino do LED vermelho
#define LED_GREEN_PIN 5           // Pino do LED verde
#define LED_BLUE_PIN 17           // Pino do LED azul
#define BUZER_PIN 4               // Pino do buzzer

// Função para inicializar a UART
void uart_init() {
  const uart_config_t uart_config = {
      .baud_rate = 115200,          // Taxa de transmissão
      .data_bits = UART_DATA_8_BITS, // 8 bits de dados
      .parity = UART_PARITY_DISABLE, // Sem paridade
      .stop_bits = UART_STOP_BITS_1, // 1 bit de parada
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE // Sem controle de fluxo
  };

  uart_param_config(UART_NUM, &uart_config); // Aplica a configuração
  uart_driver_install(UART_NUM, BUF_SIZE, 0, 0, NULL, 0); // Instala o driver UART
}

// Função para inicializar os LEDs RGB
void rgb_init() {
  gpio_set_direction(LED_RED_PIN, GPIO_MODE_OUTPUT);   // Configura o pino do LED vermelho como saída
  gpio_set_direction(LED_GREEN_PIN, GPIO_MODE_OUTPUT); // Configura o pino do LED verde como saída
  gpio_set_direction(LED_BLUE_PIN, GPIO_MODE_OUTPUT);  // Configura o pino do LED azul como saída
  gpio_set_level(LED_RED_PIN, 0);                      // Desliga o LED vermelho
  gpio_set_level(LED_GREEN_PIN, 0);                    // Desliga o LED verde
  gpio_set_level(LED_BLUE_PIN, 0);                     // Desliga o LED azul
}

// Função para desligar todos os LEDs
void set_leds() {
    gpio_set_level(LED_RED_PIN, 0); // Desliga o LED vermelho
    gpio_set_level(LED_BLUE_PIN, 0); // Desliga o LED azul
    gpio_set_level(LED_GREEN_PIN, 0); // Desliga o LED verde
}

// Definições para o ADC
#define ADC_1 1
#define ADC_2 2

// Estrutura para armazenar as configurações do NTC
typedef struct {
  uint8_t adc_device;   // Dispositivo ADC (ADC_1 ou ADC_2)
  uint8_t adc_channel;  // Canal do ADC
  float beta;           // Coeficiente beta do NTC
  float ratio;          // Razão entre a resistência do NTC a 25°C e a resistência em série
} ntc_handle_t;

// Função para obter a temperatura em Kelvin
float ntc_get_kelvin(ntc_handle_t * ntc) {
  int buffer;

  // Verifica qual ADC está sendo usado e lê o valor bruto do canal especificado
  if (ntc->adc_device == ADC_2) {
    adc2_get_raw(ntc->adc_channel, ADC_WIDTH_BIT_12, &buffer);
  } else {
    buffer = adc1_get_raw(ntc->adc_channel);
  }

  return 1.0 / (1.0 / 298.15 - log(ntc->ratio * (4096.0 / buffer - 1.0)) / ntc->beta);
}

float ntc_get_celcius(ntc_handle_t * ntc) {
  return ntc_get_kelvin(ntc) - 273.15; // Converte Kelvin para Celsius
}

void app_main() {
  // Configuração do NTC
  ntc_handle_t ntc = {
    .adc_device = ADC_1,        // Usando ADC1
    .adc_channel = ADC_CHANNEL_7, // Canal 7 do ADC1
    .beta = 3950.0,             // Coeficiente beta do NTC
    .ratio = 1.0                // Razão entre a resistência do NTC a 25°C e a resistência em série
  };

  // Configura o pino do buzzer como saída
  gpio_set_direction(BUZER_PIN, GPIO_MODE_OUTPUT);
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

  // Inicializa a UART e os LEDs RGB
  uart_init();
  rgb_init();
  set_leds();

  uint8_t data[BUF_SIZE];
  float temp;

  while (1) {
    temp = ntc_get_celcius(&ntc);
    printf("Temperatura: %.1f°C\n", temp); 
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Controle dos LEDs e buzzer com base na temperatura
    if (temp < 30) {
      // Temperatura abaixo de 30°C: LED verde ligado
      gpio_set_level(LED_GREEN_PIN, 1);
      gpio_set_level(LED_RED_PIN, 0);
      gpio_set_level(LED_BLUE_PIN, 0);
    } else if (temp >= 30 && temp <= 40) {
      // Temperatura entre 30°C e 40°C: LEDs verde e vermelho ligados
      gpio_set_level(LED_GREEN_PIN, 1);
      gpio_set_level(LED_RED_PIN, 1);
      gpio_set_level(LED_BLUE_PIN, 0);
    } else {
      // Temperatura acima de 40°C: LED vermelho ligado e buzzer ativado
      gpio_set_level(LED_GREEN_PIN, 0);
      gpio_set_level(LED_RED_PIN, 1);
      gpio_set_level(LED_BLUE_PIN, 0);

      // Ativa o buzzer por 500 ms
      gpio_set_level(BUZER_PIN, 1);
      vTaskDelay(pdMS_TO_TICKS(500));
      gpio_set_level(BUZER_PIN, 0);
    }
  }
}