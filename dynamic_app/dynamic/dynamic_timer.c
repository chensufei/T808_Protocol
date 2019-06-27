/********************************************************************************
**
** 文件名:     dynamic_timer.c
** 版权所有:   
** 文件描述:   定时器
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   |  修改记录
**===============================================================================
**|  |  | 
*********************************************************************************/

#include "dynamic_init.h"

/*******************************************************************
** 函数: dynamic_time_get_local_time
** 描述: 获取本地时间
** 参数:       
** 返回: 
********************************************************************/
void dynamic_time_get_local_time(applib_time_struct *nowTime)
{
    float tzone = 8;
    applib_time_struct sys_time;
#ifdef __XY_SUPPORT__
    XY_INFO_T * xy_info = xy_get_info();
    tzone = xy_info->tzone;
#endif
    dynamic_time_get_systime(&sys_time);
    dynamic_time_utc_to_rtc(tzone,&sys_time,nowTime);
    dynamic_debug("当前时间:%d%02d%02d%02d%02d%02d",nowTime->nYear,nowTime->nMonth,nowTime->nDay,
	                nowTime->nHour,nowTime->nMin,nowTime->nSec);
}




