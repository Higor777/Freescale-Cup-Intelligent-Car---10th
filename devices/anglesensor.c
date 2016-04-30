#include "i2c.h"
#include "anglesensor.h"
#include "uart.h"
#include "adc.h"

//==============================================================
//��������void ENC03_Init(void)
//���������������Ǵ�������ʼ��  ADC�ɼ�ģ���ѹ�ź�
//��������
//���أ���
//==============================================================
void ENC03_Init(void)
{
	ADC_QuickInit(ENC03_AR1,kADC_SingleDiff10or11);
	ADC_QuickInit(ENC03_AR2,kADC_SingleDiff10or11);
	ADC_QuickInit(ENC03_AR3,kADC_SingleDiff10or11);//ADC1_SE12_PB06��Ҫ��֤��ַ
}

//==============================================================
//��������void MMA7361_Init(void)
//�������������ٶȴ�������ʼ��  ADC�ɼ�ģ���ѹ�ź�
//��������
//���أ���
//==============================================================
void MMA7361_Init(void)
{
	ADC_QuickInit(MMA7361_XOUT,kADC_SingleDiff10or11);
	ADC_QuickInit(MMA7361_YOUT,kADC_SingleDiff10or11);
	ADC_QuickInit(MMA7361_ZOUT,kADC_SingleDiff10or11);
}
