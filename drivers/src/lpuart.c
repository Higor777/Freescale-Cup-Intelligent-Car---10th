/**
  ******************************************************************************
  * @file    uart.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "lpuart.h"
#include "gpio.h"
#include "common.h"

/* gloabl const table defination */
LPUART_Type * const UART_InstanceTable[] = LPUART_BASES;
/* callback function slot */
static LPUART_CallBackTxType LpUART_CallBackTable[ARRAY_SIZE(UART_InstanceTable)] = {NULL};
/* special use for printf */
static uint8_t UART_DebugInstance;


/**
 * @brief  ��ʼ��UARTģ�� 
 * @note   �û����Լ��������ŵĸ�������
 * @code
 *      //ʹ��UART0ģ�� ʹ��115200�����ʽ���ͨ��
 *    UART_InitTypeDef UART_InitStruct1;      //����һ���ṹ����
 *    UART_InitStruct1.instance = HW_UART0;   //ѡ��UART0ģ��
 *    UART_InitStruct1.baudrate = 115200;     //����ͨ���ٶ�Ϊ115200
 *    UART_InitStruct1.parityMode = kUART_ParityDisabled; //У��λ��ֹ
 *    UART_InitStruct1.bitPerChar = kUART_8BitsPerChar;   //ÿ֡8bit
 *    UART_Init(&UART_InitStruct1);
 * @endcode
 * @param  UART_InitTypeDef: ���ڹ������ô洢�ṹ��
 *         instance      :оƬ���ڶ˿�
 *         @arg HW_UART0 :оƬ��UART0�˿�
 *         @arg HW_UART1 :оƬ��UART1�˿�
 *         @arg HW_UART2 :оƬ��UART2�˿�
 *         @arg HW_UART3 :оƬ��UART3�˿�
 *         @arg HW_UART4 :оƬ��UART4�˿�
 *         @arg HW_UART5 :оƬ��UART5�˿�
 * @param  baudrate  :����ͨѶ��������
 * @retval None
 */
void UART_Init(UART_InitTypeDef* UART_InitStruct)
{
    uint16_t sbr;
    uint8_t brfa; 
    static bool is_fitst_init = true;
    
	/* param check */
    assert_param(IS_UART_ALL_INSTANCE(UART_InitStruct->instance));
    
    /* enable clock gate */
    SIM->SCGC5 |= SIM_SCGC5_LPUART0_MASK;

    /* clock source */
    SIM->SOPT2 &= ~SIM_SOPT2_LPUART0SRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_LPUART0SRC(1);  //use PLL/2 or FLL
    
    
    #if 0
    /* disable Tx Rx first */
    UART_InstanceTable[UART_InitStruct->instance]->C2 &= ~((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
    
    /* baud rate generation */
    sbr = (uint16_t)((UART_InitStruct->srcClock)/((UART_InitStruct->baudrate)*16));
    brfa = ((32*UART_InitStruct->srcClock)/((UART_InitStruct->baudrate)*16)) - 32*sbr;
    
    /* config baudrate */
    UART_InstanceTable[UART_InitStruct->instance]->BDH |= UART_BDH_SBR(sbr>>8); 
    UART_InstanceTable[UART_InitStruct->instance]->BDL = UART_BDL_SBR(sbr); 
    UART_InstanceTable[UART_InitStruct->instance]->C4 |= UART_C4_BRFA(brfa);
    
    /* parity */
    switch(UART_InitStruct->parityMode)
    {
        case kUART_ParityDisabled: /* standard N 8 N 1*/
            UART_InstanceTable[UART_InitStruct->instance]->C1 &= ~UART_C1_PE_MASK;
            UART_InstanceTable[UART_InitStruct->instance]->C1 &= ~UART_C1_M_MASK;
            break;
        case kUART_ParityEven:/* 8 bit data + 1bit parity */
            UART_InstanceTable[UART_InitStruct->instance]->C1 |= UART_C1_PE_MASK;
            UART_InstanceTable[UART_InitStruct->instance]->C1 &= ~UART_C1_PT_MASK;
            break;
        case kUART_ParityOdd:
            UART_InstanceTable[UART_InitStruct->instance]->C1 |= UART_C1_PE_MASK;
            UART_InstanceTable[UART_InitStruct->instance]->C1 |= UART_C1_PT_MASK;
            break;
        default:
            break;
    }
    
    /* bit per char */
    /* note: Freescale's bit size config in register are including parity bit! */
    switch(UART_InitStruct->bitPerChar)
    {
        case kUART_8BitsPerChar:
            if(UART_InstanceTable[UART_InitStruct->instance]->C1 & UART_C1_PE_MASK)
            {
                /* parity is enabled it's actually 9bit*/
                UART_InstanceTable[UART_InitStruct->instance]->C1 |= UART_C1_M_MASK;
                UART_InstanceTable[UART_InitStruct->instance]->C4 &= ~UART_C4_M10_MASK;    
            }
            else
            {
                UART_InstanceTable[UART_InitStruct->instance]->C1 &= ~UART_C1_M_MASK;
                UART_InstanceTable[UART_InitStruct->instance]->C4 &= ~UART_C4_M10_MASK;    
            }
            break;
        case kUART_9BitsPerChar:
            if(UART_InstanceTable[UART_InitStruct->instance]->C1 & UART_C1_PE_MASK)
            {
                /* parity is enabled it's actually 10 bit*/
                UART_InstanceTable[UART_InitStruct->instance]->C1 |= UART_C1_M_MASK;
                UART_InstanceTable[UART_InitStruct->instance]->C4 |= UART_C4_M10_MASK;  
            } 
            else
            {
                UART_InstanceTable[UART_InitStruct->instance]->C1 |= UART_C1_M_MASK;
                UART_InstanceTable[UART_InitStruct->instance]->C4 &= ~UART_C4_M10_MASK;      
            }
            break;
        default:
            break;
    }
    UART_InstanceTable[UART_InitStruct->instance]->S2 &= ~UART_S2_MSBF_MASK; /* LSB */
    
    /* enable Tx Rx */
    UART_InstanceTable[UART_InitStruct->instance]->C2 |= ((UART_C2_TE_MASK)|(UART_C2_RE_MASK));
    #endif
    /* link debug instance */
    /* if it's first initalized ,link getc and putc to it */
    if(is_fitst_init)
    {
        UART_DebugInstance = UART_InitStruct->instance;
    }
    is_fitst_init = false;
}

void UART_WriteByte(uint32_t instance, char ch)
{
   // while(!(UART_InstanceTable[instance]->S1 & UART_S1_TDRE_MASK));
   // UART_InstanceTable[instance]->D = (uint8_t)(ch & 0xFF);
}

    
