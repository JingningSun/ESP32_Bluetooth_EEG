

#include "TFT_LCD.h"
#include "peripherals_driver.h"
#include "oledfont.h"
#include "log_bmp.h"
#include "wificonfig_bmp.h"
#include "wificonnected_bmp.h"
#include "btconnecting_bmp.h"
#include "checkband_bmp.h"
#include "mainlogo_bmp.h"
#include "mqttoff_bmp.h"
#include "mqtton_bmp.h"
#include "wifion_bmp.h"
#include "focus_bmp.h"
#include "master_bmp.h"
#include "closed_bmp.h"
#include "test_bmp.h"
#include "open_bmp.h"
#include "calm_bmp.h"
#include "live_bmp.h"
#include "slave_bmp.h"


uint16_t BACK_COLOR;   //±³¾°É«

extern uint8_t mqtt_state, wifi_state;
extern uint8_t cur_att, cur_med;
extern uint8_t high_setlev, low_setlev, test_val;
extern uint8_t bt_connecting_disp;



/******************************************************************************
      º¯ÊýËµÃ÷£ºLCD´®ÐÐÊý¾ÝÐ´Èëº¯Êý
      Èë¿ÚÊý¾Ý£ºdat  ÒªÐ´ÈëµÄ´®ÐÐÊý¾Ý
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_Writ_Bus(uint8_t dat) 
{	
	uint8_t i;			  
	for(i=0;i<8;i++)
	{			  
		LED_SCLK_Clr();
		if(dat&0x80)
		   LED_SDIN_Set();
		else 
		   LED_SDIN_Clr();
		LED_SCLK_Set();
		dat<<=1; 
		// os_time_delay_ms(1);  
	}			
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºLCDÐ´ÈëÊý¾Ý
      Èë¿ÚÊý¾Ý£ºdat Ð´ÈëµÄÊý¾Ý
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_WR_DATA8(uint8_t dat)
{
	LED_DC_Set();//Ð´Êý¾Ý
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºLCDÐ´ÈëÊý¾Ý
      Èë¿ÚÊý¾Ý£ºdat Ð´ÈëµÄÊý¾Ý
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
	LED_DC_Set();//Ð´Êý¾Ý
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºLCDÐ´ÈëÃüÁî
      Èë¿ÚÊý¾Ý£ºdat Ð´ÈëµÄÃüÁî
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
	LED_DC_Clr();//Ð´ÃüÁî
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºÉèÖÃÆðÊ¼ºÍ½áÊøµØÖ·
      Èë¿ÚÊý¾Ý£ºx1,x2 ÉèÖÃÁÐµÄÆðÊ¼ºÍ½áÊøµØÖ·
                y1,y2 ÉèÖÃÐÐµÄÆðÊ¼ºÍ½áÊøµØÖ·
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
		LCD_WR_REG(0x2a);//ÁÐµØÖ·ÉèÖÃ
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//ÐÐµØÖ·ÉèÖÃ
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//´¢´æÆ÷Ð´
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºLCD³õÊ¼»¯º¯Êý
      Èë¿ÚÊý¾Ý£ºÎÞ
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void Lcd_Init(void)
{
	gpio_pad_select_gpio(LCD_SDA);
    gpio_pad_select_gpio(LCD_SCK);
    gpio_pad_select_gpio(LCD_DC);
    gpio_pad_select_gpio(LCD_RST);
    gpio_pad_select_gpio(LCD_BLK);
    gpio_pad_select_gpio(LCD_CS);

    gpio_set_direction(LCD_SDA, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_SCK, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_BLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_CS, GPIO_MODE_OUTPUT);

	LED_RST_Clr();
	os_time_delay_ms(20);
	LED_RST_Set();
	os_time_delay_ms(200);
	LED_BLK_Set();
	LED_CS_Clr();
	
	//************* Start Initial Sequence **********// 
	LCD_WR_REG(0xCF);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xC1);
	LCD_WR_DATA8(0X30);
	LCD_WR_REG(0xED);
	LCD_WR_DATA8(0x64);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0X12);
	LCD_WR_DATA8(0X81);
	LCD_WR_REG(0xE8);
	LCD_WR_DATA8(0x85);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x79);
	LCD_WR_REG(0xCB);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x02);
	LCD_WR_REG(0xF7);
	LCD_WR_DATA8(0x20);
	LCD_WR_REG(0xEA);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0xC0); //Power control
	LCD_WR_DATA8(0x1D); //VRH[5:0]
	LCD_WR_REG(0xC1); //Power control
	LCD_WR_DATA8(0x12); //SAP[2:0];BT[3:0]
	LCD_WR_REG(0xC5); //VCM control
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3F);
	LCD_WR_REG(0xC7); //VCM control
	LCD_WR_DATA8(0x92);
	LCD_WR_REG(0x3A); // Memory Access Control
	LCD_WR_DATA8(0x55);
	LCD_WR_REG(0x36); // Memory Access Control
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);
	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x12);
	LCD_WR_REG(0xB6); // Display Function Control
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0xA2);

	LCD_WR_REG(0x44);
	LCD_WR_DATA8(0x02);

	LCD_WR_REG(0xF2); // 3Gamma Function Disable
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0x26); //Gamma curve selected
	LCD_WR_DATA8(0x01);
	LCD_WR_REG(0xE0); //Set Gamma
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x22);
	LCD_WR_DATA8(0x1C);
	LCD_WR_DATA8(0x1B);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x48);
	LCD_WR_DATA8(0xB8);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0F);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x00);
	LCD_WR_REG(0XE1); //Set Gamma
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x23);
	LCD_WR_DATA8(0x24);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x47);
	LCD_WR_DATA8(0x4B);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x06);
	LCD_WR_DATA8(0x30);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x0F);
	LCD_WR_REG(0x11); //Exit Sleep
	os_time_delay_ms(120);
	LCD_WR_REG(0x29); //Display on
} 


