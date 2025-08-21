#include "quad_freq_read.h"

static volatile unsigned long s_pulse_count = 0;
static volatile unsigned long s_pulse_count1 = 0;
static unsigned long s_start_time;
static unsigned long s_interval_measuring; // Intervalo de medição em milissegundos

static float freq1;
static float freq2;

static int gpio_freq1_pin; // Pino de leitura de tensão
static int gpio_freq2_pin; // Pino de leitura de potência

// Interrupções para contagem de pulsos
static void IRAM_ATTR pulseISR_CF1(void *arg) { 
    s_pulse_count1++; 
}
static void IRAM_ATTR pulseISR_CF(void *arg) { 
    s_pulse_count++; 
}

// Configuração dos GPIOs
static void configure_gpio()
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    io_conf.pin_bit_mask = (1ULL << gpio_freq1_pin);
    gpio_config(&io_conf);
    /*
    io_conf.pin_bit_mask = (1ULL << gpio_freq2_pin);
    gpio_config(&io_conf);
    */
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_freq1_pin, pulseISR_CF1, NULL);
    
    //gpio_isr_handler_add(gpio_freq2_pin, pulseISR_CF, NULL);
}

// Tarefa para cálculo contínuo dos valores
static void quad_freq_task(void *pvParameters)
{
    while (1)
    {
        if ((esp_timer_get_time() / 1000) - s_start_time >= s_interval_measuring)
        {
            float freq_pin1 = (s_pulse_count1 * 1000.0) / s_interval_measuring;
            freq1 = freq_pin1;

            float freq_pin2 = (s_pulse_count * 1000.0) / s_interval_measuring;
            freq2 = freq_pin2;

            s_pulse_count = 0;
            s_pulse_count1 = 0;
            s_start_time = esp_timer_get_time() / 1000;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Inicializa o sensor e as tasks
void quad_freq_init(int freq1_pin, int freq2_pin, int measurement_interval)
{
    gpio_freq1_pin = freq1_pin;
    gpio_freq2_pin = freq2_pin;
    s_interval_measuring = measurement_interval * 1000; // Converte segundos para milissegundos

    configure_gpio();
    s_start_time = esp_timer_get_time() / 1000;

    xTaskCreate(quad_freq_task, "quad_freq_task", 1024 * 10, NULL, configMAX_PRIORITIES - 1, NULL);
}

// Funções para obter valores calculados
float quad_freq_get_freq1() { return freq1; }
float quad_freq_get_freq2() { return freq2; }