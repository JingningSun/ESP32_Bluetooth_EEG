#ifndef __LCD_H
#define __LCD_H	

#include "stdint.h"


#define USE_HORIZONTAL 3  //ÉèÖÃºáÆÁ»òÕßÊúÆÁÏÔÊ¾ 0»ò1ÎªÊúÆÁ 2»ò3ÎªºáÆÁ


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 320

#else
#define LCD_W 320
#define LCD_H 240
#endif

#define LCD_SCK       32
#define LCD_SDA       33
#define LCD_RST       25
#define LCD_DC        26
#define LCD_CS        27
#define LCD_BLK       14

#define LED_SCLK_Clr() gpio_set_level(LCD_SCK,0)
#define LED_SCLK_Set() gpio_set_level(LCD_SCK,1)

#define LED_SDIN_Clr() gpio_set_level(LCD_SDA,0)//DIN
#define LED_SDIN_Set() gpio_set_level(LCD_SDA,1)

#define LED_RST_Clr() gpio_set_level(LCD_RST,0)//RES
#define LED_RST_Set() gpio_set_level(LCD_RST,1)

#define LED_DC_Clr() gpio_set_level(LCD_DC,0)//DC
#define LED_DC_Set() gpio_set_level(LCD_DC,1)

#define LED_BLK_Clr()  gpio_set_level(LCD_BLK,0)//BLK
#define LED_BLK_Set()  gpio_set_level(LCD_BLK,1)

#define LED_CS_Clr()   gpio_set_level(LCD_CS,0)
#define LED_CS_Set()   gpio_set_level(LCD_CS,1)


extern  uint16_t BACK_COLOR;   

void os_time_delay_ms(uint32_t ms);

void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void Lcd_Init(void); 
void LCD_Clear(uint16_t Color);
void LCD_ShowChinese32x32(uint16_t x,uint16_t y,uint8_t index,uint8_t size,uint16_t color);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_DrawPoint_big(uint16_t x,uint16_t y,uint16_t colory);
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t color);
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t mode,uint16_t color);
void LCD_ShowString(uint16_t x,uint16_t y,const char *p,uint16_t color);
unsigned long mypow(uint8_t m,uint8_t n);
void LCD_ShowNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t color);
void LCD_ShowNum1(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t color);
void LCD_ShowBigNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t color);
void LCD_ShowBigDigit(uint16_t x,uint16_t y,uint8_t index,uint16_t color);
// void LCD_ShowPicture(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, const uint8_t * imag);

void LCD_ShowLog_start();
void LCD_ShowWifi_config();
void LCD_ShowWifi_connected();
void LCD_ShowBt_connecting();
void LCD_ShowCheck_band();
void LCD_ShowMain_frame();
void LCD_ShowMain_Logo();
void LCD_ShowMqtt_off();
void LCD_ShowMqtt_on();
void LCD_ShowWifi_on();
void LCD_ShowFocus();
void LCD_ShowMaster();
void LCD_ShowSlave();
void LCD_ShowClosed();
void LCD_ShowOpen();
void LCD_ShowTest();
void LCD_ShowLive();
void LCD_ShowCalm();
void LCD_ShowDelaytime(uint8_t time);
void LCD_ShowATT_val(uint8_t attval);
void LCD_ShowMED_val(uint8_t medval);
void LCD_ShowHigh_val(uint8_t highval);
void LCD_ShowLow_val(uint8_t lowval);
// void LCD_Display_process(uint8_t state);
void LCD_Display_process();



//»­±ÊÑÕÉ«
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 
#define BRRED 			 0XFC07 
#define GRAY  			 0X8430 
#define DGRAY            0x39C8


#define DARKBLUE      	 0X01CF	
#define LIGHTBLUE      	 0X7D7C	
#define GRAYBLUE       	 0X5458 

 
#define LIGHTGREEN     	 0X841F 
#define LGRAY 			 0xBDF7 

#define LGRAYBLUE        0XA651 
#define LBBLUE           0X2B12 

#define CALMCOL          0xFBE5
#define BACKCOL          0x1065
#define MEDCOL           0x1CBF
 


					  		 
#endif  
	 
	 



