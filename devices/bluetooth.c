#include <stdint.h>
#include "bluetooth.h"
#include "UMC.h"
#include "uart.h"


//==============================================================
//函数名：unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
//功能描述：CRC校验
//参数：Buf 校验数据地址 CRC_CNT 数据长度
//返回：无
//==============================================================
unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++)
		{      
        CRC_Temp ^= Buf[i];
        for (j=0;j<8;j++)
				{
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}


int16_t Out_Data[4] = {0};//要发送的的数据
int Sent_Data[4] = {0};
uint8_t NewDataSent_Flag=0;

//==============================================================
//函数名：void Out_Put_Data(void)
//功能描述：VisualScope  向四通道虚拟示波器上位机发送数据
//参数：无  发送的为全局变量 Out_Data[],两个字节为一个short int
//返回：无
//==============================================================
void Out_Put_Data(void)
{

	unsigned char databuf[10] = {0};
	unsigned char i;
	unsigned short CRC16 = 0;
	

   
	for(i=0;i<4;i++) 
	{
		databuf[i*2]   = Out_Data[i];
		databuf[i*2+1] = Out_Data[i]>>8;
	}

	//CRC校验
	CRC16 = CRC_CHECK(databuf,8);
	databuf[8] = CRC16;
	databuf[9] = CRC16>>8;
 
	UART_Send_DATA(HW_UART0,databuf,10);
}
