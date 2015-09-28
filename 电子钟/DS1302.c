 /* 
 ******************************************
			DS1302设置驱动模块
 ******************************************
 */

#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
	
sbit DS1302_CE = P1^7;
sbit DS1302_CK = P3^5;
sbit DS1302_IO = P3^4;

struct Timer{
	uint year;
	uchar mon;
	uchar day;
	uchar hour;
	uchar min;
	uchar sec;
	uchar week;
};

void DS1302ByteWrite(uchar dat)		// 发送一个字节到DS1302总线上
{
	uchar mask;
	
	for(mask = 0x01; mask != 0; mask <<= 1)		// DS1302先低位 后高位
	{
		if((mask & dat) != 0)
		{
			DS1302_IO = 1;
		}
		else
		{
			DS1302_IO = 0;
		}
		DS1302_CK = 1;		// 拉高时钟
		DS1302_CK = 0;		// 拉低时钟, 完成一个位操作
	}
	DS1302_IO = 1;		// 释放IO引脚
}

unsigned char DS1302ByteRead()		// 读取~~~
{
	uchar mask;
	uchar dat = 0;
	
	for(mask = 0x01; mask != 0; mask <<= 1)
	{
		if(DS1302_IO != 0)
		{
			dat |= mask;
		}
		DS1302_CK = 1;
		DS1302_CK = 0;
	}
	return dat;
}
void DS1302SingleWrite(uchar addr,uchar dat)	// 单次写操作__向寄存器写入一个字节 
{											// addr为寄存器地址 dat为待写入字节
	DS1302_CE = 1;		// 使能片选信号
	DS1302ByteWrite((addr << 1) | 0x80);
	DS1302ByteWrite(dat);
	DS1302_CE = 0;		// 除能片选信号
}

unsigned char DS1302SingleRead(uchar addr)	// 单次读操作__向寄存器读取一个字节 
{										// addr为寄存器地址 返回读到字节
	uchar dat;
	
	DS1302_CE = 1;		// 使能片选信号
	DS1302ByteWrite((addr << 1 )| 0x81);
	dat = DS1302ByteRead();
	DS1302_CE = 0;		// 除能片选信号
	
	return dat;
}

void DS1302BurstWrite(uchar *dat)		// Burst模式连续写入8个寄存器数据, dat为待写入数据指针
{
	uchar i;
	
	DS1302_CE = 1;
	DS1302ByteWrite(0xBE);		// 发送 Burst模式 写寄存器指令
	for(i = 0; i < 8; i++)
	{
		DS1302ByteWrite(dat[i]);	// 连续写入8字节数据
	}
	DS1302_CE = 0;
}

void DS1302BurstRead(uchar *dat)		// ~~ 读取 ~~ dat为待接收指针
{
	uchar i;
	DS1302_CE = 1;
	DS1302ByteWrite(0xBF);		// 发送 Burst模式 读寄存器指令
	for(i = 0; i < 8; i++)
	{
		dat[i] = DS1302ByteRead();	// 连续读取8字节数据
	}
	DS1302_CE = 0;
	
}

void GetRealTime(struct Timer *time)		// 从DS1302读取当前实时时间 & 转换字符串格式
{
    unsigned char buf[8];
    
    DS1302BurstRead(buf);
    time -> year = buf[6] + 0x2000;
    time -> mon  = buf[4];
    time -> day  = buf[3];
    time -> hour = buf[2];
    time -> min  = buf[1];
    time -> sec  = buf[0];
    time -> week = buf[5];
}
void SetRealTime(struct Timer *time)		// 设置实时时间
{
    unsigned char buf[8];
    
    buf[7] = 0;
    buf[6] = time -> year;
    buf[5] = time -> week;
    buf[4] = time -> mon;
    buf[3] = time -> day;
    buf[2] = time -> hour;
    buf[1] = time -> min;
    buf[0] = time -> sec;
    DS1302BurstWrite(buf);
}

void ResetDS1302()	// 初始化实时时钟
{
	uchar i = 0;
	struct Timer code ResetTime[] = {
					0x2015,0x09,0x12,0x08,0x30,0x00,0x06
	};
	
	DS1302_CE = 0;
	DS1302_CK = 0;
	i = DS1302SingleRead(0);
	if((i & 0x80) != 0)
	{
		DS1302SingleWrite(7,0x00);
		SetRealTime(&ResetTime);
	}
	
}
