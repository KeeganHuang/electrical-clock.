	 /* 2015.09 */
 /* 广东石油化工学院 */
 /* Ryan_yuans 	袁柏宁*/
 /* DS1302实时时钟__简易电子钟 */
 
 #include <reg52.h>

#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
	
sbit BUZZ = P1^6;

struct Timer{
	uint year;
	uchar mon;
	uchar day;
	uchar hour;
	uchar min;
	uchar sec;
	uchar week;
};

void ConfigTimer0(uint ms);
extern void Reset1602();
extern void ResetDS1302();
extern void ShowStr(uchar x,uchar y,uchar *str);
extern void GetRealTime(struct Timer *time);
extern void SetRealTime(struct Timer *time);
void RefreshTimeShow();
void ShowBCDByte(uchar x,uchar y,uchar bcd);
void AlarmMonitor();
extern void LCDgetTwk();
extern void LCDstopTwk();
extern void SetLoc(uchar x,uchar y);
extern void KeyDriver();
extern void KeyScan();


bit flag200 = 0;
bit buzzmonitor = 1;
bit startbuzz = 0;
struct Timer TimeNow;
uchar SetIndex = 0;		// 设置_检索
uchar T0RH = 0;
uchar T0RL = 0;
uchar AlarmHour = 0x07;
uchar AlarmMin = 0x30;		// 当前闹钟时间
uchar SetAlarmHour;
uchar SetAlarmMin;		// 设置模式下的闹钟时间缓冲

void main()
{
	uchar rec = 0xAA;
	
	EA = 1;
	ConfigTimer0(1);
	Reset1602();
	ResetDS1302();
	
	ShowStr(0,0,"20  -  -  ");
	ShowStr(1,1,"  :  :  ");	// 固定打印显示
	ShowStr(12,0,"SET");
	ShowStr(11,1,"  :  ");
	
	while(1)
	{
		KeyDriver();
		
		if(flag200 && (SetIndex == 0))		// 200ms + 非 time set 模式
		{
			flag200 = 0;
			
			AlarmMonitor();
				
			ShowBCDByte(11,1,AlarmHour);
			ShowBCDByte(14,1,AlarmMin);
			
			GetRealTime(&TimeNow);
			if(rec != TimeNow.sec)
			{
				RefreshTimeShow();
				rec = TimeNow.sec;
			}
		}
	}
}

void AlarmMonitor()		// 闹钟监控
{
	if(buzzmonitor == 1)
	{
		if((TimeNow.hour == AlarmHour) && (TimeNow.min == AlarmMin))
		{
			startbuzz = 1;
			buzzmonitor = 0;
		}
	}
	
}

void ShowBCDByte(uchar x,uchar y,uchar bcd)		// 将一个BCD码显示在屏幕上,(x,y)_屏幕LCD起始坐标
{
	uchar str[4];
	
	str[0] = (bcd >> 4) + '0';
	str[1] = (bcd & 0x0F) + '0';
	str[2] = '\0';
	ShowStr(x,y,str);
}

void RefreshTimeShow()		// 刷新日期时间显示
{
	ShowBCDByte(2,0,TimeNow.year);
	ShowBCDByte(5,0,TimeNow.mon);
	ShowBCDByte(8,0,TimeNow.day);
	ShowBCDByte(1,1,TimeNow.hour);
	ShowBCDByte(4,1,TimeNow.min);
	ShowBCDByte(7,1,TimeNow.sec);
}

void RefreshSetTimeShow()		// 刷新日期时间显示
{
	ShowBCDByte(2,0,TimeNow.year);
	ShowBCDByte(5,0,TimeNow.mon);
	ShowBCDByte(8,0,TimeNow.day);
	ShowBCDByte(1,1,TimeNow.hour);
	ShowBCDByte(4,1,TimeNow.min);
	ShowBCDByte(7,1,TimeNow.sec);
	ShowBCDByte(11,1,SetAlarmHour);
	ShowBCDByte(14,1,SetAlarmMin);
}

