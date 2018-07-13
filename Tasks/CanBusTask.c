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

�����������ʼ������ֵ��Ϊƫ������ڵ��̵���ϣ���Ϊ������̨�����ʼ��ʱ������
*********************************************************************************/
void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg)
{
		// �����ʼ������ֵ��Ϊƫ��
		v->ecd_bias = (msg->Data[0] << 8) | msg->Data[1];  
		v->ecd_value = v->ecd_bias;
		v->last_raw_value = v->ecd_bias;
		v->temp_count++;
}


/********************************************************************************
														EncoderProcess

������������յ��ĵ������Ļش���Ϣ����Բ�ͬ�����ȡ��ͬ����Ϣ
*********************************************************************************/
void EncoderProcess(volatile Encoder *v, CanRxMsg * msg)
{
		int i=0;
		int32_t temp_sum = 0;    
		v->last_raw_value = v->raw_value;
		v->raw_value = (msg->Data[0] << 8) | msg->Data[1];
		v->diff = v->raw_value - v->last_raw_value;
		
		// ���α������ķ���ֵ���̫�󣬱�ʾȦ�������˸ı�
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
		
		// ����õ������ı��������ֵ
		v->ecd_value = v->raw_value + v->round_cnt * 8192;
		
		// ����õ��Ƕ�ֵ����Χ���������
		v->ecd_angle = (float)(v->raw_value - v->ecd_bias) * 360 / 8192 + v->round_cnt * 360;
		v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
		
		if(v->buf_count == RATE_BUF_SIZE)
		{
				v->buf_count = 0;
		}
		
		// �����ٶ�ƽ��ֵ
		for(i = 0;i < RATE_BUF_SIZE; i++)
		{
				temp_sum += v->rate_buf[i];
		}
		
		v->filter_rate = (int32_t)(temp_sum / RATE_BUF_SIZE);			
}

/********************************************************************************
														CanReceiveMsgProcess

����������can�Ļش���Ϣ�����ݱ��ĵı�ʶ����ȡ��ͬ�Ĵ���취
*********************************************************************************/
void CanReceiveMsgProcess(CanRxMsg * msg)
{      
		//GMYawEncoder.ecd_bias = yaw_ecd_bias;
		can_count++;
	
		switch(msg->StdId)
		{
				case CAN_BUS2_MOTOR1_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM1Encoder ,msg)	// ��ȡ���������ĳ�ʼƫ��ֵ
														: EncoderProcess(&CM1Encoder ,msg);        
				}break;
				
				case CAN_BUS2_MOTOR2_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM2Encoder ,msg)	// ��ȡ���������ĳ�ʼƫ��ֵ
														: EncoderProcess(&CM2Encoder ,msg);
				}break;
				
				case CAN_BUS2_MOTOR3_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM3Encoder ,msg)	// ��ȡ���������ĳ�ʼƫ��ֵ
														: EncoderProcess(&CM3Encoder ,msg);   
				}break;
				
				case CAN_BUS2_MOTOR4_FEEDBACK_MSG_ID:
				{
						(can_count<=50) ? GetEncoderBias(&CM4Encoder ,msg)	// ��ȡ���������ĳ�ʼƫ��ֵ
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
//		if(!LostCounterOverflowCheck(fabs(GMYawEncoder.ecd_angle), 70.0f) || GetWorkState() == STOP_STATE)  //�����ֹͣģʽ��һֱι����ֹ��������ʧ��
//		{
//			LostCounterFeed(GetLostCounter(LOST_COUNTER_INDEX_DEADLOCK));
//		}
}

/********************************************************************************
																	Set_CM_Speed

����������Ԥ��Э������̵���巢�͵���ָ�������Ϊ1Mbps
			��һ���ֲ��ġ�RM_820R���ʹ��˵���顷�жԵ�����ձ��ĸ�ʽ��˵��
*********************************************************************************/
void Set_CM_Speed(CAN_TypeDef *CANx, int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{
    CanTxMsg tx_message;
	
		// ��ʶ��ID	0x200
    tx_message.StdId = 0x200;
	
		// ֡����		��׼֡
    tx_message.IDE = CAN_Id_Standard;
	
		// ֡��ʽ		DATA
		tx_message.RTR = CAN_RTR_Data;
	
		// DLC			8�ֽ�
		tx_message.DLC = 0x08;
    
		// �������ID 0x201
		// ����16λ����(-32768~+32767)
    tx_message.Data[0] = (uint8_t)(cm1_iq >> 8);
    tx_message.Data[1] = (uint8_t)cm1_iq;
	
		// �������ID 0x202
		// ����16λ����(-32768~+32767)
    tx_message.Data[2] = (uint8_t)(cm2_iq >> 8);
    tx_message.Data[3] = (uint8_t)cm2_iq;
	
		// �������ID 0x203
		// ����16λ����(-32768~+32767)
    tx_message.Data[4] = (uint8_t)(cm3_iq >> 8);
    tx_message.Data[5] = (uint8_t)cm3_iq;
	
		// �������ID 0x204
		// ����16λ����(-32768~+32767)
    tx_message.Data[6] = (uint8_t)(cm4_iq >> 8);
    tx_message.Data[7] = (uint8_t)cm4_iq;
    
		CAN_Transmit(CANx, &tx_message);
}

/********************************************************************************
																	Set_Gimbal_Current

����������Ԥ��Э�����̨����巢�͵���ָ��
			��һ���ֲ��ġ�RM2017_6623��̨������ʹ��˵���顷�жԵ�����ձ��ĸ�ʽ��˵��
*********************************************************************************/
void Set_Gimbal_Current(CAN_TypeDef *CANx, int16_t gimbal_yaw_iq, int16_t gimbal_pitch_iq)
{
    CanTxMsg tx_message;
		
		// ��ʶ��ID	0x1FF
    tx_message.StdId = 0x1FF;
	
		// ֡����		��׼֡
    tx_message.IDE = CAN_Id_Standard;
	
		// ֡��ʽ		DATA
		tx_message.RTR = CAN_RTR_Data;
	
		// DLC			8�ֽ�
		tx_message.DLC = 0x08;
    
		// Yaw��
		// ����16λ����(-5000~+5000)
    tx_message.Data[0] = (unsigned char)(gimbal_yaw_iq >> 8);
    tx_message.Data[1] = (unsigned char)gimbal_yaw_iq;
	
		// Pitch��
		// ����16λ����(-5000~+5000)
    tx_message.Data[2] = (unsigned char)(gimbal_pitch_iq >> 8);
    tx_message.Data[3] = (unsigned char)gimbal_pitch_iq;
		
		// Roll��
		// ����16λ����(-5000~+5000)
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
		
		// Resv Ԥ������
		// ����16λ����(-5000~+5000)
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		
    CAN_Transmit(CANx, &tx_message);
}
