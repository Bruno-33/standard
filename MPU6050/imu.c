#include "main.h"
#include "imu.h"
#include "math.h"
#include "mpu6050_driver.h"

volatile float exInt, eyInt, ezInt;  // 误差积分

volatile float mygetqval[9];	//用于存放传感器转换结果的数组
static volatile float gx, gy, gz, ax, ay, az, mx, my, mz;   //作用域仅在此文件中

static volatile float q[4]; //　四元数
volatile uint32_t lastUpdate, now; // 采样周期计数 单位 us
volatile float angle[3] = {0};
volatile float yaw_temp,pitch_temp,roll_temp;
volatile float last_yaw_temp,last_pitch_temp,last_roll_temp;
volatile float yaw_angle,pitch_angle,roll_angle; //使用到的角度值

// Fast inverse square-root
/**************************实现函数********************************************
*函数原型:	   float invSqrt(float x)
*功　　能:	   快速计算 1/Sqrt(x) 	
输入参数： 要计算的值
输出参数： 结果
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

#define BOARD_DOWN 1   //板子正面朝下摆放

	


/**************************实现函数********************************************
*函数原型:	   void IMU_getValues(volatile float * values)
*功　　能:	 读取加速度 陀螺仪 磁力计 的当前值  
输入参数： 将结果存放的数组首地址
加速度值：原始数据，-8192-+8192
角速度值：deg/s
磁力计值：原始数据
输出参数：没有
*******************************************************************************/
void IMU_getValues(volatile float * values) {  
	float accgyroval[9];
	int i;
	//读取加速度和陀螺仪的当前ADC
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
        values[i] = ((float) accgyroval[i]) / 32.8f; //转成度每秒
		//这里已经将量程改成了 1000度每秒  32.8 对应 1度每秒
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
	if(yaw_temp-last_yaw_temp>=330)  //yaw轴角度经过处理后变成连续的
	{
		yaw_count--;
	}
	else if (yaw_temp-last_yaw_temp<=-330)
	{
		yaw_count++;
	}
	yaw_angle = yaw_temp + yaw_count*360;  //yaw轴角度
	pitch_angle = angle[1];
    roll_angle = angle[2];	
}
