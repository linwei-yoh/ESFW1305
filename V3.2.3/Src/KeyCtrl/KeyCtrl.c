#include "iostm8s103f3.h"
#include "TM1618.h"
#include "MenuData.h"
#include "MotorCtrl.h"

#include "KeyCtrl.h"

static uint8 IO_InRev = 1;
static uint8 IO_InFwd = 1;  // 0 才是触发信号

uint8 Menu_Signal = NoKey;
uint8 Motor_Signal = NoKey;
/*****************************************************************************
** 函数名称：uint8 Key_Read(void)
** 功能描述：按键信号读取
*****************************************************************************/
static uint8 Key_Read(void)
{
	uint8 Read_Val = NoKey;
  
	TM_KeyRead();
	
	if(TM_KeyVal.KS4)
	  Read_Val = K_Stop;
	else if(TM_KeyVal.KS5)
	  Read_Val = K_Rev;
	else if(TM_KeyVal.KS3)
	  Read_Val = K_Fwd;
	else if(TM_KeyVal.KS2)
	  Read_Val = K_Down;
	else if(TM_KeyVal.KS1)
	  Read_Val = K_Up;	  
	else if(KEY_ENTER == 0)
	  Read_Val = K_Enter;
	else if(KEY_MODE == 0)
	  Read_Val = K_Mode;
	
	return Read_Val;
}

/*****************************************************************************
** 函数名称：uint8 KeyCtrl_Scan(void)
** 功能描述：按键扫描 获取按键信号
*****************************************************************************/
static uint8 KeyCtrl_Scan(void)
{
 	static uint8 Scan_Sta 	= 0;
	static uint8 Key_Prev	= NoKey; //上一次按键
	static uint16 Key_Dly  = 0;
	uint8 Key_Val 		 	= NoKey;
    
	Key_Val = Key_Read();
	switch(Scan_Sta)
	{
      case 0: //按键初始扫描
        if(Key_Val != NoKey)
        {
            Key_Prev = Key_Val;
            Key_Dly = 0;
            Scan_Sta = 1;
        }
        break;
      case 1://去抖判定
        if(Key_Val == NoKey || Key_Val != Key_Prev)
            Scan_Sta = 0;
        else if(Key_Dly++ >= 5)
        {
            Key_Dly = 0;
            Scan_Sta = 2;
            return (KEY_DOWN | Key_Prev);   //返回有效按键按下
        }            
        break;
      case 2://按键长按判断
        if(Key_Val == NoKey || Key_Val != Key_Prev)//无按键或 按键值改变
        {
            Scan_Sta = 0;
            return (KEY_UP|Key_Prev);           //返回有效按键松开
        }
        else if(Key_Dly++ > 500) //大于1S
        {
            Key_Dly = 0;
            Scan_Sta = 3;
        }
        break;
      case 3://按键放开判断
        if(Key_Val == NoKey || Key_Val != Key_Prev)//无按键或 按键值改变
        {
            Scan_Sta = 0;
            return (KEY_LUP | Key_Prev);//返回有效按键长按后松开
        }
        else if(Key_Dly++ > 50) //每100ms发送一次长按信号
        {
            Key_Dly = 0;
            return (KEY_LDOWN | Key_Prev);	//返回按键长按信息 
        }
        break;
      default:
        break;
	}
	
  	return NoKey;
}

/*************************************************************
* 函数名称： 	IO_InPutCheck(void)
* 功能介绍：	用于外接信号输入的去抖处理
*************************************************************/
void IO_InPutCheck(void)
{
	  static uint8 Tmp_InRev = 0xff;
	  static uint8 Tmp_InFwd = 0xff;
	  
	  Tmp_InRev = (Tmp_InRev << 1) | REV_IN;
	  Tmp_InFwd = (Tmp_InFwd << 1) | FWD_IN;
	  
	  if(Tmp_InRev == 0)
		IO_InRev = 0;
	  else if(Tmp_InRev == 0xff)
		IO_InRev = 1;
	  
	  if(Tmp_InFwd == 0)
		IO_InFwd = 0;
	  else if(Tmp_InFwd == 0xff)
		IO_InFwd = 1;
} 

/*************************************************************
* 函数名称：uint8 IO_InPutNoStop(void)
* 功能介绍：面板stop无效模式下 外接开关的信号获取
*************************************************************/
uint8 IO_InPutNoStop(void)
{
  if(IO_InFwd == 0 && IO_InRev == 1)
    return K_Fwd;
  else if(IO_InFwd == 1 && IO_InRev == 0)
    return K_Rev;
  else if(IO_InFwd == 1 && IO_InRev == 1)
    return K_Stop;
  
  return NoKey;
}

/*************************************************************
* 函数名称：uint8 IO_InPutWithStop(void)
* 功能介绍：面板stop有效模式下 外接按钮的信号获取
*************************************************************/
uint8 IO_InPutWithStop(void)
{
  if(IO_InFwd == 0 && IO_InRev == 1)
    return K_Fwd;
  else if(IO_InFwd == 1 && IO_InRev == 0)
    return K_Rev;
  
  return NoKey;
}

/*****************************************************
** 函数名称：void KeyCtrl_Init (void)
** 功能描述：按键控制初始化
******************************************************/
void KeyCtrl_Init(void)
{
  	/*ENTER*/
	PA_DDR_DDR1 = 0;		//配置PA1为输入
	PA_CR1_C11  = 1;		//上拉输入
	PA_CR2_C21  = 0;
	
	/*MODE*/
	PA_DDR_DDR2 = 0;		//配置PA2为输入
	PA_CR1_C12  = 1;		//上拉输入
	PA_CR2_C22  = 0;	
    
    /*FWDI*/
	PC_DDR_DDR7 = 0;		//配置PC7输入
	PC_CR1_C17  = 0;		//悬浮输入
	PC_CR2_C27  = 0;		//外部输入信号
	
	/*REVI*/
	PD_DDR_DDR2 = 0;		//配置PD2输入
	PD_CR1_C12  = 0;		//悬浮输入
	PD_CR2_C22  = 0;		//外部输入信号
}


/*****************************************************
** 函数名称：void KeyCtrl_Process(void)
** 功能描述：按键控制进程
******************************************************/
void KeyCtrl_Process(void)
{
  	uint8 Key_Signal = 0;
	uint16 Mode_Val = Menu_Param.stPARAM_List.Control_Mode.Val;
	
    Key_Signal = KeyCtrl_Scan();   
    IO_InPutCheck();
	
	//判断菜单信号
	if((Key_Signal & K_MENU) != 0)
		Menu_Signal = Key_Signal;
	
	//判断电机信号
	if(Mode_Val == 1 || Mode_Val == 4)
	{
	  	if(((Key_Signal & K_MENU) == 0) && ((Key_Signal & 0xf0) == KEY_DOWN))
			Motor_Signal = Key_Signal & 0x0f;
	}
	else if(Mode_Val == 2)
	  	Motor_Signal = IO_InPutNoStop();
	else if(Mode_Val == 3)
	{
	  	Motor_Signal = IO_InPutWithStop();
		if(Key_Signal == (KEY_DOWN | K_Stop))
		  Motor_Signal = K_Stop;
	}
}




