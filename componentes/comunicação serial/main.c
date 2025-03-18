#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define UART_PORT UART_NUM_0
#define BUF_SIZE 1024

// I2C
#define I2C_MASTER_SCL_IO GPIO_NUM_20
#define I2C_MASTER_SDA_IO GPIO_NUM_21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

#define LCD_ADDR 0x27

// Pinos do display de 7 segmentos
#define SEG_A GPIO_NUM_6
#define SEG_B GPIO_NUM_7
#define SEG_C GPIO_NUM_40
#define SEG_D GPIO_NUM_41
#define SEG_E GPIO_NUM_42
#define SEG_F GPIO_NUM_5
#define SEG_G GPIO_NUM_4

// LCD funcionar
#define LCD_BACKLIGHT 0x08
#define ENABLE 0x04
#define LCD_CMD 0
#define LCD_CHR 1

// Envia dados via I2C
void lcd_i2c_write(uint8_t data) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (LCD_ADDR << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, data | LCD_BACKLIGHT, true);
  i2c_master_stop(cmd);
  i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);
}

// Envia comandos para o LCD
void lcd_send(uint8_t data, uint8_t mode) {
  uint8_t high_nibble = (data & 0xF0) | mode;
  uint8_t low_nibble = ((data << 4) & 0xF0) | mode;

  lcd_i2c_write(high_nibble | ENABLE);
  lcd_i2c_write(high_nibble & ~ENABLE);
  lcd_i2c_write(low_nibble | ENABLE);
  lcd_i2c_write(low_nibble & ~ENABLE);
}

// Inicializa o LCD
void lcd_init_direct() {
  vTaskDelay(50 / portTICK_PERIOD_MS);
  lcd_send(0x03, LCD_CMD);
  vTaskDelay(5 / portTICK_PERIOD_MS);
  lcd_send(0x03, LCD_CMD);
  vTaskDelay(1 / portTICK_PERIOD_MS);
  lcd_send(0x03, LCD_CMD);
  vTaskDelay(1 / portTICK_PERIOD_MS);
  lcd_send(0x02, LCD_CMD);

  // Configuração do display
  lcd_send(0x28, LCD_CMD);
  lcd_send(0x0C, LCD_CMD);
  lcd_send(0x06, LCD_CMD);
  lcd_send(0x01, LCD_CMD);
  vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_clear() {
  lcd_send(0x01, LCD_CMD);
  vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
  uint8_t address = (row == 0) ? col : col + 0x40;
  lcd_send(0x80 | address, LCD_CMD);
}

void lcd_write_str(const char *str) {
  while (*str) {
    lcd_send((uint8_t)*str++, LCD_CHR);
  }
}

// Função para configurar o I2C
void i2c_master_init() {
  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ
  };
  ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
  ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

// Segmentos
const uint8_t segment_map[10] = {
  0b1000000, // 0
  0b1111001, // 1
  0b0100100, // 2
  0b0110000, // 3
  0b0011001, // 4
  0b0010010, // 5
  0b0000010, // 6
  0b1111000, // 7
  0b0000000, // 8
  0b0010000  // 9
};

// Configura os pinos do display
void configure_display(void) {
  gpio_config_t io_conf = {
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1ULL << SEG_A) | (1ULL << SEG_B) | (1ULL << SEG_C) |
                    (1ULL << SEG_D) | (1ULL << SEG_E) | (1ULL << SEG_F) | 
                    (1ULL << SEG_G),
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };
  gpio_config(&io_conf);
  
  gpio_set_level(SEG_A, 1);
  gpio_set_level(SEG_B, 1);
  gpio_set_level(SEG_C, 1);
  gpio_set_level(SEG_D, 1);
  gpio_set_level(SEG_E, 1);
  gpio_set_level(SEG_F, 1);
  gpio_set_level(SEG_G, 1);
}

// Exibe o número no display
void display_number(uint8_t num) {
  uint8_t segments = segment_map[num];

  gpio_set_level(SEG_A, (segments >> 0) & 1);
  gpio_set_level(SEG_B, (segments >> 1) & 1);
  gpio_set_level(SEG_C, (segments >> 2) & 1);
  gpio_set_level(SEG_D, (segments >> 3) & 1);
  gpio_set_level(SEG_E, (segments >> 4) & 1);
  gpio_set_level(SEG_F, (segments >> 5) & 1);
  gpio_set_level(SEG_G, (segments >> 6) & 1);
}

void app_main(void) {
  // Configuração da UART0
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_PORT, &uart_config);
  uart_driver_install(UART_PORT, BUF_SIZE, 0, 0, NULL, 0);

  // Configura os pinos do display
  configure_display();

  // Inicializa o I2C e o LCD
  i2c_master_init(); 
  lcd_init_direct();

  uint8_t data[BUF_SIZE];

  printf("Digite um número ou letra:\n");

  while (true) {
    // Lê os dados do terminal
    int len = uart_read_bytes(UART_PORT, data, BUF_SIZE - 1, 20 / portTICK_PERIOD_MS);

    if (len > 0) {
      data[len] = '\0';
      char input = data[0];

      if ((input >= '0' && input <= '9')) {

        uint8_t num = input - '0'; // Converte para um número
        printf("%d\n", num);
        display_number(num);

      } else if ((input >= 'A' && input <= 'Z') || (input >= 'a' && input <= 'z')) {
        
        printf("%c\n", input);
        lcd_clear();              // Limpa o LCD
        lcd_set_cursor(0, 0);
        lcd_write_str(&input);    // Escreve a letra no LCD

      } else { printf("Inválido!\n"); }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
