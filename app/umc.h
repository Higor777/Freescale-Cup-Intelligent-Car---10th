#ifndef __UMC_H_
#define	__UMC_H_

#include <stdint.h>
#include <stdbool.h>

#define QUEUE_SIZE		777     //���г���
#define UART_REC_LEN	100		//���ڽ������ݳ���

//�������ݹ������
#define UM_UART_DATA		0x01U
#define USMART_UART_DATA	0x02U
#define NONE_UART_DATA		0x00U

//����λ��ͨѶЭ��
#define OP1_BYTE	0xFFU
#define OP2_BYTE	0x01U
#define OP3_BYTE	0xCCU
#define END1_BYTE	0xEEU
#define END2_BYTE	0xDDU
#define CCD_DATA_SEL		0x01U
#define CCD_DATA_SEL_LEN	128

//�������
typedef struct{
	uint8_t arr[QUEUE_SIZE];
	uint16_t head, tail;
}Queue;

//����״̬
typedef enum
{
    Queue_Empty,
    Queue_Full,
    Queue_NotFull
}Queue_State_Type;

//����״̬
typedef enum
{
  RECOP1,		//��ʼ�ֽ�1
	RECOP2,		//��ʼ�ֽ�2
	RECOP3,		//��ʼ�ֽ�3
	RECLEN1,	//�����ֽ�1
	RECLEN2,	//�����ֽ�2
	RECSEL,		//�����ֽ�
	RECCHECK,	//У���ֽ�
	RECEND1,	//�����ֽ�1
	RECEND2		//�����ֽ�2
}Recstate_Type;

//���ݽ���״̬
typedef enum
{
  USMART_Receiver,	//�ڴ���USMART����
	UM_Receiver,		//�ڴ�����λ������
	ResetReceiver		//��λ״̬��
}ReceiverSTA_Type;

////���ڷ�������ָ��
//typedef struct
//{
//	uint8_t *head;
//	uint8_t *tail;
//}UART_TX_DATA_Type;

//extern Queue UART_RX_Queue;		//���ڽ��ն���
extern uint8_t UART_RX_DATA[UART_REC_LEN];	//���ڽ��յ�����
extern uint16_t UART_RX_STA[2];		//��������״̬����һ����¼���ݳ��ȣ��ڶ�����¼���ݹ���
//extern UART_TX_DATA_Type UART_TXDATA;


 void Queue_Init(volatile Queue * pQueue);
 uint8_t Queue_Get(volatile Queue * pQueue);
 void Queue_Put(volatile Queue * pQueue, uint8_t ch);
 Queue_State_Type Queue_State(volatile Queue * pQueue);
static void UART_RX_ISR(uint16_t byteRec);
static void UART_TX_ISR(uint16_t * byteToSend);
//void UM_DATA_Receiver(ReceiverSTA_Type *pReceiver_STA, uint8_t data);
//void USMART_DATA_Receiver(ReceiverSTA_Type *pReceiver_STA, uint8_t data);
static void Reset_Receiver(ReceiverSTA_Type *pReceiver_STA);

void UART_Send_DATA(uint32_t instance,uint8_t *data, uint16_t len);
void UART_DATA_Processor(void);
void UMC_Init(void);
void UM_Upload_Data(uint32_t instance);
void SendImageData(uint8_t * ImageData);
void SendData(void);

#endif
