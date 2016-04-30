#ifndef __CTRL_H
#define __CTRL_H

#include <stdint.h>

//#define GYRO_OFFSET 473.216672//493.868408
#define GYRO_RATIO 0.9//��Ҫʵ��ȷ��0.36*4
#define ACC_OFFSET 440.489990
#define ACC_RATIO (180.0/(683.719971-201.050003)) //180/(Zmax-Zmin)
#define ACC_RATIO_SIN (2.0/(683.719971-201.050003))
#define OMEGA_EX 0
//#define SIGMA_EX 38.5
#define ANGLE_RATIO (180.0/3.1415926)
#define SPEED_RATIO (1000.0/(500.0*20))
//#define MOTOR_OUT_DEAD_VAL 500
#define CCD_MAXVIEW 110
#define CCD_MINVIEW 90

#define DIR_LEFT 0
#define DIR_RIGHT 1

typedef enum
{
	BSTA_turning1,
	BSTA_turning2,
	BSTA_straight,
	BSTA_turning3,
	BSTA_turning4,
}Block_State_Type;

typedef enum
{
	BU_nonuse,			//��ʹ�õ���
	BU_nostop,			//ʹ�õ����������õ���ͣ��
	BU_motorstop,		//ʹ�õ������������ͣ��
	BU_cutoffstop,		//ʹ�õ�����ֱ�ӹرյ��ͣ��
}Beacon_Usage_Type;

extern uint8_t RightAngle_Mode;

void SetAnglePD(int p,int d);
void SetSpeedPI(int p,int i);
void SetMotorDead(int val);

void AngleCalculate(void);
void AngleCtrl(void);
void SpeedCalculate(void);
void Speed_EX_Ctrl(void);
void SpeedCtrl(void);
void DirectCtrl(void);
void CtrlOut(int nPeroid);
void Run_Forrest(void);

void Remode_Direct(void);
#endif
