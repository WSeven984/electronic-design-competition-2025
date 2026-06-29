#include "lcd9341.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "font.h"
#include "unistd.h"
#include "altera_avalon_pio_regs.h"

//画笔颜色,背景颜色
uint16_t POINT_COLOR = 0x0000, BACK_COLOR = 0xFFFF;
uint16_t DeviceCode;

//写寄存器
//LCD_Reg:寄存器编号
//LCD_RegValue:要写入的值
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue) {
	LCD_WR_REG(LCD_Reg);
	LCD_WR_DATA(LCD_RegValue);
}

//读寄存器
//LCD_Reg:寄存器编号
//返回值:读到的值
uint16_t LCD_ReadReg(uint8_t LCD_Reg) {
	LCD_WR_REG(LCD_Reg); //写入要读的寄存器号
	return LCD_RD_DATA();
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void) {
	if (DeviceCode == 0X9341)
		LCD_WR_REG(0x2C); //ILI9341是以0X2C开始的
	else
		LCD_WR_REG(R34);
}

//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code) {
	LCD_WR_DATA(RGB_Code);
	//写十六位GRAM
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
uint16_t LCD_BGR2RGB(uint16_t c) {
	uint16_t r, g, b, rgb;
	b = (c >> 0) & 0x1f;
	g = (c >> 5) & 0x3f;
	r = (c >> 11) & 0x1f;
	rgb = (b << 11) + (g << 5) + (r << 0);
	return (rgb);
}
//读取个某点的颜色值
//x:0~239
//y:0~319
//返回值:此点的颜色
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y) {
	uint16_t r, g, b;
	if (x >= LCD_W || y >= LCD_H)
		return 0; //超过了范围,直接返回
	LCD_SetCursor(x, y);
	if (DeviceCode == 0X9341)
		LCD_WR_REG(0X2E); //ILI9341发送读GRAM指令
	else
		LCD_WR_REG(R34); //其他IC发送读GRAM指令

	r = IORD_16DIRECT(LCD_BASE, 2);
	r = IORD_16DIRECT(LCD_BASE, 2);
	if (DeviceCode == 0X9341) {
		b = IORD_16DIRECT(LCD_BASE, 2);//读取蓝色值
		g = r & 0XFF; //对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
		g <<= 8;
	}
	//全部输出高
	if (DeviceCode == 0X9325 || DeviceCode == 0X4535 || DeviceCode == 0X4531
			|| DeviceCode == 0X8989 || DeviceCode == 0XB505)
		return r; //这几种IC直接返回颜色值
	else if (DeviceCode == 0X9341)
		return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); //ILI9341需要公式转换一下
	else
		return LCD_BGR2RGB(r); //其他IC
}

//LCD开启显示
void LCD_DisplayOn(void) {
	if (DeviceCode == 0X9341)
		LCD_WR_REG(0X29); //开启显示
	else
		LCD_WriteReg(R7, 0x0173); //开启显示
}
//LCD关闭显示
void LCD_DisplayOff(void) {
	if (DeviceCode == 0X9341)
		LCD_WR_REG(0X28); //关闭显示
	else
		LCD_WriteReg(R7, 0x0); //关闭显示
}
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
__inline void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos) {
#if USE_HORIZONTAL==1
	if(DeviceCode==0X8989)
	{
		Xpos=319-Xpos;
		LCD_WriteReg(0X4E, Ypos);
		LCD_WriteReg(0X4F, Xpos);
	} else if(DeviceCode==0X9341) //9341,设置坐标
	{
		LCD_WR_REG(0x2B);
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF);
		LCD_WR_REG(0x2A);
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF);
	} else
	{
		Xpos=319-Xpos;
		LCD_WriteReg(R32,Ypos);
		LCD_WriteReg(R33,Xpos);
	}
#else
	if (DeviceCode == 0X8989) {
		LCD_WriteReg(0X4E, Xpos);
		LCD_WriteReg(0X4F, Ypos);
	} else if (DeviceCode == 0X9341) {
		LCD_WR_REG(0x2A);
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF);
		LCD_WR_REG(0x2B);
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF);
	} else {
		LCD_WriteReg(R32, Xpos);
		LCD_WriteReg(R33, Ypos);
	}
