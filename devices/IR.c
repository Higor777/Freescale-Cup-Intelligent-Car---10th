#include "common.h"
#include "IR.h"
#include "gpio.h"

//灯塔亮时为1，灭时为0
volatile uint8_t Beacon_STA=0;
volatile uint8_t beacon_scan=0;


//==============================================================
//函数名：void IR_Init(void)
//功能描述：红外接收初始化
//参数：无
//返回：无
//==============================================================
void IR_Init(void)
{
	GPIO_QuickInit(HW_GPIOA,7, kGPIO_Mode_IPU);
	
	GPIO_CallbackInstall(HW_GPIOA, IR_ISR);

//	GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
}

//==============================================================
//函数名：void Beacon_Check(void)
//功能描述：检测红外发车灯塔状态
//参数：无
//返回：无
//==============================================================
void Beacon_Check(void)
{
	//更新Beacon_STA
	if(beacon_scan==0&&Beacon_STA==1)
	{
		Beacon_STA=0;
	}
	else if(beacon_scan==1&&Beacon_STA==0)
	{
		Beacon_STA=1;
	}
	
	beacon_scan=0;
	if(GPIO_ReadBit(HW_GPIOA, 7)&0x01)
	{
		//引脚高电平，检测下降沿
		GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
	}
	else
	{
		//引脚低电平
		beacon_scan=1;
	}
}

//==============================================================
//函数名：static void IR_ISR(uint32_t array)
//功能描述：红外接收中断
//参数：array  中断源序列
//返回：无
//==============================================================
static void IR_ISR(uint32_t array)
{
	//引脚7发生中断
    if(array&(0x01<<7))
	{
		GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, false);
		
		beacon_scan=1;
	}
}
