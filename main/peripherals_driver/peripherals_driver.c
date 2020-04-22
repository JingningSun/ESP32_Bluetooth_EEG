
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_system.h"
#include "esp_log.h"
#include "utils.h"
#include "TFT_LCD.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_adc_cal.h"
#include "peripherals_driver.h"
#include "data_process.h"
#include "uart_comm.h"
//**********************Varibles Declaration****************************//

uint8_t nowstate, newstate;
uint8_t nowmode = SLAVE;
uint8_t newmode = SLAVE;
uint8_t cur_att, cur_med;
uint8_t old_actvalue;
uint8_t high_setlev, low_setlev, test_val;
uint8_t old_highlev, old_lowlev;
uint8_t delayend_flag = 0;
extern uint8_t start_timetick, start_timeflag;
extern uint8_t bt_truedata, wificonfig_state, spp_task_alive;
extern uint8_t bt_connecting_disp;
extern uint8_t bt_on_flag;

//************************ADC********************************//
static esp_adc_cal_characteristics_t *adc_chars1;
static esp_adc_cal_characteristics_t *adc_chars2;
static esp_adc_cal_characteristics_t *adc_chars3;

static const adc1_channel_t high_channel = ADC1_CHANNEL_3; //GPIO39
static const adc1_channel_t low_channel = ADC1_CHANNEL_0;  //GPIO36
static const adc1_channel_t test_channel = ADC1_CHANNEL_6; //GPIO34   //V2 board

static const adc_atten_t atten1 = ADC_ATTEN_DB_11;
static const adc_atten_t atten2 = ADC_ATTEN_DB_11;
static const adc_atten_t atten3 = ADC_ATTEN_DB_11;
static uint32_t high_adc_val, low_adc_val, test_adc_val;
//***********************************************************//

//****************************PWM****************************//
ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
    .freq_hz = 5000,                      // frequency of PWM signal
    .speed_mode = PWM_MODE,               // timer mode
    .timer_num = PWM_TIMER                // timer index
};
// Set configuration of timer0 for high speed channels

ledc_channel_config_t ledc_channel[1] = {
    {.channel = PWM_CH0_CHANNEL,
     .duty = 0,
     .gpio_num = PWM_CH0_GPIO,
     .speed_mode = PWM_MODE,
     .timer_sel = PWM_TIMER}};

//*******************************************************************//

//*****************************************************************************//

//*****************************PWM Functions Definition************************//

void PWM_init(void)
{
  ledc_timer_config(&ledc_timer);
  // Set LED Controller with previously prepared configuration
  for (uint8_t ch = 0; ch < 1; ch++)
  {
    ledc_channel_config(&ledc_channel[ch]);
  }

  // Initialize fade service.
  ledc_fade_func_install(0);
}

void PWM_set_dutycycle(uint32_t dutycycle)
{
  uint32_t set_duty = 10 * dutycycle;
  for (uint8_t ch = 0; ch < 1; ch++)
  {
    ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, set_duty);
    ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
  }
}

void V5_switchON()
{
  gpio_set_level(V5_OUT, 1);
}

void V5_switchOFF()
{
  gpio_set_level(V5_OUT, 0);
}

void V12_relayON()
{
  gpio_set_level(V12_RELAY, 1);
}

void V12_relayOFF()
{
  gpio_set_level(V12_RELAY, 0);
}

//*******************************ADC Function Definition*********************//
void adc_on()
{
  adc_power_on();
}

void adc_off()
{
  adc_power_off();
}

void init_adc_channel1()
{
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(high_channel, atten1);

  adc_chars1 = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_characterize(ADC_UNIT_1, atten1, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars1);
}

void init_adc_channel2()
{
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(low_channel, atten2);

  adc_chars2 = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_characterize(ADC_UNIT_1, atten2, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars2);
}

void init_adc_channel3()
{
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(test_channel, atten3);

  adc_chars3 = calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_characterize(ADC_UNIT_1, atten3, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars3);
}