#endif
}
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341这个奇葩),所以,
//一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//0~7：代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341等IC已经实际测试
void LCD_Scan_Dir(uint8_t dir) {
	uint16_t regval = 0;
	uint8_t dirreg = 0;
#if USE_HORIZONTAL//使用横屏
	switch(dir) //方向转换
	{
		case 0:dir=6;break;
		case 1:dir=7;break;
		case 2:dir=4;break;
		case 3:dir=5;break;
		case 4:dir=1;break;
		case 5:dir=0;break;
		case 6:dir=3;break;
		case 7:dir=2;break;
	}
#endif
	if (DeviceCode == 0x9341) //9341,很特殊
			{
		switch (dir) {
		case L2R_U2D: //从左到右,从上到下
			regval |= (0 << 7) | (0 << 6) | (0 << 5);
			break;
		case L2R_D2U: //从左到右,从下到上
			regval |= (1 << 7) | (0 << 6) | (0 << 5);
			break;
		case R2L_U2D: //从右到左,从上到下
			regval |= (0 << 7) | (1 << 6) | (0 << 5);
			break;
		case R2L_D2U: //从右到左,从下到上
			regval |= (1 << 7) | (1 << 6) | (0 << 5);
			break;
		case U2D_L2R: //从上到下,从左到右
			regval |= (0 << 7) | (0 << 6) | (1 << 5);
			break;
		case U2D_R2L: //从上到下,从右到左
			regval |= (0 << 7) | (1 << 6) | (1 << 5);
			break;
		case D2U_L2R: //从下到上,从左到右
			regval |= (1 << 7) | (0 << 6) | (1 << 5);
			break;
		case D2U_R2L: //从下到上,从右到左
			regval |= (1 << 7) | (1 << 6) | (1 << 5);
			break;
		}
		dirreg = 0X36;
		regval |= 0X08; //BGR
		LCD_WriteReg(dirreg, regval);
		if (regval & 0X20) {
			LCD_WR_REG(0x2A);
			LCD_WR_DATA(0);
			LCD_WR_DATA(0);
			LCD_WR_DATA(319>>8);
			LCD_WR_DATA(319&0XFF);
			LCD_WR_REG(0x2B);
			LCD_WR_DATA(0);
			LCD_WR_DATA(0);
			LCD_WR_DATA(239>>8);
			LCD_WR_DATA(239&0XFF);
		} else {
			LCD_WR_REG(0x2A);
			LCD_WR_DATA(0);
			LCD_WR_DATA(0);
			LCD_WR_DATA(239>>8);
			LCD_WR_DATA(239&0XFF);
			LCD_WR_REG(0x2B);
			LCD_WR_DATA(0);
			LCD_WR_DATA(0);
			LCD_WR_DATA(319>>8);
			LCD_WR_DATA(319&0XFF);
		}
	} else {
		switch (dir) {
		case L2R_U2D: //从左到右,从上到下
			regval |= (1 << 5) | (1 << 4) | (0 << 3);
			break;
		case L2R_D2U: //从左到右,从下到上
			regval |= (0 << 5) | (1 << 4) | (0 << 3);
			break;
		case R2L_U2D: //从右到左,从上到下
			regval |= (1 << 5) | (0 << 4) | (0 << 3);
			break;
		case R2L_D2U: //从右到左,从下到上
			regval |= (0 << 5) | (0 << 4) | (0 << 3);
			break;
		case U2D_L2R: //从上到下,从左到右
			regval |= (1 << 5) | (1 << 4) | (1 << 3);
			break;
		case U2D_R2L: //从上到下,从右到左
			regval |= (1 << 5) | (0 << 4) | (1 << 3);
			break;
		case D2U_L2R: //从下到上,从左到右
			regval |= (0 << 5) | (1 << 4) | (1 << 3);
			break;
		case D2U_R2L: //从下到上,从右到左
			regval |= (0 << 5) | (0 << 4) | (1 << 3);
			break;
		}
		if (DeviceCode == 0x8989) //8989 IC
				{
			dirreg = 0X11;
			regval |= 0X6040; //65K
		} else //其他驱动IC
		{
			dirreg = 0X03;
			regval |= 1 << 12;
		}
		LCD_WriteReg(dirreg, regval);
	}
}
//画点
//x:0~239
//y:0~319
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x, uint16_t y) {
	LCD_SetCursor(x, y); //设置光标位置
	LCD_WriteRAM_Prepare(); //开始写入GRAM
	LCD_WR_DATA(POINT_COLOR);
}
//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试!
void LCD9341_Init(void) {
	usleep(50000); // delay 50 ms
	LCD_WriteReg(0x0000, 0x0001);
	usleep(50000); // delay 50 ms
	DeviceCode = LCD_ReadReg(0x0000);
	if (DeviceCode == 0x9325) //9325初始化
			{
		LCD_WriteReg(0x00E5, 0x78F0);
		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0700);
		LCD_WriteReg(0x0003, 0x1030);
		LCD_WriteReg(0x0004, 0x0000);
		LCD_WriteReg(0x0008, 0x0202);
		LCD_WriteReg(0x0009, 0x0000);
		LCD_WriteReg(0x000A, 0x0000);
		LCD_WriteReg(0x000C, 0x0000);
		LCD_WriteReg(0x000D, 0x0000);
		LCD_WriteReg(0x000F, 0x0000);
		//power on sequence VGHVGL
		LCD_WriteReg(0x0010, 0x0000);
		LCD_WriteReg(0x0011, 0x0007);
		LCD_WriteReg(0x0012, 0x0000);
		LCD_WriteReg(0x0013, 0x0000);
		LCD_WriteReg(0x0007, 0x0000);
		//vgh
		LCD_WriteReg(0x0010, 0x1690);
		LCD_WriteReg(0x0011, 0x0227);
		//delayms(100);
		//vregiout
		LCD_WriteReg(0x0012, 0x009D); //0x001b
		//delayms(100);
		//vom amplitude
		LCD_WriteReg(0x0013, 0x1900);
		//delayms(100);
		//vom H
		LCD_WriteReg(0x0029, 0x0025);
		LCD_WriteReg(0x002B, 0x000D);
		//gamma
		LCD_WriteReg(0x0030, 0x0007);
		LCD_WriteReg(0x0031, 0x0303);
		LCD_WriteReg(0x0032, 0x0003); // 0006
		LCD_WriteReg(0x0035, 0x0206);
		LCD_WriteReg(0x0036, 0x0008);
		LCD_WriteReg(0x0037, 0x0406);
		LCD_WriteReg(0x0038, 0x0304); //0200
		LCD_WriteReg(0x0039, 0x0007);
		LCD_WriteReg(0x003C, 0x0602); // 0504
		LCD_WriteReg(0x003D, 0x0008);
		//ram
		LCD_WriteReg(0x0050, 0x0000);
		LCD_WriteReg(0x0051, 0x00EF);
		LCD_WriteReg(0x0052, 0x0000);
		LCD_WriteReg(0x0053, 0x013F);
		LCD_WriteReg(0x0060, 0xA700);
		LCD_WriteReg(0x0061, 0x0001);
		LCD_WriteReg(0x006A, 0x0000);
		//
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		//
		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0600);

		LCD_WriteReg(0x0007, 0x0133);
		LCD_WriteReg(0x00, 0x0022); //
	} else if (DeviceCode == 0x9328) //ILI9328
			{
		LCD_WriteReg(0x00e7, 0x0010);
		LCD_WriteReg(0x0000, 0x0001); //开启内部时钟
		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0700); //电源开启
		//LCD_WriteReg(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常就用这个.
		// 1  1   1	   U->D	L->R
		LCD_WriteReg(0x0003, (1 << 12) | (3 << 4) | (0 << 3)); //65K
		LCD_WriteReg(0x0004, 0x0000);
		LCD_WriteReg(0x0008, 0x0207);
		LCD_WriteReg(0x0009, 0x0000);
		LCD_WriteReg(0x000a, 0x0000); //display setting
		LCD_WriteReg(0x000c, 0x0001); //display setting
		LCD_WriteReg(0x000d, 0x0000); //0f3c
		LCD_WriteReg(0x000f, 0x0000);
		//电源配置
		LCD_WriteReg(0x0010, 0x0000);
		LCD_WriteReg(0x0011, 0x0007);
		LCD_WriteReg(0x0012, 0x0000);
		LCD_WriteReg(0x0013, 0x0000);
		usleep(50000);
		LCD_WriteReg(0x0010, 0x1590);
		LCD_WriteReg(0x0011, 0x0227);
		usleep(50000);
		LCD_WriteReg(0x0012, 0x009c);
		usleep(50000);
		LCD_WriteReg(0x0013, 0x1900);
		LCD_WriteReg(0x0029, 0x0023);
		LCD_WriteReg(0x002b, 0x000e);
		usleep(50000);
		LCD_WriteReg(0x0020, 0x0000);
		LCD_WriteReg(0x0021, 0x013f);
		usleep(50000);
		//伽马校正
		LCD_WriteReg(0x0030, 0x0007);
		LCD_WriteReg(0x0031, 0x0707);
		LCD_WriteReg(0x0032, 0x0006);
		LCD_WriteReg(0x0035, 0x0704);
		LCD_WriteReg(0x0036, 0x1f04);
		LCD_WriteReg(0x0037, 0x0004);
		LCD_WriteReg(0x0038, 0x0000);
		LCD_WriteReg(0x0039, 0x0706);
		LCD_WriteReg(0x003c, 0x0701);
		LCD_WriteReg(0x003d, 0x000f);
		usleep(50000);
		LCD_WriteReg(0x0050, 0x0000); //水平GRAM起始位置
		LCD_WriteReg(0x0051, 0x00ef); //水平GRAM终止位置
		LCD_WriteReg(0x0052, 0x0000); //垂直GRAM起始位置
		LCD_WriteReg(0x0053, 0x013f); //垂直GRAM终止位置

		LCD_WriteReg(0x0060, 0xa700);
		LCD_WriteReg(0x0061, 0x0001);
		LCD_WriteReg(0x006a, 0x0000);
		LCD_WriteReg(0x0080, 0x0000);
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);

		LCD_WriteReg(0x0090, 0x0010);
		LCD_WriteReg(0x0092, 0x0000);
		LCD_WriteReg(0x0093, 0x0003);
		LCD_WriteReg(0x0095, 0x0110);
		LCD_WriteReg(0x0097, 0x0000);
		LCD_WriteReg(0x0098, 0x0000);
		//开启显示设置
		LCD_WriteReg(0x0007, 0x0133);
		LCD_WriteReg(0x0020, 0x0000);
		LCD_WriteReg(0x0021, 0x013f);
	} else if (DeviceCode == 0x9320 || DeviceCode == 0x9300) {
		LCD_WriteReg(0x00, 0x0000);
		LCD_WriteReg(0x01, 0x0100); //Driver Output Contral.
		LCD_WriteReg(0x02, 0x0700); //LCD Driver Waveform Contral.
		LCD_WriteReg(0x03, 0x1030); //Entry Mode Set.
		//LCD_WriteReg(0x03,0x1018);	//Entry Mode Set.

		LCD_WriteReg(0x04, 0x0000); //Scalling Contral.
		LCD_WriteReg(0x08, 0x0202); //Display Contral 2.(0x0207)
		LCD_WriteReg(0x09, 0x0000); //Display Contral 3.(0x0000)
		LCD_WriteReg(0x0a, 0x0000); //Frame Cycle Contal.(0x0000)
		LCD_WriteReg(0x0c, (1 << 0)); //Extern Display Interface Contral 1.(0x0000)
		LCD_WriteReg(0x0d, 0x0000); //Frame Maker Position.
		LCD_WriteReg(0x0f, 0x0000); //Extern Display Interface Contral 2.
		usleep(50000);
		LCD_WriteReg(0x07, 0x0101); //Display Contral.
		usleep(50000);
		LCD_WriteReg(0x10,
				(1 << 12) | (0 << 8) | (1 << 7) | (1 << 6) | (0 << 4)); //Power Control 1.(0x16b0)
		LCD_WriteReg(0x11, 0x0007); //Power Control 2.(0x0001)
		LCD_WriteReg(0x12, (1 << 8) | (1 << 4) | (0 << 0)); //Power Control 3.(0x0138)
		LCD_WriteReg(0x13, 0x0b00); //Power Control 4.
		LCD_WriteReg(0x29, 0x0000); //Power Control 7.

		LCD_WriteReg(0x2b, (1 << 14) | (1 << 4));
		LCD_WriteReg(0x50, 0); //Set X Star
		//水平GRAM终止位置Set X End.
		LCD_WriteReg(0x51, 239); //Set Y Star
		LCD_WriteReg(0x52, 0); //Set Y End.t.
		LCD_WriteReg(0x53, 319); //

		LCD_WriteReg(0x60, 0x2700); //Driver Output Control.
		LCD_WriteReg(0x61, 0x0001); //Driver Output Control.
		LCD_WriteReg(0x6a, 0x0000); //Vertical Srcoll Control.

		LCD_WriteReg(0x80, 0x0000); //Display Position? Partial Display 1.
		LCD_WriteReg(0x81, 0x0000); //RAM Address Start? Partial Display 1.
		LCD_WriteReg(0x82, 0x0000); //RAM Address End-Partial Display 1.
		LCD_WriteReg(0x83, 0x0000); //Displsy Position? Partial Display 2.
		LCD_WriteReg(0x84, 0x0000); //RAM Address Start? Partial Display 2.
		LCD_WriteReg(0x85, 0x0000); //RAM Address End? Partial Display 2.

		LCD_WriteReg(0x90, (0 << 7) | (16 << 0)); //Frame Cycle Contral.(0x0013)
		LCD_WriteReg(0x92, 0x0000); //Panel Interface Contral 2.(0x0000)
		LCD_WriteReg(0x93, 0x0001); //Panel Interface Contral 3.
		LCD_WriteReg(0x95, 0x0110); //Frame Cycle Contral.(0x0110)
		LCD_WriteReg(0x97, (0 << 8)); //
		LCD_WriteReg(0x98, 0x0000); //Frame Cycle Contral.
		LCD_WriteReg(0x07, 0x0173); //(0x0173)
	} else if (DeviceCode == 0x5408) {
		LCD_WriteReg(0x01, 0x0100);
		LCD_WriteReg(0x02, 0x0700); //LCD Driving Waveform Contral
		LCD_WriteReg(0x03, 0x1030); //Entry Mode设置
		//指针从左至右自上而下的自动增模式
		//Normal Mode(Window Mode disable)
		//RGB格式
		//16位数据2次传输的8总线设置
		LCD_WriteReg(0x04, 0x0000); //Scalling Control register
		LCD_WriteReg(0x08, 0x0207); //Display Control 2
		LCD_WriteReg(0x09, 0x0000); //Display Control 3
		LCD_WriteReg(0x0A, 0x0000); //Frame Cycle Control
		LCD_WriteReg(0x0C, 0x0000); //External Display Interface Control 1
		LCD_WriteReg(0x0D, 0x0000); //Frame Maker Position
		LCD_WriteReg(0x0F, 0x0000); //External Display Interface Control 2
		usleep(20000);
		//TFT 液晶彩色图像显示方法14
		LCD_WriteReg(0x10, 0x16B0); //0x14B0 //Power Control 1
		LCD_WriteReg(0x11, 0x0001); //0x0007 //Power Control 2
		LCD_WriteReg(0x17, 0x0001); //0x0000 //Power Control 3
		LCD_WriteReg(0x12, 0x0138); //0x013B //Power Control 4
		LCD_WriteReg(0x13, 0x0800); //0x0800 //Power Control 5
		LCD_WriteReg(0x29, 0x0009); //NVM read data 2
		LCD_WriteReg(0x2a, 0x0009); //NVM read data 3
		LCD_WriteReg(0xa4, 0x0000);
		LCD_WriteReg(0x50, 0x0000); //设置操作窗口的X轴开始列
		LCD_WriteReg(0x51, 0x00EF); //设置操作窗口的X轴结束列
		LCD_WriteReg(0x52, 0x0000); //设置操作窗口的Y轴开始行
		LCD_WriteReg(0x53, 0x013F); //设置操作窗口的Y轴结束行
		LCD_WriteReg(0x60, 0x2700); //Driver Output Control
		//设置屏幕的点数以及扫描的起始行
		LCD_WriteReg(0x61, 0x0001); //Driver Output Control
		LCD_WriteReg(0x6A, 0x0000); //Vertical Scroll Control
		LCD_WriteReg(0x80, 0x0000); //Display Position – Partial Display 1
		LCD_WriteReg(0x81, 0x0000); //RAM Address Start – Partial Display 1
		LCD_WriteReg(0x82, 0x0000); //RAM address End - Partial Display 1
		LCD_WriteReg(0x83, 0x0000); //Display Position – Partial Display 2
		LCD_WriteReg(0x84, 0x0000); //RAM Address Start – Partial Display 2
		LCD_WriteReg(0x85, 0x0000); //RAM address End – Partail Display2
		LCD_WriteReg(0x90, 0x0013); //Frame Cycle Control
		LCD_WriteReg(0x92, 0x0000); //Panel Interface Control 2
		LCD_WriteReg(0x93, 0x0003); //Panel Interface control 3
		LCD_WriteReg(0x95, 0x0110); //Frame Cycle Control
		LCD_WriteReg(0x07, 0x0173);
		usleep(50000);
	} else if (DeviceCode == 0x1505) {
		// second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
		LCD_WriteReg(0x0007, 0x0000);
		usleep(50000);
		LCD_WriteReg(0x0012, 0x011C); //0x011A   why need to set several times?
		LCD_WriteReg(0x00A4, 0x0001); //NVM
		LCD_WriteReg(0x0008, 0x000F);
		LCD_WriteReg(0x000A, 0x0008);
		LCD_WriteReg(0x000D, 0x0008);
		//伽马校正
		LCD_WriteReg(0x0030, 0x0707);
		LCD_WriteReg(0x0031, 0x0007); //0x0707
		LCD_WriteReg(0x0032, 0x0603);
		LCD_WriteReg(0x0033, 0x0700);
		LCD_WriteReg(0x0034, 0x0202);
		LCD_WriteReg(0x0035, 0x0002); //?0x0606
		LCD_WriteReg(0x0036, 0x1F0F);
		LCD_WriteReg(0x0037, 0x0707); //0x0f0f  0x0105
		LCD_WriteReg(0x0038, 0x0000);
		LCD_WriteReg(0x0039, 0x0000);
		LCD_WriteReg(0x003A, 0x0707);
		LCD_WriteReg(0x003B, 0x0000); //0x0303
		LCD_WriteReg(0x003C, 0x0007); //?0x0707
		LCD_WriteReg(0x003D, 0x0000); //0x1313//0x1f08
		usleep(50000);
		LCD_WriteReg(0x0007, 0x0001);
		LCD_WriteReg(0x0017, 0x0001); //开启电源
		usleep(50000);
		//电源配置
		LCD_WriteReg(0x0010, 0x17A0);
		LCD_WriteReg(0x0011, 0x0217); //reference voltage VC[2:0]   Vciout = 1.00*Vcivl
		LCD_WriteReg(0x0012, 0x011E); //0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
		LCD_WriteReg(0x0013, 0x0F00); //VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
		LCD_WriteReg(0x002A, 0x0000);
		LCD_WriteReg(0x0029, 0x000A); //0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
		LCD_WriteReg(0x0012, 0x013E); // 0x013C  power supply on
		//Coordinates Control//
		LCD_WriteReg(0x0050, 0x0000); //0x0e00
		LCD_WriteReg(0x0051, 0x00EF);
		LCD_WriteReg(0x0052, 0x0000);
		LCD_WriteReg(0x0053, 0x013F);
		//Pannel Image Control//
		LCD_WriteReg(0x0060, 0x2700);
		LCD_WriteReg(0x0061, 0x0001);
		LCD_WriteReg(0x006A, 0x0000);
		LCD_WriteReg(0x0080, 0x0000);
		//Partial Image Control//
		LCD_WriteReg(0x0081, 0x0000);
		LCD_WriteReg(0x0082, 0x0000);
		LCD_WriteReg(0x0083, 0x0000);
		LCD_WriteReg(0x0084, 0x0000);
		LCD_WriteReg(0x0085, 0x0000);
		//Panel Interface Control//
		LCD_WriteReg(0x0090, 0x0013); //0x0010 frenqucy
		LCD_WriteReg(0x0092, 0x0300);
		LCD_WriteReg(0x0093, 0x0005);
		LCD_WriteReg(0x0095, 0x0000);
		LCD_WriteReg(0x0097, 0x0000);
		LCD_WriteReg(0x0098, 0x0000);

		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0700);
		LCD_WriteReg(0x0003, 0x1030);
		LCD_WriteReg(0x0004, 0x0000);
		LCD_WriteReg(0x000C, 0x0000);
		LCD_WriteReg(0x000F, 0x0000);
		LCD_WriteReg(0x0020, 0x0000);
		LCD_WriteReg(0x0021, 0x0000);
		LCD_WriteReg(0x0007, 0x0021);
		usleep(20000);
		LCD_WriteReg(0x0007, 0x0061);
		usleep(20000);
		LCD_WriteReg(0x0007, 0x0173);
		usleep(20000);
	} else if (DeviceCode == 0xB505) {
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x0000, 0x0000);

		LCD_WriteReg(0x00a4, 0x0001);
		usleep(20000);
		LCD_WriteReg(0x0060, 0x2700);
		LCD_WriteReg(0x0008, 0x0202);

		LCD_WriteReg(0x0030, 0x0214);
		LCD_WriteReg(0x0031, 0x3715);
		LCD_WriteReg(0x0032, 0x0604);
		LCD_WriteReg(0x0033, 0x0e16);
		LCD_WriteReg(0x0034, 0x2211);
		LCD_WriteReg(0x0035, 0x1500);
		LCD_WriteReg(0x0036, 0x8507);
		LCD_WriteReg(0x0037, 0x1407);
		LCD_WriteReg(0x0038, 0x1403);
		LCD_WriteReg(0x0039, 0x0020);

		LCD_WriteReg(0x0090, 0x001a);
		LCD_WriteReg(0x0010, 0x0000);
		LCD_WriteReg(0x0011, 0x0007);
		LCD_WriteReg(0x0012, 0x0000);
		LCD_WriteReg(0x0013, 0x0000);
		usleep(20000);

		LCD_WriteReg(0x0010, 0x0730);
		LCD_WriteReg(0x0011, 0x0137);
		usleep(20000);

		LCD_WriteReg(0x0012, 0x01b8);
		usleep(20000);

		LCD_WriteReg(0x0013, 0x0f00);
		LCD_WriteReg(0x002a, 0x0080);
		LCD_WriteReg(0x0029, 0x0048);
		usleep(20000);

		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0700);
		LCD_WriteReg(0x0003, 0x1230);
		LCD_WriteReg(0x0008, 0x0202);
		LCD_WriteReg(0x000a, 0x0000);
		LCD_WriteReg(0x000c, 0x0000);
		LCD_WriteReg(0x000d, 0x0000);
		LCD_WriteReg(0x000e, 0x0030);
		LCD_WriteReg(0x0050, 0x0000);
		LCD_WriteReg(0x0051, 0x00ef);
		LCD_WriteReg(0x0052, 0x0000);
		LCD_WriteReg(0x0053, 0x013f);
		LCD_WriteReg(0x0060, 0x2700);
		LCD_WriteReg(0x0061, 0x0001);
		LCD_WriteReg(0x006a, 0x0000);
		//LCD_WriteReg(0x0080,0x0000);
		//LCD_WriteReg(0x0081,0x0000);
		LCD_WriteReg(0x0090, 0X0011);
		LCD_WriteReg(0x0092, 0x0600);
		LCD_WriteReg(0x0093, 0x0402);
		LCD_WriteReg(0x0094, 0x0002);
		usleep(20000);

		LCD_WriteReg(0x0007, 0x0001);
		usleep(20000);
		LCD_WriteReg(0x0007, 0x0061);
		LCD_WriteReg(0x0007, 0x0173);

		LCD_WriteReg(0x0020, 0x0000);
		LCD_WriteReg(0x0021, 0x0000);
		LCD_WriteReg(0x00, 0x22);
	} else if (DeviceCode == 0xC505) {
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x0000, 0x0000);
		usleep(20000);
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x0000, 0x0000);
		LCD_WriteReg(0x00a4, 0x0001);
		usleep(20000);
		LCD_WriteReg(0x0060, 0x2700);
		LCD_WriteReg(0x0008, 0x0806);

		LCD_WriteReg(0x0030, 0x0703); //gamma setting
		LCD_WriteReg(0x0031, 0x0001);
		LCD_WriteReg(0x0032, 0x0004);
		LCD_WriteReg(0x0033, 0x0102);
		LCD_WriteReg(0x0034, 0x0300);
		LCD_WriteReg(0x0035, 0x0103);
		LCD_WriteReg(0x0036, 0x001F);
		LCD_WriteReg(0x0037, 0x0703);
		LCD_WriteReg(0x0038, 0x0001);
		LCD_WriteReg(0x0039, 0x0004);

		LCD_WriteReg(0x0090, 0x0015); //80Hz
		LCD_WriteReg(0x0010, 0X0410); //BT,AP
		LCD_WriteReg(0x0011, 0x0247); //DC1,DC0,VC
		LCD_WriteReg(0x0012, 0x01BC);
		LCD_WriteReg(0x0013, 0x0e00);
		usleep(120000);
		LCD_WriteReg(0x0001, 0x0100);
		LCD_WriteReg(0x0002, 0x0200);
		LCD_WriteReg(0x0003, 0x1030);

		LCD_WriteReg(0x000A, 0x0008);
		LCD_WriteReg(0x000C, 0x0000);

		LCD_WriteReg(0x000E, 0x0020);
		LCD_WriteReg(0x000F, 0x0000);
		LCD_WriteReg(0x0020, 0x0000); //H Start
		LCD_WriteReg(0x0021, 0x0000); //V Start
		LCD_WriteReg(0x002A, 0x003D); //vcom2
		usleep(20000);
		LCD_WriteReg(0x0029, 0x002d);
		LCD_WriteReg(0x0050, 0x0000);
		LCD_WriteReg(0x0051, 0xD0EF);
		LCD_WriteReg(0x0052, 0x0000);
		LCD_WriteReg(0x0053, 0x013F);
		LCD_WriteReg(0x0061, 0x0000);
		LCD_WriteReg(0x006A, 0x0000);
		LCD_WriteReg(0x0092, 0x0300);

		LCD_WriteReg(0x0093, 0x0005);
		LCD_WriteReg(0x0007, 0x0100);
	} else if (DeviceCode == 0x8989) {
		LCD_WriteReg(0x0000, 0x0001); //打开晶振
		LCD_WriteReg(0x0003, 0xA8A4); //0xA8A4
		LCD_WriteReg(0x000C, 0x0000);
		LCD_WriteReg(0x000D, 0x080C);
		LCD_WriteReg(0x000E, 0x2B00);
		LCD_WriteReg(0x001E, 0x00B0);
		LCD_WriteReg(0x0001, 0x2B3F); //驱动输出控制320*240  0x6B3F
		LCD_WriteReg(0x0002, 0x0600);
		LCD_WriteReg(0x0010, 0x0000);
		LCD_WriteReg(0x0011, 0x6070); //定义数据格式  16位色 		横屏 0x6058
		LCD_WriteReg(0x0005, 0x0000);
		LCD_WriteReg(0x0006, 0x0000);
		LCD_WriteReg(0x0016, 0xEF1C);
		LCD_WriteReg(0x0017, 0x0003);
		LCD_WriteReg(0x0007, 0x0233); //0x0233
		LCD_WriteReg(0x000B, 0x0000);
		LCD_WriteReg(0x000F, 0x0000); //扫描开始地址
		LCD_WriteReg(0x0041, 0x0000);
		LCD_WriteReg(0x0042, 0x0000);
		LCD_WriteReg(0x0048, 0x0000);
		LCD_WriteReg(0x0049, 0x013F);
		LCD_WriteReg(0x004A, 0x0000);
		LCD_WriteReg(0x004B, 0x0000);
		LCD_WriteReg(0x0044, 0xEF00);
		LCD_WriteReg(0x0045, 0x0000);
		LCD_WriteReg(0x0046, 0x013F);
		LCD_WriteReg(0x0030, 0x0707);
		LCD_WriteReg(0x0031, 0x0204);
		LCD_WriteReg(0x0032, 0x0204);
		LCD_WriteReg(0x0033, 0x0502);
		LCD_WriteReg(0x0034, 0x0507);
		LCD_WriteReg(0x0035, 0x0204);
		LCD_WriteReg(0x0036, 0x0204);
		LCD_WriteReg(0x0037, 0x0502);
		LCD_WriteReg(0x003A, 0x0302);
		LCD_WriteReg(0x003B, 0x0302);
		LCD_WriteReg(0x0023, 0x0000);
		LCD_WriteReg(0x0024, 0x0000);
		LCD_WriteReg(0x0025, 0x8000);
		LCD_WriteReg(0x004f, 0); //行首址0
		LCD_WriteReg(0x004e, 0); //列首址0
	} else if (DeviceCode == 0x4531) {
		LCD_WriteReg(0X00, 0X0001);
		usleep(10000);
		LCD_WriteReg(0X10, 0X1628);
		LCD_WriteReg(0X12, 0X000e); //0x0006
		LCD_WriteReg(0X13, 0X0A39);
		usleep(10000);
		LCD_WriteReg(0X11, 0X0040);
		LCD_WriteReg(0X15, 0X0050);
		usleep(10000);
		LCD_WriteReg(0X12, 0X001e); //16
		usleep(10000);
		LCD_WriteReg(0X10, 0X1620);
		LCD_WriteReg(0X13, 0X2A39);
		usleep(10000);
		LCD_WriteReg(0X01, 0X0100);
		LCD_WriteReg(0X02, 0X0300);
		LCD_WriteReg(0X03, 0X1030); //改变方向的
		LCD_WriteReg(0X08, 0X0202);
		LCD_WriteReg(0X0A, 0X0008);
		LCD_WriteReg(0X30, 0X0000);
		LCD_WriteReg(0X31, 0X0402);
		LCD_WriteReg(0X32, 0X0106);
		LCD_WriteReg(0X33, 0X0503);
		LCD_WriteReg(0X34, 0X0104);
		LCD_WriteReg(0X35, 0X0301);
		LCD_WriteReg(0X36, 0X0707);
		LCD_WriteReg(0X37, 0X0305);
		LCD_WriteReg(0X38, 0X0208);
		LCD_WriteReg(0X39, 0X0F0B);
		LCD_WriteReg(0X41, 0X0002);
		LCD_WriteReg(0X60, 0X2700);
		LCD_WriteReg(0X61, 0X0001);
		LCD_WriteReg(0X90, 0X0210);
		LCD_WriteReg(0X92, 0X010A);
		LCD_WriteReg(0X93, 0X0004);
		LCD_WriteReg(0XA0, 0X0100);
		LCD_WriteReg(0X07, 0X0001);
		LCD_WriteReg(0X07, 0X0021);
		LCD_WriteReg(0X07, 0X0023);
		LCD_WriteReg(0X07, 0X0033);
		LCD_WriteReg(0X07, 0X0133);
		LCD_WriteReg(0XA0, 0X0000);
	} else if (DeviceCode == 0x4535) {
		LCD_WriteReg(0X15, 0X0030);
		LCD_WriteReg(0X9A, 0X0010);
		LCD_WriteReg(0X11, 0X0020);
		LCD_WriteReg(0X10, 0X3428);
		LCD_WriteReg(0X12, 0X0002); //16
		LCD_WriteReg(0X13, 0X1038);
		usleep(40000);
		LCD_WriteReg(0X12, 0X0012); //16
		usleep(40000);
		LCD_WriteReg(0X10, 0X3420);
		LCD_WriteReg(0X13, 0X3038);
		usleep(40000);
		LCD_WriteReg(0X30, 0X0000);
		LCD_WriteReg(0X31, 0X0402);
		LCD_WriteReg(0X32, 0X0307);
		LCD_WriteReg(0X33, 0X0304);
		LCD_WriteReg(0X34, 0X0004);
		LCD_WriteReg(0X35, 0X0401);
		LCD_WriteReg(0X36, 0X0707);
		LCD_WriteReg(0X37, 0X0305);
		LCD_WriteReg(0X38, 0X0610);
		LCD_WriteReg(0X39, 0X0610);

		LCD_WriteReg(0X01, 0X0100);
		LCD_WriteReg(0X02, 0X0300);
		LCD_WriteReg(0X03, 0X1030); //改变方向的
		LCD_WriteReg(0X08, 0X0808);
		LCD_WriteReg(0X0A, 0X0008);
		LCD_WriteReg(0X60, 0X2700);
		LCD_WriteReg(0X61, 0X0001);
		LCD_WriteReg(0X90, 0X013E);
		LCD_WriteReg(0X92, 0X0100);
		LCD_WriteReg(0X93, 0X0100);
		LCD_WriteReg(0XA0, 0X3000);
		LCD_WriteReg(0XA3, 0X0010);
		LCD_WriteReg(0X07, 0X0001);
		LCD_WriteReg(0X07, 0X0021);
		LCD_WriteReg(0X07, 0X0023);
		LCD_WriteReg(0X07, 0X0033);
		LCD_WriteReg(0X07, 0X0133);
	} else {
		//可能是9341,尝试9341的ID读取
		LCD_WR_REG(0XD3);
		LCD_RD_DATA(); //dummy read
		LCD_RD_DATA(); //读回0X00
		DeviceCode = LCD_RD_DATA() & 0XBF; //这里读回0XD3,实际是0X93才对.强制去掉第6位
		DeviceCode <<= 8;
		DeviceCode |= LCD_RD_DATA();
	}
	if (DeviceCode == 0X9341) //9341初始化
			{
		LCD_WR_REG(0xCF);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xC1);
		LCD_WR_DATA(0X30);
		LCD_WR_REG(0xED);
		LCD_WR_DATA(0x64);
		LCD_WR_DATA(0x03);
		LCD_WR_DATA(0X12);
		LCD_WR_DATA(0X81);
		LCD_WR_REG(0xE8);
		LCD_WR_DATA(0x85);
		LCD_WR_DATA(0x10);
		LCD_WR_DATA(0x7A);
		LCD_WR_REG(0xCB);
		LCD_WR_DATA(0x39);
		LCD_WR_DATA(0x2C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x34);
		LCD_WR_DATA(0x02);
		LCD_WR_REG(0xF7);
		LCD_WR_DATA(0x20);
		LCD_WR_REG(0xEA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0xC0); //Power control
		LCD_WR_DATA(0x1B);
		//VRH[5:0]
		LCD_WR_REG(0xC1); //Power control
		LCD_WR_DATA(0x01);
		//SAP[2:0];BT[3:0]
		LCD_WR_REG(0xC5); //VCM control
		LCD_WR_DATA(0x30);
		//3F
		LCD_WR_DATA(0x30);
		//3C
		LCD_WR_REG(0xC7); //VCM control2
		LCD_WR_DATA(0XB7);
		LCD_WR_REG(0x36); // Memory Access Control
		LCD_WR_DATA(0x48);
		LCD_WR_REG(0x3A);
		LCD_WR_DATA(0x55);
		LCD_WR_REG(0xB1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x1A);
		LCD_WR_REG(0xB6); // Display Function Control
		LCD_WR_DATA(0x0A);
		LCD_WR_DATA(0xA2);
		LCD_WR_REG(0xF2); // 3Gamma Function Disable
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0x26); //Gamma curve selected
		LCD_WR_DATA(0x01);
		LCD_WR_REG(0xE0); //Set Gamma
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x2A);
		LCD_WR_DATA(0x28);
		LCD_WR_DATA(0x08);
		LCD_WR_DATA(0x0E);
		LCD_WR_DATA(0x08);
		LCD_WR_DATA(0x54);
		LCD_WR_DATA(0XA9);
		LCD_WR_DATA(0x43);
		LCD_WR_DATA(0x0A);
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0XE1); //Set Gamma
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x15);
		LCD_WR_DATA(0x17);
		LCD_WR_DATA(0x07);
		LCD_WR_DATA(0x11);
		LCD_WR_DATA(0x06);
		LCD_WR_DATA(0x2B);
		LCD_WR_DATA(0x56);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x05);
		LCD_WR_DATA(0x10);
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x3F);
		LCD_WR_DATA(0x3F);
		LCD_WR_DATA(0x0F);
		LCD_WR_REG(0x2B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);
		LCD_WR_REG(0x11); //Exit Sleep
		usleep(120000);
		LCD_WR_REG(0x29); //display on
	}
	LCD_Scan_Dir(DFT_SCAN_DIR);
	IOWR_ALTERA_AVALON_PIO_DATA(LCD_BL_BASE, 1);
	//点亮背光
	LCD_Clear(WHITE);
}

