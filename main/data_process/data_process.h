
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "esp_system.h"
#include "utils.h"

#define PROC_BUFSIZE      50
#define PROCESS_TAG   "PROCESS"

uint8_t capture_data(uint8_t capturedata);
uint8_t process_data(uint8_t inputdata);
uint8_t get_btatt_val();
uint8_t get_btmedi_val();
void set_btatt_val(uint8_t att);
void set_btmedi_val(uint8_t medi);