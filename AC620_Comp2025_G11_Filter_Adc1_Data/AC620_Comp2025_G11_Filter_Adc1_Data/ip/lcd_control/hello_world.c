#include "lcd9341.h"
#include <stdio.h>
#include "001.h"
#include "002.h"
#include "003.h"


int main()
{
	printf("Hello from Nios II!\n");

	uint16_t x=0;
 	LCD9341_Init();
	POINT_COLOR=GREEN;
	BACK_COLOR=BLACK;
	LCD_Clear(BLACK);


	LCD_printf(&lcd_printf, "Thank you for your trust!\n");
	usleep(2000000);
	LCD_printf(&lcd_printf, "Preparing for Work...\n");
	usleep(2000000);
	LCD_printf(&lcd_printf, "FPGA : EP4CE10F17C8!\n");
	usleep(500000);
	LCD_printf(&lcd_printf, "SDRAM : 128Mbit\n");
	usleep(500000);
	LCD_printf(&lcd_printf, "FLASH : 16Mbit\n");
	usleep(500000);
	LCD_printf(&lcd_printf, "ADC : 8 Channels\n");
	LCD_printf(&lcd_printf, "      12bit resolution\n");
	LCD_printf(&lcd_printf, "      500Ksps\n");
	usleep(500000);
	LCD_printf(&lcd_printf, "DAC : 2 Channels\n");
	LCD_printf(&lcd_printf, "      12bit resolution\n");
	LCD_printf(&lcd_printf, "      1Msps\n");
	usleep(2000000);
	LCD_printf(&lcd_printf, "Let's Show Picture");
	usleep(2000000);
  	while(1)
	{
		switch(x)
		{
			case 0:LCD_ShowPic(gImage_001);break;
			case 1:LCD_ShowPic(gImage_002);break;
			case 2:LCD_ShowPic(gImage_003);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break;

			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;
		LCD_ShowString(30,50,"AC620");
		LCD_ShowString(30,70,"NIOS II CPU Based");
		LCD_ShowString(30,90,"xiaomeige");
		LCD_ShowString(30,110,"2016/11/14");
	    x++;
		if(x==3)x=0;
		//LED0=!LED0;
		usleep(1000000);
	}
}
