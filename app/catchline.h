#ifndef __CATCHLINE_H
#define __CATCHLINE_H

#include <stdint.h>


#define Invalid_Pixel_NUM 4  //±ßÔµÎÞÐ§ÏñËØµã
#define TURNTIMES 5

typedef enum
{
	RSTA_blackline,
	RSTA_findgap,
	RSTA_turning,
	RSTA_delay1,
	RSTA_delay2,
}RightAngle_State_Type;

extern uint8_t Binary_Image[128];
extern uint8_t Binary_Image_2[128];
extern uint8_t Threshold;
extern uint8_t Begin_Pos;
extern uint8_t LeftLine,RightLine,MiddleLine;
extern uint8_t Left_flag,Right_flag,Middle_flag;
extern uint8_t BlackLine;

void SearchBeginPos(uint8_t* pos);
void SearchLine(int16_t begin_pos);
uint8_t	ImageBinaryzation(uint8_t threshold, uint8_t* image, uint8_t* binary_image);
void ImageBinary(void);
void Rightangle_Recognition(void);

#endif
