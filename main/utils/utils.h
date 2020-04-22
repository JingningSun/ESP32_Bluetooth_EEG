#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void os_time_delay_ms(uint32_t ms);
void os_time_delay_us(uint32_t us);
uint32_t my_os_get_time();
uint8_t Dectostr( uint8_t value, char *buf);
void get_starttime();
uint8_t Delaytime_determine(uint8_t delay);

