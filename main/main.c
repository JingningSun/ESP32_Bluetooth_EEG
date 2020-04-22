/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
*
* This file is for bt_spp_vfs_initiator demo. It can discovery servers, connect one device and send data.
* run bt_spp_vfs_initiator demo, the bt_spp_vfs_initiator demo will automatically connect the bt_spp_vfs_acceptor demo,
* then send data.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "spp_task.h"
#include "driver/uart.h"
#include "TFT_LCD.h"
#include "utils.h"
#include "data_process.h"
#include "peripherals_driver.h"
#include "uart_comm.h"

#include "time.h"
#include "sys/time.h"

#include "esp_vfs.h"
#include "sys/unistd.h"

#define SPP_TAG "SPP_INITIATOR_DEMO"
#define EXCAMPLE_DEVICE_NAME "ESP_SPP_INITIATOR"
#define LOGIC_BUFSIZE     8192
#define SPP_DATA_LEN      4096
#define SEMAPHORE_CNT     2048
#define BT_TASK_PRIORITY     4
#define MAIN_TASK_PRIORITY   3

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_VFS;
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_NONE;
static const esp_spp_role_t role_master = ESP_SPP_ROLE_MASTER;

static esp_bd_addr_t peer_bd_addr;
static uint8_t peer_bdname_len;
static char peer_bdname[ESP_BT_GAP_MAX_BDNAME_LEN + 1];
static const char remote_device_name[] = "MyndBand";
static const esp_bt_inq_mode_t inq_mode = ESP_BT_INQ_MODE_GENERAL_INQUIRY;
static const uint8_t inq_len = 30;
static const uint8_t inq_num_rsps = 0;

static uint8_t spp_data[SPP_DATA_LEN];
uint8_t spp_task_alive = 0;
uint8_t btparse_disable = 0;
uint8_t bt_truedata = 0; 
uint8_t wifi_conn_disp = 0;
uint8_t bt_connecting_disp = 0;
uint8_t check_band_disp = 0;
uint8_t bt_on_flag = 0;

extern uint8_t nowmode, newmode;
extern uint8_t nowstate;
uint8_t wifi_state = 0;
uint8_t mqtt_state = 0;
uint8_t wificonfig_state = 0;

SemaphoreHandle_t Semaphore_bt = NULL;

void bluetooth_active();
void bluetooth_stop();

static void main_process(void *param)
{
    ESP_LOGI("Main", "Main task start");
    xTaskCreate(uart_recv_task, "uart_recv_task", 1024 * 4, NULL, UART_TASK_PRIORITY, NULL);
    uint8_t ret;
    determine_state();
    nowmode = newmode;
    LCD_Clear(BACKCOL);
    LCD_ShowLog_start();
    os_time_delay_ms(3000);
    LCD_Clear(BACKCOL);
    if (nowmode == SLAVE)
    {
        ESP_LOGI("Main", "Salve Mode");
        bluetooth_stop();
    }
    else if (nowmode == MASTER)
    {
        ESP_LOGI("Main", "Master Mode");
        bluetooth_active();
    }
    while (1)
    {
        ret = determine_state();
        if (wificonfig_state == 1)
        {
            LCD_ShowWifi_config();
            wifi_conn_disp = 1;
        }
        else
        {
            if (wifi_conn_disp == 1)
            {
                LCD_ShowWifi_connected();
                os_time_delay_ms(2000);
                LCD_Clear(BACKCOL);
                wifi_conn_disp = 0;
            }
            else
            {
                if (ret == 1)
                {
                    if (bt_on_flag == 1)
                    {
                        ESP_LOGI("Main", "Slave->Master");

                        bluetooth_active();
                        os_time_delay_ms(1000);
                        bluetooth_stop();
                        os_time_delay_ms(1000);
                        bluetooth_active();
                    }
                    else if (bt_on_flag == 0)
                    {
                        ESP_LOGI("Main", "Master->Slave");
                        bluetooth_stop();
                    }
                }
                if (nowstate < 13)
                {
                    if (get_testfunc() == 1)
                    {
                        if (check_band_disp == 1)
                        {
                            LCD_Clear(BACKCOL);
                            check_band_disp = 0;
                        }
                        get_setvals();
                        set_att_val(get_test_val());
                        set_medi_val(get_test_val());
                        sendtoSlave();
                        LCD_Display_process();
                        peripheral_process(nowstate);
                    }
                    else if (get_testfunc() == 0)
                    {
                        if (spp_task_alive == 1)
                        {
                            if (bt_truedata == 1)
                            {
                                if (check_band_disp == 1)
                                {
                                    LCD_Clear(BACKCOL);
                                    check_band_disp = 0;
                                }
                                set_att_val(get_btatt_val());
                                set_medi_val(get_btmedi_val());
                                sendtoSlave();
                                get_setvals();
                                LCD_Display_process();
                                peripheral_process(nowstate);
                                bt_truedata = 0;
                                bt_connecting_disp = 0;
                            }
                            else if (bt_truedata == 2)
                            {
                                bt_truedata = 0;
                                LCD_ShowCheck_band();
                                bt_connecting_disp = 0;
                                check_band_disp = 1;
                            }
                        }
                        else
                        {
                            LCD_ShowBt_connecting();
                            bt_connecting_disp = 1;
                        }
                    }
                }
                else if (nowstate > 12)
                {
                    if (check_band_disp == 1)
                    {
                        LCD_Clear(BACKCOL);
                        check_band_disp = 0;
                    }
                    if (get_testfunc() == 1)
                    {
                        set_att_val(get_test_val());
                        set_medi_val(get_test_val());
                    }
                    get_setvals();
                    LCD_Display_process();
                    peripheral_process(nowstate);
                }
            }
        }
        os_time_delay_ms(20);
    }
}

