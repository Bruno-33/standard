#include "main.h"
#include "imu.h"
#include "math.h"
#include "mpu6050_driver.h"

volatile float exInt, eyInt, ezInt;  // ������

volatile float mygetqval[9];	//���ڴ�Ŵ�����ת�����������
static volatile float gx, gy, gz, ax, ay, az, mx, my, mz;   //��������ڴ��ļ���

static volatile float q[4]; //����Ԫ��
volatile uint32_t lastUpdate, now; // �������ڼ��� ��λ us
volatile float angle[3] = {0};
volatile float yaw_temp,pitch_temp,roll_temp;
volatile float last_yaw_temp,last_pitch_temp,last_roll_temp;
volatile float yaw_angle,pitch_angle,roll_angle; //ʹ�õ��ĽǶ�ֵ

// Fast inverse square-root
/**************************ʵ�ֺ���********************************************
*����ԭ��:	   float invSqrt(float x)
*��������:	   ���ټ��� 1/Sqrt(x) 	
��������� Ҫ�����ֵ
��������� ���
*******************************************************************************/
float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

#define BOARD_DOWN 1   //�������泯�°ڷ�

	


/**************************ʵ�ֺ���********************************************
*����ԭ��:	   void IMU_getValues(volatile float * values)
*��������:	 ��ȡ���ٶ� ������ ������ �ĵ�ǰֵ  
��������� �������ŵ������׵�ַ
���ٶ�ֵ��ԭʼ���ݣ�-8192-+8192
���ٶ�ֵ��deg/s
������ֵ��ԭʼ����
���������û��
*******************************************************************************/
void IMU_getValues(volatile float * values) {  
	float accgyroval[9];
	int i;
	//��ȡ���ٶȺ������ǵĵ�ǰADC
	MPU6050_getMotion(&accgyroval[0], &accgyroval[1], &accgyroval[2], &accgyroval[3], &accgyroval[4], &accgyroval[5], &accgyroval[6], &accgyroval[7], &accgyroval[8]);
	MPU6050_Raw_Data.Accel_X = accgyroval[0];
	MPU6050_Raw_Data.Accel_Y = accgyroval[1];
	MPU6050_Raw_Data.Accel_Z = accgyroval[2];
	MPU6050_Raw_Data.Gyro_X = accgyroval[3];
	MPU6050_Raw_Data.Gyro_Y = accgyroval[4];
	MPU6050_Raw_Data.Gyro_Z = accgyroval[5];
	
	
    for(i = 0; i<6; i++) {
      if(i < 3) {
        values[i] = (float) accgyroval[i];
      }
      else {
        values[i] = ((float) accgyroval[i]) / 32.8f; //ת�ɶ�ÿ��
		//�����Ѿ������̸ĳ��� 1000��ÿ��  32.8 ��Ӧ 1��ÿ��
      }
    }
}

#define Kp 2.0f   // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.01f   // integral gain governs rate of convergence of gyroscope biases


static int yaw_count = 0;
void GetPitchYawGxGyGz()
{
	MPU6050_Real_Data.Gyro_X = mygetqval[3];
	MPU6050_Real_Data.Gyro_Y = -mygetqval[4];
	MPU6050_Real_Data.Gyro_Z = mygetqval[5];

	last_yaw_temp = yaw_temp;
	yaw_temp = angle[0]; 
	if(yaw_temp-last_yaw_temp>=330)  //yaw��ǶȾ����������������
	{
		yaw_count--;
	}
	else if (yaw_temp-last_yaw_temp<=-330)
	{
		yaw_count++;
	}
	yaw_angle = yaw_temp + yaw_count*360;  //yaw��Ƕ�
	pitch_angle = angle[1];
    roll_angle = angle[2];	
}
