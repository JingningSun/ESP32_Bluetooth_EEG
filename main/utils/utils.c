
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_system.h"
#include "utils.h"

extern uint8_t delayend_flag;
uint32_t start_timetick = 0;
uint8_t start_timeflag = 0;

uint32_t my_os_get_time()
{
    return xTaskGetTickCount() * portTICK_RATE_MS;
}


void os_time_delay_ms(uint32_t ms)
{
    vTaskDelay(ms / portTICK_RATE_MS);
}

void os_time_delay_us(uint32_t us)
{
    for (int i = 0; i < us; i++)
    {
        os_time_delay_ms(2);
    }
}

uint8_t Dectostr( uint8_t value, char *buf)
{
    if (value < 101)
    {
        buf[0] = (value/100) + '0';
        buf[1] = ((value%100)/10) + '0';
        buf[2] = (value%10) + '0';
        buf[3] = '\0';
        return 1;
    }
    else
    {
        return 0;
    }
}

void get_starttime()
{
    start_timetick = my_os_get_time();
    delayend_flag = 0;
    start_timeflag = 1;
}

uint8_t Delaytime_determine(uint8_t delay)
{
    if (start_timeflag == 1)
    {
        if (my_os_get_time() > (start_timetick + (delay * 1000)))
        {
            delayend_flag = 1;
           return 1;
        }
    }
    return 0;
}
