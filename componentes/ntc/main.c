#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/i2c.h"

// Definição dos ADCs disponíveis
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

  // Calcula a temperatura em Kelvin usando a equação do NTC
  return 1.0 / (1.0 / 298.15 - log(ntc->ratio * (4096.0 / buffer - 1.0)) / ntc->beta);
}

// Função para obter a temperatura em Celsius
float ntc_get_celcius(ntc_handle_t * ntc) {
  return ntc_get_kelvin(ntc) - 273.15;  // Converte Kelvin para Celsius
}

// Função para obter a temperatura em Fahrenheit
float ntc_get_fahrenheit(ntc_handle_t * ntc) {
  return (ntc_get_kelvin(ntc) - 273.15) * 1.8 + 32;  // Converte Kelvin para Fahrenheit
}

// Função principal do programa
void app_main() {
  // Configuração do NTC
  ntc_handle_t ntc = {
    .adc_device = ADC_1,        // Usando ADC1
    .adc_channel = ADC_CHANNEL_7, // Canal 7 do ADC1
    .beta = 3950.0,             // Coeficiente beta do NTC
    .ratio = 1.0                // Razão entre a resistência do NTC a 25°C e a resistência em série
  };

  // Configuração do ADC1 para 12 bits de resolução e atenuação de 11 dB
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

  // Loop principal
  while (1) {
    // Exibe a temperatura em Celsius no console
    printf("Temperatura: %.1f°C\n", ntc_get_celcius(&ntc));
    printf("--------------------\n");
    
    // Aguarda 1 segundo antes de realizar a próxima leitura
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}