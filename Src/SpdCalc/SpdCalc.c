#include "iostm8s103f3.h"
#include "SpdCalc.h"
#include "PwmGen.h"
#include "Eeprom.h"
#include "MenuData.h"
#include "String.h"


static SPEED_MOD SpdMod = SPEED_MOD_DEFAULT;
/****************************************************************************
** 函数名称 ：static void TIM1_RegConfig (void)
** 函数功能 ：TIM1寄存器初始化
****************************************************************************/
static void TIM1_RegConfig (void)
{
	HALF_WORD Tmp = {0};
	
  	CLK_PCKENR1 |= 1 << 7;				//TIM1时钟使能
	
	TIM1_CR1_CEN = 0;					//关闭定时器计数
	
	TIM1_CR1   = (1 << 7)|				//预装载寄存器缓冲
			     (0 << 5)|				//边沿对齐
			     (0 << 4)|				//向上计数
			     (0 << 3)|				//非单脉冲模式
			     (1 << 2)|				//计数器溢出时发出中断
			     (0 << 1);				//允许更新事件
	
	TIM1_CR2   = 0;
	
	TIM1_SMCR  = 0;
	
	TIM1_IER   = (1 << 4)|				//允许捕获4中断
				 (1 << 3)|				//允许捕获3中断
			     (1 << 0);				//使能更新中断
	
	TIM1_CCMR1 = (3 << 0);				//IC1映射在TRC上(为了用软件触发，防止硬件误触发)
	
	TIM1_CCMR3 = (7 << 4)|				//Fmaster/4,N = 8输入滤波(滤波时间128us)
				 (0 << 2)|				//无输入预分频
			     (1 << 0);				//IC3映射在TI3FP3上
	
	TIM1_CCMR4 = (5 << 4)|				//Fmaster/2,N = 8输入滤波（滤波时间64us）
				 (0 << 2)|				//无输入预分频
			     (1 << 0);				//IC4映射在TI4FP4上
	
	TIM1_CCER1 = (1 << 0);				//捕获使能
	
	TIM1_CCER2 = (1 << 5)|				//IC4的下降沿捕获
				 (1 << 4)|				//IC4捕获使能
				 (1 << 1)|				//IC3的下降沿捕获
				 (1 << 0);				//IC3捕获使能
	
	TIM1_PSCRH = 0;						//64分频
	TIM1_PSCRL = 63;
	
	Tmp.HalfWord = TIM1_ARR_VALUE;		//预装载值
	TIM1_ARRH = Tmp.BYTE.HByte;	
	TIM1_ARRL = Tmp.BYTE.LByte;	
	
	TIM1_EGR_UG = 1;					//更新寄存器
	
	TIM1_CR1_CEN = 1;					//启动定时器计数
}

/****************************************************************************
** 函数名称 ：static void TIM1_IOConfig (void)
** 函数功能 ：TIM1的IO口初始化
****************************************************************************/
static void TIM1_IOConfig (void)
{
  	PC_DDR_DDR3 = 0;		//配置PC3为SPDIN（捕获输入）
	PC_CR1_C13  = 0;		//浮空输入
	PC_CR2_C23  = 0;		//禁止外部中断
	
	/*SZERO*/
	PC_DDR_DDR4 = 0;		//配置PC4为输入
	PC_CR1_C14  = 1;		//上拉输入
	PC_CR2_C24  = 0;		//禁止外部中断
}

/****************************************************************************
** 函数名称 ：void SpdCalc_Init (void)
** 函数功能 ：反馈速度运算初始化
****************************************************************************/
void SpdCalc_Init (void)
{
	TIM1_IOConfig();
	TIM1_RegConfig();
}

/****************************************************************************
** 函数名称 ：uint16 Filter_1st(uint16 OldData,uint16 NewData)
** 函数功能 ：一阶滤波
****************************************************************************/
uint16 Filter_1st(uint16 OldData,uint16 NewData)
{
	uint16 usResult;
	
	if(NewData < OldData) usResult = OldData - ((OldData - NewData) * SPD_K  + 128)/ 256;
	else usResult = OldData + ((NewData - OldData) * SPD_K + 128)/ 256;
	
	if(usResult < 5) usResult = 0;
	return usResult;
}

