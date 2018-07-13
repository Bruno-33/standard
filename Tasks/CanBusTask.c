#include "main.h"
#include "stdio.h"

static uint32_t can_count = 0;

volatile Encoder CM1Encoder = {0,0,0,0,0,0,0,0,0};
volatile Encoder CM2Encoder = {0,0,0,0,0,0,0,0,0};
volatile Encoder CM3Encoder = {0,0,0,0,0,0,0,0,0};
volatile Encoder CM4Encoder = {0,0,0,0,0,0,0,0,0};
volatile Encoder GMYawEncoder = {0,0,0,0,0,0,0,0,0};
volatile Encoder GMPitchEncoder = {0,0,0,0,0,0,0,0,0};


/********************************************************************************
														GetEncoderBias

描述：保存初始编码器值作为偏差（仅用在底盘电机上，因为期望云台电机初始化时回正）
*********************************************************************************/
void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg)
{
		// 保存初始编码器值作为偏差
		v->ecd_bias = (msg->Data[0] << 8) | msg->Data[1];  
		v->ecd_value = v->ecd_bias;
		v->last_raw_value = v->ecd_bias;
		v->temp_count++;
}


/********************************************************************************
														EncoderProcess

描述：处理接收到的电机电调的回传信息并针对不同的轴获取不同的信息
*********************************************************************************/
void EncoderProcess(volatile Encoder *v, CanRxMsg * msg)
{
		int i=0;
		int32_t temp_sum = 0;    
		v->last_raw_value = v->raw_value;
		v->raw_value = (msg->Data[0] << 8) | msg->Data[1];
		v->diff = v->raw_value - v->last_raw_value;
		
		// 两次编码器的反馈值差别太大，表示圈数发生了改变
		if(v->diff < -7500)    
		{
				v->round_cnt++;
				v->ecd_raw_rate = v->diff + 8192;
		}
		else if(v->diff > 7500)
		{
				v->round_cnt--;
				v->ecd_raw_rate = v->diff - 8192;
		}
		else
		{
				v->ecd_raw_rate = v->diff;
		}
		
		// 计算得到连续的编码器输出值
		v->ecd_value = v->raw_value + v->round_cnt * 8192;
		
		// 计算得到角度值，范围正负无穷大
		v->ecd_angle = (float)(v->raw_value - v->ecd_bias) * 360 / 8192 + v->round_cnt * 360;
		v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
		
		if(v->buf_count == RATE_BUF_SIZE)
		{
				v->buf_count = 0;
		}
		
		// 计算速度平均值
		for(i = 0;i < RATE_BUF_SIZE; i++)
		{
				temp_sum += v->rate_buf[i];
		}
		
		v->filter_rate = (int32_t)(temp_sum / RATE_BUF_SIZE);			
}

/********************************************************************************
														CanReceiveMsgProcess

描述：接收can的回传信息并根据报文的标识符采取不同的处理办法
*********************************************************************************/
void CanReceiveMsgProcess(CanRxMsg * msg)
{      
		//GMYawEncoder.ecd_bias = yaw_ecd_bias;
		can_count++;
	
		switch(msg->StdId)
		{
				case CAN_BUS2_MOTOR1_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM1Encoder ,msg)	// 获取到编码器的初始偏差值
														: EncoderProcess(&CM1Encoder ,msg);        
				}break;
				
				case CAN_BUS2_MOTOR2_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM2Encoder ,msg)	// 获取到编码器的初始偏差值
														: EncoderProcess(&CM2Encoder ,msg);
				}break;
				
				case CAN_BUS2_MOTOR3_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM3Encoder ,msg)	// 获取到编码器的初始偏差值
														: EncoderProcess(&CM3Encoder ,msg);   
				}break;
				
				case CAN_BUS2_MOTOR4_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM4Encoder ,msg)	// 获取到编码器的初始偏差值
														: EncoderProcess(&CM4Encoder ,msg);
				}break;
				
				case CAN_BUS2_MOTOR5_FEEDBACK_MSG_ID:
				{
						EncoderProcess(&GMYawEncoder, msg); 
				}break;
				
				case CAN_BUS2_MOTOR6_FEEDBACK_MSG_ID:
				{
						EncoderProcess(&GMPitchEncoder, msg);
				}break;
				
				default: { }
		}
		
		// check if deadlock, meeans the yaw angle is overflow 
		// time should keep for a long time to avoid bug		
