#include <stdio.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Definições dos pinos GPIO
#define PIR_GPIO 13          // Pino do sensor PIR (detector de movimento)
#define BUZZER_GPIO 5        // Pino do buzzer
#define LED_VERDE_GPIO 7     // Pino do LED verde
#define LED_AMARELO_GPIO 15  // Pino do LED amarelo
#define LED_VERMELHO_GPIO 6  // Pino do LED vermelho
#define BOTAO_GPIO 4         // Pino do botão

// Configuração da UART
#define UART_PORT UART_NUM_0 // Porta UART utilizada
#define BUF_SIZE 1024        // Tamanho do buffer de leitura da UART

// Configuração do PWM (LEDC)
#define LEDC_TIMER LEDC_TIMER_0           // Timer utilizado para o PWM
#define LEDC_MODE LEDC_LOW_SPEED_MODE     // Modo de operação do LEDC
#define LEDC_CHANNEL_VERDE LEDC_CHANNEL_0 // Canal PWM para o LED verde
#define LEDC_CHANNEL_AMARELO LEDC_CHANNEL_1 // Canal PWM para o LED amarelo
#define LEDC_CHANNEL_VERMELHO LEDC_CHANNEL_2 // Canal PWM para o LED vermelho
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT   // Resolução do duty cycle (13 bits)
#define LEDC_FREQ 5000                    // Frequência do PWM (5 kHz)

// Variáveis globais
uint32_t tempo_temporizacao = 5000; // Tempo de temporização padrão (5 segundos)
bool buzzer_ligado = false;         // Estado do buzzer (ligado/desligado)

// Função para configurar o PWM
void configurar_pwm() {
    // Configuração do timer do PWM
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_config);

    // Configuração do canal PWM para o LED verde
    ledc_channel_config_t canal_verde = {
        .gpio_num = LED_VERDE_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_VERDE,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0, // Duty cycle inicial (0 = desligado)
        .hpoint = 0
    };
    ledc_channel_config(&canal_verde);

    // Configuração do canal PWM para o LED amarelo
    ledc_channel_config_t canal_amarelo = {
        .gpio_num = LED_AMARELO_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_AMARELO,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0, // Duty cycle inicial (0 = desligado)
        .hpoint = 0
    };
    ledc_channel_config(&canal_amarelo);

    // Configuração do canal PWM para o LED vermelho
    ledc_channel_config_t canal_vermelho = {
        .gpio_num = LED_VERMELHO_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL_VERMELHO,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0, // Duty cycle inicial (0 = desligado)
        .hpoint = 0
    };
    ledc_channel_config(&canal_vermelho);
}

// Função para ajustar o brilho de um LED via PWM
void ajustar_brilho_led(ledc_channel_t canal, uint32_t brilho) {
    ledc_set_duty(LEDC_MODE, canal, brilho); // Define o duty cycle
    ledc_update_duty(LEDC_MODE, canal);      // Atualiza o duty cycle
}