//清屏函数
//Color:要清屏的填充色
void LCD_Clear(uint16_t Color) {
	uint32_t index = 0;
	LCD_SetCursor(0x00, 0x0000); //设置光标位置
	LCD_WriteRAM_Prepare(); //开始写入GRAM
	for (index = 0; index < 76800; index++) {
		LCD_WR_DATA(Color);
	}
}

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey,
		uint16_t color) {
	uint16_t i, j;
	uint16_t xlen = 0;
	LCD_Scan_Dir(L2R_U2D);
#if USE_HORIZONTAL==1
	xlen=ey-sy+1;
	for(i=sx;i<=ex;i++)
	{
		LCD_SetCursor(i,sy); //设置光标位置
		LCD_WriteRAM_Prepare();//开始写入GRAM
		for(j=0;j<xlen;j++)LCD_WR_DATA(color);//设置光标位置
	}
#else
	xlen = ex - sx + 1;
	for (i = sy; i <= ey; i++) {
		LCD_SetCursor(sx, i); //设置光标位置
		LCD_WriteRAM_Prepare(); //开始写入GRAM
		for (j = 0; j < xlen; j++)
			LCD_WR_DATA(color);
		//设置光标位置
	}
#endif
	LCD_Scan_Dir(DFT_SCAN_DIR);
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	uint16_t t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;

	delta_x = x2 - x1; //计算坐标增量
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;
	if (delta_x > 0)
		incx = 1; //设置单步方向
	else if (delta_x == 0)
		incx = 0; //垂直线
	else {
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; //水平线
	else {
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; //选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) //画线输出
			{
		LCD_DrawPoint(uRow, uCol); //画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance) {
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance) {
			yerr -= distance;
			uCol += incy;
		}
	}
}
//画矩形
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	LCD_DrawLine(x1, y1, x2, y1);
	LCD_DrawLine(x1, y1, x1, y2);
	LCD_DrawLine(x1, y2, x2, y2);
	LCD_DrawLine(x2, y1, x2, y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r) {
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - (r << 1); //判断下个点位置的标志
	while (a <= b) {
		LCD_DrawPoint(x0 - b, y0 - a); //3
		LCD_DrawPoint(x0 + b, y0 - a); //0
		LCD_DrawPoint(x0 - a, y0 + b); //1
		LCD_DrawPoint(x0 - b, y0 - a); //7
		LCD_DrawPoint(x0 - a, y0 - b); //2
		LCD_DrawPoint(x0 + b, y0 + a); //4
		LCD_DrawPoint(x0 + a, y0 - b); //5
		LCD_DrawPoint(x0 + a, y0 + b); //6
		LCD_DrawPoint(x0 - b, y0 + a);
		a++;
		//使用Bresenham算法画圆
		if (di < 0)
			di += 4 * a + 6;
		else {
			di += 10 + 4 * (a - b);
			b--;
		}
		LCD_DrawPoint(x0 + a, y0 + b);
	}
}
//在指定位置显示一个字符
//x:0~234
//y:0~308
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
//在指定位置显示一个字符
//x:0~234
//y:0~308
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size,
		uint8_t mode) {
#if USE_HORIZONTAL==1
#define MAX_CHAR_POSX 312
#define MAX_CHAR_POSY 232
#else
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 312
#endif
	uint8_t temp;
	uint8_t pos, t;
	uint16_t x0 = x;
	uint16_t colortemp = POINT_COLOR;
	if (x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
		return;
	//设置窗口
	num = num - ' '; //得到偏移后的值
	if (!mode) //非叠加方式
	{
		for (pos = 0; pos < size; pos++) {
			if (size == 12)
				temp = asc2_1206[num][pos]; //调用1206字体
			else
				temp = asc2_1608[num][pos]; //调用1608字体
			for (t = 0; t < size / 2; t++) {
				if (temp & 0x01)
					POINT_COLOR = colortemp;
				else
					POINT_COLOR = BACK_COLOR;
				LCD_DrawPoint(x, y);
				temp >>= 1;
				x++;
			}
			x = x0;
			y++;
		}
	} else //叠加方式
	{
		for (pos = 0; pos < size; pos++) {
			if (size == 12)
				temp = asc2_1206[num][pos]; //调用1206字体
			else
				temp = asc2_1608[num][pos]; //调用1608字体
			for (t = 0; t < size / 2; t++) {
				if (temp & 0x01)
					LCD_DrawPoint(x + t, y + pos); //画一个点
				temp >>= 1;
			}
		}
	}
	POINT_COLOR = colortemp;
}
//m^n函数
uint32_t mypow(uint8_t m, uint8_t n) {
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}
//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len,
		uint8_t size) {
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++) {
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1)) {
			if (temp == 0) {
				LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
				continue;
			} else
				enshow = 1;

		}
		LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
	}
}
//显示2个数字
//x,y:起点坐标
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~99);
void LCD_Show2Num(uint16_t x, uint16_t y, uint16_t num, uint8_t len,
		uint8_t size, uint8_t mode) {
	uint8_t t, temp;
	for (t = 0; t < len; t++) {
		temp = (num / mypow(10, len - t - 1)) % 10;
		LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode);
	}
}
//显示字符串
//x,y:起点坐标
//*p:字符串起始地址
//用16字体
void LCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p) {
	while (*p != '\0') {
		if (x > MAX_CHAR_POSX) {
			x = 0;
			y += 16;
		}
		if (y > MAX_CHAR_POSY) {
			y = x = 0;
			LCD_Clear(WHITE);
		}
		LCD_ShowChar(x, y, *p, 16, 0);
		x += 8;
		p++;
	}
}

