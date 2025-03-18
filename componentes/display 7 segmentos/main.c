#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define L0 GPIO_NUM_17 //A
#define L1 GPIO_NUM_16 //B
#define L2 GPIO_NUM_23 //C
#define L3 GPIO_NUM_5  //D
#define L4 GPIO_NUM_18 //E
#define L5 GPIO_NUM_19 //F
#define L6 GPIO_NUM_21 //G


#define GPIO_OUTPUT ((1ULL << L0)|(1ULL << L1)|(1ULL << L2)|(1ULL << L3)|(1ULL << L4)|(1ULL << L5)|(1ULL << L6))

//mascara de bits utilizada
const uint8_t mask[16] = 
{ 
0b00111111, //0
0b00000110, //1
0b01011011, //2
0b01001111, //3
0b01100110, //4
0b01101101, //5
0b01111101, //6
0b00000111, //7
0b01111111, //8
0b01101111, //9
}; 


//prototypes functions
void config_pins();
void mostrar_digit(int i);

void app_main(void){
    config_pins();
    while(1){
        for(int i = 0; i < 10;i++){
            mostrar_digit(i);
        }
    }
}

void mostrar_digit(int i){
    //tecnica do mascaramento de bits para display
    gpio_set_level(L0, ~(mask[i] >> 0) & 1); 
    gpio_set_level(L1, ~(mask[i] >> 1) & 1);
    gpio_set_level(L2, ~(mask[i] >> 2) & 1);
    gpio_set_level(L3, ~(mask[i] >> 3) & 1);
    gpio_set_level(L4, ~(mask[i] >> 4) & 1);
    gpio_set_level(L5, ~(mask[i] >> 5) & 1);
    gpio_set_level(L6, ~(mask[i] >> 6) & 1);
    vTaskDelay(1000/portTICK_PERIOD_MS);
}


void config_pins(){
    //configurando pinos para segmentos do display
    gpio_config_t led_conf = {};
    led_conf.intr_type = GPIO_INTR_DISABLE;
    led_conf.mode = GPIO_MODE_OUTPUT;
    led_conf.pin_bit_mask = GPIO_OUTPUT;
    led_conf.pull_down_en = 0;
    led_conf.pull_up_en = 0;
    gpio_config(&led_conf);
}