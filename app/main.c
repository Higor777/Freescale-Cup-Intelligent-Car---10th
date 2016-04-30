#include <math.h>
/*Drivers*/
#include "common.h"
#include "gpio.h"
#include "uart.h"
#include "tpm.h"
#include "i2c.h"
#include "adc.h"
#include "pit.h"
/*Devices*/
#include "motor.h"
#include "oled.h"
#include "ccd.h"
#include "BatteryMon.h"
#include "encoder.h"
#include "anglesensor.h"
#include "key.h"
#include "bluetooth.h"
#include "led.h"
#include "IR.h"
/*app*/
#include "umc.h"
#include "ctrl.h"
#include "Debug.h"
#include "filter.h"
#include "catchline.h"
#include "remote.h"

#include "sprint.h"

uint8_t TIME0flag_20ms=0;
uint8_t TIME1flag_5ms=0;
uint8_t TIME1flag_10ms=0;
uint8_t TIME1flag_20ms=0;
uint8_t TIME1flag_100ms=0;
uint8_t TIME1flag_1000ms=0;
static void PIT0_CallBack(void);
static void PIT1_CallBack(void);

extern float Acc_Z,Gyro_AR2;
extern float Gyro_Integ;
extern float Angle;
extern float Acc_Sigma;
extern float CarSpeed;
extern uint8_t Motor_start;
uint8_t Clock_Tick=0;

