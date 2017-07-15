#include "iostm8s103f3.h"
#include "PwmGen.h"


/****************************************************************************
						      数据类型申明
****************************************************************************/
typedef enum {PwmIdle = 0,PwmDly,PwmTrig} PWM_STATUS;

/****************************************************************************
						      局部变量申明
****************************************************************************/
static PWM_STATUS PwmSta = PwmIdle;

static uint8  TRIG_SW = DIS_TRIGO;
static uint8  TRIG_DIR = 0;
static uint16 TirgDly = 0;

/****************************************************************************
** 函数名称 ：static void TIM2_RegConfig (void)
** 函数功能 ：TIM2寄存器初始化
****************************************************************************/
static void TIM2_RegConfig (void)
{
	HALF_WORD ARRTmp = {0};
	
  	CLK_PCKENR1 |= 1 << 5;				//TIM2时钟使能
	
	TIM2_CR1_CEN = 0;					//关闭定时器计数
	
	TIM2_CR1   = (1 << 7)|				//预装载寄存器缓冲
			     (1 << 3)|				//单脉冲模式
			     (1 << 2)|				//计数器溢出时发出中断
			     (0 << 1);				//允许更新事件
	
	TIM2_IER   = (1 << 0);				//允许更新中断
	
	TIM2_PSCR  = 4;						//16分频
	
	ARRTmp.HalfWord = TIM2_ARR_VALUE;	//预装载值
	TIM2_ARRH  = ARRTmp.BYTE.HByte;	
	TIM2_ARRL  = ARRTmp.BYTE.LByte;	
	
	TIM2_EGR_UG = 1;					//更新寄存器
	
	TIM2_CCMR1 = 0;
	TIM2_CCMR2 = 0;
	TIM2_CCMR3 = 0;
	TIM2_CCER1 = 0;	
	TIM2_CCER2 = 0;				
}

/****************************************************************************
** 函数名称 ：static void TIM2_IOConfig (void)
** 函数功能 ：TIM2的IO口初始化
****************************************************************************/
static void TIM2_IOConfig (void)
{	
	/*FWDO*/
	PC_ODR_ODR5 = 0;		//关闭FWDO输出
	PC_DDR_DDR5 = 1;		//配置PC5为输出
	PC_CR1_C15  = 1;		//推挽输出
	PC_CR2_C25  = 0;		//输出摆率为2MHz
	
	/*REVO*/
	PC_ODR_ODR6 = 0;		//关闭REVO输出
	PC_DDR_DDR6 = 1;		//配置PC6为输出
	PC_CR1_C16  = 1;		//推挽输出
	PC_CR2_C26  = 0;		//输出摆率为2MHz
}

/****************************************************************************
** 函数名称 ：void PwmGen_Init (void)
** 函数功能 ：可控硅触发输出初始化
****************************************************************************/
void PwmGen_Init (void)
{
	TIM2_IOConfig();
	TIM2_RegConfig();
}

/*****************************************************************************
** 函数名称：void IOCtrl_TrunOnTRIAC (void)
** 功能描述：开启FWDO输出
*****************************************************************************/
static void PwmGen_TrunOnTRIAC (void)
{
  	if(TRIG_DIR == 0)
		PC_ODR_ODR5 = 1;
	else
		PC_ODR_ODR6 = 1;
}

/*****************************************************************************
** 函数名称：void IOCtrl_TrunOffTRIAC (void)
** 功能描述：关闭FWDO输出
*****************************************************************************/
static void PwmGen_TrunOffTRIAC (void)
{
	PC_ODR_ODR5 = 0;
	PC_ODR_ODR6 = 0;
}

/*****************************************************************************
** 函数名称：void PwmGen_EnableTRIGO (void)
** 功能描述：允许产生可控硅触发脉冲
*****************************************************************************/
void PwmGen_EnableTRIGO (void)
{
	TRIG_SW = EN_TRIGO;
}

/*****************************************************************************
** 函数名称：void PwmGen_DisableTRIGO (void)
** 功能描述：禁止产生可控硅触发脉冲
*****************************************************************************/
void PwmGen_DisableTRIGO (void)
{
	TRIG_SW = DIS_TRIGO;
}

/*****************************************************************************
** 函数名称：uint8 PwmGen_GotTRIGSW (void)
** 功能描述：获取触发脉冲开关状态
*****************************************************************************/
uint8 PwmGen_GotTRIGSW (void)
{
	return TRIG_SW;
}

/*****************************************************************************
** 函数名称：void PwmGen_SetTRIGDir (uint8 Dir)
** 功能描述：设置可控硅触发转向
*****************************************************************************/
void PwmGen_SetTRIGDir (uint8 Dir)
{
	TRIG_DIR = Dir;
}

/*****************************************************************************
** 函数名称：void PwmGen_SetTRIGStren (int32 Stren)
** 功能描述：设置可控硅触发脉冲宽度
*****************************************************************************/
void PwmGen_SetTRIGStren (int32 Stren)
{
	uint32 Ratio = _IQsat(Stren, 4096L, 0L); 
	
	TirgDly = MAX_TRIG_DLY - (MAX_TRIG_DLY - MIN_TRIG_DLY)*Ratio/4096UL;
}

/****************************************************************************
** 函数名称 ：void PwmGen_GenTRIGO (void)
** 函数功能 ：输出可控硅触发脉冲
****************************************************************************/
void PwmGen_GenTRIGO (void)
{
	HALF_WORD Tmp = {0};
	
	PwmGen_TrunOffTRIAC();
	Tmp.HalfWord = _IQsat(TirgDly, MAX_TRIG_DLY, MIN_TRIG_DLY);
	TIM2_ARRH    = Tmp.BYTE.HByte;
	TIM2_ARRL    = Tmp.BYTE.LByte;
	TIM2_EGR_UG  = 1;

	PwmSta = PwmDly;
	
	Tmp.HalfWord = TRIG_WIDTH;
	TIM2_ARRH    = Tmp.BYTE.HByte;
	TIM2_ARRL    = Tmp.BYTE.LByte;
	
	TIM2_CR1_CEN = 1;
}

/****************************************************************************
** 函数名称 ：__interrupt void TIM2_OVR_IRQHandler (void)
** 函数功能 ：TIM2溢出中断处理
****************************************************************************/
#pragma vector = TIM2_OVR_UIF_vector
__interrupt void TIM2_OVR_IRQHandler (void)
{		
	if (PwmSta == PwmDly)
	{
		PwmGen_TrunOnTRIAC();
		PwmSta = PwmTrig;
		TIM2_CR1_CEN = 1;
	}
	else
	{
		PwmGen_TrunOffTRIAC();
		PwmSta = PwmIdle;
	}
	
	TIM2_SR1_UIF = 0;
}


