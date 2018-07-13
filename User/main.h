#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"
#include <math.h>
#include "fifo.h"
#include "LostCounter.h"
#include "common.h"
#include "pid_regulator.h"
#include "ramp.h"
#include "pwm.h"
#include "bsp.h"
#include "mpu6050_interrupt.h"
#include "mpu6050_driver.h"
#include "timer.h"
#include "imu.h"
#include "led.h"
#include "encoder.h"

#include "delay.h"
#include "usart1.h"
#include "usart3.h"
#include "usart1.h"
#include "usart6.h"
#include "can1.h"
#include "can2.h"

#include "CanBusTask.h"
#include "ControlTask.h"
#include "RemoteTask.h"

#endif 