/******************************************************************************
      º¯ÊýËµÃ÷£ºLCDÇåÆÁº¯Êý
      Èë¿ÚÊý¾Ý£ºÎÞ
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_Clear(uint16_t Color)
{
	uint16_t i,j;  	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	 {
	  for (j=0;j<LCD_H;j++)
	   	{
        	LCD_WR_DATA(Color);	 			 
	    }

	  }
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºLCDÏÔÊ¾ºº×Ö
      Èë¿ÚÊý¾Ý£ºx,y   ÆðÊ¼×ø±ê
                index ºº×ÖµÄÐòºÅ
                size  ×ÖºÅ
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/



/******************************************************************************
      º¯ÊýËµÃ÷£ºLCDÏÔÊ¾ºº×Ö
      Èë¿ÚÊý¾Ý£ºx,y   ÆðÊ¼×ø±ê
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_Address_Set(x,y,x,y);//ÉèÖÃ¹â±êÎ»ÖÃ 
	LCD_WR_DATA(color);
} 


/******************************************************************************
      º¯ÊýËµÃ÷£ºLCD»­Ò»¸ö´óµÄµã
      Èë¿ÚÊý¾Ý£ºx,y   ÆðÊ¼×ø±ê
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_DrawPoint_big(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_Fill(x-1,y-1,x+1,y+1,color);
} 


/******************************************************************************
      º¯ÊýËµÃ÷£ºÔÚÖ¸¶¨ÇøÓòÌî³äÑÕÉ«
      Èë¿ÚÊý¾Ý£ºxsta,ysta   ÆðÊ¼×ø±ê
                xend,yend   ÖÕÖ¹×ø±ê
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{          
	uint16_t i,j; 
	LCD_Address_Set(xsta,ysta,xend,yend);      //ÉèÖÃ¹â±êÎ»ÖÃ 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//ÉèÖÃ¹â±êÎ»ÖÃ 	    
	} 					  	    
}


/******************************************************************************
      º¯ÊýËµÃ÷£º»­Ïß
      Èë¿ÚÊý¾Ý£ºx1,y1   ÆðÊ¼×ø±ê
                x2,y2   ÖÕÖ¹×ø±ê
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //¼ÆËã×ø±êÔöÁ¿ 
	delta_y=y2-y1;
	uRow=x1;//»­ÏßÆðµã×ø±ê
	uCol=y1;
	if(delta_x>0)incx=1; //ÉèÖÃµ¥²½·½Ïò 
	else if (delta_x==0)incx=0;//´¹Ö±Ïß 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//Ë®Æ½Ïß 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //Ñ¡È¡»ù±¾ÔöÁ¿×ø±êÖá 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//»­µã
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t mode,uint16_t color)
{
    uint8_t temp;
    uint8_t pos,t;
	  uint16_t x0=x;     
    if(x>LCD_W-16||y>LCD_H-16)return;	    //ÉèÖÃ´°¿Ú		   
	num=num-' ';//µÃµ½Æ«ÒÆºóµÄÖµ
	LCD_Address_Set(x,y,x+8-1,y+16-1);      //ÉèÖÃ¹â±êÎ»ÖÃ 
	if(!mode) //·Çµþ¼Ó·½Ê½
	{
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(uint16_t)num*16+pos];		 //µ÷ÓÃ1608×ÖÌå
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_WR_DATA(color);
				    else LCD_WR_DATA(BACK_COLOR);
				    temp>>=1; 
				    x++;
		    }
			x=x0;
			y++;
		}	
	}else//µþ¼Ó·½Ê½
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(uint16_t)num*16+pos];		 //µ÷ÓÃ1608×ÖÌå
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//»­Ò»¸öµã     
		        temp>>=1; 
		    }
		}
	}   	   	 	  
}

void LCD_ShowBigDigit(uint16_t x,uint16_t y,uint8_t index,uint16_t color)	
{  
	uint16_t i,j;
	uint8_t *temp;
	uint16_t size1;
    temp=digit_4824;
    LCD_Address_Set(x,y,x+32-1,y+64-1); //ÉèÖÃÒ»¸öºº×ÖµÄÇøÓò
    size1=32*64/8;//Ò»¸öºº×ÖËùÕ¼µÄ×Ö½Ú
	temp+=index*size1;//Ð´ÈëµÄÆðÊ¼Î»ÖÃ
	for(j=0;j<size1;j++)
	{
		for(i=0;i<8;i++)
		{
		 	if((*temp&(1<<i))!=0)//´ÓÊý¾ÝµÄµÍÎ»¿ªÊ¼¶Á
			{
				LCD_WR_DATA(color);//µãÁÁ
			}
			else
			{
				LCD_WR_DATA(BACKCOL);//²»µãÁÁ
			}
		}
		temp++;
	}
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºÏÔÊ¾×Ö·û´®
      Èë¿ÚÊý¾Ý£ºx,y    Æðµã×ø±ê
                *p     ×Ö·û´®ÆðÊ¼µØÖ·
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_ShowString(uint16_t x,uint16_t y,const char *p,uint16_t color)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0,color);
        x+=8;
        p++;
    }  
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºÏÔÊ¾Êý×Ö
      Èë¿ÚÊý¾Ý£ºmµ×Êý£¬nÖ¸Êý
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
unsigned long mypow(uint8_t m,uint8_t n)
{
	unsigned long result=1;	 
	while(n--)result*=m;    
	return result;
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºÏÔÊ¾Êý×Ö
      Èë¿ÚÊý¾Ý£ºx,y    Æðµã×ø±ê
                num    ÒªÏÔÊ¾µÄÊý×Ö
                len    ÒªÏÔÊ¾µÄÊý×Ö¸öÊý
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_ShowNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t color)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0,color);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color); 
	}
} 

void LCD_ShowBigNum(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint16_t color)
{         	
	uint8_t t,temp;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		LCD_ShowBigDigit(x+t*32+5, y, temp, color);
	}
} 


/******************************************************************************
      º¯ÊýËµÃ÷£ºÏÔÊ¾Ð¡Êý
      Èë¿ÚÊý¾Ý£ºx,y    Æðµã×ø±ê
                num    ÒªÏÔÊ¾µÄÐ¡Êý
                len    ÒªÏÔÊ¾µÄÊý×Ö¸öÊý
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
void LCD_ShowNum1(uint16_t x,uint16_t y,float num,uint8_t len,uint16_t color)
{         	
	uint8_t t,temp;
	uint16_t num1;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+8*(len-2),y,'.',0,color);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color);
	}
}


/******************************************************************************
      º¯ÊýËµÃ÷£ºÏÔÊ¾40x40Í¼Æ¬
      Èë¿ÚÊý¾Ý£ºx,y    Æðµã×ø±ê
      ·µ»ØÖµ£º  ÎÞ
******************************************************************************/
// void LCD_ShowPicture(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, const uint8_t * imag)
// {
// 	long i;
// 	  LCD_Address_Set(x1,y1,x2,y2);
// 	  for(i=0;i<76800;i++)
// 	  { 	
// 			LCD_WR_DATA8(imag[i*2+1]);	 
// 			LCD_WR_DATA8(imag[i*2]);			
// 	  }			
// }

