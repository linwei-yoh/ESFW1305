#ifndef __PI_H__
#define __PI_H__


#include "Config.h"
/******************************************************************
						    宏定义
******************************************************************/	
#define PID_MACRO(v)												\
	v.up  = (v.Ref - v.Fdb)*4L;										\
	v.ui  = (v.Out == v.v1) ? (v.Ki*v.up + v.i1) : v.i1;			\
	v.i1  = v.ui;													\
	v.v1  = (v.Kp*(v.up + v.ui/4096L))/4096L;						\
	v.Out = _IQsat(v.v1, v.Umax, v.Umin);
	
/******************************************************************
						  数据类型申明
******************************************************************/
typedef struct 
{  
        long  Ref;   			// Input: Reference input 
        long  Fdb;   			// Input: Feedback input 
		long  Out;
        long  Kp;		        // Variable: Proportional output 
        long  Ki;		        // Variable: Integral output 
        long  Umax;		    	// Parameter: Maximum output 
        long  Umin;	    		// Parameter: Minimum output
		long  up;
		long  ui;
		long  v1;
		long  i1;
		long  w1;
 } PIDREG;






#endif
