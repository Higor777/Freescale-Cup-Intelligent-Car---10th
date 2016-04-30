#include "i2c.h"
#include "anglesensor.h"
#include "uart.h"
#include "adc.h"

//==============================================================
//函数名：void ENC03_Init(void)
//功能描述：陀螺仪传感器初始化  ADC采集模拟电压信号
//参数：无
//返回：无
//==============================================================
void ENC03_Init(void)
{
	ADC_QuickInit(ENC03_AR1,kADC_SingleDiff10or11);
	ADC_QuickInit(ENC03_AR2,kADC_SingleDiff10or11);
	ADC_QuickInit(ENC03_AR3,kADC_SingleDiff10or11);//ADC1_SE12_PB06需要验证地址
}

//==============================================================
//函数名：void MMA7361_Init(void)
//功能描述：加速度传感器初始化  ADC采集模拟电压信号
//参数：无
//返回：无
//==============================================================
void MMA7361_Init(void)
{
	ADC_QuickInit(MMA7361_XOUT,kADC_SingleDiff10or11);
	ADC_QuickInit(MMA7361_YOUT,kADC_SingleDiff10or11);
	ADC_QuickInit(MMA7361_ZOUT,kADC_SingleDiff10or11);
}
