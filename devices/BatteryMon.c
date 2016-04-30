#include "common.h"
#include "BatteryMon.h"
#include "adc.h"
#include "OLED.h"
#include "sprint.h"

//==============================================================
//函数名：void BatteryMon_Init(void)
//功能描述：电池电压采集初始化  ADC采集模拟电压信号
//参数：无
//返回：无
//==============================================================
void BatteryMon_Init(void)
{
	ADC_QuickInit(ADC0_SE14A_PC00, kADC_SingleDiff10or11);
}

//==============================================================
//函数名：double Get_BatVol(void)
//功能描述：电池电压采集  平均值滤波
//参数：无
//返回：无
//==============================================================
double Get_BatVol(void)
{
	static int32_t pool[20]={0};
	static uint8_t pool_pos=0;
	int32_t value=0;
	
	pool[pool_pos++] = ADC_QuickReadValue(ADC0_SE14A_PC00);
	pool_pos=pool_pos%20;
	
	for(uint8_t i=0;i<20;i++)
	{
		value+=pool[i];
	}
	value/=20;
	return (double)value/0x3FF*9.9;
}

//==============================================================
//函数名：void Show_BatVol(void)
//功能描述：显示电池电压
//参数：无
//返回：无
//==============================================================
void Show_BatVol(void)
{
	double  voltage;
	voltage=Get_BatVol();
	
	char str[10];
	sprint(str,9,2,voltage);
	OLED_Print(0,6,(uint8_t*)str);
}
