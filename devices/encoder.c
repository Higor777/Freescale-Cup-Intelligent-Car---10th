#include "tpm.h"
#include "encoder.h"
#include "common.h"

//���������ݲɼ����ٶȷ���ʹ��D������ʶ��
Encoder_Speed_Type LeftSpeed={0,0},RightSpeed={0,0};

//==============================================================
//��������void Encoder_Init(void)
//�������������������������ʼ��
//��������
//���أ���
//==============================================================
void Encoder_Init(void)
{
	GPIO_QuickInit(HW_GPIOE,29, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOB,17, kGPIO_Mode_IFT);
	tpm_pulse_init(HW_TPM1,TPM_CLKIN0,kTPM_ClockDiv1);
	tpm_pulse_init(HW_TPM2,TPM_CLKIN1,kTPM_ClockDiv1);
	tpm_pulse_clean(HW_TPM1);
	tpm_pulse_clean(HW_TPM2);
}

//==============================================================
//��������void Encoder_GetPulse(void)
//������������ȡ����������
//��������
//���أ���
//==============================================================
void Encoder_GetPulse(void)
{
	if(Encoder_LeftDir()==1)
	{
		LeftSpeed.value=-(int)tpm_pulse_get(HW_TPM1);
	}
	else
	{
		LeftSpeed.value=(int)tpm_pulse_get(HW_TPM1);
	}
  tpm_pulse_clean(HW_TPM1);
	
	if(Encoder_RightDir()==0)
	{
		RightSpeed.value=-(int)tpm_pulse_get(HW_TPM2);
	}
	else
	{
		RightSpeed.value=(int)tpm_pulse_get(HW_TPM2);
	}
	tpm_pulse_clean(HW_TPM2);
}
