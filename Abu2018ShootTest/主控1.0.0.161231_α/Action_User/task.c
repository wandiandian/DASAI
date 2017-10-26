#include "includes.h"
#include <app_cfg.h>
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "can.h"
#include "elmo.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_usart.h"
#include "gasvalvecontrol.h"
#include "dma.h"

/*
===============================================================
						信号量定义
===============================================================
*/
OS_EXT INT8U OSCPUUsage;
OS_EVENT *PeriodSem;

//定义互斥型信号量用于管理CAN发送资源
OS_EVENT *CANSendMutex;

static  OS_STK  App_ConfigStk[Config_TASK_START_STK_SIZE];

static 	OS_STK  ShootTaskStk[SHOOT_TASK_STK_SIZE];

void App_Task()
{
	CPU_INT08U  os_err;
	os_err = os_err;		  /*防止警告...*/

	/*创建信号量*/
	PeriodSem				=	OSSemCreate(0);

	//创建互斥型信号量
	CANSendMutex			=   OSMutexCreate(9,&os_err);

	/*创建任务*/
	os_err = OSTaskCreate(	(void (*)(void *)) ConfigTask,				/*初始化任务*/
							(void		  * ) 0,
							(OS_STK		* )&App_ConfigStk[Config_TASK_START_STK_SIZE-1],
							(INT8U		   ) Config_TASK_START_PRIO);

	os_err = OSTaskCreate(	(void (*)(void *)) ShootTask,
							(void		  * ) 0,
							(OS_STK		* )&ShootTaskStk[SHOOT_TASK_STK_SIZE-1],
							(INT8U		   ) SHOOT_TASK_PRIO);

}

/*
   ===============================================================
   初始化任务
   ===============================================================
   */
uint32_t canErrCode = 0;

int32_t motorPos = 0;
int32_t startPos = 0;   

void ConfigTask(void)
{
	CPU_INT08U  os_err;
	os_err = os_err;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//定时器初始化
	TIM_Init(TIM2, 99, 839, 0, 0);   //1ms主定时器

	LEDInit();
	PhotoelectricityInit();
	BeepInit();
	KeyInit();

	
	CAN_Config(CAN1, 500, GPIOB, GPIO_Pin_8, GPIO_Pin_9);

	delay_ms(100);

	//电机初始化及使能
	elmo_Init(CAN1);

	elmo_Enable(CAN1,1);
	
	Pos_cfg(CAN1,1,300000,300000,200000);
	
	ClampClose();
	
	ReadActualPos(CAN1, 1);
	
	delay_ms(20);
	
	startPos = motorPos;
	
	OSTaskSuspend(OS_PRIO_SELF);
}

void ShootTask(void)
{
	CPU_INT08U  os_err;
	os_err = os_err;
#define SHOOT_ANGLE  (45)
#define COUNTS_PER_ROUND (2000.0f)
#define REDUCTION_RATIO (91.0f/6.0f)
#define COUNTS_PER_ROUND_REDUC  (COUNTS_PER_ROUND * REDUCTION_RATIO)

	PosCrl(CAN1,1,1,180000);

	OSSemSet(PeriodSem, 0, &os_err);
	while(1)
	{
		OSSemPend(PeriodSem, 0, &os_err);
		ReadActualPos(CAN1,1);
		ReadActualVel(CAN1,1);
		if(abs((int)(abs(motorPos - startPos)*360.0f/COUNTS_PER_ROUND_REDUC)%360 - SHOOT_ANGLE)<3)
		{
			ClampOpen();
		}
	}
}