void RefreshSetShow()		// 刷新当前位置光标
{
	switch(SetIndex)
	{
		case 1:
			SetLoc(2,0); break;
		case 2:
			SetLoc(3,0); break;
		case 3:
			SetLoc(5,0); break;
		case 4:
			SetLoc(6,0); break;
		case 5:
			SetLoc(8,0); break;
		case 6:
			SetLoc(9,0); break;
		case 7:
			SetLoc(1,1); break;
		case 8:
			SetLoc(2,1); break;
		case 9:
			SetLoc(4,1); break;
		case 10:
			SetLoc(5,1); break;
		case 11:
			SetLoc(7,1); break;
		case 12:
			SetLoc(8,1); break;
		case 13:
			SetLoc(11,1); break;
		case 14:
			SetLoc(12,1); break;
		case 15:
			SetLoc(14,1); break;
		case 16:
			SetLoc(15,1); break;
		default:
			break;
		
	}
}


unsigned char IncBCDHigh(uchar dat)	// 递增一个BCD码高位
{
	if((dat & 0xF0) < 0x90)
	{
		dat += 0x10;
	}
	else
	{
		dat &= 0x0F;
	}
	
	return dat;
}

unsigned char IncBCDLow(uchar dat)	// 递增 ~ 低位7
{
	if((dat & 0x0F) < 0x09)
	{
		dat += 0x01;
	}
	else
	{
		dat &= 0xF0;
	}
	
	return dat;
}

unsigned char DecBCDHigh(uchar dat)	// 递减一个BCD码高位
{
	if((dat & 0xF0) > 0x00)
	{
		dat -= 0x10;
	}
	else
	{
		dat |= 0x90;
	}
	
	return dat;
}

unsigned char DecBCDLow(uchar dat)	// 递减 ~ 低位
{
	if((dat & 0x0F) > 0x00)
	{
		dat -= 0x01;
	}
	else
	{
		dat |= 0x09;
	}
	
	return dat;
}

void IncTimeSet()	// 递增时间设置位的值
{
	switch(SetIndex)
	{
		case 1:
			TimeNow.year = IncBCDHigh(TimeNow.year); break;
		case 2:
			TimeNow.year = IncBCDLow(TimeNow.year); break;
		case 3:
			TimeNow.mon = IncBCDHigh(TimeNow.mon); break;
		case 4:
			TimeNow.mon = IncBCDLow(TimeNow.mon); break;
		case 5:
			TimeNow.day = IncBCDHigh(TimeNow.day); break;
		case 6:
			TimeNow.day = IncBCDLow(TimeNow.day); break;
		case 7:
			TimeNow.hour = IncBCDHigh(TimeNow.hour); break;
		case 8:
			TimeNow.hour = IncBCDLow(TimeNow.hour); break;
		case 9:
			TimeNow.min = IncBCDHigh(TimeNow.min); break;
		case 10:
			TimeNow.min = IncBCDLow(TimeNow.min); break;
		case 11:
			TimeNow.sec = IncBCDHigh(TimeNow.sec); break;
		case 12:
			TimeNow.sec = IncBCDLow(TimeNow.sec); break;
		case 13:
			SetAlarmHour = IncBCDHigh(SetAlarmHour); break;
		case 14:
			SetAlarmHour = IncBCDLow(SetAlarmHour); break;
		case 15:
			SetAlarmMin = IncBCDHigh(SetAlarmMin); break;
		case 16:
			SetAlarmMin = IncBCDLow(SetAlarmMin); break;
		default: 
			break;
	}
	RefreshSetTimeShow();
	RefreshSetShow();
}

