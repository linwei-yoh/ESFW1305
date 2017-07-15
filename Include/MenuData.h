#ifndef __MENUDATA_H__
#define __MENUDATA_H__


#include "type.h"

typedef struct
{
	uint16 Min;
	uint16 Max;
}V_RANGE;

typedef struct
{
    uint16      Val;
    V_RANGE  	*Range;
}PARAM_VAL;


typedef struct 
{
	PARAM_VAL Cur_Speed;                        //0  电机设定速度
	PARAM_VAL Dis_Mode;                         //1  数值显示方式 1：直接显示  2. 倍率显示  3.反馈显示
	PARAM_VAL Ratio;                            //2  倍率设置  
	PARAM_VAL Control_Mode;              	    //3  运行控制 1:面板控制/无记忆 2:外接控制/STOP无效 3:外接控制/STOP有效 4:面板控制/记忆
	PARAM_VAL Rotate_Mode;		        		//4  旋转方式 1:允许正反转 2:仅正转 3:仅反转
	PARAM_VAL Rotate_Dir;			        	//5  旋转方向 1:不取反 2:取反
	PARAM_VAL SpeedCtrl_Mode;	                //6  速度调整 1:面板上下键控制 2:面板旋钮控制
	PARAM_VAL Max_Speed;                        //7  最高转速
	PARAM_VAL Min_Speed;                        //8  最低转速
	PARAM_VAL Cor_BTime;			        	//9  正转启动加速时间 1-100（0.01S）										                       
	PARAM_VAL Cor_Mode;			        		//10 正转停止方式 1:自由减速停止 2:缓慢减速停止										             
	PARAM_VAL Cor_ETime;			        	//11 正转停止时缓慢减速时间  1-100（0.01S）										                
	PARAM_VAL Bak_BTime;			        	//12 反转启动加速世界 1-100（0.01S）													   							 
	PARAM_VAL Bak_Mode;			        		//13 反转停止方式	1:自由减速停止 2:缓慢减速停止                         		 
	PARAM_VAL Bak_ETime;			        	//14 反转停止时缓慢减速时间  1-100（0.01S）	
	PARAM_VAL Err_Report;                       //15 故障报错使能    1.有效 2无效
	PARAM_VAL Rev_En;                           //16 循环运行使能  1.无效 2使能  
	PARAM_VAL Rev_Num;                          //17 运行次数 1-9999
	PARAM_VAL Rev_FRTime;                       //18 正方向运行时间 (s  0-9999
	PARAM_VAL Rev_RRTime;                       //19 反方向运行时间 (s  0-9999
	PARAM_VAL Rev_STime;                        //20 中间停止时间   (s  1-9999
	PARAM_VAL Rev_CorSpd;						//21 正方向运行速度 (90-3000
	PARAM_VAL Rev_BakSpd;						//22 反方向运行速度 (90-3000
	PARAM_VAL ReSet;			        		//23 恢复出厂设定  1:不恢复 2:恢复出厂设定
	PARAM_VAL SpdFdb;                           //24 速度反馈池大小
}MUME_LIST;

#define LIST_Size (sizeof(MUME_LIST) / sizeof(PARAM_VAL))

typedef union
{
  MUME_LIST  stPARAM_List;
  PARAM_VAL  stList[LIST_Size];
}MENU_PARAM;

extern int16 Radio_Spd ;
extern MENU_PARAM Menu_Param;
extern void MenuData_Init(void);

#endif
