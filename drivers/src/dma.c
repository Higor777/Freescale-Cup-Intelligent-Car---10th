/**
  ******************************************************************************
  * @file    dma.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    ���ļ�ΪоƬDMAģ��ĵײ㹦�ܺ���
  ******************************************************************************
  */

#include "dma.h"
#include "common.h"

static DMA_CallBackType DMA_CallBackTable[4] = {NULL};

static const IRQn_Type DMA_IRQnTable[] = 
{
    DMA0_IRQn,
    DMA1_IRQn,
    DMA2_IRQn,
    DMA3_IRQn,
};



/**
 * @brief  ��ʼ��DMAģ��
 * @param  DMA_InitStruct :DMA��ʼ�����ýṹ�壬���dma.h
 * @retval None
 */
void DMA_Init(DMA_InitTypeDef *DMA_InitStruct)
{
	/* enable DMA and DMAMUX clock */
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;    
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
    
    /* disable chl first */
    DMA0->DMA[DMA_InitStruct->chl].DCR &= ~DMA_DCR_ERQ_MASK;
    
    /* dma chl source config */
    DMAMUX0->CHCFG[DMA_InitStruct->chl] = DMAMUX_CHCFG_SOURCE(DMA_InitStruct->chlTriggerSource);
    
    /* trigger mode */
    switch(DMA_InitStruct->triggerSourceMode)
    {
        case kDMA_TriggerSource_Normal:
            DMAMUX0->CHCFG[DMA_InitStruct->chl] &= ~DMAMUX_CHCFG_TRIG_MASK;
            break;
        case kDMA_TriggerSource_Periodic:
            DMAMUX0->CHCFG[DMA_InitStruct->chl] |= DMAMUX_CHCFG_TRIG_MASK;
            break;
        default:
            break;
    }
    
    /* transfer bytes cnt */
    DMA0->DMA[DMA_InitStruct->chl].DSR_BCR = DMA_DSR_BCR_BCR(DMA_InitStruct->transferByteCnt);
    
    /* source config */
    DMA0->DMA[DMA_InitStruct->chl].SAR  = DMA_InitStruct->sAddr;
    DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_SSIZE(DMA_InitStruct->sDataWidth);
    (DMA_InitStruct->sAddrIsInc)?(DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_SINC_MASK):(DMA0->DMA[DMA_InitStruct->chl].DCR &= ~DMA_DCR_SINC_MASK);
    DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_SMOD(DMA_InitStruct->sMod);
    
    /* dest config */
    DMA0->DMA[DMA_InitStruct->chl].DAR  = DMA_InitStruct->dAddr;
    DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_DSIZE(DMA_InitStruct->sDataWidth);
    (DMA_InitStruct->dAddrIsInc)?(DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_DINC_MASK):(DMA0->DMA[DMA_InitStruct->chl].DCR &= ~DMA_DCR_DINC_MASK);
    DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_DMOD(DMA_InitStruct->dMod);
    
    /* defaut: cycle steal */
    DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_CS_MASK;
    
    /* defaut: enable auto disable req */
    DMA0->DMA[DMA_InitStruct->chl].DCR |= DMA_DCR_D_REQ_MASK;
    
    /* enable chl */
    DMAMUX0->CHCFG[DMA_InitStruct->chl] |= DMAMUX_CHCFG_ENBL_MASK;
}

/**
 * @brief  ��� DMA Counter ����ֵ
 * @param  chl: DMAͨ����
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @retval ����ֵ
 */
uint32_t DMA_GetTransferByteCnt(uint8_t chl)
{
    return (DMA0->DMA[chl].DSR_BCR & DMA_DSR_BCR_BCR_MASK )>>DMA_DSR_BCR_BCR_SHIFT;
}

void DMA_SetTransferByteCnt(uint8_t chl, uint32_t val)
{
    DMA0->DMA[chl].DSR_BCR = DMA_DSR_BCR_BCR(val);
}

/**
 * @brief  ʹ��ͨ����Ӧ����
 * @code
 *     //����DMA ��0ͨ���������ݴ���
 *     DMA_EnableRequest(HW_DMA_CH0);
 * @endcode
 * @param  chl: DMAͨ����
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @retval None
 */
void DMA_EnableRequest(uint8_t chl)
{
    DMA0->DMA[chl].DCR |= DMA_DCR_ERQ_MASK;
}


