#include <math.h>
#include "common.h"
#include "tpm.h"
#include "ctrl.h"
#include "encoder.h"
#include "motor.h"
#include "debug.h"
#include "filter.h"
#include "catchline.h"
#include "led.h"

float Gyro_AR1;
float Gyro_AR2=0;
float Gyro_AR3;
float Acc_X;
float Acc_Y;
float Acc_Z=0;

float Gyro_Offset=0;
float Gyro_Omega=0;
float Gyro_Sigma;
float Gyro_Integ=0;

float Acc_Sigma;
float Last_Angle=0,Angle=0,Anglar_Rate=0;
float Angle_Integ=0;
float AngleCtrlOut=0;
float Angle_P=1600;//2000;//350;//340
float Angle_D=50;//35;//30;//60
float Angle_I=0;	//50
float ACC_Angle=0;

float CarSpeed=0;
float Speed_Integ=0;
float SpeedCtrlOutOld=0,SpeedCtrlOutNew=0,SpeedCtrlOutEach=0;
float Speed_P=1500;//5000;//2500;//50;
float Speed_D=30;//50;
float Speed_I=0;//10;//250;
float Speed_EX=0;
float Speed_Setting=11.0;//2.1;//1.8;
float SpeedCtrl_Limit;

float DirectCtrlOut=0;
float Direct_P=57;//50;//40;//34;
float Direct_I=0;
float Direct_D=350;//280;//180;//102; 
float LeftPWM=0,RightPWM=0;
uint8_t ForceDirectCtrl_flag=0;
float ForceDirectCtrl_value=0;

float MOTOR_OUT_DEAD_VAL=0;

float Sigma_EX=38.9;

Beacon_Usage_Type Beacon_Mode=BU_motorstop;

//1为快跑，0为站住
uint8_t RUN_flag=0;

uint8_t Dir_Save[20];

//==============================================================
//函数名：void SetAnglePD(int p,int d)
//功能描述：设置直立角度PD参数
//参数： p P参数  d D参数
//返回：无
//==============================================================
void SetAnglePD(int p,int d)
{
	Angle_P=p;
	Angle_D=d;
}

//==============================================================
//函数名：void SetSpeedPI(int p,int i)
//功能描述：设置速度PI参数
//参数： p P参数  i I参数
//返回：无
//==============================================================
void SetSpeedPI(int p,int i)
{
	Speed_P=p;
	Speed_I=i;
}

//==============================================================
//函数名：void SetMotorDead(int val)
//功能描述：设置电机死区
//参数：val 死区大小
//返回：无
//==============================================================
void SetMotorDead(int val)
{
	MOTOR_OUT_DEAD_VAL=val;
}

//==============================================================
//函数名：void AngleCalculate(void)
//功能描述：直立角度采集与融合滤波
//参数： 无
//返回：无
//==============================================================
void AngleCalculate(void)
{
	float rat=0;
	
	Gyro_Omega=(Gyro_Offset-Gyro_AR2)*GYRO_RATIO;
	//Acc_Sigma=(Acc_Z-ACC_OFFSET)*ACC_RATIO;
	rat=(Acc_Z-ACC_OFFSET)*ACC_RATIO_SIN;
	
	if(rat>1)
		rat=1;
	else if(rat<-1)
		rat=-1;
	Acc_Sigma=asin(rat)*ANGLE_RATIO;
	
	KalmanFliter(Acc_Sigma,Gyro_Omega);
}

//==============================================================
//函数名：void AngleCtrl(void)
//功能描述：直立角度控制
//参数： 无
//返回：无
//==============================================================
void AngleCtrl(void)
{
	float angle_err=0;
	extern float Ts;
	
	angle_err=(Sigma_EX-ACC_Angle)-Angle;
	
	if(angle_err>0.5||angle_err<-0.5)
	{
		Angle_Integ+=angle_err;
	}
	else
	{
		Angle_Integ=0;
	}
	AngleCtrlOut=angle_err*Angle_P+Angle_Integ*Angle_I+(OMEGA_EX-Anglar_Rate)*Angle_D;
}

