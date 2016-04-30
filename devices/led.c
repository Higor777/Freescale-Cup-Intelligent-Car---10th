#include "led.h"
#include "gpio.h"

//==============================================================
//函数名：void LED_Init(void)
//功能描述：LED初始化
//参数：无
//返回：无
//==============================================================
void LED_Init(void)
{
	GPIO_QuickInit(HW_GPIOE, 31, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOE, 24, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOE, 25, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOC, 12, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOC, 11, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOC, 10, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOC, 9, kGPIO_Mode_OPP);
	
	GPIO_WriteBit(HW_GPIOE, 31, 1);
	GPIO_WriteBit(HW_GPIOE, 24, 1);
	GPIO_WriteBit(HW_GPIOE, 25, 1);
	GPIO_WriteBit(HW_GPIOE, 26, 1);
	GPIO_WriteBit(HW_GPIOC, 12, 1);
	GPIO_WriteBit(HW_GPIOC, 11, 1);
	GPIO_WriteBit(HW_GPIOC, 10, 1);
	GPIO_WriteBit(HW_GPIOC, 9, 1);
	
	//核心板LED
	GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 6, kGPIO_Mode_OPP);
	GPIO_QuickInit(HW_GPIOD, 7, kGPIO_Mode_OPP);
	GPIO_WriteBit(HW_GPIOD, 4, 1);
	GPIO_WriteBit(HW_GPIOD, 5, 1);
	GPIO_WriteBit(HW_GPIOD, 6, 1);
	GPIO_WriteBit(HW_GPIOD, 7, 1);
}

//==============================================================
//函数名：void LED_Config(void)
//功能描述：配置LED亮灭
//参数：无
//返回：无
//==============================================================
void LED_Config(void)
{
	extern uint8_t Beacon_STA;
	extern uint8_t RightAngle_Indicator;
	extern uint8_t Clock_Tick;
	
//	LED_Red1(1);
//	LED_Green1(1);
//	LED_Blue1(1);
//	LED_Yellow1(1);
//	LED_Red2(1);
	LED_Green2(Clock_Tick);
//	LED_Blue2(1);
//	LED_Yellow2(1);

	//核心板LED
	c_LED_1(Beacon_STA);
//	c_LED_2(1);
	c_LED_3(RightAngle_Indicator);
//	c_LED_4(1);
	
}
