#include "common.h"
#include "gpio.h"
#include "adc.h"
#include "ccd.h"


uint32_t IntegTime=10;
//使用了两个线性CCD
uint8_t ImageData[128]={0};
uint8_t ImageData_2[128]={0};


void Delay100Ns(uint8_t ns)
{
	for(uint8_t i=0;i<ns;i++)
	{
		__nop();
	}
}

//==============================================================
//函数名：void CCD_Init(void)
//功能描述：线性CCD采集初始化  ADC采集模拟电压
//参数：无
//返回：无
//==============================================================
void CCD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.instance=HW_GPIOA;
	GPIO_InitStruct.mode=kGPIO_Mode_OPP;
	GPIO_InitStruct.pinx=5;
	GPIO_Init(&GPIO_InitStruct);	//CCD1_CLK
	GPIO_InitStruct.pinx=6;
	GPIO_Init(&GPIO_InitStruct);	//CCD1_SI
	
	ADC_QuickInit(ADC0_SE8A_PB00,kADC_SingleDiff10or11);	//CCD1_AO;
	
	GPIO_InitStruct.instance=HW_GPIOB;
	GPIO_InitStruct.pinx=7;
	GPIO_Init(&GPIO_InitStruct);	//CCD2_CLK
	GPIO_InitStruct.pinx=8;
	GPIO_Init(&GPIO_InitStruct);	//CCD2_SI
	
	ADC_QuickInit(ADC0_SE9A_PB01,kADC_SingleDiff10or11);	//CCD2_AO;
}

//==============================================================
//函数名：void CCD_Start(void)
//功能描述：线性CCD开始曝光
//参数：无
//返回：无
//==============================================================
void CCD_Start(void)
{
	CCD_SI_Set();//SI=1;
	Delay100Ns(2);
	CCD_CLK_Set();//CLK=1;
	Delay100Ns(2);
	CCD_SI_Reset();//SI=0;
	Delay100Ns(2);
	CCD_CLK_Reset();//CLK=0;
	
	for(uint8_t i=0;i<127;i++)
	{
		Delay100Ns(4);
		CCD_CLK_Set();
		Delay100Ns(4);
		CCD_CLK_Reset();
	}
	Delay100Ns(4);
	CCD_CLK_Set();
	Delay100Ns(4);
	CCD_CLK_Reset();
}

//==============================================================
//函数名：void CCD_ImageCapture(uint8_t ImageData[])
//功能描述：线性CCD图像采集
//参数：ImageData 图像数据地址
//返回：无
//==============================================================
void CCD_ImageCapture(uint8_t ImageData[])
{
	uint8_t cnt=0;
	
	CCD_SI_Set();
	CCD2_SI_Set();
	Delay100Ns(2);
	
	CCD_CLK_Set();
	CCD2_CLK_Set();
	Delay100Ns(2);
	
	CCD_SI_Reset();
	CCD2_SI_Reset();
	Delay100Ns(2);
	
	DelayUs(10);//第一个像素点样本，修改延时时间平滑图像
	
	ImageData[cnt]=(uint8_t)(ADC_QuickReadValue(ADC0_SE8A_PB00)>>2);
	ImageData_2[cnt]=(uint8_t)(ADC_QuickReadValue(ADC0_SE9A_PB01)>>2);
	cnt++;
	CCD_CLK_Reset();
	CCD2_CLK_Reset();
	for(uint8_t i=0;i<127;i++)
	{
		Delay100Ns(4);
		CCD_CLK_Set();
		CCD2_CLK_Set();
		Delay100Ns(4);
		
		ImageData[cnt]=(uint8_t)(ADC_QuickReadValue(ADC0_SE8A_PB00)>>2);
		ImageData_2[cnt]=(uint8_t)(ADC_QuickReadValue(ADC0_SE9A_PB01)>>2);
		cnt++;
		CCD_CLK_Reset();
		CCD2_CLK_Reset();
	}
	
	Delay100Ns(4);
	CCD_CLK_Set();
	CCD2_CLK_Set();
	Delay100Ns(4);
	
	CCD_CLK_Reset();
	CCD2_CLK_Reset();
}

//==============================================================
//函数名：void CCD_IntegTime(const uint8_t ImageData[])
//功能描述：线性CCD积分（曝光）时间
//参数：ImageData 图像数据地址
//返回：无
//==============================================================
void CCD_IntegTime(const uint8_t ImageData[])
{
	uint32_t AverGreyLevel=0;
	uint32_t AverGreyLevel_EX=100;//期望采集均值，需要调节
	int32_t error=0;
	int32_t error_EX=3;//期望离差，需要调节
	
	for(int32_t i=0;i<128;i++)
	{
		AverGreyLevel+=(uint32_t)ImageData[i];
	}
	AverGreyLevel>>=7;
	
	error=AverGreyLevel_EX-AverGreyLevel;
	if(error<-error_EX || error>error_EX)
	{
		if(error<-10)
			error=-10;
		if(error>10)
			error=10;
		
		IntegTime+=(error/2);//P调节
	}
}

//==============================================================
//函数名：void CCD_Diff(uint8_t ImageData[],int16_t ImageDataDiff[])
//功能描述：线性CCD图像差分
//参数：ImageData 图像数据地址 ImageDataDiff 差分图像数据地址
//返回：无
//==============================================================
void CCD_Diff(uint8_t ImageData[],int16_t ImageDataDiff[])
{
	for(uint8_t i=1;i<128;i++)
	{
		ImageDataDiff[i-1]=ImageData[i]-ImageData[i-1];
	}
}
