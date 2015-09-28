 /* 
 ******************************************
			4*4 矩阵按键驱动模块
 ******************************************
 */

#include<reg52.h>
 
#define uchar unsigned char
#define ulong unsigned long

sbit KEY_IN1 = P2^4;
sbit KEY_IN2 = P2^5;
sbit KEY_IN3 = P2^6;
sbit KEY_IN4 = P2^7;
sbit KEY_OUT1 = P2^3;
sbit KEY_OUT2 = P2^2;
sbit KEY_OUT3 = P2^1;
sbit KEY_OUT4 = P2^0;

uchar code KeycodeMap[4][4] = { 
																{ 0x31, 0x32, 0x33, 0x26 }, //数字键1、数字键2、数字键3、向上键
																{ 0x34, 0x35, 0x36, 0x25 }, //数字键4、数字键5、数字键6、向左键
																{ 0x37, 0x38, 0x39, 0x28 }, //数字键7、数字键8、数字键9、向下键
																{ 0x30, 0x1B, 0x0D, 0x27 }  //数字键0、ESC键、  回车键、 向右键
														 };			// 按键区

uchar pdata Keysta[4][4] = { {1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1} };		// 按键判断
	
extern void KeyAction(uchar keycode);
	
void KeyDriver()			// 判断按键是否按下_函数
{
	extern void KeyAction(uchar keycode);
	
	uchar i,j;
 
	static uchar pdata backup[4][4] = {  {1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}	};
	
	for(i = 0; i < 4; i++)
			for(j = 0; j < 4; j++)
					{
						if(Keysta[i][j] != backup[i][j])
						{
								if(backup[i][j] != 0)
								{
										KeyAction(KeycodeMap[i][j]);
								}
							backup[i][j] = Keysta[i][j];
						}
					}
}

void KeyScan()				//按键扫描__函数
{
 uchar i;
 static uchar keyout = 0;
 static uchar keybuf[4][4] = {
																{0XFF,0XFF,0XFF,0XFF},{0XFF,0XFF,0XFF,0XFF},
																{0XFF,0XFF,0XFF,0XFF},{0XFF,0XFF,0XFF,0XFF}
															};
 
keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN1;
keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN2;
keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN3;
keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN4;

for(i = 0; i < 4; i++)
	if((keybuf[keyout][i] & 0X0F) == 0X00)
	{
			Keysta[keyout][i] = 0;
	}
	else if((keybuf[keyout][i] & 0X0F) == 0x0F)
	{
			Keysta[keyout][i] = 1;
	}
	else ;
	
	keyout++;
	keyout = keyout & 0X03;			// 满_4_化_0
	
	switch(keyout)
	{
		case 0:
			{KEY_OUT4 = 1; KEY_OUT1 = 0;}break;
		case 1:
			{KEY_OUT1 = 1; KEY_OUT2 = 0;}break;
		case 2:
			{KEY_OUT2 = 1; KEY_OUT3 = 0;}break;
		case 3:
			{KEY_OUT3 = 1; KEY_OUT4 = 0;}break;
		default: break;
	}
 
}
