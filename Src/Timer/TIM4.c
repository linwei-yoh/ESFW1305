#include "iostm8s103f3.h"
#include "TIM4.h"

uint8 TIM4SIG = 0;
/****************************************************************************
** 函数名称 ：void TIM4_Init (void)
** 函数功能 ：TIM4初始化
****************************************************************************/
void TIM4_Init (void)
{
	CLK_PCKENR1 |= 1 << 4;		//TIM4时钟使能
	
	TIM4_CR1_CEN = 0;			//关闭定时器计数
	
	TIM4_CR1 = (0 << 7)|		//预装载寄存器无缓冲
			   (0 << 3)|		//非单脉冲模式
			   (0 << 2)|		//计数器溢出时发出中断
			   (0 << 1);		//当计数器溢出时产生更新事件
	
	TIM4_IER = 1 << 0;			//使能更新中断
	
	TIM4_PSCR = 7;				//128分频
	
	TIM4_ARR = TIM4_ARR_VALUE;	//预装载值
	
	TIM4_CR1_CEN = 1;			//启动定时器计数
}


/****************************************************************************
** 函数名称 ：__interrupt void TIM4_OVR_IRQHandler (void)
** 函数功能 ：TIM4溢出中断处理
****************************************************************************/
#pragma vector = TIM4_OVR_UIF_vector
__interrupt void TIM4_OVR_IRQHandler (void)
{	
	TIM4SIG = 1;	
	TIM4_SR_UIF = 0;
}


