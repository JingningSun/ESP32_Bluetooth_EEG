#include "string.h"
#include "uart_comm.h"
#include "utils.h"
#include "data_process.h"
#include "esp_log.h"
#include "peripherals_driver.h"
#include "TFT_LCD.h"

#define UARTBUFFERSIZE		2048
#define TXD1_PIN 			(GPIO_NUM_17)
#define RXD1_PIN 			(GPIO_NUM_16)

static int uart_s, uart_e;
static uint8_t rx_buf[UARTBUFFERSIZE];
// static uint8_t tx_buf[UARTBUFFERSIZE];
static uint8_t pure_buf[UARTBUFFERSIZE];
extern uint8_t wifi_state, mqtt_state, wificonfig_state;
// uint8_t uartdata_true = 0;


void init_uart(void)
{
	uart_config_t uart1_config;
	uart1_config.baud_rate = 115200;					
	uart1_config.data_bits = UART_DATA_8_BITS;			
	uart1_config.parity = UART_PARITY_DISABLE;			
	uart1_config.stop_bits = UART_STOP_BITS_1;			
	uart1_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
	uart1_config.rx_flow_ctrl_thresh = 0;
	uart1_config.use_ref_tick = 0;
	uart_param_config(UART_NUM_1, &uart1_config);		
	uart_set_pin(UART_NUM_1, TXD1_PIN, RXD1_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_driver_install(UART_NUM_1, UARTBUFFERSIZE * 2, UARTBUFFERSIZE * 2, 0, NULL, 0);

}

void send_bytes(uint8_t * src, size_t size)
{
    uart_write_bytes(UART_NUM_1, (char *)src, size);
}

void my_printf(char * pStr)
{
    send_bytes((uint8_t *)pStr, strlen(pStr) + 1);
}

int get_rx_buf_size()
{
	return (uart_s + UARTBUFFERSIZE - uart_e) % UARTBUFFERSIZE;
}

uint8_t get_rx_buf_at(int idx)
{
	return rx_buf[(uart_e + idx) % UARTBUFFERSIZE];
}

void cut_rx_buffer(int size)
{
	uart_e += size;
	if (uart_e >= UARTBUFFERSIZE) 
		uart_e -= UARTBUFFERSIZE;
}

uint8_t check_sum(uint8_t * buf, uint8_t size)
{
    uint16_t check_sum = 0;
	for (uint8_t i = 0; i < size; i++)
	{
		check_sum += buf[i];
	}
	return (uint8_t)(check_sum / 10);
}

void uart_Recv_etimation()
{
	uint8_t cmd_buf[32];

	int rx_bytes = uart_read_bytes(UART_NUM_1, pure_buf, UARTBUFFERSIZE, 10 / portTICK_RATE_MS);
	if (rx_bytes > 0)
	{
		for (int i = 0; i < rx_bytes; i++)
		{
			rx_buf[uart_s] = pure_buf[i];
			uart_s++;
			if (uart_s >= UARTBUFFERSIZE)
				uart_s -= UARTBUFFERSIZE;
		}
		// esp_log_buffer_hex("Masterdate", &rx_buf[uart_s - rx_bytes], rx_bytes);
	}

	if (get_rx_buf_size() >= 1)
	{
		cmd_buf[0] = get_rx_buf_at(0);
		if (cmd_buf[0] == 0xAA)
		{
			if (get_rx_buf_size() >= 2)
			{
				cmd_buf[1] = get_rx_buf_at(1);
				if (cmd_buf[1] == 0xAA)
				{
					if (get_rx_buf_size() >= 7)
					{
						cmd_buf[3] = get_rx_buf_at(3);
						cmd_buf[4] = get_rx_buf_at(4);
						if (get_testfunc() == 0)
						{
                           set_att_val(cmd_buf[3]);
						   set_medi_val(cmd_buf[4]);
						//    uartdata_true = 1;
						}
						ESP_LOGI("Uart", "Data success");
						cut_rx_buffer(7);
					}
				}
				else
				{
					cut_rx_buffer(1);
				}
			}
		}
		else if (cmd_buf[0] == 0xBB)
		{
			if (get_rx_buf_size() >= 2)
			{
				cmd_buf[1] = get_rx_buf_at(1);
				if (cmd_buf[1] == 0xBB)
				{
					if (get_rx_buf_size() >= 3)
					{
						cmd_buf[2] = get_rx_buf_at(2);
						if (cmd_buf[2] == 0xCC)
						{

							if (get_rx_buf_size() >= 4)
							{
								cmd_buf[3] = get_rx_buf_at(3);
								 if (cmd_buf[3] == 0xCC)
								 {
									 wifi_state = 1;
									 ESP_LOGI("Uart", "Wifi connect");
									 cut_rx_buffer(4);
								 }
								 else if (cmd_buf[3] == 0xDD)
								 {
									 wifi_state = 0;
									 ESP_LOGI("Uart", "Wifi disconnect");
									 cut_rx_buffer(4);
								 }
								 else
								 {
									 cut_rx_buffer(1);
								 }
							}
						}
						else if (cmd_buf[2] == 0xDD)
						{
							if (get_rx_buf_size() >= 4)
							{
								cmd_buf[3] = get_rx_buf_at(3);
								if (cmd_buf[3] == 0xDD)
								{
									mqtt_state = 1;
									ESP_LOGI("Uart", "MQTT connect");
									cut_rx_buffer(4);
								}
								else if (cmd_buf[3] == 0xEE)
								{
									mqtt_state = 0;
									ESP_LOGI("Uart", "MQTT disconnect");
									cut_rx_buffer(4);
								}
								else
								{
									cut_rx_buffer(1);
								}
							}
						}
						else if (cmd_buf[2] == 0xEE)
						{
							if (get_rx_buf_size() >= 4)
							{
								cmd_buf[3] = get_rx_buf_at(3);
								if (cmd_buf[3] == 0xDD)
								{
									wificonfig_state = 0;
									ESP_LOGI("Uart", "wificonfig end");
									cut_rx_buffer(4);
								}
								else if (cmd_buf[3] == 0xEE)
								{
									wificonfig_state = 1;
									ESP_LOGI("Uart", "wificonfig start");
									cut_rx_buffer(4);
								}
								else
								{
									cut_rx_buffer(1);
								}
							}
						}
						else
						{
							cut_rx_buffer(1);
						}
					}
				}
				else
				{
					cut_rx_buffer(1);
				}
			}
		}
		else
		{
			cut_rx_buffer(1);
		}
	}
	os_time_delay_ms(10);
}

void uart_recv_task()
{
    while (1)
		uart_Recv_etimation();
}

