#include "lcd_i2c.h"

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0x04
#define LCD_RW 0x02
#define LCD_RS 0x01

#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_HOME 0x02
#define LCD_CMD_ENTRY_MODE 0x06
#define LCD_CMD_DISPLAY_ON 0x0C
#define LCD_CMD_FUNCTION_SET 0x28

static const char *TAG = "lcd_i2c";

static i2c_port_t lcd_i2c_port;
static uint8_t lcd_addr;
static uint8_t lcd_cols, lcd_rows;

static esp_err_t lcd_send_nibble(uint8_t nibble, uint8_t mode);
static esp_err_t lcd_send_byte(uint8_t byte, uint8_t mode);
static void lcd_pulse_enable(uint8_t data);

esp_err_t lcd_i2c_init(i2c_port_t i2c_num, uint8_t address, uint8_t cols, uint8_t rows)
{
    lcd_i2c_port = i2c_num;
    lcd_addr = address;
    lcd_cols = cols;
    lcd_rows = rows;

    vTaskDelay(pdMS_TO_TICKS(50)); // Aguarda LCD iniciar

    lcd_send_nibble(0x03 << 4, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03 << 4, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03 << 4, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    lcd_send_nibble(0x02 << 4, 0); // Modo 4 bits

    lcd_send_byte(LCD_CMD_FUNCTION_SET, 0);
    lcd_send_byte(LCD_CMD_DISPLAY_ON, 0);
    lcd_send_byte(LCD_CMD_CLEAR, 0);
    lcd_send_byte(LCD_CMD_ENTRY_MODE, 0);
    vTaskDelay(pdMS_TO_TICKS(5));

    return ESP_OK;
}

void lcd_i2c_clear(void)
{
    lcd_send_byte(LCD_CMD_CLEAR, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_i2c_home(void)
{
    lcd_send_byte(LCD_CMD_HOME, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void lcd_i2c_set_cursor(uint8_t col, uint8_t row)
{
    const uint8_t offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row >= lcd_rows)
        row = lcd_rows - 1;
    lcd_send_byte(0x80 | (col + offsets[row]), 0);
}

void lcd_i2c_write_char(char c)
{
    lcd_send_byte((uint8_t)c, LCD_RS);
}

void lcd_i2c_write_str(const char *str)
{
    while (*str)
    {
        lcd_i2c_write_char(*str++);
    }
}

// --- Funções internas ---

static esp_err_t lcd_send_nibble(uint8_t nibble, uint8_t mode)
{
    uint8_t data = nibble | LCD_BACKLIGHT | mode;
    esp_err_t err = i2c_master_write_to_device(lcd_i2c_port, lcd_addr, &data, 1, pdMS_TO_TICKS(100));
    if (err != ESP_OK)
        return err;

    lcd_pulse_enable(data);
    return ESP_OK;
}

static esp_err_t lcd_send_byte(uint8_t byte, uint8_t mode)
{
    esp_err_t err;
    err = lcd_send_nibble(byte & 0xF0, mode);
    if (err != ESP_OK)
        return err;

    err = lcd_send_nibble((byte << 4) & 0xF0, mode);
    return err;
}

static void lcd_pulse_enable(uint8_t data)
{
    uint8_t pulse[2] = {data | LCD_ENABLE, data & ~LCD_ENABLE};
    i2c_master_write_to_device(lcd_i2c_port, lcd_addr, pulse, 2, pdMS_TO_TICKS(100));
    esp_rom_delay_us(1);
}