/**
 * @brief  ��ֹͨ����Ӧ����
 * @code
 *     //����DMA ��0ͨ���������ݴ���
 *     DMA_EnableRequest(HW_DMA_CH0);
 * @endcode
 * @param  chl: DMAͨ����
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @retval None
 */
void DMA_DisableRequest(uint8_t chl)
{
    DMA0->DMA[chl].DCR &= ~DMA_DCR_ERQ_MASK;
}


/**
 * @brief  ÿ��һ�������ź�, ֻ����һ�δ���
 */
void DMA_EnableCycleSteal(uint8_t chl, bool flag)
{
    (flag)?
    (DMA0->DMA[chl].DCR |= DMA_DCR_CS_MASK):
    (DMA0->DMA[chl].DCR &= ~DMA_DCR_CS_MASK);
}

/**
 * @brief  ��Majloop ������  �Ƿ��Զ��ر�Request
 */
void DMA_EnableAutoDisableRequest(uint8_t chl , bool flag)
{
    (flag)?
    (DMA0->DMA[chl].DCR |= DMA_DCR_D_REQ_MASK):
    (DMA0->DMA[chl].DCR &= ~DMA_DCR_D_REQ_MASK);
}


/**
 * @brief  ����DMAģ��ָ��ͨ����Ŀ���ַ
 * @param  chl: DMAͨ����
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @param address: 32λ��Ŀ�����ݵ�ַ
 * @retval None
 */
void DMA_SetDestAddress(uint8_t ch, uint32_t address)
{
    DMA0->DMA[ch].DAR = address;
}

uint32_t DMA_GetDestAddress(uint8_t ch)
{
    return DMA0->DMA[ch].DAR;
}

/**
 * @brief  ����DMAģ��ָ��ͨ����Դ��ַ
 * @param  chl: DMAͨ����
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @param address: 32λ��Դ���ݵ�ַ
 * @retval None
 */
void DMA_SetSourceAddress(uint8_t ch, uint32_t address)
{
    DMA0->DMA[ch].SAR = address;
}

uint32_t DMA_GetSourceAddress(uint8_t ch)
{
    return DMA0->DMA[ch].SAR;
}


/**
 * @brief  ����DMA��������ж�
 * @code
 *     //����DMA ��0ͨ���Ĵ�������жϹ���
 *     DMA_StartTransfer(HW_DMA_CH0);
 * @param  chl: DMAͨ����
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @param config: ����ģʽ
 *         @arg kDMA_IT_Half_Disable ��ֹDMA����һ���жϴ���
 *         @arg kDMA_IT_Major_Disable ��ֹDMA��������жϴ���
 *         @arg kDMA_IT_Half ����DMA����һ���жϴ���
 *         @arg kDMA_IT_Major ����DMA��������жϴ���
 * @retval None
 */
void DMA_ITConfig(uint8_t chl, DMA_ITConfig_Type config, bool status)
{
    if(status)
    {
        NVIC_EnableIRQ(DMA_IRQnTable[chl]);
    }
    
    switch(config)
    {
        case kDMA_IT_Major:
            (status)?
            (DMA0->DMA[chl].DCR |= DMA_DCR_EINT_MASK):
            (DMA0->DMA[chl].DCR &= DMA_DCR_EINT_MASK);
            break; 
        default:
            break;
    }
}

/**
 * @brief  ע���жϻص�����
 * @param  chl: DMAͨ����
 *         @arg HW_DMA_CH0
 *         @arg HW_DMA_CH1
 *         @arg HW_DMA_CH2
 *         @arg HW_DMA_CH3
 * @param AppCBFun: �ص�����ָ��
 * @retval None
 */
void DMA_CallbackInstall(uint8_t chl, DMA_CallBackType AppCBFun)
{
    if(AppCBFun != NULL)
    {
        DMA_CallBackTable[chl] = AppCBFun;
    }
}

void DMA0_IRQHandler(void)
{
    DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    if(DMA_CallBackTable[0]) DMA_CallBackTable[0]();
}

void DMA1_IRQHandler(void)
{
    DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    if(DMA_CallBackTable[1]) DMA_CallBackTable[1]();
}

void DMA2_IRQHandler(void)
{
    DMA0->DMA[2].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    if(DMA_CallBackTable[2]) DMA_CallBackTable[2]();
}

void DMA3_IRQHandler(void)
{
    DMA0->DMA[3].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
    if(DMA_CallBackTable[3]) DMA_CallBackTable[3]();
}

