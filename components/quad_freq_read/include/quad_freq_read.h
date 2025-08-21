#ifndef SENSOR_METER_H
#define SENSOR_METER_H

#include <math.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

void quad_freq_init(int freq1_pin, int freq2_pin, int measurement_interval);

float quad_freq_get_freq2();

float quad_freq_get_freq1();

#endif // SENSOR_METER_H