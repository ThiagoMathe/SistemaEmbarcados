#include <stdio.h>
#include "driver/i2c.h"

#define I2C_MASTER_NUM I2C_NUM_0        // Número do I2C (I2C0)
#define I2C_MASTER_SDA_IO 21           // GPIO para SDA
#define I2C_MASTER_SCL_IO 20           // GPIO para SCL
#define I2C_MASTER_FREQ_HZ 100000      // Frequência do barramento I2C
#define MPU6050_ADDR 0x68              // Endereço do MPU6050
#define MPU6050_PWR_MGMT_1 0x6B        // Registrador de gerenciamento de energia
#define MPU6050_ACCEL_XOUT_H 0x3B      // Registrador de saída do eixo X do acelerômetro
#define MPU6050_TEMP_OUT_H 0x41        // Registrador de saída da temperatura
#define MPU6050_GYRO_XOUT_H 0x43       // Registrador de saída do eixo X do giroscópio
#define MPU6050_GYRO_CONFIG 0x1B       // Registrador de configuração do giroscópio
#define MPU6050_ACCEL_CONFIG 0x1C      // Registrador de configuração do acelerômetro

void mpu6050_init() {
    // Sair do modo sleep
    uint8_t pwr_mgmt_1 = 0x00; // Desativar o modo sleep
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, &pwr_mgmt_1, 1, pdMS_TO_TICKS(100)));

    // Configurar o giroscópio para ±250°/s
    uint8_t gyro_config = 0x00; // FS_SEL = 0 (250°/s)
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, &gyro_config, 1, pdMS_TO_TICKS(100)));

    // Configurar o acelerômetro para ±2g
    uint8_t accel_config = 0x00; // AFS_SEL = 0 (±2g)
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, &accel_config, 1, pdMS_TO_TICKS(100)));

    printf("MPU6050 inicializado\n");
}

esp_err_t read_mpu6050(uint8_t reg_addr, uint8_t *data, size_t len) {
    // Escreve o endereço do registrador que será lido
    esp_err_t ret = i2c_master_write_read_device(
        I2C_MASTER_NUM,
        MPU6050_ADDR,
        &reg_addr,
        1,
        data,
        len,
        pdMS_TO_TICKS(100)
    );
    return ret;
}

float read_temperature() {
    uint8_t data[2]; // 2 bytes para a temperatura
    esp_err_t ret = read_mpu6050(MPU6050_TEMP_OUT_H, data, 2);

    if (ret == ESP_OK) {
        int16_t temp_raw = (int16_t)(data[0] << 8 | data[1]);
        float temperature = (temp_raw / 340.0) + 36.53; // Fórmula do datasheet
        return temperature;
    } else {
        printf("Erro ao ler temperatura do MPU6050\n");
        return -1.0; // Retorna um valor inválido em caso de erro
    }
}

void read_acceleration(float *accel_x, float *accel_y, float *accel_z) {
    uint8_t data[6]; // 6 bytes: 2 para cada eixo (X, Y, Z)
    esp_err_t ret = read_mpu6050(MPU6050_ACCEL_XOUT_H, data, 6);

    if (ret == ESP_OK) {
        int16_t accel_x_raw = (int16_t)((data[0] << 8) | data[1]);
        int16_t accel_y_raw = (int16_t)((data[2] << 8) | data[3]);
        int16_t accel_z_raw = (int16_t)((data[4] << 8) | data[5]);

        // Converter para g (escala de ±2g)
        *accel_x = (accel_x_raw / 16384.0); // 32768 / 2 = 16384
        *accel_y = (accel_y_raw / 16384.0);
        *accel_z = (accel_z_raw / 16384.0);
    } else {
        printf("Erro ao ler dados de aceleração do MPU6050\n");
    }
}

void read_gyroscope(float *gyro_x, float *gyro_y, float *gyro_z) {
    uint8_t data[6]; // 6 bytes: 2 para cada eixo (X, Y, Z)
    esp_err_t ret = read_mpu6050(MPU6050_GYRO_XOUT_H, data, 6);

    if (ret == ESP_OK) {
        int16_t gyro_x_raw = (int16_t)((data[0] << 8) | data[1]);
        int16_t gyro_y_raw = (int16_t)((data[2] << 8) | data[3]);
        int16_t gyro_z_raw = (int16_t)((data[4] << 8) | data[5]);

        // Converter para graus por segundo (escala de ±250°/s)
        *gyro_x = (gyro_x_raw / 131.0); // 32768 / 250 = 131.072
        *gyro_y = (gyro_y_raw / 131.0);
        *gyro_z = (gyro_z_raw / 131.0);
    } else {
        printf("Erro ao ler dados do giroscópio do MPU6050\n");
    }
}

void i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

void app_main() {
    i2c_master_init();
    mpu6050_init();

    while (1) {
        // Ler temperatura
        float temperature = read_temperature();
        if (temperature != -1.0) {
            printf("Temperatura: %.2f°C\n", temperature);
        }

        // Ler aceleração
        float accel_x, accel_y, accel_z;
        read_acceleration(&accel_x, &accel_y, &accel_z);
        printf("Aceleração: X=%.2fg, Y=%.2fg, Z=%.2fg\n", accel_x, accel_y, accel_z);

        // Ler giroscópio
        float gyro_x, gyro_y, gyro_z;
        read_gyroscope(&gyro_x, &gyro_y, &gyro_z);
        printf("Giroscópio: X=%.2f°/s, Y=%.2f°/s, Z=%.2f°/s\n", gyro_x, gyro_y, gyro_z);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Atualiza os dados a cada 1 segundo
    }
}