void LCD_ShowPic(uint8_t *p) {
	int i;
	LCD_SetCursor(0, 0);
	LCD_WriteRAM_Prepare(); //开始写入GRAM
	for (i = 0; i < 320 * 240 * 2;) {
		LCD_WR_DATA((p[i]<<8)|p[i+1]);
		i = i + 2;
	}
}


/*******************************************************
 *@功能描述:用size大小的字体显示中文
 *@输入参数: alt_u16 x         : 显示左上角的x坐标
 *			alt_u16 y         : 显示左上角的y坐标
 *			alt_u16 size      : 字体大小 输入24则为24*24,48则为48*48，不一定要求是12的整数倍
 *			alt_u8 overlay    : 是否叠加显示
 *			char    *p        : 字模数组指针
 *			alt_u8  num       ： 显示第几个字
 *@返回值  :0:正常, 1:画矩形失败
*******************************************************/
alt_u8 LCD_ShowChinese(alt_u16 x,alt_u16 y,alt_u16 size,alt_u8 overlay,alt_u8 num,const unsigned char *p)
{
	alt_u8 temp;
	alt_u16 i,j,line_byte,line_bit; //line_byte表示一个汉字一列在数组中几个字节,line_bit表示占多少位
	unsigned char * show ;

	line_byte= (alt_u16)((size+7)/8);//由于取模软件1字节对齐，不足8位按8位算
	line_bit = line_byte*8;
	show = (unsigned char*)p;
	show = show+ num*size*line_byte;
	if( x > (LCD_W - size) || y > (LCD_H - size))
		return 1;

	//为了提高显示效率，显示时，取模软件在最后一字节自动填充的0不考虑，只显示有价值的点
	for(i=0;i<size;i++)
	{
		for(j=0;j<line_bit;j++)
		{
			if(j%8 == 0)
				temp = *show;
			else if( ((j+1)%8)== 0 )
				show++;

			if(j<size)//为了提高显示效率，显示时，取模软件在最后一字节自动填充的0不考虑，只显示有价值的点
			{
				if( temp&0x80 )
					LCD_DrawPoint(x+i, y+j);//按列取模先加y
				else if(overlay == 0)
					LCD_DrawPoint(x+i, y+j);//不叠加显示就用背景色打点

				temp = temp<<1;
			}
		}
	}
	return 0;
}