void set_highlevel()
{
  uint32_t adc_reading1 = 0;

  for (int i = 0; i < NO_OF_SAMPLES; i++)
    adc_reading1 += adc1_get_raw((adc1_channel_t)high_channel);

  adc_reading1 /= NO_OF_SAMPLES;

  high_adc_val = esp_adc_cal_raw_to_voltage(adc_reading1, adc_chars1);
  // ESP_LOGI("adc value", "adc = %d", high_adc_val);
  high_setlev = (uint8_t)(((high_adc_val - 135) * 10) / 153);
  if (high_setlev > 99)
  {
    high_setlev = 100;
  }
}

void set_lowlevel()
{
  uint32_t adc_reading2 = 0;

  for (int i = 0; i < NO_OF_SAMPLES; i++)
    adc_reading2 += adc1_get_raw((adc1_channel_t)low_channel);

  adc_reading2 /= NO_OF_SAMPLES;

  low_adc_val = esp_adc_cal_raw_to_voltage(adc_reading2, adc_chars2);
  low_setlev = (uint8_t)(((low_adc_val - 135) * 10) / 153);
  if (low_setlev > 99)
  {
    low_setlev = 100;
  }
}

void set_testval()
{
  uint32_t adc_reading3 = 0;

  for (int i = 0; i < NO_OF_SAMPLES; i++)
    adc_reading3 += adc1_get_raw((adc1_channel_t)test_channel);

  adc_reading3 /= NO_OF_SAMPLES;

  test_adc_val = esp_adc_cal_raw_to_voltage(adc_reading3, adc_chars3);
  test_val = (uint8_t)(((test_adc_val - 135) * 10) / 153);
  if (test_val > 99)
  {
    test_val = 100;
  }
}

uint8_t get_high_level()
{
  return high_setlev;
}

uint8_t get_low_level()
{
  return low_setlev;
}

uint8_t get_test_val()
{
  return test_val;
}

void set_att_val(uint8_t att)
{
  cur_att = att;
}

void set_medi_val(uint8_t med)
{
  cur_med = med;
}

uint8_t get_curmed()
{
  return cur_med;
}

uint8_t get_curatt()
{
  return cur_att;
}

void get_setvals()
{
  adc_on();
  init_adc_channel1();
  set_highlevel();
  adc_off();
  os_time_delay_ms(50);
  adc_on();
  init_adc_channel2();
  set_lowlevel();
  adc_off();
  os_time_delay_ms(50);
  adc_on();
  init_adc_channel3();
  set_testval();
  adc_off();
}

//*********************************************************************************************//

//******************************Peripheral GPIO Functions Definition***************************//

void init_peripherals()
{
  gpio_pad_select_gpio(SEL_MODE);
  gpio_pad_select_gpio(V5_OUT);
  gpio_pad_select_gpio(V12_RELAY);
  gpio_pad_select_gpio(SEL_TEST);
  gpio_pad_select_gpio(SEL_MS_SL);
  gpio_pad_select_gpio(SEL_PROTOCOL);
  gpio_pad_select_gpio(SEL_SCALE);
  gpio_pad_select_gpio(SOLEN_1);
  gpio_pad_select_gpio(SOLEN_2);

  gpio_set_direction(SEL_MODE, GPIO_MODE_INPUT);
  gpio_set_direction(SEL_TEST, GPIO_MODE_INPUT);
  gpio_set_direction(SEL_PROTOCOL, GPIO_MODE_INPUT);
  gpio_set_direction(SEL_MS_SL, GPIO_MODE_INPUT);
  gpio_set_direction(SEL_SCALE, GPIO_MODE_INPUT);
  gpio_set_direction(V5_OUT, GPIO_MODE_OUTPUT);
  gpio_set_direction(V12_RELAY, GPIO_MODE_OUTPUT);
  gpio_set_direction(SOLEN_1, GPIO_MODE_OUTPUT);
  gpio_set_direction(SOLEN_2, GPIO_MODE_OUTPUT);

  gpio_set_level(V5_OUT, 0);
  gpio_set_level(V12_RELAY, 0);
  gpio_set_level(SOLEN_1, 1);
  gpio_set_level(SOLEN_2, 0);

  PWM_init();
}

