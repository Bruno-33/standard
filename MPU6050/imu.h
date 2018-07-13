#ifndef _IMU_H_
#define _IMU_H_
#include <math.h>
#include "main.h"
#define M_PI  (float)3.1415926535
	
void Init_Quaternion(void);
void IMU_getYawPitchRoll(volatile float * ypr); //������̬
void GetPitchYawGxGyGz(void);


extern volatile float angle[3];
extern volatile float yaw_angle,pitch_angle,roll_angle; //ʹ�õ��ĽǶ�ֵ

#endif

