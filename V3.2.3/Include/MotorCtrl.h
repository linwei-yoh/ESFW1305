#ifndef __MOTORCTRL_H__
#define __MOTORCTRL_H__

#include "Config.h"
#include "PI.h"
/******************************************************************
						  数据类型申明
******************************************************************/
typedef enum {MIdle = 0, MRun, MErr} MOTOR_STATUS;
typedef enum{WIdle = 0,WStop,WFwd,WRev} WORK_STATUS;

typedef struct
{
	uint16  SpdIn;
	uint16  SpdTmp;
	uint16  RefSpd;
	uint16  FdbSpd;
	int16   SpdTar;
	uint16  AccRate;
	uint16  DecRate;
	uint16  AccDly;
	uint16  DecDly;
}SPEED_CVT;

typedef struct
{
	MOTOR_STATUS  Status;
	WORK_STATUS   Work_Sta;
	SPEED_CVT     SpdCvt;
	PIDREG        SpdPid;
	uint16        LockedRTCnt;
	uint16        BounceCnt;
	uint8		  IsWorking;
}MOTOR;

/******************************************************************
						    数据类型缺省值
******************************************************************/
#define SPEED_CVT_DEFAULT	{0, 0, 0, 0, 0, ACC_RATE, DEC_RATE,0,0}
#define SPD_PID_DEFAULT		{0, 0, 0, 3000, 50, 4096L, 0L, 0, 0, 0, 0, 0}
#define MOTOR_DEFAULT		{	MIdle,				\
								WIdle,				\
								SPEED_CVT_DEFAULT,	\
								SPD_PID_DEFAULT,	\
								0,					\
								0,					\
								0					\
							}

/******************************************************************
						  接口函数申明
******************************************************************/
extern void MotorCtrl_Init (void);
extern void MotorCtrl_Process (void);
extern MOTOR Motor;

#endif
