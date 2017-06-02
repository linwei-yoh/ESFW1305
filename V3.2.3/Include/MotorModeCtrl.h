#ifndef __MOTORMODE_H__
#define __MOTORMODE_H__

#include "MotorParam.h"

typedef enum{CIdle=0,CRun,CStop,CEnd}CYC_STA;

typedef struct
{
  	CYC_STA Cyc_Sta;
	uint8 Fwd_Enable;
	uint8 Rev_Enable;
	uint8 Cyc_Dir;
	uint16 Round_Count;
	uint16 Delay;
	uint16 SecDelay;
}CYCLE;

#define CYCLE_DEFAULT {CIdle,0,0,0,0,0,0}

extern void MotorMode_SetCmd(WORK_STATUS Sta);
extern void MotorMode_NormalProcess(void);

extern void MotorMode_CycReSet(void);
extern void MotorMode_CycleProcess(void);

extern uint16 usCyc_Spd;
extern uint8  ucCys_Mark;

#endif
