#include "iostm8s103f3.h"
#include "MenuData.h"
#include "MotorParam.h"
#include "MotorCtrl.h"
#include "MotorModeCtrl.h"
#include "Display.h"
#include "MenuCtrl.h"

#include "Eeprom.h"

uint8 EEPROM_Count = 0; //用于掉电检测
static uint8 Save_Mark = 0; //有数据保存的标志位

uint8 WordCount = 0;
uint16 *Point;

/******************************************************************************
*函数名： static void EEPROM_UnLock(void)
*作用：   EEProm解锁
*******************************************************************************/
static void EEPROM_UnLock(void){
  	FLASH_DUKR = 0xae;
	FLASH_DUKR = 0x56;
	while(!(FLASH_IAPSR&0x08));         //等待解密就绪 
}

/******************************************************************************
*函数名： void EEPROM_ChMark(void)
*作用：   参数变动标志
*******************************************************************************/
void EEPROM_ChMark(void){
    Save_Mark = 1;
}

/******************************************************************************
*函数名： EEPROM_WordProgram(uint16 L_Half,uint16 H_Half)
*作用：   字存储 需要先完成解锁
*******************************************************************************/
static void EEPROM_WordProgram(uint16 HalfWord)
{
    switch(WordCount)
    {
        case 0:
            //字编程使能
            FLASH_CR2_WPRG = 1;
            FLASH_NCR2_NWPRG = 0;  
            *Point++ = HalfWord;
            WordCount = 1;
        break;
        case 1:
            *Point++ = HalfWord;
            WordCount = 0;
            while(FLASH_IAPSR_EOP == 0);
        break;
        default:
            break;
    }
}

/******************************************************************************
*函数名： EEPROM_EndProgram
*作用：   EEPROM保存操作的收尾 如果没凑齐字 则补0XFFFF
*******************************************************************************/
static void EEPROM_EndProgram(void)
{
    if(WordCount == 1)
        EEPROM_WordProgram(0xffff);
}

/******************************************************************************
*函数名： EEPROM_SaveALL
*作用：   EEPROM保存所有参数
*******************************************************************************/
static void EEPROM_SaveALL(void){
    uint8 i;
    
    Point = (uint16 *)EE_START;    
    WordCount = 0;  
	EEPROM_UnLock();
        
    //存每项16位的菜单数据值
    for(i=0;i<LIST_Size;i++)
        EEPROM_WordProgram(Menu_Param.stList[i].Val);
    
    //存电机状态
    EEPROM_WordProgram((uint16)Motor.Work_Sta);
    //结束字编程
    EEPROM_EndProgram();
    
    *((uint16*)EE_MARK) = EE_MARK_VAL;
    
    FLASH_IAPSR_DUL = 0;//DATA EEPROM区域写保护使能  
}

/******************************************************************************
*函数名： EEPROM_Read
*作用：   读取EEPROM保存的参数
*******************************************************************************/
static void EEPROM_Read(void){
    uint16 *p;
    uint8 i;
  
    p = (uint16 *)EE_START;    
    for(i=0;i<LIST_Size;i++)
  		Menu_Param.stList[i].Val = *p++; 
          
	MotorMode_SetCmd((WORK_STATUS)*p);	
	MotorParam_LockCtrl((BOOL)Menu_Param.stPARAM_List.Err_Report.Val);
	Menu_GetRadioSpdVal();
	Dis_SetNum(MotorParam_GetDisSpd(),0);
}

/******************************************************************************
*函数名： EEPROM_Process
*作用：   EEPROM处理进程
*******************************************************************************/
void EEPROM_Process(void){
  //有标记位 且处于断电 计数不清零
  if((++EEPROM_Count < 25) || (Save_Mark == 0)) return ;
  
  if(Save_Mark == 1)
  {
    EEPROM_Count = 0;
    Save_Mark = 0;	
    EEPROM_SaveALL();
  }
}

/******************************************************************************
*函数名： EERPROM_Init
*作用：   EEPROM初始化 接口函数
*******************************************************************************/
void EERPROM_Init(void){
    if(*((uint16*)EE_MARK) != EE_MARK_VAL)  //EEPROM 内部没有数据  是第一次下载运行
      EEPROM_SaveALL(); 
    else
      EEPROM_Read();
}