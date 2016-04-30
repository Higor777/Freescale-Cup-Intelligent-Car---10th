#ifndef __OELD_H
#define __OELD_H

#include "gpio.h"

//在此修改OLED引脚
#define OLED_SCL_Set()    do{GPIO_WriteBit(HW_GPIOC, 5, 1);}while(0)
#define OLED_SCL_Reset()  do{GPIO_WriteBit(HW_GPIOC, 5, 0);}while(0)
#define OLED_SDA_Set()    do{GPIO_WriteBit(HW_GPIOC, 7, 1);}while(0)
#define OLED_SDA_Reset()  do{GPIO_WriteBit(HW_GPIOC, 7, 0);}while(0)
#define OLED_RST_Reset()  do{GPIO_WriteBit(HW_GPIOC, 8, 0);}while(0)
#define OLED_RST_Set()    do{GPIO_WriteBit(HW_GPIOC, 8, 1);}while(0)
#define OLED_DC_Set()     do{GPIO_WriteBit(HW_GPIOC, 6, 1);}while(0)
#define OLED_DC_Reset()   do{GPIO_WriteBit(HW_GPIOC, 6, 0);}while(0)
#define OLED_CS_Set()     do{GPIO_WriteBit(HW_GPIOC, 4, 1);}while(0)
#define OLED_CS_Reset()   do{GPIO_WriteBit(HW_GPIOC, 4, 0);}while(0)

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xCF

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define u8 uint8_t
#define u16 uint16_t


 
 void OLED_Init(void);
 void OLED_CLS(void);
 void OLED_P6x8Str(u8 x,u8 y,u8 ch[]);
 void OLED_P8x16Str(u8 x,u8 y,u8 ch[]);
 void OLED_P14x16Str(u8 x,u8 y,u8 ch[]);
 void OLED_Print(u8 x, u8 y, u8 ch[]);
 void OLED_PutPixel(u8 x,u8 y);
 void OLED_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 gif);
 void OLED_Set_Pos(u8 x, u8 y);
 void OLED_WrDat(u8 data);
 void OLED_Fill(u8 dat);
 void Delay_ms(u16 ms);

#endif