//控制模式  0 自动循迹 1 遥控
float Ctrl_mode = 0;
int main()
{
	int nPeroid=0;
	
	/*================延时初始化=================*/
	DelayInit();
	/*================LED初始化=================*/
	LED_Init();
	/*=================电池电压初始化=================*/
	BatteryMon_Init();
	/*=================电机初始化,频率10k=================*/
	GPIO_QuickInit(HW_GPIOB, 21, kGPIO_Mode_IPU);//D触发器方向
	GPIO_QuickInit(HW_GPIOB, 22, kGPIO_Mode_IPU);
	Motor_Init(10000);
	/*=================编码器初始化=================*/
	Encoder_Init();
	/*=================上位机通信=================*/
	UMC_Init();
	/*=================OLED初始化=================*/
	OLED_Init();
	/*=================CCD初始化=================*/
	CCD_Init();
	/*=================PIT0初始化,0.2ms定时中断=================*/
	PIT_QuickInit(HW_PIT_CH0,2000);
	PIT_CallbackInstall(HW_PIT_CH0,PIT0_CallBack);
	PIT_ITDMAConfig(HW_PIT_CH0,kPIT_IT_TOF,true);
	
	PIT_QuickInit(HW_PIT_CH1,5000);
	PIT_CallbackInstall(HW_PIT_CH1,PIT1_CallBack);
	PIT_ITDMAConfig(HW_PIT_CH1,kPIT_IT_TOF,true);
	/*=================AngleSensor初始化===================*/
	ENC03_Init();
	MMA7361_Init();
	
	IR_Init();
	
	Acc_Z=ADC_QuickReadValue(MMA7361_ZOUT);
	Gyro_Integ=(Acc_Z-ACC_OFFSET)*ACC_RATIO_SIN;
	if(Gyro_Integ>1)
		Gyro_Integ=1;
	else if(Gyro_Integ<-1)
		Gyro_Integ=-1;
	Gyro_Integ=asin(Gyro_Integ)*ANGLE_RATIO;/**************************/
	
	/*==================平均滤波器初始化=======================*/
	for(uint8_t i=0; i<FILTER_BUFSIZE; i++)
	{
		AverFilter_getVal(&AverFilter_Gyro_AR2,ADC_QuickReadValue(ENC03_AR2));
		AverFilter_getVal(&AverFilter_Acc_Z,ADC_QuickReadValue(MMA7361_ZOUT));
		DelayMs(5);
	}
	/*====================键盘初始化=======================*/
	Key_Init();
	M_Encoder_Init();
	ParameterList_Init();
	Data_Init();
	
//	float gyro_sum=0,acc_sum=0;
//	float gyro_aver,acc_aver;
	uint8_t dynamic_threshold;
	float speed_cnt=0;
	
	while(1)
	{
		/*========================AngleSensor测试=========================*/
//		/*计算零偏均值*/
//		Gyro_AR2=ADC_QuickReadValue(ENC03_AR2);
//		Acc_Z=ADC_QuickReadValue(MMA7361_ZOUT);
//		gyro_sum+=Gyro_AR2;
//		acc_sum+=Acc_Z;
//		cnt++;
//		gyro_aver=gyro_sum/cnt;
//		acc_aver=acc_sum/cnt;
//		printf("%d->%f %f\r\n",cnt,gyro_aver,acc_aver);
//		/*计算零偏均值*/
		/*================================================================*/
		
		if(Ctrl_mode==1)Remote_Mode();
		
		if(TIME0flag_20ms==1)
		{
			TIME0flag_20ms=0;

		}
		
		if(TIME1flag_5ms==1)
		{
			TIME1flag_5ms=0;
			
			Gyro_AR2=ADC_QuickReadValue(ENC03_AR2);
			Acc_Z=ADC_QuickReadValue(MMA7361_ZOUT);
			AngleCalculate();
			AngleCtrl();
			if(Motor_start==1)CtrlOut(nPeroid++);
		}
		if(TIME1flag_10ms==1)
		{
			TIME1flag_10ms=0;
			
			Key_Scan();
		}
		if(TIME1flag_20ms==1)
		{
			TIME1flag_20ms=0;
			
			extern float Threshold_Para;
			if(Threshold_Para!=Threshold)
			{
				Threshold=(uint8_t)Threshold_Para;
			}
			
			
			if(Ctrl_mode == 0)
			{
				CCD_ImageCapture(ImageData);
				dynamic_threshold=ImageBinaryzation(Threshold,ImageData,Binary_Image);
				ImageBinaryzation(Threshold,ImageData_2,Binary_Image_2);
				SearchLine((uint16_t)Begin_Pos);
				SearchBeginPos(&Begin_Pos);
				
				nPeroid=0;
				
				Speed_EX_Ctrl();
				SpeedCalculate();
				SpeedCtrl();
				Rightangle_Recognition();
				DirectCtrl();
				
				Beacon_Check();
				Run_Forrest();
			}
			else 
			{
				Remode_Direct();
				SpeedCalculate();
				SpeedCtrl();
				nPeroid = 0;
			}
			
			
			extern float Gyro_Omega,Gyro_Integ;
			
//			Out_Data[0]=(int)(Gyro_Omega*20);
//			Out_Data[1]=(int)(Gyro_Integ*100);
//			Out_Data[2]=(int)(Acc_Sigma*100);
//			Out_Data[3]=(int)(Angle*100);
//		
//			Out_Put_Data();
			
			speed_cnt+=CarSpeed*100;
			
			Clock_Tick=Clock_Tick?0:1;
			
			LED_Config();
		}
		if(TIME1flag_100ms==1)
		{
			TIME1flag_100ms=0;
			
//			uint8_t imagechoose=2;
//			uint8_t temp[128]={0};
//			switch(imagechoose)
//			{
//				//二值化图像
//				case 0:
//				{
//					for(uint8_t i=0; i<128; i++)
//					{
//						temp[i]=Binary_Image[i]*250;
//					}
//					break;
//				}
//				//原始图像
//				case 1:
//				{
//					for(uint8_t i=0; i<128; i++)
//					{
//						temp[i]=ImageData[i];
//					}
//					break;
//				}
//				case 2:
//				{
//					for(uint8_t i=0; i<128; i++)
//					{
//						temp[i]=ImageData_2[i];
//					}
//					break;
//				}
//				default:
//					break;
//			}
//			SendImageData(temp);
			for(uint8_t i=0; i<128; i++)
			{
				Binary_Image[i]=Binary_Image[i]*250;
				Binary_Image_2[i]=Binary_Image_2[i]*250;
			}
      //SendData();
		}
		if(TIME1flag_1000ms==1)
		{
			TIME1flag_1000ms=0;
			
			char s1[10],s2[10],s3[10];

			sprint(s1,6,2,Angle);
			sprint(s2,6,2,dynamic_threshold);
			sprint(s3,6,2,CarSpeed);

			OLED_Print(80,0,(uint8_t*)s1);
			OLED_Print(80,2,(uint8_t*)s2);
			OLED_Print(80,4,(uint8_t*)s3);
			Show_BatVol();
		}
		
		Parameter_Change();
		Car_Protect();
	}
}

static void PIT0_CallBack()
{
//	extern uint32_t IntegTime;
//	extern void CCD_Start();
//	uint8_t integ_pos=0;
	static uint8_t pit0_cnt=0;
	
	pit0_cnt++;
//	integ_pos=100-IntegTime;
//	if(integ_pos>=2 && pit0_cnt==integ_pos)
//	{
////		CCD_Start();
//	}

	if(pit0_cnt>=100)
	{
		pit0_cnt=0;
		TIME0flag_20ms=1;
	}
	
	M_Encoder_Scan();
}

static void PIT1_CallBack()
{
	static uint8_t pit1_cnt=0;
	
	pit1_cnt++;
	if(pit1_cnt%1==0)
	{
		TIME1flag_5ms=1;
	}
	if(pit1_cnt%2==0)
	{
		TIME1flag_10ms=1;
	}
	if(pit1_cnt%4==0)
	{
		TIME1flag_20ms=1;
	}
	if(pit1_cnt%20==0)
	{
		TIME1flag_100ms=1;
	}
	if(pit1_cnt%200==0)
	{
		TIME1flag_1000ms=1;
	}
	
	if(pit1_cnt>=200)
	{
		pit1_cnt=0;
	}
}