//==============================================================
//函数名：void SpeedCalculate(void)
//功能描述：	速度采集
//参数： 无
//返回：无
//==============================================================
void SpeedCalculate(void)
{
	extern Encoder_Speed_Type LeftSpeed,RightSpeed;
	extern LP1Filter_Type LP1Filter_CarSpeed;

	Encoder_GetPulse();
	
	if(LeftSpeed.value>-50000 && LeftSpeed.value<50000 && RightSpeed.value>-50000 && RightSpeed.value<50000)
	{
		CarSpeed=(LeftSpeed.value+RightSpeed.value)>>1;
		CarSpeed*=SPEED_RATIO;
		CarSpeed=LP1Filter_getVal(&LP1Filter_CarSpeed,CarSpeed,0.1);
	}
}

//==============================================================
//函数名：void Speed_EX_Ctrl(void)
//功能描述：	目标速度设置
//参数： 无
//返回：无
//==============================================================
void Speed_EX_Ctrl(void)
{
	if(RUN_flag==1)
	{
		if(Speed_EX<Speed_Setting)
		{
		  //Speed_EX=Speed_Setting;
			Speed_EX+=3.0;
			if(Speed_EX>Speed_Setting)
			{
				Speed_EX=Speed_Setting;
			}
		}
	}
	else if(RUN_flag==0)
	{
		Speed_EX=0;
	}
}

//==============================================================
//函数名：void SpeedCtrl(void)
//功能描述：	速度控制
//参数： 无
//返回：无
//==============================================================
void SpeedCtrl(void)
{
	float speed_err=0;
	extern float Ts;
	static float speed_err_old=0;
	
	speed_err=CarSpeed-Speed_EX;
	Speed_Integ+=speed_err*Ts;
	//遇限消弱
	if(Speed_Integ>=500 || Speed_Integ<=-500)
	{
		Speed_Integ-=speed_err*Ts;
	}
	
	SpeedCtrlOutOld=SpeedCtrlOutNew;
	SpeedCtrlOutNew=Speed_P*speed_err+Speed_I*Speed_Integ+Speed_D*(speed_err-speed_err_old);
	speed_err_old=speed_err;
	
	SpeedCtrl_Limit=5*Angle_P;
	if(SpeedCtrlOutNew>SpeedCtrl_Limit)
	{
		SpeedCtrlOutNew=SpeedCtrl_Limit;
	}
	else if(SpeedCtrlOutNew<-SpeedCtrl_Limit)
	{
		SpeedCtrlOutNew=-SpeedCtrl_Limit;
	}
	SpeedCtrlOutEach=(SpeedCtrlOutNew-SpeedCtrlOutOld)/4.0;
}

