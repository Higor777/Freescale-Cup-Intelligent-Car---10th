#ifndef __KEY_H_
#define	__KEY_H_

#include <stdint.h>
#include <stdbool.h>

#define Key_1    (GPIO_ReadBit(HW_GPIOB,9))
#define Key_2    (GPIO_ReadBit(HW_GPIOB,11))
#define Key_3    (GPIO_ReadBit(HW_GPIOB,16))
#define Key_4    (GPIO_ReadBit(HW_GPIOB,10))
#define M_Encoder_A   (GPIO_ReadBit(HW_GPIOA,2))
#define M_Encoder_B   (GPIO_ReadBit(HW_GPIOA,1))

#define Key_1_MASK		0x01U
#define Key_1_SHIFT 	0
#define Key_2_MASK 		0x02U
#define Key_2_SHIFT 	1
#define Key_3_MASK 		0x04U
#define Key_3_SHIFT 	2
#define Key_4_MASK 		0x08U
#define Key_4_SHIFT 	3

#define M_Encoder_CW_MASK 		0x01U	//˳ʱ��
#define M_Encoder_CW_SHIFT 		0
#define M_Encoder_CCW_MASK 		0x02U	//��ʱ��	
#define M_Encoder_CCW_SHIFT 	1

#define M_Encoder_A_MASK   	0x01U
#define M_Encoder_A_SHIFT	0
#define M_Encoder_B_MASK   	0x02U
#define M_Encoder_B_SHIFT	1

#define KEY_Scan_STA_Update_MASK	0x80U
#define M_Encoder_STA_Update_MASK	0x80U

//�����ȴ�ʱ��
#define Key_KeepTime	30
//���������ʱ����
#define Key_KeepStep	5

//�Ƿ��ڶ�ʱ���ж�����ɨ�谴��IO״̬
//ɨ�谴��IO״̬Ӧÿ10msһ��
#define USE_PIT_KEYSCAN 1

extern volatile uint8_t M_Encoder_STA;

void Key_Init(void);
void Key_Scan(void);
uint8_t Key_STA(void);
void M_Encoder_Init(void);
void M_Encoder_Scan(void);

#endif