static void spp_read_handle(void * param)
{
    
    spp_task_alive = 1;
    int i, size = 0;
    int fd = (int)param;
    do {
        size = read (fd, spp_data, SPP_DATA_LEN);
        // ESP_LOGI(SPP_TAG, "fd = %d data_len = %d", fd, size);
        if (size == -1) {
            break;
        }
        // esp_log_buffer_hex(SPP_TAG, spp_data, size);
            for (i = 0; i < size; i++)
            {
                uint8_t ret = 0;
                ret = process_data(spp_data[i]);
                if (ret == 1)
                {
                    bt_truedata = 1;
                }
                else if (ret == 2)
                {
                    bt_truedata = 2;
                }                    
            }
        if (size == 0) {
            /*read fail due to there is no data, retry after 1s*/
            vTaskDelay(200 / portTICK_PERIOD_MS);
        } 
    } while (1);
    ESP_LOGI(SPP_TAG, "SPP task shutdown");
    spp_task_alive = 0;
    spp_wr_task_shut_down();
}

static bool get_name_from_eir(uint8_t *eir, char *bdname, uint8_t *bdname_len)
{
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    if (!eir) {
        return false;
    }

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > ESP_BT_GAP_MAX_BDNAME_LEN) {
            rmt_bdname_len = ESP_BT_GAP_MAX_BDNAME_LEN;
        }

        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }

    return false;
}

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(EXCAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        esp_bt_gap_start_discovery(inq_mode, inq_len, inq_num_rsps);

        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT status=%d scn_num=%d",param->disc_comp.status, param->disc_comp.scn_num);
        if (param->disc_comp.status == ESP_SPP_SUCCESS) {
            esp_spp_connect(sec_mask, role_master, param->disc_comp.scn[0], peer_bd_addr);
        }
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        spp_wr_task_start_up(spp_read_handle, param->open.fd);
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        spp_task_alive = 0;
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        spp_wr_task_start_up(spp_read_handle, param->srv_open.fd);
        break;
    default:
        break;
    }
}

static void esp_spp_stack_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    spp_task_work_dispatch((spp_task_cb_t)esp_spp_cb, event, param, sizeof(esp_spp_cb_param_t), NULL);
}

static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch(event){
    case ESP_BT_GAP_DISC_RES_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_RES_EVT");
        esp_log_buffer_hex(SPP_TAG, param->disc_res.bda, ESP_BD_ADDR_LEN);
        for (int i = 0; i < param->disc_res.num_prop; i++){
            if (param->disc_res.prop[i].type == ESP_BT_GAP_DEV_PROP_EIR
                && get_name_from_eir(param->disc_res.prop[i].val, peer_bdname, &peer_bdname_len)){
                esp_log_buffer_char(SPP_TAG, peer_bdname, peer_bdname_len);
                if (strlen(remote_device_name) == peer_bdname_len
                    && strncmp(peer_bdname, remote_device_name, peer_bdname_len) == 0) {
                    memcpy(peer_bd_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
                    esp_spp_start_discovery(peer_bd_addr);
                    esp_bt_gap_cancel_discovery();
                }
            }
        }
        break;
    case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_DISC_STATE_CHANGED_EVT");
        spp_task_alive = 0;
        break;
    case ESP_BT_GAP_RMT_SRVCS_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVCS_EVT");
        break;
    case ESP_BT_GAP_RMT_SRVC_REC_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_RMT_SRVC_REC_EVT");
        break;
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }
    default:
        break;
    }
}

void bluetooth_active()
{
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if (esp_bt_controller_init(&bt_cfg) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed", __func__);
        return;
    }

    if (esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s enable controller failed", __func__);
        return;
    }

    if (esp_bluedroid_init() != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed", __func__);
        return;
    }

    if (esp_bluedroid_enable() != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed", __func__);
        return;
    }

    if (esp_bt_gap_register_callback(esp_bt_gap_cb) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s gap register failed", __func__);
        return;
    }

    if (esp_spp_register_callback(esp_spp_stack_cb) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s spp register failed", __func__);
        return;
    }

    esp_spp_vfs_register();
    spp_task_task_start_up();
    if (esp_spp_init(esp_spp_mode) != ESP_OK)
    {
        ESP_LOGE(SPP_TAG, "%s spp init failed", __func__);
        return;
    }
}

void bluetooth_stop()
{
    esp_bt_controller_deinit();
    esp_bt_controller_disable();
    esp_bluedroid_deinit();
    esp_bluedroid_disable();
    esp_bt_gap_register_callback(NULL);
    esp_spp_register_callback(NULL);
    spp_task_task_shut_down();
}

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    os_time_delay_ms(100);
    Lcd_Init();
    init_peripherals();
    init_uart();
    wificonfig_state = 0;
    xTaskCreate(main_process, "main_task", 1024 * 2, NULL, MAIN_TASK_PRIORITY, NULL);
      
    
    // /*
    //  * Set default parameters for Legacy Pairing
    //  * Use variable pin, input pin code when pairing
    //  */
    // esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    // esp_bt_pin_code_t pin_code;
    // esp_bt_gap_set_pin(pin_type, 0, pin_code);
}