//==============================================================
//函数名：void DirectCtrl(void)
//功能描述：	方向控制
//参数： 无
//返回：无
//==============================================================
void DirectCtrl(void)
{
	extern float Ts;
	extern uint8_t LeftLine,RightLine;
	extern uint8_t Left_flag,Right_flag,Block_flag;
	extern int32_t CCD_View,CCD_HalfView,CCD_LeftView,CCD_RightView;
	float dir_err=0;
	static float dir_err_old=0;
	static float Dir_Integ=0;
	
	if(ForceDirectCtrl_flag==0)
	{
		if(Left_flag==1 && Right_flag==1)
		{
			CCD_View=RightLine-LeftLine;
			if(CCD_View>=CCD_MAXVIEW)
			{
				CCD_View=CCD_MAXVIEW;
			}
			else if(CCD_View<=CCD_MINVIEW)
			{
				CCD_View=CCD_MINVIEW;
			}
			CCD_HalfView=CCD_View>>1;
			CCD_LeftView=63-LeftLine;
			CCD_RightView=RightLine-64;
			if(CCD_LeftView<=CCD_HalfView)
			{
				dir_err=-(CCD_HalfView-CCD_LeftView);
			}
			else if(CCD_RightView<=CCD_HalfView)
			{
				dir_err=(CCD_HalfView-CCD_RightView);
			}
		}
		else if(Left_flag==1 && Right_flag==0)
		{
			RightLine=127;
			if(LeftLine>=63)
			{
				CCD_LeftView=LeftLine-63+CCD_HalfView;
				dir_err=-CCD_LeftView;
			}
			else
			{
				CCD_LeftView=63-LeftLine;
				dir_err=-(CCD_HalfView-CCD_LeftView);
			}
		}
		else if(Left_flag==0 && Right_flag==1)
		{
			LeftLine=0;
			if(RightLine<=64)
			{
				CCD_RightView=64-RightLine+CCD_HalfView;
				dir_err=CCD_RightView;
			}
			else
			{
				CCD_RightView=RightLine-64;
				dir_err=(CCD_HalfView-CCD_RightView);
			}
		}
		else if(Left_flag==0 && Right_flag==0)
		{
			CCD_LeftView=0;
			CCD_RightView=127;
		}

		Dir_Integ+=dir_err*Ts;
		//遇限消弱
		if(Dir_Integ>=500 || Dir_Integ<=-500)
		{
			Dir_Integ-=dir_err*Ts;
		}
	
		//中线跑道
		extern uint8_t Middle_flag,MiddleLine;
		if(Middle_flag==1)
		{
			if(64>MiddleLine)
			{
				dir_err=(64-MiddleLine);
			}
			else
			{
				dir_err=-(MiddleLine-64);
			}
		}
		
		if(Block_flag==0)
		{
			DirectCtrlOut=Direct_P*dir_err+Direct_I*Dir_Integ+Direct_D*(dir_err-dir_err_old);
		  dir_err_old=dir_err;
		}
	}
	else if(ForceDirectCtrl_flag==1)
	{
		ForceDirectCtrl_flag=0;
		DirectCtrlOut=ForceDirectCtrl_value;
	}
	
	/***************************************************************/
	static uint32_t pdir=0;//保存方向
	if(DirectCtrlOut>0)
	{
		Dir_Save[pdir%20]=DIR_LEFT;
		pdir++;
	}
	else
	{
		Dir_Save[pdir%20]=DIR_RIGHT;
		pdir++;
	}
	/****************************************************************/
	
	//障碍
	//基本能用，但未细调
	
	static Block_State_Type block_sta=BSTA_turning1;
	static float BlockTurnOut=0;
//	static float BlockRadioCnt=0;
	static uint8_t isFirst=TURNTIMES;
	
  if(Block_flag)
	{
//		BlockRadioCnt+=CarSpeed;
		switch(block_sta)
		{
			case BSTA_turning1:
			{
				LED_Red2(0);
				if(isFirst)
				{
				  BlockTurnOut=DirectCtrlOut;
					isFirst--;
//				}
//				
          DirectCtrlOut=BlockTurnOut;				
//				if(BlockRadioCnt>15)
//				{
				}
				else
				{
					block_sta=BSTA_turning2;
					isFirst=TURNTIMES;
				}
				break;
			}
			case BSTA_turning2:
			{
				if(isFirst)
				{
				  DirectCtrlOut=-BlockTurnOut;
					isFirst--;
//				}
//				if(BlockRadioCnt>30)
//				{
				}
				else
				{
				  block_sta=BSTA_straight;
					isFirst=16;
				}
				break;
			}
			case BSTA_straight:
			{
				if(isFirst)
				{
				  DirectCtrlOut=0;
					isFirst--;
//				}
//				if(BlockRadioCnt>70)
//				{
				}
				else
				{
				  block_sta=BSTA_turning3;
					isFirst=TURNTIMES;
				}
				break;
			}
			case BSTA_turning3:
			{
				if(isFirst)
				{
				  DirectCtrlOut=-BlockTurnOut;
					isFirst--;
//				}
//				if(BlockRadioCnt>70)
//				{
				}
				else
				{
				  block_sta=BSTA_turning4;
					isFirst=TURNTIMES+1;
				}
				break;
			}
			case BSTA_turning4:
			{
				if(isFirst)
				{
				  DirectCtrlOut=BlockTurnOut;
					isFirst--;
//				}
//				if(BlockRadioCnt>84)
//				{
				}
				else
				{
					LED_Red2(1);
					block_sta=BSTA_turning1;
					isFirst=TURNTIMES;
					Block_flag=0;
//					BlockRadioCnt=0;
				}
				break;
			}
			default:
				break;
		}
	}
}