void LCD_ShowLog_start()
{
	long i;
	LCD_Address_Set(61, 110, 258, 129);
	for (i = 0; i < 3960; i++)
	{
		LCD_WR_DATA8(gImage_log[i * 2 + 1]);
		LCD_WR_DATA8(gImage_log[i * 2]);
	}
}

void LCD_ShowWifi_config()
{
	long i;
	LCD_Address_Set(0, 0, 319, 239);
	for (i = 0; i < 76800; i++)
	{
		LCD_WR_DATA8(gImage_wificonfig[i * 2 + 1]);
		LCD_WR_DATA8(gImage_wificonfig[i * 2]);
	}		
}

void LCD_ShowWifi_connected()
{
	long i;
	LCD_Address_Set(0, 0, 319, 239);
	for (i = 0; i < 76800; i++)
	{
		LCD_WR_DATA8(gImage_wificonnected[i * 2 + 1]);
		LCD_WR_DATA8(gImage_wificonnected[i * 2]);
	}		
}

void LCD_ShowBt_connecting()
{
	long i;
	LCD_Address_Set(0, 0, 319, 239);
	for (i = 0; i < 76800; i++)
	{
		LCD_WR_DATA8(gImage_bt_connecting[i * 2 + 1]);
		LCD_WR_DATA8(gImage_bt_connecting[i * 2]);
	}		
}

