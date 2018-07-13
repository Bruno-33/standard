#include "main.h"
volatile MPU6050_RAW_DATA    MPU6050_Raw_Data;    //原始数据
volatile MPU6050_REAL_DATA   MPU6050_Real_Data;
extern uint8_t Re_buf[11];
float MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz,MPU6050_Lastwax,MPU6050_Lastway,MPU6050_Lastwaz,MPU6050_Lastwx,MPU6050_Lastwy,MPU6050_Lastwz;
extern int MPU6050_Readdone;
float MPU6050_FIFO[9][11] = {0};
/*
*函数原型：        int MPU6050_a_DataSave(void)
*功能：            MPU6050数据处理，取平均值
*/
void MPU6050_a_DataSave(float ax,float ay,float az) 
{
	MPU6050_FIFO[0][0]=ax;
	MPU6050_FIFO[1][0]=ay;
	MPU6050_FIFO[2][0]=az;

}
void MPU6050_wa_DataSave(float wax,float way,float waz) //[0]-[9]???10??? [10]?10???????
{
	MPU6050_FIFO[3][0]=wax;
	MPU6050_FIFO[4][0]=way;
	MPU6050_FIFO[5][0]=waz;
}
void MPU6050_w_DataSave(float wx,float wy,float wz) //[0]-[9]???10??? [10]?10???????
{
	MPU6050_FIFO[6][0]=wx;
	MPU6050_FIFO[7][0]=wy;
	MPU6050_FIFO[8][0]=wz;
}


void MPU6050_getMotion(float* ax, float* ay, float* az, float* wax, float* way, float* waz,float* wx, float* wy, float* wz) 
{
	if(MPU6050_Readdone==1)
	{
		if(Re_buf[0]==0x55) //0x55是数据包的开头 
		{  
			switch(Re_buf [1])
			{
				case 0x51:      //加速度包
					MPU6050_Lastax= ((short)(Re_buf [3]<<8| Re_buf [2]))/32768.0*16*9.8;
					MPU6050_Lastay= ((short)(Re_buf [5]<<8| Re_buf [4]))/32768.0*16*9.8;
					MPU6050_Lastaz= ((short)(Re_buf [7]<<8| Re_buf [6]))/32768.0*16*9.8;
					MPU6050_a_DataSave(MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz);
					break;
				case 0x52:
					MPU6050_Lastwax= ((short)(Re_buf [3]<<8| Re_buf [2]))/32768.0*2000;
					MPU6050_Lastway= ((short)(Re_buf [5]<<8| Re_buf [4]))/32768.0*2000;
					MPU6050_Lastwaz= ((short)(Re_buf [7]<<8| Re_buf [6]))/32768.0*2000;
					MPU6050_wa_DataSave(MPU6050_Lastwax,MPU6050_Lastway,MPU6050_Lastwaz);
					break;
				case 0x53:
					MPU6050_Lastwx= ((short)(Re_buf [3]<<8| Re_buf [2]))/32768.0*180;
					MPU6050_Lastwy= ((short)(Re_buf [5]<<8| Re_buf [4]))/32768.0*180;
					MPU6050_Lastwz= ((short)(Re_buf [7]<<8| Re_buf [6]))/32768.0*180;
					MPU6050_w_DataSave(MPU6050_Lastwx,MPU6050_Lastwy,MPU6050_Lastwz);
					break;
			} 
			//*ax = MPU6050_FIFO[0][0]+9.8*(sin(MPU6050_FIFO[7][0]/180*3.14159);//=MPU6050_FIFO[0][10];
			//*ay = MPU6050_FIFO[1][0]-9.8*sin(MPU6050_FIFO[6][0]/180*3.14159);//=MPU6050_FIFO[1][10];
			*az = MPU6050_FIFO[2][0];//=MPU6050_FIFO[2][10];
			*wax = MPU6050_FIFO[3][0];//=MPU6050_FIFO[3][10];
			*way = MPU6050_FIFO[4][0];//=MPU6050_FIFO[4][10];
			*waz = MPU6050_FIFO[5][0];//=MPU6050_FIFO[5][10];
			*wx  =MPU6050_FIFO[6][0] ;
			*wy = MPU6050_FIFO[7][0] ;
			*wz = MPU6050_FIFO[8][0] ;
		} 
		else
		{       
		//	*ax = MPU6050_FIFO[0][0]+9.8*sin(MPU6050_FIFO[7][0]/180*3.14159);//9.8*sin(MPU6050_FIFO[7][0]/180*3.14159);//=MPU6050_FIFO[0][10];
		//	*ay = MPU6050_FIFO[1][0]-9.8*sin(MPU6050_FIFO[6][0]/180*3.14159);//9.8*sin(MPU6050_FIFO[6][0]/180*3.14159);//=MPU6050_FIFO[1][10];
			*az = MPU6050_FIFO[2][0];//=MPU6050_FIFO[2][10];
			*wax = MPU6050_FIFO[3][0];//=MPU6050_FIFO[3][10];
			*way = MPU6050_FIFO[4][0];//=MPU6050_FIFO[4][10];
			*waz = MPU6050_FIFO[5][0];//=MPU6050_FIFO[5][10];
			*wx  =MPU6050_FIFO[6][0] ;
			*wy = MPU6050_FIFO[7][0] ;
			*wz = MPU6050_FIFO[8][0] ;
		}
		MPU6050_Readdone=0;
	}
}
