/********************************************************************************
**
** 文件名:     dynamic_timer.h
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
#ifndef __DYNAMIC_TIMER_H__
#define __DYNAMIC_TIMER_H__ 1

#include "dynamic_init.h"

typedef enum enum_timer_
{
	enum_timer_begin = 1000,
	enum_timer_reset_timer,
	enum_timer_reset_delay_check_timer,
    enum_timer_start_reset_timer,
    enum_timer_start_reset_later,
    enum_timer_sms_timer,
    enum_timer_gps_aid_timer,
    enum_timer_write_epo_overtime_timer,
    enum_timer_gps_timer,
    enum_timer_sensor_task_timer,
    enum_timer_http_task_timer,
    enum_timer_http_continue_timer,
    enum_timer_vib_delay_timer,
    enum_timer_watchdog_task_timer,
#ifdef __XY_SUPPORT__
    enum_timer_alm_task_timer,
    enum_timer_anti_thrief_position_timer,
    enum_timer_led_timer,
    enum_timer_soc_task_timer,
    enum_timer_soc_err_check_timer,
    enum_timer_track_task_timer,
    enum_timer_track_task_dataup_timer,
    enum_timer_sms_reset_timer,
    enum_timer_soc_read_timer,
    enum_timer_call_timer,
    enum_timer_fs_backup_timer,
	enum_timer_track_heart_timer,
	enum_timer_track_heart_wait_ask_timer,
	enum_timer_sos_call_timer,
	enum_timer_xy_soc_close_timer,
#endif     
    
	enum_timer_end
}enum_timer; // dynamic_timer_start使用


/*******************************************************************
** 函数: dynamic_time_get_local_time
** 描述: 获取本地时间
** 参数:       
** 返回: 
********************************************************************/
void dynamic_time_get_local_time(applib_time_struct *nowTime);

#endif

