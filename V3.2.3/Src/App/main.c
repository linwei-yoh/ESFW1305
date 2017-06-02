#include "main.h"



/*****************************************************************************
** 函数名称：int main (void)
** 功能描述：主函数
*****************************************************************************/
int main (void)
{
	uint16 StartDelay;
	
	/*启动延迟等待其他器件完成上电过程*/
	for (StartDelay = 60000;StartDelay > 0;StartDelay--);
	
	asm("sim");  				//禁止中断
	SYSCLK_INIT();				//时钟初始化		
	PanelCtrl_Init();			//面板模块初始化
	MotorCtrl_Init();			//电机控制模块初始化
    MenuData_Init();
	EERPROM_Init();				//EEPROM初始化
	TIM4_Init();				//定时器4初始化	
	asm("rim");  				//全局中断使能
	
	while(1)
	{
		if (TIM4SIG)   
		{	
			MotorCtrl_Process();	//电机控制处理		
			PanelCtrl_Process();	//面板模块处理
			MenuCtrl_Process();		//菜单控制处理		
			EEPROM_Process();		//EEPROM控制
			TIM4SIG = 0;			//清除定时信号
		}
	}
}