/*******************************************************
 *@功能描述: 滚动显示中文
 *@输入参数: alt_u16 x         : 显示左上角的x坐标
 *			alt_u16 y         : 显示左上角的y坐标
 *			alt_u16 size      : 字体大小 输入24则为24*24,48则为48*48，不一定要求是12的整数倍
 *			alt_u16 total     : 全部要显示的字数
 *			alt_u16 num       ： 滚动的起始地址在原来第几列
 *			char    *p        : 字模数组指针
 *
 *@返回值  :0:正常, 1:画矩形失败
*******************************************************/
alt_u8 LCD_MoveChinese(alt_u16 x,alt_u16 y,alt_u16 size,alt_u16 total,alt_u16 num,const unsigned char *p)
{
	alt_u8 temp;
	alt_u16 i,j,line_byte,line_bit; //v_byte表示一个汉字一列在数组中几个字节

	unsigned char * show ;

	line_byte= (alt_u16)((size+7)/8);//垂直方向几个Byte，不足一个字节按一个字节计算
	line_bit = line_byte*8;//垂直方向几个bit,不足一个字节按一个字节计算
	show = (unsigned char*)p;
	num = (num % (total*size));
	show = show+ num*line_byte;

	if( x > (LCD_W - size) || y >(LCD_H - size) )
		return 1;

	//为了提高显示效率，显示时，取模软件在最后一字节自动填充的0不考虑，只显示有价值的点
	for(i=0;i<size*total;i++)
	{
		for(j=0;j<line_bit;j++)
		{
			if(j%8 == 0)
				temp = *show;
			else if( ((j+1)%8)== 0 )
				show++;

			if(show >= (unsigned char*)p + line_byte*size*total)
				show =(unsigned char*)p;

			if(j<size)//为了提高显示效率，显示时，取模软件在最后一字节自动填充的0不考虑，只显示有价值的点
			{
				if( temp&0x80 )
					LCD_DrawPoint(x+i, y+j);//按列取模先加y
				else
					LCD_DrawPoint(x+i, y+j);//不叠加显示就用背景色打点

				temp = temp<<1;
			}
		}
	}
	return 0;
}

