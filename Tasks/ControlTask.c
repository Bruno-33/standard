#include "main.h"
/*
*地盘电机的PID
*/
PID_Regulator_t CMRotatePID = CHASSIS_MOTOR_ROTATE_PID_DEFAULT; 
PID_Regulator_t CM1SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM2SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM3SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator_t CM4SpeedPID = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
/*
*发射机构的pid
*/
PID_Regulator_t ShootMotorPositionPID = SHOOT_MOTOR_POSITION_PID_DEFAULT;      //shoot motor
PID_Regulator_t ShootMotorSpeedPID = SHOOT_MOTOR_SPEED_PID_DEFAULT;
/*
*云台的pid
*/
PID_Regulator_t GMPPositionPID = GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT;     
PID_Regulator_t GMPSpeedPID = GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT;
PID_Regulator_t GMYPositionPID = GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT;			
PID_Regulator_t GMYSpeedPID = GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT;

/*
*PID参数结构体
*/
PIDParamStruct_t PitchPositionSavedPID;        	//PID offset data
PIDParamStruct_t PitchSpeedSavedPID;        	//PID offset data
PIDParamStruct_t YawPositionSavedPID;        	//PID offset data
PIDParamStruct_t YawSpeedSavedPID;        	    //PID offset data
/*--------------------------------------------CTRL Variables----------------------------------------*/
WorkState_e lastWorkState = PREPARE_STATE;
WorkState_e workState = PREPARE_STATE;
RampGen_t GMPitchRamp = RAMP_GEN_DAFAULT;
RampGen_t GMYawRamp = RAMP_GEN_DAFAULT;
	
float error_now=0,error_last=0;
float error_now_yaw=0,error_last_yaw=0;

/*
*********************************************************************************************************
*                                            FUNCTIONS 
*********************************************************************************************************
*/

static void SetWorkState(WorkState_e state)
{
    workState = state;
}

