#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ssd1306.h"
#include "quad_freq_read.h"

#define TAG "SSD1306"
#define PINFREQ1 25
#define INTERVAL 1

void app_main(void)
{
    quad_freq_init(PINFREQ1, -1, INTERVAL);
    SSD1306_t dev;

    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xFF);

    int freq = 0;
    char buffer[20];

    ssd1306_display_text(&dev, 0, "Frequêncimetro", 14, false);

    while (1)
    {
        // Limpa tela
        ssd1306_clear_screen(&dev, false);

        // Primeira linha: título centralizado
        ssd1306_display_text(&dev, 0 + 2, "Frequencimetro", 14, false);

        freq = quad_freq_get_freq1();

        // Segunda linha: frequência em Hz
        sprintf(buffer, "%7d Hz", freq);

        int len = strlen(buffer);

        ssd1306_display_text(&dev, 3 + 2, buffer, len, false);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// Example with I2C LCD
// Uncomment to use with I2C LCD
/*
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"

#include "lcd_i2c.h"
#include "driver/i2c.h"
#include "quad_freq_read.h"

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0

#define PINFREQ1 25
#define INTERVAL 1

void app_main()
{

    quad_freq_init(PINFREQ1, -1, INTERVAL);
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 200000,
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    lcd_i2c_init(I2C_MASTER_NUM, 0x27, 16, 2);
    lcd_i2c_set_cursor(1, 0);
    lcd_i2c_write_str("Frequencimetro");

    char buffer[17];
    char last_buffer[17] = "";

    while (1)
    {
        float freq = quad_freq_get_freq1();

        snprintf(buffer, sizeof(buffer), "%.0f Hz            ", freq > 0 ? freq : secondfreq);

        printf("%s\n", buffer);

        if (strcmp(buffer, last_buffer) != 0)
        {
            lcd_i2c_set_cursor(0, 1);
            lcd_i2c_write_str(buffer);
            strcpy(last_buffer, buffer);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
    */