void DecTimeSet()	// 递减时间设置位的值
{
	switch(SetIndex)
	{
		case 1:
			TimeNow.year = DecBCDHigh(TimeNow.year); break;
		case 2:
			TimeNow.year = DecBCDLow(TimeNow.year); break;
		case 3:
			TimeNow.mon = DecBCDHigh(TimeNow.mon); break;
		case 4:
			TimeNow.mon = DecBCDLow(TimeNow.mon); break;
		case 5:
			TimeNow.day = DecBCDHigh(TimeNow.day); break;
		case 6:
			TimeNow.day = DecBCDLow(TimeNow.day); break;
		case 7:
			TimeNow.hour = DecBCDHigh(TimeNow.hour); break;
		case 8:
			TimeNow.hour = DecBCDLow(TimeNow.hour); break;
		case 9:
			TimeNow.min = DecBCDHigh(TimeNow.min); break;
		case 10:
			TimeNow.min = DecBCDLow(TimeNow.min); break;
		case 11:
			TimeNow.sec = DecBCDHigh(TimeNow.sec); break;
		case 12:
			TimeNow.sec = DecBCDLow(TimeNow.sec); break;
		case 13:
			SetAlarmHour = DecBCDHigh(SetAlarmHour); break;
		case 14:
			SetAlarmHour = DecBCDLow(SetAlarmHour); break;
		case 15:
			SetAlarmMin = DecBCDHigh(SetAlarmMin); break;
		case 16:
			SetAlarmMin = DecBCDLow(SetAlarmMin); break;
		default: 
			break;
	}
	RefreshSetTimeShow();
	RefreshSetShow();
}

void LeftShiftTimeSet()	// 左移时间设置位
{
	if(SetIndex != 0)
	{
		if(SetIndex > 1)
		{
			SetIndex--;
		}
		else
		{
			SetIndex = 16;
		}
		RefreshSetShow();
	}
}

void RightShiftTimeSet()	// 右移时间设置位
{
	if(SetIndex != 0)
	{
		if(SetIndex < 16)
		{
			SetIndex++;
		}
		else
		{
			SetIndex = 1;
		}
		RefreshSetShow();
	}
}

void EnterTimeSet()		// Enter time set
{
	SetIndex = 1;
	SetAlarmHour = AlarmHour;
	SetAlarmMin = AlarmMin;
	RefreshSetShow();	// 即时刷新
	LCDgetTwk();
}

void ExitTimeSet(bit save)		// Exit time set
{
	SetIndex = 0;
	AlarmHour = SetAlarmHour;
	AlarmMin = SetAlarmMin;
	if(save)
	{
		SetRealTime(&TimeNow);
	}
	LCDstopTwk();
	buzzmonitor = 1;	// 打开闹钟监控
}

void KeyAction(uchar keycode) // 按键动作_函数,keycode为按键键码
{
	if((keycode >= 0x30) && (keycode <= 0x39))
	{
		;
	}
	else if(keycode == 0x26)	// 向上键
	{
		IncTimeSet();
	}
	else if(keycode == 0x28)	// 向下键
	{
		DecTimeSet();
	}
	else if(keycode == 0x25)	// 向左键
	{
		LeftShiftTimeSet();
	}
	else if(keycode == 0x27)	// 向右键
	{
		RightShiftTimeSet();
	}
	else if(keycode == 0x0D)	// 回车键
	{
		if(SetIndex == 0)
		{
			EnterTimeSet();
		}
		else
		{
			ExitTimeSet(1);
		}
	}
	else if(keycode == 0x1B)	// Ese键
	{
		ExitTimeSet(0);
	}
}

void ConfigTimer0(uint ms)
{
	ulong tmp;
	
	tmp = 11059200 / 12;
	tmp = (tmp * ms) / 1000;
	tmp = 65536 - tmp;
	tmp = tmp + 12;		// 补偿中断响应延时时造成的误差
	T0RH = (uchar)(tmp >> 8);
	T0RL = (uchar)tmp;
	TMOD &= 0XF0;
	TMOD |= 0X01;
	TH0 = T0RH;
	TL0 = T0RL;
	ET0 = 1;
	TR0 = 1;
}

void Timer0() interrupt 1
{
	static uchar tmp200ms = 0;
	static uint tmp5000ms = 0;
	
	TH0 = T0RH;
	TL0 = T0RL;
	KeyScan();
	tmp200ms++;
	
	if(startbuzz == 1)
	{
		BUZZ = ~BUZZ;
		tmp5000ms++;
		
		if(tmp5000ms == 5000)
		{
			tmp5000ms = 0;
			startbuzz = 0;
		}
	}
	
	if(tmp200ms >= 200)
	{
		tmp200ms = 0;
		flag200 = 1;
	}
}