uint8_t get_actmode()
{
  if (gpio_get_level(SEL_MODE) == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

uint8_t get_protocol()
{
  if (gpio_get_level(SEL_PROTOCOL) == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

uint8_t get_testfunc()
{
  if (gpio_get_level(SEL_TEST) == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

uint8_t get_masterfunc()
{
  if (gpio_get_level(SEL_MS_SL) == 1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//**************************Peripheral Main Process Function**********************//

void Scalemode_run(uint8_t value)         //Scale mode 
{
  if (high_setlev == 100)
  {
    if (value > low_setlev)
    {
      old_actvalue = value;
      start_timeflag = 0;
      delayend_flag = 0;
      PWM_set_dutycycle((uint32_t)value);
    }
    else
    {
      PWM_set_dutycycle(0);
    }
  }
  else if (low_setlev == 0)
  {
    if (value < high_setlev)
    {
      old_actvalue = value;
      start_timeflag = 0;
      delayend_flag = 0;
      PWM_set_dutycycle((uint32_t)value);
    }
    else
    {
      PWM_set_dutycycle(0);
    }
  }
  else
  {
    if ((value < high_setlev) && (value > low_setlev))
    {
      old_actvalue = value;
      start_timeflag = 0;
      delayend_flag = 0;
      PWM_set_dutycycle((uint32_t)value);
    }
    else
    {
      PWM_set_dutycycle(0);
    }
  }
}

void Switchmode_run(uint8_t value, uint8_t mode, uint8_t delay_flag)  //Switch mode
{
  if (mode == OPEN_MODE)
  {
    if (high_setlev == 100)
    {
      if (value > low_setlev)
      {
        old_actvalue = value;
        start_timeflag = 0;
        delayend_flag = 0;
        V5_switchOFF();
        V12_relayOFF();
      }
      else
      {
        if ((delay_flag == 1) && (get_test_val() != 0))
        {
          if (start_timeflag == 0)
          {
            get_starttime();
          }
          if (Delaytime_determine(test_val))
          {
            V5_switchON();
            V12_relayON();
          }
          else
          {
            V5_switchOFF();
            V12_relayOFF();
          }
        }
        else
        {
          V5_switchON();
          V12_relayON();
        }
      }
    }
    else if (low_setlev == 0)
    {
      if (value > low_setlev)
      {
        old_actvalue = value;
        start_timeflag = 0;
        delayend_flag = 0;
        V5_switchOFF();
        V12_relayOFF();
      }
      else
      {
        if ((delay_flag == 1) && (get_test_val() != 0))
        {
          if (start_timeflag == 0)
          {
            get_starttime();
          }
          if (Delaytime_determine(test_val))
          {
            V5_switchON();
            V12_relayON();
          }
          else
          {
            V5_switchOFF();
            V12_relayOFF();
          }
        }
        else
        {
          V5_switchON();
          V12_relayON();
        }
      }
    }
    else
    {
      if ((value < high_setlev) && (value > low_setlev))
      {
        old_actvalue = value;
        start_timeflag = 0;
        delayend_flag = 0;
        V5_switchOFF();
        V12_relayOFF();
      }
      else
      {
        if ((delay_flag == 1) && (get_test_val() != 0))
        {
          if (start_timeflag == 0)
          {
            get_starttime();
          }
          if (Delaytime_determine(test_val))
          {
            V5_switchON();
            V12_relayON();
          }
          else
          {
            V5_switchOFF();
            V12_relayOFF();
          }
        }
        else
        {
          V5_switchON();
          V12_relayON();
        }
      }
    }
  }
  else if (mode == CLOSE_MODE)
  {
    if (high_setlev == 100)
    {
      if (value > low_setlev)
      {
        old_actvalue = value;
        start_timeflag = 0;
        delayend_flag = 0;
        V5_switchON();
        V12_relayON();
      }
      else
      {
        if ((delay_flag == 1) && (get_test_val() != 0))
        {
          if (start_timeflag == 0)
          {
            get_starttime();
          }
          if (Delaytime_determine(test_val))
          {
            V5_switchOFF();
            V12_relayOFF();
          }
          else
          {
            V5_switchON();
            V12_relayON();
          }
        }
        else
        {
          V5_switchOFF();
          V12_relayOFF();
        }
      }
    }
    else if (low_setlev == 0)
    {
      if (value < high_setlev)
      {
        old_actvalue = value;
        start_timeflag = 0;
        delayend_flag = 0;
        V5_switchON();
        V12_relayON();
      }
      else
      {
        if ((delay_flag == 1) && (get_test_val() != 0))
        {
          if (start_timeflag == 0)
          {
            get_starttime();
          }
          if (Delaytime_determine(test_val))
          {
            V5_switchOFF();
            V12_relayOFF();
          }
          else
          {
            V5_switchON();
            V12_relayON();
          }
        }
        else
        {
          V5_switchOFF();
          V12_relayOFF();
        }
      }
    }
    else
    {
      if ((value < high_setlev) && (value > low_setlev))
      {
        old_actvalue = value;
        start_timeflag = 0;
        delayend_flag = 0;
        V5_switchON();
        V12_relayON();
      }
      else
      {
        if ((delay_flag == 1) && (get_test_val() != 0))
        {
          if (start_timeflag == 0)
          {
            get_starttime();
          }
          if (Delaytime_determine(test_val))
          {
            V5_switchOFF();
            V12_relayOFF();
          }
          else
          {
            V5_switchON();
            V12_relayON();
          }
        }
        else
        {
          V5_switchOFF();
          V12_relayOFF();
        }
      }
    }
  }
}

void Solenoid_forward()
{
  gpio_set_level(SOLEN_2, 1);
  gpio_set_level(SOLEN_1, 1);
}

void Solenoid_reverse()
{
  gpio_set_level(SOLEN_2, 0);
  gpio_set_level(SOLEN_1, 0);
}

void Solenoid_break()
{
  gpio_set_level(SOLEN_2, 0);
  gpio_set_level(SOLEN_1, 1);
}

void Solenoid_run(uint8_t value)
{
  if (value > high_setlev)
  {
    Solenoid_reverse();
  }
  else if (value < low_setlev)
  {
    Solenoid_forward();
  }
  else
  {
    Solenoid_break();
  }
}

/******************************************************************************/
/*******************Uart communication with ESP8266 ***************************/
void sendtoSlave()
{
  uint8_t send_buf[7];
  send_buf[0] = 0xAA;
  send_buf[1] = 0xAA;
  send_buf[2] = nowstate;
  send_buf[3] = cur_att;
  send_buf[4] = cur_med;
  send_buf[5] = high_setlev;
  send_buf[6] = low_setlev; 

  send_bytes(send_buf, 7);
}

// uint8_t parseMasterdata(uint8_t * buf)
// {
//   if ((buf[0] == 0xAA) && (buf[1] == 0xAA))
//   {
//     cur_att = buf[3];
//     cur_med = buf[4];
//     return 1;
//   }
//   return 0;
// }
/****************************************************************************/
/*****************************Peripheral Process******************************/
void peripheral_process(uint8_t nowstate)
{
  switch (nowstate)
  {
  case stMRAO:
    Switchmode_run(cur_att, OPEN_MODE, 1);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stMRAC:
    Switchmode_run(cur_att, CLOSE_MODE, 1);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stMRAP:
    
    break;
  case stMRMO:
    Switchmode_run(cur_med, OPEN_MODE, 1);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stMRMC:
    Switchmode_run(cur_med, CLOSE_MODE, 1);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stMRMP:
    
    break;
  case stMTAO:
    Switchmode_run(cur_att, OPEN_MODE, 0);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stMTAC:
    Switchmode_run(cur_att, CLOSE_MODE, 0);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stMTAP:
    
    break;
  case stMTMO:
    Switchmode_run(cur_med, OPEN_MODE, 0);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stMTMC:
    Switchmode_run(cur_med, CLOSE_MODE, 0);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stMTMP:
    
    break;
  case stSRAO:
    Switchmode_run(cur_att, OPEN_MODE, 1);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stSRAC:
    Switchmode_run(cur_att, CLOSE_MODE, 1);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stSRAP:
    
    break;
  case stSRMO:
    Switchmode_run(cur_med, OPEN_MODE, 1);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stSRMC:
    Switchmode_run(cur_med, CLOSE_MODE, 1);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stSRMP:
    
    break;
  case stSTAO:
    Switchmode_run(cur_att, OPEN_MODE, 0);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stSTAC:
    Switchmode_run(cur_att, CLOSE_MODE, 0);
    Scalemode_run(cur_att);
    Solenoid_run(cur_att);
    break;
  case stSTAP:
    
    break;
  case stSTMO:
    Switchmode_run(cur_med, OPEN_MODE, 0);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stSTMC:
    Switchmode_run(cur_med, CLOSE_MODE, 0);
    Scalemode_run(cur_med);
    Solenoid_run(cur_med);
    break;
  case stSTMP:
    
    break;
  }
}

/****************************************************************************/

/*****************************State Process*********************************/
uint8_t determine_state()
{
  if (get_masterfunc() == 1)
  {
    newmode = MASTER;
    if (get_testfunc() == 1)
    {
      if (get_protocol() == 1)
      {
        if (get_actmode() == 1)
        {
          newstate = stMTAO;
        }
        else
        {
          newstate = stMTAC;
        }
      }
      else
      {
        if (get_actmode() == 1)
        {
          newstate = stMTMO;
        }
        else
        {
          newstate = stMTMC;
        }
      }
    }
    else
    {
      if (get_protocol() == 1)
      {
        if (get_actmode() == 1)
        {
          newstate = stMRAO;
        }
        else
        {
          newstate = stMRAC;
        }
      }
      else
      {
        if (get_actmode() == 1)
        {
          newstate = stMRMO;
        }
        else
        {
          newstate = stMRMC;
        }
      }
    }
  }
  else
  {
    newmode = SLAVE;
    if (get_testfunc() == 1)
    {
      if (get_protocol() == 1)
      {
        if (get_actmode() == 1)
        {
          newstate = stSTAO;
        }
        else
        {
          newstate = stSTAC;
        }
      }
      else
      {
        if (get_actmode() == 1)
        {
          newstate = stSTMO;
        }
        else
        {
          newstate = stSTMC;
        }
      }
    }
    else
    {
      if (get_protocol() == 1)
      {
        if (get_actmode() == 1)
        {
          newstate = stSRAO;
        }
        else
        {
          newstate = stSRAC;
        }
      }
      else
      {
        if (get_actmode() == 1)
        {
          newstate = stSRMO;
        }
        else
        {
          newstate = stSRMC;
        }
      }
    }
  }
  if (nowstate != newstate)
  {
    if ((bt_connecting_disp == 1) && (newstate > 6))
    {
      LCD_Clear(BACKCOL);
      bt_connecting_disp = 0;
    }
    if ((nowstate < 7) && (newstate > 6))
    {
      bt_on_flag = 0;
      nowstate = newstate;
      return 1;
    }
    else if ((newstate < 7) && (nowstate > 6))
    {
      bt_on_flag = 1;
      nowstate = newstate;
      return 1;
    }
    nowstate = newstate;
    return 0;
  }
  // if (nowmode != newmode)
  // {
  //   nowmode = newmode;
  //   // LCD_Clear(BACKCOL);
  //   return 1;
  // }
  else
  {
    return 0;
  }
}

//**********************************************************************************************//
