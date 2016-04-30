#include "tpm.h"
#include "motor.h"
#include "gpio.h"
#include "common.h"

//==============================================================
//函数名：void Motor_Init(uint32_t freq)
//功能描述：电机初始化
//参数：freq 驱动频率
//返回：无
//==============================================================
void Motor_Init(uint32_t freq)
{
	TPM_PWM_QuickInit(TPM0_CH0_PD00, kPWM_EdgeAligned, freq);//左正
	TPM_PWM_QuickInit(TPM0_CH1_PD01, kPWM_EdgeAligned, freq);//左反
	TPM_PWM_QuickInit(TPM0_CH2_PD02, kPWM_EdgeAligned, freq);//右正
	TPM_PWM_QuickInit(TPM0_CH3_PD03, kPWM_EdgeAligned, freq);//右反
	
	TPM_PWM_ChangeDuty(HW_TPM0,0,0);
	TPM_PWM_ChangeDuty(HW_TPM0,1,0);
	TPM_PWM_ChangeDuty(HW_TPM0,2,0);
	TPM_PWM_ChangeDuty(HW_TPM0,3,0);
	
}

//==============================================================
//函数名：void Motor_PWM(int32_t Left_PWM,int32_t Right_PWM)
//功能描述：电机控制函数
//参数：Left_PWM  Right_PWM 符号位方向，占空比为绝对值/PWM_MAX
//返回：无
//==============================================================
void Motor_PWM(int32_t Left_PWM,int32_t Right_PWM)
{
	if(Left_PWM>PWM_MAX)
		Left_PWM=PWM_MAX;
	else if(Left_PWM<PWM_MIN)
		Left_PWM=PWM_MIN;
	if(Right_PWM>PWM_MAX)
		Right_PWM=PWM_MAX;
	else if(Right_PWM<PWM_MIN)
		Right_PWM=PWM_MIN;
		
	if(Left_PWM>=0)
	{
		TPM_PWM_ChangeDuty(HW_TPM0,0,Left_PWM);
		TPM_PWM_ChangeDuty(HW_TPM0,1,0);
	}
	else
	{
		TPM_PWM_ChangeDuty(HW_TPM0,0,0);
		TPM_PWM_ChangeDuty(HW_TPM0,1,-Left_PWM);
	}
	if(Right_PWM>=0)
	{
		TPM_PWM_ChangeDuty(HW_TPM0,2,Right_PWM);
		TPM_PWM_ChangeDuty(HW_TPM0,3,0);
	}
	else
	{
		TPM_PWM_ChangeDuty(HW_TPM0,2,0);
		TPM_PWM_ChangeDuty(HW_TPM0,3,-Right_PWM);
	}
}

//电机启动标志
uint8_t Motor_start=0;
//==============================================================
//函数名：void Motor_STOP(void)
//功能描述：失能电机
//参数：无
//返回：无
//==============================================================
void Motor_STOP(void)
{
	Motor_PWM(0,0);
	Motor_start=0;
}
//==============================================================
//函数名：void Motor_START(void)
//功能描述：使能电机
//参数：无
//返回：无
//==============================================================
void Motor_START(void)
{
	Motor_start=1;
}
