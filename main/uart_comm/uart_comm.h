#ifndef _LM55_6B_UART_COMM_H_
#define _LM55_6B_UART_COMM_H_

#include "esp_system.h"
#include "driver/uart.h"

#define UART_TASK_PRIORITY   2

void init_uart();
void my_printf(char * pStr);
uint8_t check_sum(uint8_t * buf, uint8_t size);
void send_bytes(uint8_t * src, size_t size);
void uart_send_data(uint8_t att, uint8_t medi);
void uart_Recv_etimation();
void uart_send_master_cmd(void);
void uart_send_slave_cmd(void);
void uart_recv_task();
#endif