/***********printf格式化输出到液晶相关函数***********/
_Typedef_lcdprintf lcd_printf =
{
	0,  //光标当前位置
	0,
	0,  //设置显示起始位置
	0,
	7,  //最小宽度，默认最小7位宽度
	3,  //精度，默认3位小数精度
	0,  //显示模式，不叠加显示
	0,  //输出字符清零
};

alt_u32 Equation_Calculate(alt_u8 x, alt_u8 n)
{
	alt_u32 dat=1;
	while(n--)	dat *= x;  //n个x相乘
	return dat;
}

/**
  *****************************************************************************
  * @功能描述: 将数字转换成字符串
  *
  * @输入参数: arr:    要转换的数字
  * 		  strout: 字符串输出缓存
  * @返回值   : 字符串输出指针
  *****************************************************************************
**/
alt_u8 *Array_To_String(alt_u32 arr, alt_u8* strout)
{
	alt_u8 tmp=0,i=0;
	alt_u8 rec=0;

	for(i = 0;i < 10;i++)
	{
		tmp = (arr / (Equation_Calculate(10, 10-i-1))) % 10;    //取得数字的最高位
		if(rec==0 && i<9)  //最后一位不管是不是0都转换
		{
			if(tmp == 0)	continue;  //高位为0不转换
			else  rec = 1;  //不需要转换了
		}
		*strout = tmp + 0x30;
		strout++;
	}
	*strout = '\0';	//加入结束符

	return strout;
}

