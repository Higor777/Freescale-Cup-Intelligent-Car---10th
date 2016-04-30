#include "common.h"
#include "Debug.h"
#include "KEY.h"
#include "gpio.h"
#include "Motor.h"
#include "OLED.h"
#include "filter.h"
#include "adc.h"
#include "anglesensor.h"
#include <stdio.h>
#include "sprint.h"
#include "ctrl.h"
#include <string.h>
#include "uart.h"
float Temp_Para=110;//450;
float Threshold_Para=90;

//˳��������飬�������С
ParameterNode_Type P_Ctrl[20];

//==============================================================
//��������void ParameterList_Init(void)
//������������ʼ���ɵ������б�����ѭ��˳���
//������ ��
//���أ���
//==============================================================
void ParameterList_Init(void)
{
	ParameterNode_Type node;
	ParameterNode_Type *blank_node;
	ParameterList_Type insert_place=0;
	
	blank_node=P_Ctrl;
	
	extern float Sigma_EX;
	node.exterdata=&(Sigma_EX);
	node.step=0.1;
	sprintf(node.name,"Sigma_EX"); 
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Ctrl_mode;
	node.exterdata=&(Ctrl_mode);
	node.step=1;
	sprintf(node.name,"Ctrl_mode"); 
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	
	extern float Angle_P;
	node.exterdata=&(Angle_P);
	node.step=10;
	sprintf(node.name,"Angle_P"); 
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Angle_D;
	node.exterdata=&(Angle_D);
	node.step=1;
	sprintf(node.name,"Angle_D");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	
	extern float Speed_P;
	node.exterdata=&(Speed_P);
	node.step=100;
	sprintf(node.name,"Speed_P");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Speed_I;
	node.exterdata=&(Speed_I);
	node.step=10;
	sprintf(node.name,"Speed_I");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Speed_D;
	node.exterdata=&(Speed_D);
	node.step=1;
	sprintf(node.name,"Speed_D");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Direct_P;
	node.exterdata=&(Direct_P);
	node.step=1;
	sprintf(node.name,"Direct_P");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Direct_D;
	node.exterdata=&(Direct_D);
	node.step=1;
	sprintf(node.name,"Direct_D");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	node.exterdata=&(Temp_Para);
	node.step=10;
	sprintf(node.name,"Temp_Para");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	node.exterdata=&(Threshold_Para);
	node.step=1;
	sprintf(node.name,"Threshold");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Dfindgap;
	node.exterdata=&(Dfindgap);
	node.step=100;
	sprintf(node.name,"Dfindgap");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Ddelay1;
	node.exterdata=&(Ddelay1);
	node.step=100;
	sprintf(node.name,"Ddelay1");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	extern float Speed_Setting;
	node.exterdata=&(Speed_Setting);
	node.step=0.1;
	sprintf(node.name,"Speed_SET");
	Insert_ParameterNode(&node,&insert_place,blank_node++);
	
	Show_Parameter(P_Ctrl,0x01|0x02|0x04);
}

//==============================================================
//��������void Data_Init(void)
//�������������ɵ�������ʼ��������������ƫ
//������ ��
//���أ���
//==============================================================
void Data_Init(void)
{
	/*���ɵ�����*/
	extern float Gyro_Offset;
	float temp1=0;
	for(uint8_t i=0; i<10; i++)
	{
		float temp2=0;
		for(uint8_t j=0; j<10; j++)
		{
			temp2+=ADC_QuickReadValue(ENC03_AR2);
			DelayMs(5);
		}
		temp1+=(temp2/10);
	}
	Gyro_Offset=temp1/10;
}

