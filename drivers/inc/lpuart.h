/**
  ******************************************************************************
  * @file    uart.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_UART_H__
#define __CH_LIB_UART_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
	 
#ifdef UART_USE_STDIO
#define UART_printf(fmt,args...)    printf (fmt ,##args)
#endif
     
     
/*!< LPUART Ӳ��ģ��� */
#define HW_LPUART0  (0x00U)

/*!< parity ѡ�� */
typedef enum
{
    kUART_ParityDisabled = 0x0,  // У��λ��ֹ
    kUART_ParityEven     = 0x2,  // 1λ ��У�� 
    kUART_ParityOdd      = 0x3,  // 1λ żУ�� 
} UART_ParityMode_Type;

/*!< ÿ֡����λ���� */
typedef enum 
{
    kUART_8BitsPerChar  = 0,   // 8-bit ���� ������У��λ 
    kUART_9BitsPerChar  = 1,   // 9-bit ���� ������У��λ 
} UART_BitPerChar_Type;

/*!< UART �ص��������� */
typedef void (*LPUART_CallBackTxType)(void* pram);

/*!< �жϼ�DMA���� */
typedef enum
{
    kUART_IT_Tx,                // ����ÿ����һ֡��������ж� 
    kUART_DMA_Tx,               // ����ÿ����һ֡������ɴ���DMA 
    kUART_IT_Rx,                // ����ÿ����һ֡��������ж� 
    kUART_DMA_Rx,               // ����ÿ����һ֡������ɴ���DMA 
}UART_ITDMAConfig_Type;

/*!< UART��ʼ���ṹ */
typedef struct
{
    uint32_t                srcClock;       // ʱ��ԴƵ��
    uint8_t                 instance;       // UART ģ��� HW_UART0~HW_UART5
    uint32_t                baudrate;       // UART ������ 
    UART_ParityMode_Type    parityMode;     // UART У��λ 
    UART_BitPerChar_Type    bitPerChar;     // UART ÿһ֡������λ���� 
}UART_InitTypeDef;
     
/*!< API functions */

uint8_t UART_QuickInit(uint32_t MAP, uint32_t baudrate);
void UART_Init(UART_InitTypeDef * UART_InitStruct);
int UART_printf(const char *format,...);
uint8_t UART_ReadByte(uint32_t instance, uint16_t *ch);
void UART_WriteByte(uint32_t instance, char ch);
void UART_SelectDebugInstance(uint32_t instance);


#ifdef __cplusplus
}
#endif

#endif