/**
  *****************************************************************************
  * @功能描述: 获取数值长度
  *			: 数字0返回的位数为0，即输入的数值为0。高位为0的不计算在内，默认从高位开始计算
  *
  * @输入参数: Num: 要检测的数值。数值范围: 0 ~ 4294967295
  * @返回值   : 数值长度，默认最大返回10位
  *****************************************************************************
**/
#define MAX_NUM_LEN          10    //最大返回位数
alt_u8 Get_NumLen(alt_u32 Num)
{
	alt_u8 i=0;
	alt_u8 tmp=0;

	i = MAX_NUM_LEN - 1;    //得到返回位数
	while(1)
	{
		tmp = (Num/Equation_Calculate(10, i)) % 10;    //得到位数
		if(tmp || i==0)
			break;    //完成
		i--;
	}
	     if(i)    return i+1;    //返回数值位数
	else if(tmp)  return 1;    //输入的数值只有一位
	else          return 0;    //输入的是0
}


#define ASCII_HEIGHT 16		//显示字符高度
#define ASCII_WIDTH   8		//显示字符宽度
/**
  *****************************************************************************
  * @功能描述: printf输出字符
  *			:
  *
  * @输入参数: lcd_ch: printf结构体指针
  * 		  ch:     要输出的字符
  *
  * @返回值   : none
  *****************************************************************************
**/
void _printfch(_Typedef_lcdprintf* lcd_ch, const char ch)
{
	if(ch == '\r')  //LF
	{
		lcd_ch->cursorx  = 0;
		lcd_ch->cursory += ASCII_HEIGHT;
		if(lcd_ch->cursory >= LCD_H)
			lcd_ch->cursory = 0;
	}
	else if(ch == '\n')  //CR
	{
		lcd_ch->cursorx  = lcd_ch->cursorx0 - ASCII_WIDTH;
		lcd_ch->cursory += ASCII_HEIGHT;
		if(lcd_ch->cursory >= LCD_H)
			lcd_ch->cursory = 0;
	}
	else if(ch == '\t')  //Tab
	{
		lcd_ch->cursorx += (ASCII_WIDTH * 3);  //默认4个字符宽度
		if(lcd_ch->cursorx >= LCD_W)  lcd_ch->cursorx = 0;
	}
	else  //en
	{
		LCD_ShowChar(lcd_ch->cursorx, lcd_ch->cursory,ch,16,0);
		lcd_ch->charcnt++;
	}
}

