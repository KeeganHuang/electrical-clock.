 /* 
 *****************************************
			1602 液晶驱动模块
 *****************************************
 */

#include<reg52.h>

#define LCD_DB P0

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
	
sbit LCD_RS = P1^0;
sbit LCD_RW = P1^1;
sbit LCD_E = P1^5;

void Reset1602()		// 重新配置LCD1602 向LCD1602写入命令
{
	void WriteCmd(uchar cmd);
	
	WriteCmd(0X38);
	WriteCmd(0X0C);
	WriteCmd(0X06);
	WriteCmd(0X01);
}

void WriteCmd(uchar cmd)	 	// 写入命令 配置 ———— 底层
{
	void WaitReady();
	
	WaitReady();
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_DB = cmd;
	LCD_E = 1;
	LCD_E = 0;

}

void WaitReady()		// 	判断“忙”，并决策 “等待” 或 “执行”    change something.		
{
	uchar str;
	
	LCD_DB = 0XFF;
	LCD_RS = 0;
	LCD_RW = 1;
	
	do
	{
		LCD_E = 1;
		str = LCD_DB;
		LCD_E = 0;
	}while(str & 0x80);
	
}

void ShowStr(uchar x,uchar y,uchar *str)		// 传地址 + 传数据
{
	void SetLoc(uchar x,uchar y);
	void WriteData(uchar dat);
	
	SetLoc(x,y);
	
	while(*str != '\0')
	{
			WriteData(*str++);
	}
}

void SetLoc(uchar x,uchar y)		// 写入的【数据】地址 配置 ———— 底层
{
	uchar addr;
	
	if(y == 0)
	{
			addr = 0x80 + 0x00 + x;
	}
	else
	{
			addr = 0x80 + 0x40 + x;
	}
	WriteCmd(addr);
}

void WriteData(uchar dat)		// 向 LCD1602 上写入数据 配置 ———— 底层
{
	
	WaitReady();
	
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_DB = dat;
	LCD_E = 1;
	LCD_E = 0;
}

void LCDgetTwk()	// 打开闪烁
{
	WriteCmd(0x0F);
}

void LCDstopTwk()	// 关闭闪速
{
	WriteCmd(0x0C);
}
	
/*
void ClearAll()
{
	WriteCmd(0X01);
}

void ClearArea(uchar x,uchar y,uchar len)		// 指定位置清屏 Area
{
	SetLoc(x,y);
	
	while(len--)
	{
			WriteData(' ');
	}
}
*/