//==============================================================
//��������void Parameter_Change(void)
//���������������ı�������ʾ
//������ ��
//���أ���
//==============================================================
void Parameter_Change(void)
{
	uint8_t keysta=0,m_encoder_sta=0;
	keysta=Key_STA();
	if(M_Encoder_STA&M_Encoder_STA_Update_MASK)
	{
		m_encoder_sta=M_Encoder_STA&(~M_Encoder_STA_Update_MASK);
		M_Encoder_STA=0;
	}
	static ParameterNode_Type* para=P_Ctrl;
	
	if(keysta&Key_1_MASK)
	{
		para=para->prepara;
		Show_Parameter(para,0x01|0x02|0x04);
	}
	
	if(keysta&Key_2_MASK)
	{
		para=para->nextpara;
		Show_Parameter(para,0x01|0x02|0x04);
	}
	
	if(keysta&Key_3_MASK)
	{
		//(para->step)++;
		//Show_Parameter(para,0);
		
		extern Beacon_Usage_Type Beacon_Mode;
		switch(Beacon_Mode)
		{
			case BU_nonuse:
			{
				Beacon_Mode=BU_nostop;
				OLED_Print(80,6,(uint8_t*)"nostop");
				break;
			}
			case BU_nostop:
			{
				Beacon_Mode=BU_motorstop;
				OLED_Print(80,6,(uint8_t*)" motor");
				break;
			}
			case BU_motorstop:
			{
				Beacon_Mode=BU_cutoffstop;
				OLED_Print(80,6,(uint8_t*)"cutoff");
				break;
			}
			case BU_cutoffstop:
			{
				Beacon_Mode=BU_nonuse;
				OLED_Print(80,6,(uint8_t*)"nonuse");
				break;
			}
		}
		
		extern uint8_t RUN_flag;
		RUN_flag=0;
	}
	
	if(keysta&Key_4_MASK)
	{
		//(para->step)--;
		//Show_Parameter(para,0);
		extern uint8_t RightAngle_Mode;
		RightAngle_Mode=0;
		extern uint8_t Begin_Pos;
		Begin_Pos=64;
		extern uint8_t Track_Width;
		Track_Width=100;
		extern uint8_t RUN_flag;
		RUN_flag=0;
		extern uint8_t Block_flag;
		Block_flag=0;
		Motor_START();
		
//		extern uint16_t TraceWidthHash[];
//		for(uint8_t i=0;i<64;i++)
//		{
//			TraceWidthHash[i]=0;
//		}
	}
	extern float Ctrl_mode;
	
	if(m_encoder_sta&M_Encoder_CCW_MASK)
	{
		if(!strcmp(para->name,"Ctrl_mode"))
		{
			Ctrl_mode = 1-Ctrl_mode;
			Show_Parameter(para,0x02);
			if(Ctrl_mode==0)UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, false);
			else UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
		}else
		{
			*(para->exterdata)-=para->step;
			Show_Parameter(para,0x02);
		}
	}
	
	if(m_encoder_sta&M_Encoder_CW_MASK)
	{
		if(!strcmp(para->name,"Ctrl_mode"))
		{
			Ctrl_mode = 1-Ctrl_mode;
			Show_Parameter(para,0x02);
			if(Ctrl_mode==0)UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, false);
			else UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
		}else
		{
			*(para->exterdata)+=para->step;
			Show_Parameter(para,0x02);
		}
	}
}

//==============================================================
//��������void Show_Parameter(ParameterNode_Type *para, uint8_t config)
//������������ʾ����
//������para����ʾ���� config bit0:	��ʾ������ bit1:	��ʾ����  bit2:	��ʾ����ֵ
//���أ���
//==============================================================
void Show_Parameter(ParameterNode_Type *para, uint8_t config)
{
	char str[10];
	
	//��ʾ������
	if(config&0x01)
	{
		sprintf(str,"%-9s",para->name);
		OLED_Print(0,0,(uint8_t*)str);
	}
	//��ʾ����
	if(config&0x02)
	{
		sprint(str,9,2,*(para->exterdata));
		OLED_Print(0,2,(uint8_t*)str);
	}
	//��ʾ����ֵ
	if(config&0x04)
	{
		sprint(str,9,2,para->step);
		OLED_Print(0,4,(uint8_t*)str);
	}
}


//==============================================================
//��������static void Insert_ParameterNode(ParameterNode_Type *insert_node, ParameterList_Type *insert_place, ParameterNode_Type *blank_node)
//���������������½ڵ�
//������insert_node Ҫ����Ľڵ����ݵ�ַ insert_place ��insert_place�ڵ������½ڵ�,��ʹinsert_placeָ���²���Ľڵ� blank_node ����ĵ�ַ
//���أ���
//==============================================================
static void Insert_ParameterNode(ParameterNode_Type *insert_node, ParameterList_Type *insert_place, ParameterNode_Type *blank_node)
{
	//��������
	blank_node->exterdata=insert_node->exterdata;
	for(uint8_t i=0;i<ParameterName_Size;i++)
	{
		blank_node->name[i]=insert_node->name[i];
	}
	blank_node->step=insert_node->step;
	
	//����ڵ�
	if((*insert_place)!=0)
	{
		blank_node->nextpara=(*insert_place)->nextpara;
		((*insert_place)->nextpara)->prepara=blank_node;
		(*insert_place)->nextpara=blank_node;
		blank_node->prepara=(*insert_place);
	}
	else
	{
		blank_node->nextpara=blank_node;
		blank_node->prepara=blank_node;
	}
	(*insert_place)=blank_node;
}

//==============================================================
//��������void Car_Protect(void)
//����������������������̬��̫ƫ��ֱ���Ƕ�ʧ�ܵ��
//��������
//���أ���
//==============================================================
extern float CarSpeed;
void Car_Protect(void)
{
	extern float Angle;
	if((Angle<10)||(Angle>57) || (CarSpeed>19.0) || (CarSpeed<-19.0))
	{
		Motor_STOP();
	}
}
