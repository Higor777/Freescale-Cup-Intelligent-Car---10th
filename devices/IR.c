#include "common.h"
#include "IR.h"
#include "gpio.h"

//������ʱΪ1����ʱΪ0
volatile uint8_t Beacon_STA=0;
volatile uint8_t beacon_scan=0;


//==============================================================
//��������void IR_Init(void)
//����������������ճ�ʼ��
//��������
//���أ���
//==============================================================
void IR_Init(void)
{
	GPIO_QuickInit(HW_GPIOA,7, kGPIO_Mode_IPU);
	
	GPIO_CallbackInstall(HW_GPIOA, IR_ISR);

//	GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
}

//==============================================================
//��������void Beacon_Check(void)
//���������������ⷢ������״̬
//��������
//���أ���
//==============================================================
void Beacon_Check(void)
{
	//����Beacon_STA
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
		//���Ÿߵ�ƽ������½���
		GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, true);
	}
	else
	{
		//���ŵ͵�ƽ
		beacon_scan=1;
	}
}

//==============================================================
//��������static void IR_ISR(uint32_t array)
//������������������ж�
//������array  �ж�Դ����
//���أ���
//==============================================================
static void IR_ISR(uint32_t array)
{
	//����7�����ж�
    if(array&(0x01<<7))
	{
		GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_FallingEdge, false);
		
		beacon_scan=1;
	}
}