void LCD_ShowCheck_band()
{
	long i;
	LCD_Address_Set(0, 0, 319, 239);
	for (i = 0; i < 76800; i++)
	{
		LCD_WR_DATA8(gImage_Check_Band[i * 2 + 1]);
		LCD_WR_DATA8(gImage_Check_Band[i * 2]);
	}		
}

void LCD_ShowMain_Logo()
{
	long i;
	LCD_Address_Set(18, 16, 151, 31);
	for (i = 0; i < 2144; i++)
	{
		LCD_WR_DATA8(gImage_logo_mainframe[i * 2 + 1]);
		LCD_WR_DATA8(gImage_logo_mainframe[i * 2]);
	}		
}

void LCD_ShowMqtt_off()
{
	long i;
	LCD_Address_Set(249, 13, 270, 34);
	for (i = 0; i < 484; i++)
	{
		LCD_WR_DATA8(gImage_mqtt_off[i * 2 + 1]);
		LCD_WR_DATA8(gImage_mqtt_off[i * 2]);
	}		
}

void LCD_ShowMqtt_on()
{
	long i;
	LCD_Address_Set(249, 13, 270, 34);
	for (i = 0; i < 484; i++)
	{
		LCD_WR_DATA8(gImage_mqtt_on[i * 2 + 1]);
		LCD_WR_DATA8(gImage_mqtt_on[i * 2]);
	}		
}


void LCD_ShowWifi_on()
{
	long i;
	LCD_Address_Set(280, 13, 305, 34);
	for (i = 0; i < 572; i++)
	{
		LCD_WR_DATA8(gImage_wifi_on[i * 2 + 1]);
		LCD_WR_DATA8(gImage_wifi_on[i * 2]);
	}		
}

void LCD_ShowFocus()
{
	long i;
	LCD_Address_Set(18, 210, 69, 225);
	for (i = 0; i < 832; i++)
	{
		LCD_WR_DATA8(gImage_Focus[i * 2 + 1]);
		LCD_WR_DATA8(gImage_Focus[i * 2]);
	}		
}

void LCD_ShowCalm()
{
	long i;
	LCD_Address_Set(18, 210, 69, 225);
	for (i = 0; i < 832; i++)
	{
		LCD_WR_DATA8(gImage_calm[i * 2 + 1]);
		LCD_WR_DATA8(gImage_calm[i * 2]);
	}		
}


void LCD_ShowMaster()
{
	long i;
	LCD_Address_Set(238, 210, 302, 225);
	for (i = 0; i < 1040; i++)
	{
		LCD_WR_DATA8(gImage_master[i * 2 + 1]);
		LCD_WR_DATA8(gImage_master[i * 2]);
	}		
}

void LCD_ShowSlave()
{
	long i;
	LCD_Address_Set(238, 210, 302, 225);
	for (i = 0; i < 1040; i++)
	{
		LCD_WR_DATA8(gImage_slave[i * 2 + 1]);
		LCD_WR_DATA8(gImage_slave[i * 2]);
	}		
}


