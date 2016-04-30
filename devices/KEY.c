#include "common.h"
#include "KEY.h"
#include "gpio.h"

volatile uint8_t KEY_Scan_STA=0;
//KEY_Scan_STA���壺
//Bit		7		6		5		4		3		2		1		0
//Key		���±�ʶ	NC		NC		NC		KEY4	KEY3	KEY2	KEY1
//1��ʾ��Ч��0��ʾ��Ч

volatile uint8_t M_Encoder_STA=0;
//M_Encoder_STA���壺
//Bit		7		6		5		4		3		2		1		0
//Key		���±�ʶ	NC		NC		NC		NC		NC		CCW		CW
//1��ʾ��Ч��0��ʾ��Ч


//==============================================================
//��������void Key_Scan(void))
//��������������IOɨ�躯��
//��������
//���أ���
//==============================================================
void Key_Scan(void)
{
	static uint8_t keysta_old=0;
	uint8_t keysta_new=0x00;
	
	keysta_new|=(~Key_4)&0x01;
	keysta_new=keysta_new<<1;
	keysta_new|=(~Key_3)&0x01;
	keysta_new=keysta_new<<1;
	keysta_new|=(~Key_2)&0x01;
	keysta_new=keysta_new<<1;
	keysta_new|=(~Key_1)&0x01;
	
#if USE_PIT_KEYSCAN==0
	
	keysta_old=keysta_new;
	keysta_new=0x00;
	
	DelayMs(10);	//��������
	
	keysta_new|=(~Key_4)&0x01;
	keysta_new=keysta_new<<1;
	keysta_new|=(~Key_3)&0x01;
	keysta_new=keysta_new<<1;
	keysta_new|=(~Key_2)&0x01;
	keysta_new=keysta_new<<1;
	keysta_new|=(~Key_1)&0x01;
	
#endif
	
	KEY_Scan_STA=keysta_new&keysta_old;
	
	KEY_Scan_STA|=KEY_Scan_STA_Update_MASK;		//��ӱ�ʶλ
	
	keysta_old=keysta_new;
}

//==============================================================
//��������void Key_Init(void)
//��������������IO��ʼ��
//��������
//���أ���
//==============================================================
void Key_Init(void)
{
	GPIO_QuickInit(HW_GPIOB,9, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOB,11, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOB,16, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOB,10, kGPIO_Mode_IFT);
	
	Key_Scan();	//��ʼ��KeyUp_old,KeyDown_old
	
#if USE_PIT_KEYSCAN!=0
	DelayMs(10);
	Key_Scan();
#endif
}


//==============================================================
//��������void M_Encoder_Scan(void)
//������������������ɨ�躯����Ҫÿ1~5ms����һ��,�����������õ����м��ʱ��ɸ��̣����ڵ�����
//��������
//���أ���
//==============================================================
void M_Encoder_Scan(void)
{
	//ab_staΪ������AB��ÿһ��ɨ���״̬��¼����
	//bit0ΪA�࣬bit1ΪB��
	static uint8_t ab_sta_old=0;
	//ab_scansta_oldΪ������AB���ȶ�״̬��¼����
	//bit0ΪA�࣬bit1ΪB��
	static uint8_t ab_scansta_old=0;
	
	uint8_t ab_sta_new=0;
	uint8_t ab_scansta_new=0;
	
	ab_sta_new|=M_Encoder_A&0x01;
	ab_sta_new=ab_sta_new<<1;
	ab_sta_new|=M_Encoder_B&0x01;
	
	//���±�����AB��״̬
	ab_scansta_new=(((ab_sta_new^ab_sta_old)&ab_scansta_old)|((~(ab_sta_new^ab_sta_old))&ab_sta_old))&0x03;
	ab_sta_old=ab_sta_new;
	
	if((ab_scansta_new^ab_scansta_old)&M_Encoder_A_MASK)
	{
		//������A��״̬�ı�
		if(((ab_scansta_old&M_Encoder_A_MASK)>>M_Encoder_A_SHIFT)==((ab_scansta_old&M_Encoder_B_MASK)>>M_Encoder_B_SHIFT))
		{
			M_Encoder_STA|=M_Encoder_CW_MASK;
		}
		else
		{
			M_Encoder_STA|=M_Encoder_CCW_MASK;
		}
		M_Encoder_STA|=M_Encoder_STA_Update_MASK;	//��ӱ�ʶλ
	}
	ab_scansta_old=ab_scansta_new;
}

//==============================================================
//��������void M_Encoder_Init(void)
//������������������ɨ���ʼ��
//��������
//���أ���
//==============================================================
void M_Encoder_Init(void)
{
	GPIO_QuickInit(HW_GPIOA,1, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOA,2, kGPIO_Mode_IFT);
	
	for(uint8_t i=0; i<4; i++)
	{
		M_Encoder_Scan();
		DelayMs(5);
	}
	M_Encoder_STA=0;
}


//==============================================================
//��������uint8_t Key_STA(void)
//�������������ذ���״̬������ʵ�ֶఴ���������
//��������
//���أ�keysta ����״̬
//==============================================================
uint8_t Key_STA(void)
{
	static uint8_t keysta_mask=0xFF;
	static uint8_t keysta_mask_old=0xFF;
	static uint8_t keysta_cnt=0;
	
	uint8_t keysta;
//	uint8_t keysta_out;
	
#if USE_PIT_KEYSCAN==0
	
	Key_Scan();
	
#endif
	
	if(KEY_Scan_STA&0x80)
	{
		keysta=KEY_Scan_STA&0x3F;
		KEY_Scan_STA=0;
	}
	else 
		return 0;
	
	
	//���MASK
	keysta_mask|=~keysta;
	keysta_mask_old|=~keysta;
	
	//cnt����
 	if(keysta_mask&keysta)
	{
		keysta_cnt=0;
		keysta_mask_old=keysta_mask;
	}
	
	//���keysta
	if(keysta_cnt>=Key_KeepTime)
	{
		keysta&=keysta_mask_old;
		keysta_cnt-=Key_KeepStep;
	}
	else
	{
		keysta&=keysta_mask;
		keysta_cnt++;
	}
	
	//�趨MASK
	keysta_mask&=~keysta;
	
	return keysta;
}
