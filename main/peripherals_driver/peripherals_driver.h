
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_system.h"
#include "utils.h"
#include "driver/ledc.h"

#define PERIPHERAL_TAG   "PERIPHERAL"
#define DEFAULT_VREF    1450        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling
         
#define SEL_PROTOCOL     22
#define SEL_TEST         35
#define SEL_MODE         2
#define SEL_MS_SL        5
#define SEL_SCALE         2
#define V12_OUT          19
#define V5_OUT           21
#define V12_RELAY        4  
#define SOLEN_1          0
#define SOLEN_2          18

#define PWM_TIMER          LEDC_TIMER_0
#define PWM_MODE           LEDC_HIGH_SPEED_MODE
#define PWM_CH0_GPIO       V12_OUT
#define PWM_CH0_CHANNEL    LEDC_CHANNEL_0

/***********************Define State***********************/
#define stMRAO            1  //master-real-attention-open
#define stMRAC            2  //master-real-attention-close
#define stMRAP            3  //master-real-attention-polarity
#define stMRMO            4  //master-real-meditation-open
#define stMRMC            5  //master-real-meditation-close
#define stMRMP            6  //master-real-meditation-polarity
#define stMTAO            7  //master-test-attention-open
#define stMTAC            8  //master-test-attention-close
#define stMTAP            9  //master-test-attention-polarity
#define stMTMO            10 //master-test-meditation-open
#define stMTMC            11 //master-test-meditation-close
#define stMTMP            12 //master-test-meditation-polarity
#define stSRAO            13 //slave-real-attention-open
#define stSRAC            14 //slave-real-attention-close
#define stSRAP            15 //slave-real-attention-polarity
#define stSRMO            16 //slave-real-meditation-open
#define stSRMC            17 //slave-real-meditation-close
#define stSRMP            18 //slave-real-meditation-polarity
#define stSTAO            19 //slave-test-attention-open
#define stSTAC            20 //slave-test-attention-close
#define stSTAP            21 //slave-test-attention-polarity
#define stSTMO            22 //slave-test-meditation-open
#define stSTMC            23 //slave-test-meditation-close
#define stSTMP            24 //slave-test-meditation-polarity
#define stWifiCONFIG      25 //Wifi configure
#define stBTCON           26 //bluetooth connecting
#define stCHBAND          27 //check band
#define stWifiCONN        28 //Wifi connected


#define MASTER            1
#define SLAVE             2

#define OPEN_MODE         1
#define CLOSE_MODE        0

void init_adc_channel1();
void init_adc_channel2();
void init_adc_channel3();
void set_highlevel();
void set_lowlevel();
void set_testval();
uint8_t get_high_level();
uint8_t get_low_level();
uint8_t get_test_val();
uint8_t get_mode();
uint8_t get_protocol();
uint8_t get_testfunc();
uint8_t get_masterfunc();
uint8_t get_actmode();
void adc_on();
void adc_off();
void init_peripherals();
void peripherals_action();
void set_att_val(uint8_t att);
void set_medi_val(uint8_t med);
void peripheral_process(uint8_t nowstate);
uint8_t determine_state();
void get_setvals();
uint8_t get_curmed();
uint8_t get_curatt();
void sendtoSlave();
void PWM_init(void);
void PWM_gpio_reinit(void);
