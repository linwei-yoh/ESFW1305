#ifndef __IOCTRL_H__
#define __IOCTRL_H__


#include "Config.h"
#include "MotorCtrl.h"
/******************************************************************
						    宏定义
******************************************************************/
#define SHOW_STOP			0
#define SHOW_FWD			1
#define SHOW_REV			2
#define SHUT_LED			3

#define TRUN_ON_STOP		PD_ODR_ODR4 = 1
#define TRUN_OFF_STOP		PD_ODR_ODR4 = 0
#define TRUN_ON_REV			PD_ODR_ODR5 = 1
#define TRUN_OFF_REV		PD_ODR_ODR5 = 0
#define TRUN_ON_FWD			PD_ODR_ODR6 = 1
#define TRUN_OFF_FWD		PD_ODR_ODR6 = 0


/******************************************************************
						  接口函数申明
******************************************************************/
extern void IOCtrl_Init (void);
extern void IOCtrl_LedCtrl (WORK_STATUS Cmd);





#endif
