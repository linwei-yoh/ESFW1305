#include "TM1618.h"
#include "IOCtrl.h"


KEY_VAL TM_KeyVal = {0};
/*****************************************************************************
** 函数名称：void TM1618_IOConfig (void)
** 功能描述：TM1618引脚初始化
*****************************************************************************/
static void TM1618_IOConfig(void)
{
	/*STB*/
	PA_DDR_DDR3 = 1;		//配置PA3为输出
	PA_CR1_C13  = 1;		//推挽输出
	PA_CR2_C23  = 0;
	
	/*CLK*/
	PB_DDR_DDR4 = 1;		//配置PB4为输出
	PB_CR1_C14  = 1;		//推挽输出
	PB_CR2_C24  = 0;
	
	/*DIO*/
	PB_DDR_DDR5 = 1;		//配置PB5为输出
	PB_CR1_C15  = 1;		//推挽输出
	PB_CR2_C25  = 0;
}

/****************************************************
*函数名称:TM_SendByte
*功能介绍:向TM1618发送8位数据
*备注    :调用此函数会将DIO引脚配置为输出模式
*****************************************************/
void TM_SendByte(uint8 ucData)
{
    uint8 i = 0;
		
    //配置DIO为输出		
	PB_DDR_DDR5 = 1;
 
    for(i=0;i<8;i++)
    {
      TM_CLK = 0;   		
	  DIO_W = ucData & 0x01;				
      TM_CLK = 1;   
 
      ucData >>= 1;
    }
}

/****************************************************
*函数名称:TM_Command
*功能介绍:向TM1618芯片发送命令
*备注：   因为时钟周期需求不同，仅在设置数据命令，显示
					控制命令时使用
*****************************************************/
void TM_Command(uint8 ucCom)
{
  TM_STB = 0;	  		
  TM_SendByte(ucCom);
  TM_STB = 1;	  
}

/*************************************************************
* 函数名称： 	TM_KeyRead(void)
* 功能介绍：	从TM1618芯片中读到按键信息的指令
*************************************************************/
void TM_KeyRead(void)
{
	int8 i = 0,j = 0;
	uint8 Key_Buff[3] = {0}; 
	
	PB_DDR_DDR5 = 1; //DIO输出	
	TM_STB = 0;
	
	TM_SendByte(0X42);
	PB_DDR_DDR5 = 0;//DIO输入
	
	for(i=0;i<3;i++)
	{
	  	for(j=0;j<8;j++)
     	{
            TM_CLK = 0;
            Key_Buff[i] >>= 1;
            TM_CLK = 1;
			
            if(DIO_R == 1)
              Key_Buff[i] |= 0x80;
    	}	
	}
	TM_STB = 1;
	
	TM_KeyVal.KS1 = ((Key_Buff[0] & 0x02) == 0)?0:1;
	TM_KeyVal.KS2 = ((Key_Buff[0] & 0x10) == 0)?0:1;
	TM_KeyVal.KS3 = ((Key_Buff[1] & 0x02) == 0)?0:1;
	TM_KeyVal.KS4 = ((Key_Buff[1] & 0x10) == 0)?0:1;
	TM_KeyVal.KS5 = ((Key_Buff[2] & 0x02) == 0)?0:1;
}

/*****************************************************************************
** 函数名称：void TM_Init (void)
** 功能描述：TM1618初始化
*****************************************************************************/
void TM_Init(void)
{
  	TM1618_IOConfig();
	TM_Command(0x00);
}