/****************************************************************************
** 函数名称 ：void SpdCalc_Process (void)
** 函数功能 ：反馈速度运算
****************************************************************************/
void SpdCalc_Process (void)
{
	HALF_WORD CapTmp = {0};
	uint16 SpdTmp = 0;
    static uint16 FdbNum = 0;
    
    if(FdbNum != Menu_Param.stPARAM_List.SpdFdb.Val)
    {
        FdbNum = Menu_Param.stPARAM_List.SpdFdb.Val;
        SpdMod.FdbSpdSum = 0;
        SpdMod.FdbCnt = 0;
        
        memset(SpdMod.FdbSpd,0,sizeof(SpdMod.FdbSpd));
    }

	TIM1_EGR_CC1G = 1;
	SpdTmp = (SpdMod.CapValue == 0) ? 0 : SPEED_CVT(SpdMod.CapValue);   //计算反馈速度
	
	/*在速度反馈捕获发生前，预先估算反馈速度*/
	if (SpdMod.CapEnble == 1)
	{
		CapTmp.BYTE.HByte = TIM1_CCR1H;
		CapTmp.BYTE.LByte = TIM1_CCR1L; //上一次捕获的速度
		TIM1_SR1_CC1IF = 0;
		
		if (CapTmp.HalfWord > MIN_CAPVALUE)
			SpdTmp = (SPEED_CVT(CapTmp.HalfWord) < SpdTmp) ? SPEED_CVT(CapTmp.HalfWord) : SpdTmp;
	}
	
	/*反馈速度滑动滤波算法*/
	SpdMod.FdbSpdSum += SpdTmp;
	SpdMod.FdbSpdSum -= SpdMod.FdbSpd[SpdMod.FdbCnt];
	
	SpdMod.FdbSpd[SpdMod.FdbCnt++] = SpdTmp;
	SpdMod.FdbCnt = (SpdMod.FdbCnt >= FdbNum) ? 0 : SpdMod.FdbCnt;
	
	SpdMod.CurSpd = SpdMod.FdbSpdSum/FdbNum;
	
	/*显示速度进行一阶滤波*/
	SpdMod.DisSpd = Filter_1st(SpdMod.DisSpd , SpdMod.CurSpd);
}

/****************************************************************************
** 函数名称 ：uint16 SpdCalc_GotSpd (void)
** 函数功能 ：获取反馈速度
****************************************************************************/
uint16 SpdCalc_GotSpd (void)
{
	return SpdMod.CurSpd;
}

/****************************************************************************
** 函数名称 ：uint16 SpdCalc_GotDisSpd (void)
** 函数功能 ：获取显示用速度
****************************************************************************/
uint16 SpdCalc_GotDisSpd(void)
{
  	return SpdMod.DisSpd;
}

/****************************************************************************
** 函数名称 ：__interrupt void TIM1_OVR_IRQHandler (void)
** 函数功能 ：TIM1溢出中断处理
****************************************************************************/
#pragma vector = TIM1_OVR_UIF_vector
__interrupt void TIM1_OVR_IRQHandler (void)
{
	SpdMod.CapValue = 0;	
	SpdMod.CapEnble = 0;	
	
	TIM1_SR1_UIF = 0;
}

/****************************************************************************
** 函数名称 ：__interrupt void TIM1_CAPCOM_CC3IF_IRQHandler (void)
** 函数功能 ：TIM1捕获中断处理
****************************************************************************/
#pragma vector = TIM1_CAPCOM_CC3IF_vector
__interrupt void TIM1_CAPCOM_CC3IF_IRQHandler (void)
{
	HALF_WORD CapTmp = {0};
	
	//速度反馈中断处理
	if (TIM1_SR1_CC3IF)
	{
		if (SpdMod.CapEnble == 1) 
		{
			CapTmp.BYTE.HByte  = TIM1_CCR3H;
			CapTmp.BYTE.LByte  = TIM1_CCR3L;
			
			if (CapTmp.HalfWord > MIN_CAPVALUE)
			{
				if (CapTmp.HalfWord < SpdMod.CapValue)
				{
					//正常的情况，捕获值是小于前一个值的一半
					if ((SpdMod.CapValue - CapTmp.HalfWord) < SpdMod.CapValue/2)	
					{
						SpdMod.CapErrCnt = 0;
						SpdMod.CapTmp = 0;
						SpdMod.CapValue = CapTmp.HalfWord;
					}
					else
					{
						//为防止电机真实捕获值被当干扰信号处理掉，要对干扰信号进行4次取样，其最大值必是真实值
						if (SpdMod.CapErrCnt < 3) 
						{
							SpdMod.CapTmp = (CapTmp.HalfWord > SpdMod.CapTmp) ? CapTmp.HalfWord : SpdMod.CapTmp;
							SpdMod.CapErrCnt++;
						}
						else
						{
							SpdMod.CapErrCnt = 0;
							SpdMod.CapValue = SpdMod.CapTmp;
							SpdMod.CapTmp = 0;
						}
					}
				}
				else 
				{
					SpdMod.CapErrCnt = 0;
					SpdMod.CapTmp = 0;
					SpdMod.CapValue = CapTmp.HalfWord;
				}
			}
			else 
			{
				SpdMod.CapErrCnt = 0;
				SpdMod.CapTmp = 0;
			}
		}
		else 
		{
			SpdMod.CapErrCnt = 0;
			SpdMod.CapTmp = 0;
			
			//给定一个初始化转速捕获值，使电机不至于启动时过冲
			SpdMod.CapValue = INIT_CAPVALUE;	
			SpdMod.CapEnble = 1;
		}
		
		TIM1_EGR_UG    = 1;
		TIM1_SR1_CC3IF = 0;
	}
	
	//过零信号中断处理
	if (TIM1_SR1_CC4IF)
	{
        EEPROM_Count = 0;
		if (PwmGen_GotTRIGSW() == EN_TRIGO) PwmGen_GenTRIGO();
		TIM1_SR1_CC4IF = 0;
	}
}





