#include <stdint.h>
#include "bluetooth.h"
#include "UMC.h"
#include "uart.h"


//==============================================================
//��������unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
//����������CRCУ��
//������Buf У�����ݵ�ַ CRC_CNT ���ݳ���
//���أ���
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


int16_t Out_Data[4] = {0};//Ҫ���͵ĵ�����
int Sent_Data[4] = {0};
uint8_t NewDataSent_Flag=0;

//==============================================================
//��������void Out_Put_Data(void)
//����������VisualScope  ����ͨ������ʾ������λ����������
//��������  ���͵�Ϊȫ�ֱ��� Out_Data[],�����ֽ�Ϊһ��short int
//���أ���
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

	//CRCУ��
	CRC16 = CRC_CHECK(databuf,8);
	databuf[8] = CRC16;
	databuf[9] = CRC16>>8;
 
	UART_Send_DATA(HW_UART0,databuf,10);
}
