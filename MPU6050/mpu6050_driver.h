#ifndef __MPU6050_DRIVER_H__
#define __MPU6050_DRIVER_H__
#include "main.h"
#define	ACCEL_XOUT_H	          0x3B
#define MPU6050_DEVICE_ADDRESS    0xD0
#define MPU6050_DATA_START        ACCEL_XOUT_H   //�������ݴ�ŵ�ַ�������ģ�����һ������
typedef struct __MPU6050_RAW_Data__
{
    short Accel_X;  //�Ĵ���ԭ��X����ٶȱ�ʾֵ
    short Accel_Y;  //�Ĵ���ԭ��Y����ٶȱ�ʾֵ
    short Accel_Z;  //�Ĵ���ԭ��Z����ٶȱ�ʾֵ
    short Temp;     //�Ĵ���ԭ���¶ȱ�ʾֵ
    short Gyro_X;   //�Ĵ���ԭ��X�������Ǳ�ʾֵ
    short Gyro_Y;   //�Ĵ���ԭ��Y�������Ǳ�ʾֵ
    short Gyro_Z;   //�Ĵ���ԭ��Z�������Ǳ�ʾֵ
	  short Mag_X;   //�Ĵ���ԭ��X�������Ǳ�ʾֵ
    short Mag_Y;   //�Ĵ���ԭ��Y�������Ǳ�ʾֵ
    short Mag_Z;   //�Ĵ���ԭ��Z�������Ǳ�ʾֵ
	
}MPU6050_RAW_DATA;

typedef struct __MPU6050_REAL_Data__
{
    float Accel_X;  //ת����ʵ�ʵ�X����ٶȣ�
    float Accel_Y;  //ת����ʵ�ʵ�Y����ٶȣ�
    float Accel_Z;  //ת����ʵ�ʵ�Z����ٶȣ�
    float Temp;     //ת����ʵ�ʵ��¶ȣ���λΪ���϶�
    float Gyro_X;   //ת����ʵ�ʵ�X��Ǽ��ٶȣ�
    float Gyro_Y;   //ת����ʵ�ʵ�Y��Ǽ��ٶȣ�
    float Gyro_Z;   //ת����ʵ�ʵ�Z��Ǽ��ٶ�
	  float Mag_X;   //ת����ʵ�ʵ�X��Ǽ��ٶȣ�
    float Mag_Y;   //ת����ʵ�ʵ�Y��Ǽ��ٶȣ�
    float Mag_Z;   //ת����ʵ�ʵ�Z��Ǽ��ٶ�
	
}MPU6050_REAL_DATA;
extern volatile MPU6050_RAW_DATA    MPU6050_Raw_Data; 
extern volatile MPU6050_REAL_DATA   MPU6050_Real_Data;
void MPU6050_getMotion(float* ax, float* ay, float* az, float* wax, float* way, float* waz,float* wx, float* wy, float* wz);
#endif
