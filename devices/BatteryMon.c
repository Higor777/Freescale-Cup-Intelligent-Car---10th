#include "common.h"
#include "BatteryMon.h"
#include "adc.h"
#include "OLED.h"
#include "sprint.h"

//==============================================================
//��������void BatteryMon_Init(void)
//������������ص�ѹ�ɼ���ʼ��  ADC�ɼ�ģ���ѹ�ź�
//��������
//���أ���
//==============================================================
void BatteryMon_Init(void)
{
	ADC_QuickInit(ADC0_SE14A_PC00, kADC_SingleDiff10or11);
}

//==============================================================
//��������double Get_BatVol(void)
//������������ص�ѹ�ɼ�  ƽ��ֵ�˲�
//��������
//���أ���
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
//��������void Show_BatVol(void)
//������������ʾ��ص�ѹ
//��������
//���أ���
//==============================================================
void Show_BatVol(void)
{
	double  voltage;
	voltage=Get_BatVol();
	
	char str[10];
	sprint(str,9,2,voltage);
	OLED_Print(0,6,(uint8_t*)str);
}
