#ifndef __CCD_H
#define __CCD_H

#include <stdint.h>
#include "gpio.h"

#define CCD_CLK_Set()   do{GPIO_WriteBit(HW_GPIOA,5,1);}while(0)
#define CCD_CLK_Reset() do{GPIO_WriteBit(HW_GPIOA,5,0);}while(0)
#define CCD_SI_Set()    do{GPIO_WriteBit(HW_GPIOA,6,1);}while(0)
#define CCD_SI_Reset()  do{GPIO_WriteBit(HW_GPIOA,6,0);}while(0)

#define CCD2_CLK_Set()   do{GPIO_WriteBit(HW_GPIOB,7,1);}while(0)	
#define CCD2_CLK_Reset() do{GPIO_WriteBit(HW_GPIOB,7,0);}while(0)
#define CCD2_SI_Set()    do{GPIO_WriteBit(HW_GPIOB,8,1);}while(0)
#define CCD2_SI_Reset()  do{GPIO_WriteBit(HW_GPIOB,8,0);}while(0)


extern uint8_t ImageData[128];
extern uint8_t ImageData_2[128];

void Delay100Ns(uint8_t ns);
void CCD_Init(void);
void CCD_Start(void);
void CCD_ImageCapture(uint8_t ImageData[]);
void CCD_IntegTime(const uint8_t ImageData[]);
//void CCD_Diff(uint8_t ImageData[],int16_t ImageDataDiff[]);


#endif
