#ifndef __LED_H
#define __LED_H


#define LED_Red1(n)     GPIO_WriteBit(HW_GPIOC, 9, n)
#define LED_Green1(n)   GPIO_WriteBit(HW_GPIOC, 11, n)
#define LED_Blue1(n)    GPIO_WriteBit(HW_GPIOC, 12, n)
#define LED_Yellow1(n)  GPIO_WriteBit(HW_GPIOC, 10, n)
#define LED_Red2(n)     GPIO_WriteBit(HW_GPIOE, 26, n)
#define LED_Green2(n)   GPIO_WriteBit(HW_GPIOE, 31, n)
#define LED_Blue2(n)    GPIO_WriteBit(HW_GPIOE, 24, n)
#define LED_Yellow2(n)  GPIO_WriteBit(HW_GPIOE, 25, n)

//ºËÐÄ°åLED
#define c_LED_1(n)		GPIO_WriteBit(HW_GPIOD, 4, n)
#define c_LED_2(n)		GPIO_WriteBit(HW_GPIOD, 5, n);
#define c_LED_3(n)		GPIO_WriteBit(HW_GPIOD, 6, n);
#define c_LED_4(n)		GPIO_WriteBit(HW_GPIOD, 7, n);
						
void LED_Init(void);
void LED_Config(void);

#endif