void LCD_ShowClosed() 
{
	long i;
	LCD_Address_Set(98, 210, 157, 225);
	for (i = 0; i < 960; i++)
	{
		LCD_WR_DATA8(gImage_closed[i * 2 + 1]);
		LCD_WR_DATA8(gImage_closed[i * 2]);
	}
}

void LCD_ShowOpen() 
{
	long i;
	LCD_Address_Set(98, 210, 157, 225);
	for (i = 0; i < 960; i++)
	{
		LCD_WR_DATA8(gImage_open[i * 2 + 1]);
		LCD_WR_DATA8(gImage_open[i * 2]);
	}
}

void LCD_ShowTest() 
{
	long i;
	LCD_Address_Set(180, 210, 217, 225);
	for (i = 0; i < 608; i++)
	{
		LCD_WR_DATA8(gImage_test[i * 2 + 1]);
		LCD_WR_DATA8(gImage_test[i * 2]);
	}
}

void LCD_ShowLive() 
{
	long i;
	LCD_Address_Set(180, 210, 217, 225);
	for (i = 0; i < 608; i++)
	{
		LCD_WR_DATA8(gImage_live[i * 2 + 1]);
		LCD_WR_DATA8(gImage_live[i * 2]);
	}
}


void LCD_ShowATT_val(uint8_t attval)
{
	LCD_ShowBigNum(20, 56, (uint16_t)attval, 3, CALMCOL);
}

void LCD_ShowMED_val(uint8_t medval)
{
	LCD_ShowBigNum(20, 120, (uint16_t)medval, 3, MEDCOL);
}

void LCD_ShowHigh_val(uint8_t highval)
{
	LCD_ShowBigNum(190, 56, (uint16_t)highval, 3, LGRAY);
}

void LCD_ShowLow_val(uint8_t lowval)
{
	LCD_ShowBigNum(190, 120, (uint16_t)lowval, 3, LGRAY);
}

void LCD_ShowDelaytime(uint8_t time)
{
	uint8_t tmp1 = time/60;
	uint8_t tmp2 = time%60;
	LCD_ShowNum(190, 16, tmp1, 1, GREEN);
	LCD_ShowChar(200, 16, ':', 0, GREEN);
	LCD_ShowNum(210, 16, tmp2, 2, GREEN);

}

// void LCD_Display_process(uint8_t state)
void LCD_Display_process()
{
	// if (state == stWifiCONFIG)
	// {
    //    LCD_ShowWifi_config();
	// }
	// else if (state == stBTCON)
	// {
    //    LCD_ShowBt_connecting();
	// }
	// else if (state == stCHBAND)
	// {
    //    LCD_ShowCheck_band();
	// }
	// else
	// {
        LCD_ShowMain_Logo();
        LCD_DrawLine(0, 47, 319, 47, DGRAY);
        LCD_DrawLine(0, 48, 319, 48, DGRAY);
        LCD_DrawLine(0, 189, 319, 189, DGRAY);
        LCD_DrawLine(0, 190, 319, 190, DGRAY);
        LCD_DrawLine(160, 69, 160, 169, DGRAY);
        LCD_DrawLine(161, 69, 161, 169, DGRAY);
		if (wifi_state == 1)
		{
           LCD_ShowWifi_on();
		}
		else
		{
           LCD_ShowWifi_on();
		}
		if (mqtt_state == 1)
		{
           LCD_ShowMqtt_off();
		}
		else
		{
           LCD_ShowMqtt_off();
		}
		if (get_masterfunc() == 1)
		{
			LCD_ShowMaster();
		}
		else
		{
			LCD_ShowSlave();
		}
		if (get_protocol() == 1)
		{
			LCD_ShowFocus();
		}
		else
		{
			LCD_ShowCalm();
		}
		if (get_testfunc() == 1)
		{
			LCD_ShowTest();
			LCD_Fill(190, 16, 230, 32, BACKCOL);
		}
		else
		{
			LCD_ShowLive();
			LCD_ShowDelaytime(test_val);
		}
		if (get_actmode() == 1)
		{
			LCD_ShowOpen();
		}
		else
		{
			LCD_ShowClosed(); 
		}  
        LCD_ShowATT_val(cur_att);
        LCD_ShowMED_val(cur_med);
        LCD_ShowHigh_val(high_setlev);
        LCD_ShowLow_val(low_setlev);
	// }
}