WorkState_e GetWorkState()
{
	return workState;
}
//底盘控制任务
void CMControlLoop(void)
{  
	//底盘旋转量计算
	if(GetWorkState()==PREPARE_STATE) //启动阶段，底盘不旋转
	{
		ChassisSpeedRef.rotate_ref = 0;	 
	}
	else if(fabs(GimbalRef.yaw_angle_dynamic_ref - 0.0f) < 1e-6)
	{
		ChassisSpeedRef.rotate_ref = 0;
	}
	else
	{
	   ChassisSpeedRef.rotate_ref = 5*GimbalRef.yaw_angle_dynamic_ref;
		//printf("rotate_ref:%d\t",ChassisSpeedRef.rotate_ref);                                       
	}           
	
	CM1SpeedPID.ref =  -ChassisSpeedRef.forward_back_ref*0.075 + ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;
	CM2SpeedPID.ref = ChassisSpeedRef.forward_back_ref*0.075 + ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;
	CM3SpeedPID.ref = ChassisSpeedRef.forward_back_ref*0.075 - ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;
	CM4SpeedPID.ref = -ChassisSpeedRef.forward_back_ref*0.075 - ChassisSpeedRef.left_right_ref*0.075 + 0.9*ChassisSpeedRef.rotate_ref;
	//printf("ref:%f\t",CM1SpeedPID.ref);

	CM1SpeedPID.fdb = 0.04*CM1Encoder.filter_rate;
	CM2SpeedPID.fdb = 0.04*CM2Encoder.filter_rate;
	CM3SpeedPID.fdb = 0.04*CM3Encoder.filter_rate;
	CM4SpeedPID.fdb = 0.04*CM4Encoder.filter_rate;
	//printf("fdb:%f\n",CM1SpeedPID.fdb);
	
	CM1SpeedPID.Calc(&CM1SpeedPID);
	CM2SpeedPID.Calc(&CM2SpeedPID);
	CM3SpeedPID.Calc(&CM3SpeedPID);
	CM4SpeedPID.Calc(&CM4SpeedPID);
	
	 if((GetWorkState() == STOP_STATE) || GetWorkState() == CALI_STATE || GetWorkState() == PREPARE_STATE)//if((GetWorkState() == STOP_STATE) ||Is_Serious_Error() || GetWorkState() == CALI_STATE || GetWorkState() == PREPARE_STATE)    //|| dead_lock_flag == 1紧急停车，编码器校准，无控制输入时都会使底盘控制停止
	 {
		 Set_CM_Speed(CAN1, 0,0,0,0);
	 }
	 else
	 {
		// printf("output1:%f\toutput2:%f\toutput3:%f\toutput4:%f\n",CM1SpeedPID.output,CM2SpeedPID.output,CM3SpeedPID.output,CM4SpeedPID.output);
		 //Set_CM_Speed(CAN1, CHASSIS_SPEED_ATTENUATION * CM1SpeedPID.output, CHASSIS_SPEED_ATTENUATION * CM2SpeedPID.output, CHASSIS_SPEED_ATTENUATION * CM3SpeedPID.output, CHASSIS_SPEED_ATTENUATION * CM4SpeedPID.output);		 
		 //Set_CM_Speed(CAN1,1000,1000,1000,1000);
		 Set_CM_Speed(CAN1, CHASSIS_SPEED_BASE * CM1SpeedPID.ref,  CHASSIS_SPEED_BASE * CM2SpeedPID.ref, CHASSIS_SPEED_BASE * CM3SpeedPID.ref,  CHASSIS_SPEED_BASE * CM4SpeedPID.ref);		 
//		 printf("%f\n",CM1SpeedPID.ref);
	 } 

}
//发射机构射击电机任务
int16_t pwm_ccr = 0;
void ShooterMControlLoop(void)	
{				      
	if(GetShootState() == SHOOTING)
	{
		ShootMotorSpeedPID.ref = PID_SHOOT_MOTOR_SPEED;				
	}
	else
	{
		ShootMotorSpeedPID.ref = 0;		
	}
	
	ShootMotorSpeedPID.fdb = GetQuadEncoderDiff();   
	ShootMotorSpeedPID.Calc(&ShootMotorSpeedPID);
	PWM3 = ShootMotorSpeedPID.output;	
}
static uint32_t time_tick_1ms = 0;
//控制任务，放在timer6 1ms定时中断中执行
void Control_Task(void)
{
	time_tick_1ms++;
	WorkStateFSM();
	WorkStateSwitchProcess();
	GMPitchControlLoop();    //计算pitch轴的控制Pid
	GMYawControlLoop();     //计算yaw轴的控制Pid
	SetGimbalMotorOutput();  //pitch和yaw的电机统一输出
	//chassis motor control
	if(time_tick_1ms%4 == 0)         //motor control frequency 4ms
	{
		CMControlLoop();     //底盘电机的Pid计算连同输出
		ShooterMControlLoop();  
	}
	
}
/**********************************************************
*工作状态切换状态机,与1ms定时中断同频率
**********************************************************/

void WorkStateFSM(void)
{
	lastWorkState = workState;
	switch(workState)
	{
		case PREPARE_STATE:
		{
			if(GetInputMode() == STOP) //|| Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			else if(time_tick_1ms > PREPARE_TIME_TICK_MS)
			{
				workState = NORMAL_STATE;
			}			
		}break;
		case NORMAL_STATE:     
		{
			if(GetInputMode() == STOP) //|| Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			
		}break;
		case STANDBY_STATE:     
		{
			if(GetInputMode() == STOP) //|| Is_Serious_Error())
			{
				workState = STOP_STATE;
			}
			else if(IsRemoteBeingAction() || (GetShootState()==SHOOTING) || GetFrictionState() == FRICTION_WHEEL_START_TURNNING)
			{
				workState = NORMAL_STATE;
			}				
		}break;
		case STOP_STATE:   
		{
			if(GetInputMode() != STOP) //&& !Is_Serious_Error())
			{
				workState = PREPARE_STATE;   
			}
		}break;  
		default:
		{
			
		}
	}	
}

static void WorkStateSwitchProcess(void)
{
	//如果从其他模式切换到prapare模式，要将一系列参数初始化
	if((lastWorkState != workState) && (workState == PREPARE_STATE))  
	{
		ControtLoopTaskInit();
		RemoteTaskInit();
	}
}

