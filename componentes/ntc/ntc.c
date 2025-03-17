#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/i2c.h"

// INÍCIO DA BIBLIOTECA DO NTC -------------------------------------------

#define ADC_1 1
#define ADC_2 2

typedef struct {
  uint8_t adc_device;
  uint8_t adc_channel;
  float beta;
  float ratio; // Resistência do NTC a 25°C dividido pela resistência em série.
} ntc_handle_t;

float ntc_get_kelvin(ntc_handle_t * ntc) {
  int buffer;

  if (ntc->adc_device == ADC_2) {
    adc2_get_raw(ntc->adc_channel, ADC_WIDTH_BIT_12, &buffer);
  }

  else {
    buffer = adc1_get_raw(ntc->adc_channel);
  }

  return 1.0 / (1.0 / 298.15 - log(ntc->ratio * (4096.0 / buffer - 1.0)) / ntc->beta);
}

float ntc_get_celcius(ntc_handle_t * ntc) {
  return ntc_get_kelvin(ntc) - 273.15;
}

float ntc_get_fahrenheit(ntc_handle_t * ntc) {
  return (ntc_get_kelvin(ntc) - 273.15) * 1.8 + 32;
}

// FIM DA BIBLIOTECA DO NTC ----------------------------------------------

void app_main() {
  ntc_handle_t ntc = {
    .adc_device = ADC_1,
    .adc_channel = ADC_CHANNEL_7,
    .beta = 3950.0,
    .ratio = 1.0
  };

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);

  while (1) {
    printf("Temperatura: %.1f°C\n", ntc_get_celcius(&ntc));
    printf("--------------------\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
