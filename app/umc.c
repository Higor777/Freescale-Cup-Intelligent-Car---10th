#include "common.h"
#include "UMC.h"
#include "uart.h"
#include <string.h>

volatile Queue UART_RX_Queue;	//串口接收队列
Queue UART_TX_Queue;	//串口发送队列

uint8_t UART_RX_DATA[UART_REC_LEN]; //串口接收的数据
uint16_t UART_RX_STA[2];	//串口数据状态，第一个记录数据长度，第二个记录数据归属


//==============================================================
//函数名：void UMC_Init(void)
//功能描述：串口通信初始化
//参数：无
//返回：无
//==============================================================
extern float Ctrl_mode;
void UMC_Init(void)
{
	Queue_Init(&UART_RX_Queue);
	UART_QuickInit(UART0_RX_PA15_TX_PA14, 115200);
	UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
	if(Ctrl_mode==1)UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
	Queue_Init(&UART_TX_Queue);
	UART_CallbackTxInstall(HW_UART0, UART_TX_ISR);
	//需要发送数据时开启
//	UART_ITDMAConfig(HW_UART0, kUART_IT_Tx, true);
}


//==============================================================
//函数名：void UM_Upload_Data(uint32_t instance)
//功能描述：向上位机发送数据
//参数：instance 发送UART模块
//返回：无
//==============================================================
void UM_Upload_Data(uint32_t instance)
{
	uint8_t buf[300];
	uint16_t len=0;
	uint8_t checksum=0;
	
	//CCD数据
	extern uint8_t ImageData[128];
	buf[len++]=CCD_DATA_SEL;
	for(uint8_t i=0;i<CCD_DATA_SEL_LEN;i++)
	{
		buf[len++]=ImageData[i];
	}
	
	uint8_t tmp[3];
	//起始字节
	tmp[0]=OP1_BYTE;
	tmp[1]=OP2_BYTE;
	tmp[2]=OP3_BYTE;
	UART_Send_DATA(instance,tmp,3);
	//长度字节
	tmp[0]=len>>8;
	checksum+=tmp[0];
	tmp[1]=len;
	checksum+=tmp[1];
	UART_Send_DATA(instance,tmp,2);
	//数据段
	for(uint8_t i=0;i<len;i++)
	{
		checksum+=buf[i];
	}
	UART_Send_DATA(instance,buf,len);
	//校验位
	UART_Send_DATA(instance,&checksum,1);
	//结束字节
	tmp[0]=END1_BYTE;
	tmp[1]=END2_BYTE;
	UART_Send_DATA(instance,tmp,2);
}



//==============================================================
//函数名：void SendData(void)
//功能描述：向上位机发送图像数据
//参数：instance 发送UART模块
//返回：无
//==============================================================
void SendData(void)
{
	uint16_t len=0;
	uint8_t len8h,len8l;
	uint8_t checksum=0;
	extern uint8_t ImageData[],Binary_Image[];
	extern uint8_t ImageData_2[],Binary_Image_2[];
	extern uint8_t Left_flag,Right_flag;
	extern uint8_t LeftLine,RightLine;
	extern uint8_t Begin_Pos;
	
	len=128*2+2+1;
	for(int i=0;i<128;i++)
	{
		checksum+=ImageData_2[i];
		checksum+=Binary_Image_2[i];
	}
	//checksum=checksum+LeftLine+RightLine;
	checksum+=Begin_Pos;
	len8l=(uint8_t)(len);
	len8h=(uint8_t)(len>>8);
	checksum=checksum+len8h+len8l;
	
	UART_Send_DATA(HW_UART0,(uint8_t*)"\xFF",1);
	UART_Send_DATA(HW_UART0,(uint8_t*)"\x01",1);
	UART_Send_DATA(HW_UART0,(uint8_t*)"\xCC",1);
	
	UART_Send_DATA(HW_UART0,&len8h,1);
	UART_Send_DATA(HW_UART0,&len8l,1);
  UART_Send_DATA(HW_UART0,ImageData_2,128);
	UART_Send_DATA(HW_UART0,Binary_Image_2,128);
	
	//UART_Send_DATA(HW_UART0,&LeftLine,1);
	//UART_Send_DATA(HW_UART0,&RightLine,1);
	if(Left_flag)
	{
	  UART_Send_DATA(HW_UART0,&LeftLine,1);
		checksum+=LeftLine;
	}
	else
	{
		UART_Send_DATA(HW_UART0,(uint8_t*)"\x00",1);
		checksum+=(uint8_t)0;
	}
	if(Right_flag)
	{
	  UART_Send_DATA(HW_UART0,&RightLine,1);
		checksum+=RightLine;
	}
	else
	{
		UART_Send_DATA(HW_UART0,(uint8_t*)"\x7F",1);
		checksum+=(uint8_t)127;
	}
	UART_Send_DATA(HW_UART0,&Begin_Pos,1);
	UART_Send_DATA(HW_UART0,&checksum,1);
	
	UART_Send_DATA(HW_UART0,(uint8_t*)"\xEE",1);
	UART_Send_DATA(HW_UART0,(uint8_t*)"\xDD",1);
}



