
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_system.h"
#include "utils.h"
#include "data_process.h"
#include "TFT_LCD.h"
#include "peripherals_driver.h"
#include "esp_log.h"


static uint8_t process_buf[PROC_BUFSIZE];
uint32_t proc_buf_cnt = 0;
uint8_t proc_buf_fill = 0;
uint8_t bt_att = 0;    
uint8_t bt_med = 0;   
uint8_t signalquality = 0;
uint16_t  Checksum = 0;
uint8_t high_set_val, low_set_val;
uint8_t mainframe_avail = 0;

uint8_t get_btatt_val()
{
   return bt_att;
}

uint8_t get_btmedi_val()
{
    return bt_med;
}

void set_btatt_val(uint8_t att)
{
    bt_att = att;
}

void set_btmedi_val(uint8_t medi)
{
    bt_med = medi;
}

uint8_t capture_data(uint8_t capturedata)
{
    process_buf[proc_buf_cnt] = capturedata;

    if(proc_buf_cnt == 0 && process_buf[proc_buf_cnt] == 0xAA)
    {     
            proc_buf_cnt++;  
    } 
    else if(proc_buf_cnt == 1 && process_buf[proc_buf_cnt] == 0xAA)
    {  
        proc_buf_cnt++;  
    }
    else if(proc_buf_cnt == 2 && process_buf[proc_buf_cnt] == 0x20)
    {  
        proc_buf_cnt++;  
    }    
    else if(proc_buf_cnt == 3 && process_buf[proc_buf_cnt] == 0x02)
    {  
        proc_buf_cnt++;  
    }  
    else if(proc_buf_cnt == 4)
    {  
        proc_buf_cnt++;
    }
    else if(proc_buf_cnt == 5 && process_buf[proc_buf_cnt] == 0x83)
    {  
        proc_buf_cnt++;
    }
    else if(proc_buf_cnt == 6 && process_buf[proc_buf_cnt] == 0x18)
    {  
        proc_buf_cnt++;
    }
    else if (proc_buf_cnt >= 7 && proc_buf_cnt < 35)
    {
        proc_buf_cnt++; 
    }        
    else if(proc_buf_cnt == 35)
    {  
        proc_buf_cnt=0;    
        proc_buf_fill = 1;
    }  
    else
    {  
        proc_buf_cnt=0;
    }

    if (proc_buf_fill == 1)
    {
       proc_buf_fill = 0;
       return 1;
    }  
    else
    {
        proc_buf_fill = 0;
        return 0;
    }
        
}

uint8_t process_data(uint8_t inputdata)
{
	if(capture_data(inputdata))
	{
		for(uint8_t i = 0; i < 32; i++)
		{  
			Checksum += process_buf [ i + 3 ];
		}      
		Checksum = (~Checksum) & 0xff; 
		if(Checksum == process_buf[35])
		{      
			Checksum = 0;
			signalquality = 0;
			
			signalquality = process_buf[4];     

			if(signalquality != 29 && signalquality != 54 && signalquality != 55
            && signalquality != 56 && signalquality != 80 && signalquality !=81
            && signalquality != 82 && signalquality != 107 && signalquality != 200)
			{
                set_btatt_val(process_buf[32]);
                set_btmedi_val(process_buf[34]);
                os_time_delay_ms(100);
                return 1;
			}
            else
            {
            //    if (get_testfunc() == 0)
            //    {
            //        lcd_display_check();
            //    } 
                return 2;
            } 
		}
        else
        {
            return 0;
        }
        
	}
    else
    {
        return 0;
    }
    
}