/**
  *****************************************************************************
  * @功能描述: printf输出字符
  *			:
  *
  * @输入参数: lcd_st: printf结构体指针
  * 		  *str:   要输出的字符串
  *
  * @返回值   : none
  *****************************************************************************
**/
void _printfstr(_Typedef_lcdprintf* lcd_st, const char *str)
{
	for(;*str != '\0';str++)
	{
		if(*str < 0x80)  //en
		{
			_printfch(lcd_st, *str);
			lcd_st->cursorx += ASCII_WIDTH;
			if(lcd_st->cursorx >= LCD_W)  lcd_st->cursory += ASCII_HEIGHT;
			if(lcd_st->cursory >= LCD_H)  {lcd_st->cursory = 0;lcd_st->cursorx = 0;}
		}
	}
}

/**
  *****************************************************************************
  * @Name   : printf输出数字
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf结构体指针
  *           dec:    要输出的数字
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfint(_Typedef_lcdprintf* lcd_dec, const int dec)
{
	char st[10];
	int tmp = dec;

	if(tmp == 0)  return;
	else if(tmp < 0)  //负数
	{
		tmp = -tmp;
		_printfch(lcd_dec, '-');
		lcd_dec->cursorx += ASCII_WIDTH;
	}

	Array_To_String    ((alt_u32)tmp, (alt_u8*)st);
	_printfstr(lcd_dec, (const char *)st);
}

/**
  *****************************************************************************
  * @Name   : printf输出浮点数
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf结构体指针
  *           flt:    要输出的浮点数
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfflt(_Typedef_lcdprintf* lcd_dec, const float flt)
{
	int pre=0;
	int flt_int=0;
	int flt_flo=0;
	float tmp=flt;

	if(tmp < 0)
	{
		tmp = -tmp;
		_printfch(lcd_dec, '-');
		lcd_dec->cursorx += ASCII_WIDTH;
	}

	pre = (int)Equation_Calculate(10, lcd_dec->precision);  //得到小数精度
	flt_int = (int)(tmp);  //整数
	flt_flo = (int)((tmp - flt_int) * pre);  //小数

	pre = (int)Equation_Calculate(10, (lcd_dec->min_width - lcd_dec->precision - 1));

	_printfint(lcd_dec, flt_int%pre);  //显示整数位
	_printfch(lcd_dec, '.');
	lcd_dec->cursorx += ASCII_WIDTH;
	_printfint(lcd_dec, flt_flo);  //显示小数位
}

/**
  *****************************************************************************
  * @Name   : printf输出十六进制数
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf结构体指针
  *           Hex:    要输出的十六进制数
  *           index:  要转换的进制数。16: 十六进制；8: 八进制
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfhex(_Typedef_lcdprintf* lcd_hex, const int Hex, int index)
{
	char HexIndex[16] = "0123456789ABCDEF";
	char str[16];
	char tmp=0;
	int i = 0,j = 0;
	int temp=Hex;

	do
	{
		str[i++] = HexIndex[temp%index];
		temp /= index;
	}while(temp);
	str[i] = '\0';

	//字符串倒序
	for(j = 0;j <(i-1)/2;j++)
	{
		tmp = str[j];
		str[j] = str[i-1-j];
		str[i-1-j] = tmp;
	}

	if(index == 16)  //十六进制
	{
		_printfch(lcd_hex, '0');lcd_hex->cursorx += ASCII_WIDTH;
		_printfch(lcd_hex, 'x');lcd_hex->cursorx += ASCII_WIDTH;
	}
	else if(index == 8)  //八进制
	{
		_printfch(lcd_hex, '0');lcd_hex->cursorx += ASCII_WIDTH;
	}
	_printfstr(lcd_hex, (const char *)str);
}


/**
  *****************************************************************************
  * @功能描述 : printf格式化输出到液晶
  *
  * @输入参数 : lcd_ch: printf结构体指针
  *           format: 格式化字符串
  *           ...:    长参数
  *
  *
  * @返回值   : none
  *****************************************************************************
**/
int LCD_printf(_Typedef_lcdprintf* lcd_pr, const alt_u8 *format, ...)
{
    const char *str;
    char c=0;
    float valflt=0;
    int dec=0;
    alt_u8 index=0;

    va_list ap;
    va_start(ap, format);

    lcd_pr->min_width = 7;
    lcd_pr->precision = 3;
    lcd_pr->charcnt   = 0;

    while(*format != '\0')  //直到遇到结束符
    {
        if(*format == 0x5c)  //符号'\'
        {
            format++;
            switch(*format)
            {
                case 'r':  //LF
                        _printfch(lcd_pr, '\r');
                        format++;
                        break;

                case 'n':  //CR
                        _printfch(lcd_pr, '\n');
                        format++;
                        break;

                case 't':  //Tab
                        _printfch(lcd_pr, '\t');
                        format++;
                        break;

                default:
                        format++;
                        break;
            }
        }
        else if(*format == '%')  //带格式输出
        {
            format++;
            index = 0;
            lcd_pr->min_width = 0;
            while((*format <= '9') && (*format >= '0'))  //域宽
            {
                lcd_pr->min_width = (lcd_pr->min_width * index * 10) + (*format - '0');
                index++;
                format++;
            }
            if(*format == '.')  //检测到点
            {
                format++;
                index = 0;
                lcd_pr->precision = 0;
                while((*format <= '9') && (*format >= '0'))  //精度
                {
                    lcd_pr->precision = (lcd_pr->precision * index * 10) + (*format - '0');
                    index++;
                    format++;
                }
            }
            switch(*format)  //检测格式码
            {
                case '%':  //输出%符号
                        _printfch(lcd_pr, '%');
                        lcd_pr->cursorx += ASCII_WIDTH;
                        format++;
                        break;

                case 'c':  //输出字符
                        c = va_arg(ap, int);
                        _printfch(lcd_pr, c);
                        format++;
                        break;

                case 'S':
                case 's':  //输出字符串
                        str = va_arg(ap, const char *);
                        _printfstr(lcd_pr, (char*)str);
                        format++;
                        break;

                case 'd':  //输出十进制数
                        dec = va_arg(ap, int);
                        _printfint(lcd_pr, dec);
                        format++;
                        break;

                case 'f':  //输出浮点数
                        valflt = va_arg(ap, double);
                        _printfflt(lcd_pr, valflt);
                        format++;
                        break;

                case 'X':
                case 'x':  //输出十六进制数
                        dec = va_arg(ap, int);
                        _printfhex(lcd_pr, dec, 16);
                        format++;
                        break;

                case 'o':  //输出八进制数
                        dec = va_arg(ap, int);
                        _printfhex(lcd_pr, dec, 8);
                        format++;
                        break;

                case 'm':
                        dec = va_arg(ap, int);
                        if(dec > 21)  dec = 21;  //限制最大的域宽是21，包括小数点
                        lcd_pr->min_width = (alt_u16)dec;
                        format++;
                        if(*format == '.')
                        {
                            format++;
                            if(*format == 'n')
                            {
                                dec = va_arg(ap, int);
                                if(dec > 8)  dec = 8;  //限制最小的小数精度是，0.00000001
                                lcd_pr->precision = (alt_u16)dec;
                                format++;
                                if(*format == 'f')
                                {
                                    valflt = va_arg(ap, double);
                                    _printfflt(lcd_pr, valflt);
                                    format++;
                                }
                            }
                        }
                        break;

                default:
                        _printfch(lcd_pr, *format);
                        format++;
                        break;
            }  //end check format
        }
        else if(*format < 0x80)  //字符输出
        {
            _printfch(lcd_pr, *format);
            lcd_pr->cursorx += ASCII_WIDTH;  //下一个字符位置--暂时使用16
            format++;
        }
        //由于没有SD卡字库，暂时不支持中文显示
    }//end while
    va_end(ap);

    return (int)lcd_pr->charcnt;
}