//以下是队列基本操作函数
/***************************************我是分割线***************************************/
 void Queue_Init(volatile Queue * pQueue)
{
	memset(pQueue, 0, sizeof(Queue));
}

 uint8_t Queue_Get(volatile Queue * pQueue)
{
	uint8_t data = pQueue->arr[pQueue->head];
	pQueue->head = (pQueue->head + 1) % QUEUE_SIZE;
	return data;
}

 void Queue_Put(volatile Queue * pQueue, uint8_t ch)
{
	pQueue->arr[pQueue->tail] = ch;
	pQueue->tail = (pQueue->tail + 1) % QUEUE_SIZE;
}

 Queue_State_Type Queue_State(volatile Queue * pQueue)
{
	if (pQueue->head == pQueue->tail)
		return Queue_Empty;
	else if ((pQueue->tail + 1) % QUEUE_SIZE == pQueue->head)
		return Queue_Full;
	else
		return Queue_NotFull;
}

//以下是串口处理函数
/***************************************我是分割线***************************************/

//==============================================================
//函数名：static void UART_RX_ISR(uint16_t byteRec)
//功能描述：串口接收中断
//参数：byteRec 接收到的数据
//返回：无
//==============================================================
static void UART_RX_ISR(uint16_t byteRec)
{
    if(Queue_State(&UART_RX_Queue)==Queue_Full)
	{
		//队列溢出处理
	}
	else
	{
		Queue_Put(&UART_RX_Queue,(uint8_t)byteRec);
	}
}


//==============================================================
//函数名：static void UART_TX_ISR(uint16_t * byteToSend)
//功能描述：串口发送中断
//参数：byteToSend 需要发送的数据
//返回：无
//==============================================================
static void UART_TX_ISR(uint16_t * byteToSend)
{
	
	*byteToSend=(uint16_t)Queue_Get(&UART_TX_Queue);
	if(Queue_State(&UART_TX_Queue)==Queue_Empty)
	{
		UART_ITDMAConfig(HW_UART0, kUART_IT_Tx, false);
	}
}


//==============================================================
//函数名：void UART_Send_DATA(uint32_t instance,uint8_t *data, uint16_t len)
//功能描述：串口数据发送函数
//参数：instance 发送UART模块 data 数据地址 len 数据长度
//返回：无
//==============================================================
void UART_Send_DATA(uint32_t instance,uint8_t *data, uint16_t len)
{
	uint8_t i;
	
	for(i=0;i<len;i++)
	{
		Queue_Put(&UART_TX_Queue,data[i]);
		if(Queue_State(&UART_TX_Queue)==Queue_Full)
		{
			break;
		}
	}
	
	if(0)
	{
		//开启了DMA发送数据
	}
	else if(1)
	{
		//使用中断发送数据
		UART_ITDMAConfig(instance, kUART_IT_Tx, true);
	}
	else
	{
		while(Queue_State(&UART_TX_Queue)!=Queue_Empty)
		{
			UART_WriteByte(instance,(uint16_t)Queue_Get(&UART_TX_Queue));
		}
	}
}


