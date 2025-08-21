#pragma once

#include "driver/i2c.h"
#include "esp_err.h"
#include <stdint.h>
#include "freertos/task.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Inicializa o display LCD via I2C.
     *
     * Configura o barramento I2C e o display LCD com o número de colunas e linhas fornecido.
     *
     * @param i2c_num Porta I2C utilizada (por exemplo, I2C_NUM_0 ou I2C_NUM_1).
     * @param lcd_addr Endereço I2C do módulo LCD (ex: 0x27 ou 0x3F).
     * @param cols Número de colunas do display LCD (geralmente 16 ou 20).
     * @param rows Número de linhas do display LCD (geralmente 2 ou 4).
     *
     * @return `ESP_OK` se a inicialização for bem-sucedida, ou um erro do tipo `esp_err_t` caso contrário.
     */
    esp_err_t lcd_i2c_init(i2c_port_t i2c_num, uint8_t lcd_addr, uint8_t cols, uint8_t rows);

    /**
     * @brief Limpa completamente o conteúdo do display LCD.
     */
    void lcd_i2c_clear(void);

    /**
     * @brief Retorna o cursor para a posição inicial (0,0) no display LCD.
     */
    void lcd_i2c_home(void);

    /**
     * @brief Move o cursor para uma posição específica no display LCD.
     *
     * @param col Coluna de destino (iniciando em 0).
     * @param row Linha de destino (iniciando em 0).
     */
    void lcd_i2c_set_cursor(uint8_t col, uint8_t row);

    /**
     * @brief Escreve um caractere individual na posição atual do cursor.
     *
     * @param c Caractere a ser escrito.
     */
    void lcd_i2c_write_char(char c);

    /**
     * @brief Escreve uma string a partir da posição atual do cursor.
     *
     * @param str String a ser exibida no display.
     */
    void lcd_i2c_write_str(const char *str);

#ifdef __cplusplus
}
#endif
