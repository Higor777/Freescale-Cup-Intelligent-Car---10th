#include "remote.h"
#include <math.h>
#include "common.h"
#include "motor.h"
#include "debug.h"
#include "led.h"
#include "umc.h"
#include "ctrl.h"
#define data_len  12
//单帧数据形式 0xE0 0x01 data_len字节遥控数据 0x00 0xE0 一字节和校验
void handle_data(uint8_t buf[]);


extern Queue UART_RX_Queue;
enum recstate								   		//状态机 
{
		RECFF1,RECFF2,RECSEL1,RECFF3,RECFF4,RECCHECK1
} Recstate = RECFF1;
	uint16_t checksum=0,cur=0;
	uint8_t  Buf[UART_REC_LEN] = {0};

//==============================================================
//函数名：void Remote_Mode(void)
//功能描述：遥控模式数据接收处理校验状态机
//参数：无
//返回：无
//==============================================================
void Remote_Mode(void)
{

	uint8_t data;

	if(Queue_State(&UART_RX_Queue)==Queue_Empty)
							return;

		data = (uint8_t)Queue_Get(&UART_RX_Queue);
  
		switch (Recstate) 
		{
			case RECFF1:
				if (data == 0xE0)	  
				{	
					checksum = 0;
					Recstate = RECFF2;	
					checksum+=data;		
					cur = 0;								//校验位清零
				}
	
				break;
	
			case RECFF2:
				if (data == 0x01)
					checksum+=data,Recstate = RECSEL1;
				else
					Recstate = RECFF1;
				break;
	
			case RECSEL1:
				checksum += data;
				Buf[cur++] = data;
				if(cur >= data_len)
					Recstate = RECFF3;
				break;
				
			case RECFF3:
				if (data == 0x00)	 //0x01 
				{	
					Recstate = RECFF4;	
					checksum+=data;		
					cur = 0;								//校验位清零
				}
	
				break;
	
			case RECFF4:
				if (data == 0xE0)
					checksum+=data,Recstate = RECCHECK1;
				else
					Recstate = RECFF1;
				break;
				
			case RECCHECK1:
				checksum=checksum%255;
				if(data == checksum)
				{				
					handle_data(Buf);	
					checksum=0;	
					Recstate = RECFF1;	 
				}
				else
					Recstate = RECFF1;
				break;
			 default:
					Recstate = RECFF1;
		}	
}//Remote_Mode

uint16_t testdata[5]={0};

extern float Speed_EX;
extern uint8_t MiddleLine;
//==============================================================
//函数名：void handle_data(uint8_t buf[])
//功能描述：遥控数据处理
//参数：buf 遥控数据地址
//返回：无
//==============================================================
void handle_data(uint8_t buf[])
{


		testdata[0] = buf[0]+((uint16_t)buf[1]<<8);
		testdata[1] = buf[2]+((uint16_t)buf[3]<<8);
		testdata[2] = buf[4]+((uint16_t)buf[5]<<8);
		testdata[3] = buf[6]+((uint16_t)buf[7]<<8);
		testdata[4] = buf[8]+((uint16_t)buf[9]<<8);
		Speed_EX = 11.0*((float)testdata[4]-32768.0)/32768.0;//0x8000--32768.0
	  MiddleLine = 27*(testdata[2]-32768)/32768+64;
		switch (buf[10]&0x0f)
		{
			case 0x08 :
				break;
			case 0x01 :
				break;
			case 0x02 :
				break;
			case 0x04 :
				break;
			default :
				break;
		}
}