//		if(!LostCounterOverflowCheck(fabs(GMYawEncoder.ecd_angle), 70.0f) || GetWorkState() == STOP_STATE)  //如果是停止模式，一直喂狗防止重新启动失败
//		{
//			LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_DEADLOCK));
//		}
}

/********************************************************************************
																	Set_CM_Speed

描述：根据预定协议给底盘电调板发送电流指令，波特率为1Mbps
			这一部分参阅《RM_820R电调使用说明书》中对电调接收报文格式的说明
*********************************************************************************/
void Set_CM_Speed(CAN_TypeDef *CANx, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{
    CanTxMsg tx_message;
	
		// 标识符ID	0x200
    tx_message.StdId = 0x200;
	
		// 帧类型		标准帧
    tx_message.IDE = CAN_Id_Standard;
	
		// 帧格式		DATA
		tx_message.RTR = CAN_RTR_Data;
	
		// DLC			8字节
		tx_message.DLC = 0x08;
    
		// 电调驱动ID 0x201
		// 给定16位电流(-32768~+32767)
    tx_message.Data[0] = (uint8_t)(cm1_iq >> 8);
    tx_message.Data[1] = (uint8_t)cm1_iq;
	
		// 电调驱动ID 0x202
		// 给定16位电流(-32768~+32767)
    tx_message.Data[2] = (uint8_t)(cm2_iq >> 8);
    tx_message.Data[3] = (uint8_t)cm2_iq;
	
		// 电调驱动ID 0x203
		// 给定16位电流(-32768~+32767)
    tx_message.Data[4] = (uint8_t)(cm3_iq >> 8);
    tx_message.Data[5] = (uint8_t)cm3_iq;
	
		// 电调驱动ID 0x204
		// 给定16位电流(-32768~+32767)
    tx_message.Data[6] = (uint8_t)(cm4_iq >> 8);
    tx_message.Data[7] = (uint8_t)cm4_iq;
    
		CAN_Transmit(CANx, &tx_message);
}

/********************************************************************************
																	Set_Gimbal_Current

描述：根据预定协议给云台电调板发送电流指令
			这一部分参阅《RM2017_6623云台电机电调使用说明书》中对电调接收报文格式的说明
*********************************************************************************/
void Set_Gimbal_Current(CAN_TypeDef *CANx, int16_t gimbal_yaw_iq, int16_t gimbal_pitch_iq)
{
    CanTxMsg tx_message;
		
		// 标识符ID	0x1FF
    tx_message.StdId = 0x1FF;
	
		// 帧类型		标准帧
    tx_message.IDE = CAN_Id_Standard;
	
		// 帧格式		DATA
		tx_message.RTR = CAN_RTR_Data;
	
		// DLC			8字节
		tx_message.DLC = 0x08;
    
		// Yaw轴
		// 给定16位电流(-5000~+5000)
    tx_message.Data[0] = (unsigned char)(gimbal_yaw_iq >> 8);
    tx_message.Data[1] = (unsigned char)gimbal_yaw_iq;
	
		// Pitch轴
		// 给定16位电流(-5000~+5000)
    tx_message.Data[2] = (unsigned char)(gimbal_pitch_iq >> 8);
    tx_message.Data[3] = (unsigned char)gimbal_pitch_iq;
		
		// Roll轴
		// 给定16位电流(-5000~+5000)
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
		
		// Resv 预留的轴
		// 给定16位电流(-5000~+5000)
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		
    CAN_Transmit(CANx, &tx_message);
}