// Função principal
void app_main(void) {
    // Configuração dos GPIOs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE; // Desabilita interrupções
    io_conf.mode = GPIO_MODE_OUTPUT;       // Configura como saída
    io_conf.pin_bit_mask = (1ULL << BUZZER_GPIO); // Pino do buzzer
    io_conf.pull_down_en = 0;              // Desabilita pull-down
    io_conf.pull_up_en = 0;                // Desabilita pull-up
    gpio_config(&io_conf);

    // Configura os pinos do PIR e do botão como entrada com pull-up
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIR_GPIO) | (1ULL << BOTAO_GPIO);
    io_conf.pull_up_en = 1; // Habilita pull-up
    gpio_config(&io_conf);

    // Configuração da UART
    uart_config_t uart_config = {
        .baud_rate = 115200,          // Taxa de transmissão
        .data_bits = UART_DATA_8_BITS, // 8 bits de dados
        .parity = UART_PARITY_DISABLE, // Sem paridade
        .stop_bits = UART_STOP_BITS_1, // 1 bit de parada
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE // Sem controle de fluxo
    };
    uart_param_config(UART_PORT, &uart_config); // Aplica a configuração
    uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0); // Instala o driver UART

    // Configura o PWM
    configurar_pwm();

    // Inicializa os LEDs e o buzzer
    ajustar_brilho_led(LEDC_CHANNEL_VERDE, 4095); // LED verde ligado (brilho máximo)
    ajustar_brilho_led(LEDC_CHANNEL_AMARELO, 0);  // LED amarelo desligado
    ajustar_brilho_led(LEDC_CHANNEL_VERMELHO, 0); // LED vermelho desligado
    gpio_set_level(BUZZER_GPIO, 0);               // Buzzer desligado
    printf("Sistema iniciado. Modo: Espera (LED verde ligado).\n");

    // Buffer para leitura da UART
    char buffer[BUF_SIZE];
    while (1) {
        // Lê dados da UART
        int len = uart_read_bytes(UART_PORT, (uint8_t*)buffer, BUF_SIZE - 1, 20 / portTICK_PERIOD_MS);
        if (len > 0) {
            buffer[len] = '\0'; // Adiciona terminador de string
            int valor = atoi(buffer); // Converte o valor lido para inteiro

            // Verifica se o valor está entre 1 e 10
            if (valor >= 1 && valor <= 10) {
                tempo_temporizacao = valor * 1000; // Ajusta o tempo de temporização
                printf("Tempo de temporização ajustado para %i segundos.\n", valor);
            } else {
                printf("Valor inválido! Insira um valor entre 1 e 10.\n");
            }
        }

        // Verifica se o sensor PIR detectou movimento
        if (gpio_get_level(PIR_GPIO)) {
            printf("Movimento detectado!\n");

            // Desliga o LED verde e liga o LED amarelo
            ajustar_brilho_led(LEDC_CHANNEL_VERDE, 0);
            ajustar_brilho_led(LEDC_CHANNEL_AMARELO, 8191); // Brilho médio
            printf("Temporização ativa. Aguardando %lu segundos...\n", (unsigned long)(tempo_temporizacao / 1000));

            // Aguarda o tempo de temporização
            vTaskDelay(pdMS_TO_TICKS(tempo_temporizacao));

            // Verifica se o movimento persiste
            if (gpio_get_level(PIR_GPIO)) {
                // Desliga o LED amarelo e liga o LED vermelho e o buzzer
                ajustar_brilho_led(LEDC_CHANNEL_AMARELO, 0);
                ajustar_brilho_led(LEDC_CHANNEL_VERMELHO, 8191); // Brilho médio
                gpio_set_level(BUZZER_GPIO, 1);
                buzzer_ligado = true;
                printf("Alerta acionado! Movimento persistente detectado.\n");

                // Aguarda 2 segundos
                vTaskDelay(pdMS_TO_TICKS(2000));

                // Desliga o LED vermelho e o buzzer, e retorna ao modo de espera
                ajustar_brilho_led(LEDC_CHANNEL_VERMELHO, 0);
                gpio_set_level(BUZZER_GPIO, 0);
                buzzer_ligado = false;
                ajustar_brilho_led(LEDC_CHANNEL_VERDE, 4095);
                printf("Alerta desativado. Retornando ao modo de espera.\n");
            } else {
                // Desliga o LED amarelo e retorna ao modo de espera
                ajustar_brilho_led(LEDC_CHANNEL_AMARELO, 0);
                ajustar_brilho_led(LEDC_CHANNEL_VERDE, 4095);
                printf("Movimento parou. Retornando ao modo de espera.\n");
            }
        }

        // Verifica se o botão foi pressionado
        if (gpio_get_level(BOTAO_GPIO) == 0) {
            uint32_t tempo_pressionado = 0;
            while (gpio_get_level(BOTAO_GPIO) == 0) {
                vTaskDelay(pdMS_TO_TICKS(100)); // Aguarda 100 ms
                tempo_pressionado += 100;

                // Se o botão for pressionado por 3 segundos e o buzzer estiver ligado, desliga o buzzer
                if (tempo_pressionado >= 3000 && buzzer_ligado) {
                    gpio_set_level(BUZZER_GPIO, 0);
                    buzzer_ligado = false;
                    printf("Botão pressionado por 3 segundos. Buzzer desativado.\n");
                    break;
                }
            }
        }

        // Aguarda 100 ms antes de repetir o loop
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}