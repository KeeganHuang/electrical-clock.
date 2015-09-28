 /* 
 ******************************************
			DS1302��������ģ��
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

void DS1302ByteWrite(uchar dat)		// ����һ���ֽڵ�DS1302������
{
	uchar mask;
	
	for(mask = 0x01; mask != 0; mask <<= 1)		// DS1302�ȵ�λ ���λ
	{
		if((mask & dat) != 0)
		{
			DS1302_IO = 1;
		}
		else
		{
			DS1302_IO = 0;
		}
		DS1302_CK = 1;		// ����ʱ��
		DS1302_CK = 0;		// ����ʱ��, ���һ��λ����
	}
	DS1302_IO = 1;		// �ͷ�IO����
}

unsigned char DS1302ByteRead()		// ��ȡ~~~
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
void DS1302SingleWrite(uchar addr,uchar dat)	// ����д����__��Ĵ���д��һ���ֽ� 
{											// addrΪ�Ĵ�����ַ datΪ��д���ֽ�
	DS1302_CE = 1;		// ʹ��Ƭѡ�ź�
	DS1302ByteWrite((addr << 1) | 0x80);
	DS1302ByteWrite(dat);
	DS1302_CE = 0;		// ����Ƭѡ�ź�
}

unsigned char DS1302SingleRead(uchar addr)	// ���ζ�����__��Ĵ�����ȡһ���ֽ� 
{										// addrΪ�Ĵ�����ַ ���ض����ֽ�
	uchar dat;
	
	DS1302_CE = 1;		// ʹ��Ƭѡ�ź�
	DS1302ByteWrite((addr << 1 )| 0x81);
	dat = DS1302ByteRead();
	DS1302_CE = 0;		// ����Ƭѡ�ź�
	
	return dat;
}

void DS1302BurstWrite(uchar *dat)		// Burstģʽ����д��8���Ĵ�������, datΪ��д������ָ��
{
	uchar i;
	
	DS1302_CE = 1;
	DS1302ByteWrite(0xBE);		// ���� Burstģʽ д�Ĵ���ָ��
	for(i = 0; i < 8; i++)
	{
		DS1302ByteWrite(dat[i]);	// ����д��8�ֽ�����
	}
	DS1302_CE = 0;
}

void DS1302BurstRead(uchar *dat)		// ~~ ��ȡ ~~ datΪ������ָ��
{
	uchar i;
	DS1302_CE = 1;
	DS1302ByteWrite(0xBF);		// ���� Burstģʽ ���Ĵ���ָ��
	for(i = 0; i < 8; i++)
	{
		dat[i] = DS1302ByteRead();	// ������ȡ8�ֽ�����
	}
	DS1302_CE = 0;
	
}

void GetRealTime(struct Timer *time)		// ��DS1302��ȡ��ǰʵʱʱ�� & ת���ַ�����ʽ
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
void SetRealTime(struct Timer *time)		// ����ʵʱʱ��
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

void ResetDS1302()	// ��ʼ��ʵʱʱ��
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