//云台pitch轴控制程序
void GMPitchControlLoop(void)
{
	GMPPositionPID.kp = PITCH_POSITION_KP_DEFAULTS + PitchPositionSavedPID.kp_offset;
	GMPPositionPID.ki = PITCH_POSITION_KI_DEFAULTS + PitchPositionSavedPID.ki_offset;
	GMPPositionPID.kd = PITCH_POSITION_KD_DEFAULTS + PitchPositionSavedPID.kd_offset;
		
	GMPSpeedPID.kp = PITCH_SPEED_KP_DEFAULTS + PitchSpeedSavedPID.kp_offset;
	GMPSpeedPID.ki = PITCH_SPEED_KI_DEFAULTS + PitchSpeedSavedPID.ki_offset;
	GMPSpeedPID.kd = PITCH_SPEED_KD_DEFAULTS + PitchSpeedSavedPID.kd_offset;
	
	GMPPositionPID.ref = GimbalRef.pitch_angle_dynamic_ref;
	GMPPositionPID.fdb = -GMPitchEncoder.ecd_angle * GMPitchRamp.Calc(&GMPitchRamp);    //加入斜坡函数
	GMPPositionPID.Calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量
	//pitch speed control
	GMPSpeedPID.ref = GMPPositionPID.output;
	//GMPSpeedPID.fdb = MPU6050_Real_Data.Gyro_Y;
	GMPSpeedPID.fdb = GMPitchEncoder.ecd_angle;
	GMPSpeedPID.Calc(&GMPSpeedPID);
}

void GMYawControlLoop(void)
{
	GMYPositionPID.kp = YAW_POSITION_KP_DEFAULTS + YawPositionSavedPID.kp_offset;//  gAppParamStruct.YawPositionPID.kp_offset;  //may be bug if more operation  done
	GMYPositionPID.ki = YAW_POSITION_KI_DEFAULTS + YawPositionSavedPID.ki_offset;
	GMYPositionPID.kd = YAW_POSITION_KD_DEFAULTS + YawPositionSavedPID.kd_offset;
	
	GMYSpeedPID.kp = YAW_SPEED_KP_DEFAULTS + YawSpeedSavedPID.kp_offset;
	GMYSpeedPID.ki = YAW_SPEED_KI_DEFAULTS + YawSpeedSavedPID.ki_offset;
	GMYSpeedPID.kd = YAW_SPEED_KD_DEFAULTS + YawSpeedSavedPID.kd_offset;
	
	GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;
	GMYPositionPID.fdb = -GMYawEncoder.ecd_angle * GMYawRamp.Calc(&GMYawRamp);
	GMYPositionPID.Calc(&GMYPositionPID);
	//yaw speed control
	GMYSpeedPID.ref = GMYPositionPID.output;
	//GMYSpeedPID.fdb = MPU6050_Real_Data.Gyro_Z;
	GMYSpeedPID.fdb = GMYawEncoder.ecd_angle;
	GMYSpeedPID.Calc(&GMYSpeedPID);			
}
void SetGimbalMotorOutput(void)
{
	//云台控制输出								
	if((GetWorkState() == STOP_STATE))   
	{
		Set_Gimbal_Current(CAN2, 0, 0);     //yaw + pitch			
	}
	else
	{		
		
		//Set_Gimbal_Current(CAN2, 10*GimbalRef.pitch_angle_dynamic_ref, 10*GimbalRef.pitch_angle_dynamic_ref);
		Set_Gimbal_Current(CAN2, -(int16_t)GMYSpeedPID.output, -(int16_t)GMPSpeedPID.output);     //yaw + pitch	
		//printf("YAWSpeed:%d\tPITCHspeed:%d\n",-(int16_t)GMYSpeedPID.output,-(int16_t)GMPSpeedPID.output);
	}		
}
//控制任务初始化程序
void ControtLoopTaskInit(void)
{
	//计数初始化
	time_tick_1ms = 0;   //中断中的计数清零
	//设置工作模式
	SetWorkState(PREPARE_STATE);
    //云台给定角度初始化
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = 0.0f;
	//PID初始化
	ShootMotorSpeedPID.Reset(&ShootMotorSpeedPID);
	GMPPositionPID.Reset(&GMPPositionPID);
	GMPSpeedPID.Reset(&GMPSpeedPID);
	GMYPositionPID.Reset(&GMYPositionPID);
	GMYSpeedPID.Reset(&GMYSpeedPID);
	CMRotatePID.Reset(&CMRotatePID);
	CM1SpeedPID.Reset(&CM1SpeedPID);
	CM2SpeedPID.Reset(&CM2SpeedPID);
	CM3SpeedPID.Reset(&CM3SpeedPID);
	CM4SpeedPID.Reset(&CM4SpeedPID);
}
