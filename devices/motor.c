#include "tpm.h"
#include "motor.h"
#include "gpio.h"
#include "common.h"

//==============================================================
//��������void Motor_Init(uint32_t freq)
//���������������ʼ��
//������freq ����Ƶ��
//���أ���
//==============================================================
void Motor_Init(uint32_t freq)
{
	TPM_PWM_QuickInit(TPM0_CH0_PD00, kPWM_EdgeAligned, freq);//����
	TPM_PWM_QuickInit(TPM0_CH1_PD01, kPWM_EdgeAligned, freq);//��
	TPM_PWM_QuickInit(TPM0_CH2_PD02, kPWM_EdgeAligned, freq);//����
	TPM_PWM_QuickInit(TPM0_CH3_PD03, kPWM_EdgeAligned, freq);//�ҷ�
	
	TPM_PWM_ChangeDuty(HW_TPM0,0,0);
	TPM_PWM_ChangeDuty(HW_TPM0,1,0);
	TPM_PWM_ChangeDuty(HW_TPM0,2,0);
	TPM_PWM_ChangeDuty(HW_TPM0,3,0);
	
}

//==============================================================
//��������void Motor_PWM(int32_t Left_PWM,int32_t Right_PWM)
//����������������ƺ���
//������Left_PWM  Right_PWM ����λ����ռ�ձ�Ϊ����ֵ/PWM_MAX
//���أ���
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

//���������־
uint8_t Motor_start=0;
//==============================================================
//��������void Motor_STOP(void)
//����������ʧ�ܵ��
//��������
//���أ���
//==============================================================
void Motor_STOP(void)
{
	Motor_PWM(0,0);
	Motor_start=0;
}
//==============================================================
//��������void Motor_START(void)
//����������ʹ�ܵ��
//��������
//���أ���
//==============================================================
void Motor_START(void)
{
	Motor_start=1;
}
