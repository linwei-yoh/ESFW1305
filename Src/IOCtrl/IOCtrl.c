#include "iostm8s103f3.h"
#include "IOCtrl.h"


/*****************************************************************************
** 函数名称：void IOCtrl_Init (void)
** 功能描述：IO口初始化
*****************************************************************************/
void IOCtrl_Init (void)
{	
	/*LED_FWD*/	 
	PD_DDR_DDR6 = 1;		//配置PD6为输出
	PD_CR1_C16  = 1;		//推挽输出
	PD_CR2_C26  = 0;		//输出摆率为2MHz
	
	/*LED_REV*/		 
	PD_DDR_DDR5 = 1;		//配置PD5为输出
	PD_CR1_C15  = 1;		//推挽输出
	PD_CR2_C25  = 0;		//输出摆率为2MHz
	
	/*LED_STOP*/	 
	PD_DDR_DDR4 = 1;		//配置PD4为输出
	PD_CR1_C14  = 1;		//推挽输出
	PD_CR2_C24  = 0;		//输出摆率为2MHz
}

/*****************************************************************************
** 函数名称：void IOCtrl_LedCtrl (WORK_STATUS Cmd)
** 功能描述：LED颜色控制
*****************************************************************************/
void IOCtrl_LedCtrl (WORK_STATUS Cmd)
{
	switch(Cmd)
	{
		case WFwd:
			TRUN_ON_FWD;
			TRUN_OFF_REV;
			TRUN_OFF_STOP;
			break;
		case WRev:
			TRUN_OFF_FWD;
			TRUN_ON_REV;
			TRUN_OFF_STOP;
			break;
		case WIdle:
		case WStop:
			TRUN_OFF_FWD;
			TRUN_OFF_REV;
			TRUN_ON_STOP;
			break;
		default:
			break;
	}
}




