#ifndef __MOTORPARAM_H__
#define __MOTORPARAM_H__

#include "MotorCtrl.h"

typedef enum{DIR_FWD=0,DIR_REV}MOTOR_DIR;
typedef enum{ENABLE = 1,DISABLE=2}BOOL;

extern uint16 MotorParam_GetDisSpd(void);

extern void MotorParam_SetSpdIn(void);
extern void MotorParam_SetDir(MOTOR_DIR Dir);
extern void MotorParam_LockCtrl(BOOL Flag);
extern void MotorParam_SetWorkSta(WORK_STATUS Sta);

extern BOOL Motor_LockEnable;


#endif
