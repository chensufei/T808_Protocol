/********************************************************************************
**
** �ļ���:     dynamic_timer.c
** ��Ȩ����:   
** �ļ�����:   ��ʱ��
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   |  �޸ļ�¼
**===============================================================================
**|  |  | 
*********************************************************************************/

#include "dynamic_init.h"

/*******************************************************************
** ����: dynamic_time_get_local_time
** ����: ��ȡ����ʱ��
** ����:       
** ����: 
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
    dynamic_debug("��ǰʱ��:%d%02d%02d%02d%02d%02d",nowTime->nYear,nowTime->nMonth,nowTime->nDay,
	                nowTime->nHour,nowTime->nMin,nowTime->nSec);
}




