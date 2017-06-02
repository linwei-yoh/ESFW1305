#ifndef __SPDCALC_H__
#define __SPDCALC_H__


#include "Config.h"
/******************************************************************
						    宏定义
******************************************************************/
#define TIM1_ARR_VALUE			(uint16)((Fmaster/64UL)*0.2L)
#define MIN_CAPVALUE			(uint16)((Fmaster/64UL)/(MAX_SPEED*CYC_PULSE/60.0L))
#define INIT_CAPVALUE			(uint16)((Fmaster/64UL)/(BASE_SPEED*CYC_PULSE/60.0L))

#define SPEED_CVT(a)			(uint16)(((Fmaster/64UL)*60UL/12UL)/(a))

/******************************************************************
						  数据类型申明
******************************************************************/
typedef struct
{
	uint8   CapEnble;
	uint16  CapValue;
	uint16  CapErrCnt;
	uint16  CapTmp;
	uint32  FdbSpdSum;
	uint16  FdbSpd[SPDFDB_NUM];
	uint16  FdbCnt;
	uint16  CurSpd;
	uint16  DisSpd;
}SPEED_MOD;

/******************************************************************
						  数据类型缺省值
******************************************************************/
#define SPEED_MOD_DEFAULT		{0, 0, 0, 0, 0, {0}, 0, 0, 0}

/******************************************************************
						  接口函数申明
******************************************************************/
extern void SpdCalc_Init (void);
extern void SpdCalc_Process (void);
extern uint16 SpdCalc_GotSpd (void);
extern uint16 SpdCalc_GotDisSpd(void);






#endif