//==============================================================
//函数名：void CtrlOut(int nPeroid)
//功能描述：	总的电机控制输出
//参数： nPeroid 用于平滑输出，当前为最大控制周期的第几次
//返回：无
//==============================================================
void CtrlOut(int nPeroid)
{
	LeftPWM=AngleCtrlOut+(SpeedCtrlOutOld+nPeroid*SpeedCtrlOutEach)-DirectCtrlOut;
	RightPWM=AngleCtrlOut+(SpeedCtrlOutOld+nPeroid*SpeedCtrlOutEach)+DirectCtrlOut;
	
	/*死区*/
	if(LeftPWM>0)
		LeftPWM+=MOTOR_OUT_DEAD_VAL;
	else if(LeftPWM<0)
		LeftPWM-=MOTOR_OUT_DEAD_VAL;
	if(RightPWM>0)
		RightPWM+=MOTOR_OUT_DEAD_VAL;
	else if(RightPWM<0)
		RightPWM-=MOTOR_OUT_DEAD_VAL;

	Motor_PWM((int)LeftPWM,(int)RightPWM);
}

//==============================================================
//函数名：void Run_Forrest(void)
//功能描述：	起跑与停车状态检测
//参数： 无
//返回：无
//==============================================================
void Run_Forrest(void)
{
	static uint8_t beacon_sta_mask=1;
	extern volatile uint8_t Beacon_STA;
	if(Beacon_Mode==BU_nonuse)
	{
		RUN_flag=1;
	}
	else
	{
		if((Beacon_STA&beacon_sta_mask)==1)
		{
			beacon_sta_mask=(~Beacon_STA)&0x01;
		}
		else if((Beacon_STA|beacon_sta_mask)==0)
		{
			beacon_sta_mask=(~Beacon_STA)&0x01;
			
			if(Beacon_Mode==BU_nostop)
			{
				RUN_flag=1;
			}
			else if(Beacon_Mode==BU_motorstop)
			{
				RUN_flag=RUN_flag?0:1;
			}
			else if(Beacon_Mode==BU_cutoffstop)
			{
				RUN_flag=RUN_flag?0:1;
				if(RUN_flag==0) Motor_STOP();
			}
		}
	}
}


//==============================================================
//函数名：void Remode_Direct(void)
//功能描述：遥控模式方向控制
//参数： 无
//返回：无
//==============================================================
void Remode_Direct(void)
{
			//中线跑道
	extern uint8_t MiddleLine;
	extern float Ts;
	float dir_err=0;
	static float dir_err_old=0;
	static float Dir_Integ=0;

	dir_err=(64-MiddleLine);
	Dir_Integ+=dir_err*Ts;
	//遇限消弱
	if(Dir_Integ>=500 || Dir_Integ<=-500)
	{
		Dir_Integ-=dir_err*Ts;
	}
	DirectCtrlOut=Direct_P*dir_err+Direct_I*Dir_Integ+Direct_D*(dir_err-dir_err_old);
	dir_err_old=dir_err;

}



