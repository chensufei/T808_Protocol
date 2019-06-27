/********************************************************************************
**
** 文件名:     xy_track.c
** 版权所有:   
** 文件描述:   
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   |  修改记录
**===============================================================================
**|  |  | 
*********************************************************************************/

#include "dynamic_init.h"
#ifdef __XY_SUPPORT__


/*******************************************************************
** 函数名:     xy_track_dataup_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_dataup_task(void*ptr)
{
	XY_INFO_T * xy_info = xy_get_info();
	T808_PARA_T *t808_para = &(xy_info->t808_para);
	dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
	static double last_pos_lat = 0;
    static double last_pos_lng = 0;
    kal_int32 task_time = 5;
	kal_uint32 distance = 0;
#if 0

    if (xy_info->mode == XY_TRACK_MODE3 && xy_info->sport_state == 0)
    {
        applib_time_struct systime;
        kal_uint32 systime_sec = 0;

        dynamic_time_get_systime(&systime);
        systime_sec = dynamic_timer_time2sec(&systime);
        if (systime_sec < xy_info->slp_sectime)
        {
            xy_info->slp_sectime = systime_sec;
        }
        else
        {
            if ((systime_sec - xy_info->slp_sectime) > (24*60*60))
            {
            	/* 休眠每隔24小时上传一条位置数据 */
                xy_info->slp_sectime = systime_sec;				
                //xy_soc_location_up(); // 模式3 24小时上传一条数据
            }
        }
    }
    else
    {
        xy_soc_location_up();

        if (xy_info->mode == XY_TRACK_MODE1)
        {
            task_time = xy_info->freq;
        }
        else
        {
            task_time = xy_info->freq2;
        }
        
        if(dynamic_gps_is_inflection_point())
        {
        	task_time = 3;
        }
        
        if (task_time == 0)
        {
            task_time = 10;
            dynamic_debug("xy_track_dataup_task freq err:%d,%d",xy_info->freq,xy_info->freq2);
        }
    }

	if (task_time == 0)
    {
        dynamic_error("xy_track_dataup_task 异常参数");
        task_time= 40;
    }

	dynamic_timer_start(enum_timer_track_task_dataup_timer,task_time*1000,(void*)xy_track_dataup_task,NULL,FALSE);	
#else
	
	dynamic_debug("time_sec:%d, cycle:%d\r\n",
				t808_para->tracking_time_sec, t808_para->tracking_cylce);
	
	dynamic_debug("type:%d, way:%d, mode:%d, freq:%d\r\n",
					t808_para->report_type, t808_para->report_way, xy_info->mode, xy_info->freq);
	/* 如果有下发临时跟踪设备 */
	if ((t808_para->tracking_time_sec > 0) && (t808_para->tracking_cylce > 0))
	{
		xy_soc_location_up();

		last_pos_lat = gps_info->lat;
		last_pos_lng = gps_info->lng;
		
		if (t808_para->tracking_time_sec <= t808_para->tracking_cylce)
		{
			/* 当跟踪的时间小于跟踪时间间隔，就只上传最后一次了 */
			t808_para->tracking_cylce = 0;
			t808_para->tracking_time_sec = 0;
		}

		task_time = t808_para->tracking_cylce;

		dynamic_timer_start(enum_timer_track_task_dataup_timer, task_time * 1000,(void*)xy_track_dataup_task,NULL,FALSE);	
		return;
	}

	/* 根据上报方式，定时上报还是定距离上报，还是定时和定距上报 */
	if (0 == t808_para->report_type) //定时上报
	{
		//如果是紧急状态下

		if ((0 == t808_para->report_way) || (xy_soc_get_auth_ok_state() && (1 == t808_para->report_way)))
		{
			if (dynamic_gps_is_inflection_point())
			{
				xy_soc_location_up();

				last_pos_lat = gps_info->lat;
				last_pos_lng = gps_info->lng;
				task_time = 5;
			}
			else if ((XY_TRACK_MODE1 == xy_info->mode) ||
				((XY_TRACK_MODE2 == xy_info->mode) && (1 == xy_info->sport_state)))//直接根据acc去判断
			{
				xy_soc_location_up();

				last_pos_lat = gps_info->lat;
				last_pos_lng = gps_info->lng;

				dynamic_log("cycle:%d\r\n", xy_info->freq);
				task_time = xy_info->freq;
			}
		}

		if (0 == task_time)
		{
			task_time = 5;
			dynamic_log("task_time:%d\r\n", task_time);
		}

		dynamic_timer_start(enum_timer_track_task_dataup_timer, task_time * 1000,(void*)xy_track_dataup_task,NULL,FALSE);	
		return;
	}

	if (1 == t808_para->report_type)//定距上报
	{
		if (XY_TRACK_MODE1 == xy_info->mode)
		{
			//直接根据acc去判断
			if ((0 == t808_para->report_way) || (xy_soc_get_auth_ok_state() && (1 == t808_para->report_way)))
			{
				if (dynamic_gps_is_inflection_point())
				{
					xy_soc_location_up();

					last_pos_lat = gps_info->lat;
					last_pos_lng = gps_info->lng;
					task_time = 5;
				}
				else
				{
					/* 从其他模式过来的话，里面就上报一个点 */
					distance = dynamic_gps_get_distance(gps_info->lat, gps_info->lng, last_pos_lat, last_pos_lng);
			        if (distance >= t808_para->def_distance)
			        {
			        	xy_soc_location_up();

						last_pos_lat = gps_info->lat;
						last_pos_lng = gps_info->lng;				
					}
					task_time = 1;					
				}
			}
		}

		dynamic_timer_start(enum_timer_track_task_dataup_timer, task_time * 1000,(void*)xy_track_dataup_task,NULL,FALSE);
		return;
	}

	if (2 == t808_para->report_type)//定时定距上报
	{
		
		return;
	}
#endif
}

/*******************************************************************
** 函数名:     xy_track_wakeup
** 函数描述:   唤醒x分钟
** 参数: 
** 返回:       
********************************************************************/
void xy_track_wakeup(void)
{    
    applib_time_struct systime;
    kal_uint32 systime_sec = 0;
    XY_INFO_T * xy_info = xy_get_info();

    dynamic_time_get_systime(&systime);
    systime_sec = dynamic_timer_time2sec(&systime);
    xy_info->sport_sectime = systime_sec;      
    xy_info->sport_state = 1;
	
}

/*******************************************************************
** 函数名:     xy_track_mode_check
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_mode_check(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint8 gps_onoff = 0;
    applib_time_struct systime;
    kal_uint32 systime_sec = 0;

    dynamic_time_get_systime(&systime);
    systime_sec = dynamic_timer_time2sec(&systime); 

	dynamic_debug("acc_state:%d, mode:%d, slp_sw:%d, acc off flag:%d, sport_state:%d, %d, %d\r\n",
			xy_info->acc_state, xy_info->mode, xy_info->slp_sw, xy_info->acc_off_flg,
			xy_info->sport_state, xy_info->sport_sectime, (systime_sec - xy_info->sport_sectime));


    if (xy_info->sport_state == 1)
    {
        if (systime_sec < xy_info->sport_sectime)
        {
            xy_info->sport_sectime = systime_sec;
        }
        else
        {
            if ((systime_sec - xy_info->sport_sectime) >= xy_info->slp_d_t)
            {
                if ((systime_sec - xy_info->sport_sectime) > 1000)
                {
                    dynamic_debug("开机同步时间导致唤醒时间异常，刚上电串口下载一半可能休眠");
                    xy_info->sport_sectime = systime_sec;
                }
                else
                {
                    xy_info->sport_sectime = systime_sec;
                    xy_info->sport_state = 0;
                    dynamic_debug("切换为静止状态");
                    xy_info_save();
                }
            }
        }
    }

    if (xy_info->vib_alm == 1)
    {
        if ((xy_info->sport_sectime > 0) && ((systime_sec - xy_info->sport_sectime) >= 300)) // 震动报警180s一次
        {
            xy_info->vib_alm = 0;
            xy_info_save();
        }
    }
	
    if (xy_info->acc_state == XY_ACC_ON)
    {
        if (xy_info->mode != XY_TRACK_MODE1)
        {
            xy_track_set_mode(XY_TRACK_MODE1);
        }
    }
    else if (xy_info->slp_sw == 0)
    {
        if (xy_info->mode != XY_TRACK_MODE2)
        {
            xy_track_set_mode(XY_TRACK_MODE2);
        }
    }
    else
    {
        if (xy_info->acc_off_flg)//从ON->OFF
        {
            if ((xy_info->mode != XY_TRACK_MODE2) && (XY_TRACK_MODE3 != xy_info->mode))
            {
            	//非休眠的情况下
                xy_track_set_mode(XY_TRACK_MODE2);
            }

            if (systime_sec > xy_info->accoff_sectime)
            {
                kal_uint32 sleeptime = xy_info->slp_d_t;

                if ((systime_sec - xy_info->accoff_sectime) >=  sleeptime)//熄火持续三分钟
                {
                    xy_info->acc_off_flg = 0;
                    xy_track_set_mode(XY_TRACK_MODE3);//休眠
                }
            }
            else
            {
                xy_info->accoff_sectime = systime_sec;
            }
        }
        else//从OFF->ON，点火
        {
            if (xy_info->sport_state == 1)//有持续震动
            {
                xy_track_set_mode(XY_TRACK_MODE2);
            }
            else if (xy_info->mode != XY_TRACK_MODE3)
            {
                xy_track_set_mode(XY_TRACK_MODE3);
            }
        }
    }

    if (xy_info->mode == XY_TRACK_MODE3)
    {
        gps_onoff = 0;
    }
    else
    {
        gps_onoff = 1;
    }
    dynamic_gps_pow_ctrl(gps_onoff);

    dynamic_timer_start(enum_timer_track_task_timer,1000,(void*)xy_track_mode_check,NULL,FALSE);
}

/*******************************************************************
** 函数名:     xy_track_set_mode
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_set_mode(kal_uint8 mode)
{
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint8 pre_mode = xy_info->mode;

    if (xy_info->mode != mode)
    {
		if (XY_TRACK_MODE3 == mode)
		{
			/* 一旦休眠，需要重新设置gsensor采集的次数 */
			dynamic_sensor_clear_check_cnt();
		}
	
        dynamic_debug("切换工作模式:%d,%d",pre_mode,mode);
        xy_info->mode = mode;
        xy_info_save();
        dynamic_timer_start(enum_timer_track_task_dataup_timer,1000,(void*)xy_track_dataup_task,NULL,FALSE);
    }
}

/*******************************************************************
** 函数名:     xy_track_sms_set_mode
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_sms_set_mode(XY_TRACK_MODE_E mode)
{ 
    XY_INFO_T * xy_info = xy_get_info();
    
    switch (mode)
    {
        case XY_TRACK_MODE1:
            if (xy_info->mode != XY_TRACK_MODE1)
            {
                xy_track_wakeup();
                xy_info->acc_state = XY_ACC_ON;
                xy_info->acc_off_flg= 0;
                dynamic_debug("SMS ACC ON!");
                xy_info->check_anti_thrief = 0;
                xy_soc_alm_assem(XY_ALM_TYPE_NONE);
                xy_track_set_mode(XY_TRACK_MODE1);
            }
            dynamic_debug("SMS MODE1");
        break;

        case XY_TRACK_MODE2:
            if (xy_info->mode != XY_TRACK_MODE2)
            {
                applib_time_struct systime;
                kal_uint32 systime_sec = 0;

                xy_track_wakeup();
                dynamic_time_get_systime(&systime);
                systime_sec = dynamic_timer_time2sec(&systime);
                xy_info->accoff_sectime = systime_sec;
                xy_info->acc_off_flg = 1;

                xy_info->acc_state = XY_ACC_OFF;
                if (xy_info->anti_thrief == 0)
                {
                    xy_info->check_anti_thrief = 1;
                }
                dynamic_debug("SMS ACC OFF!");
                xy_soc_alm_assem(XY_ALM_TYPE_NONE);    
                xy_track_set_mode(XY_TRACK_MODE2);
            }
            dynamic_debug("SMS MODE2");
        break;

        case XY_TRACK_MODE3:
            if (xy_info->mode != XY_TRACK_MODE3)
            {
                if (xy_info->acc_state == XY_ACC_ON)
                {
                    xy_info->acc_state = XY_ACC_OFF;
                    xy_soc_alm_assem(XY_ALM_TYPE_NONE);    
                }
                xy_info->sport_sectime = 0;      
                xy_info->sport_state = 0;
                xy_info->accoff_sectime = 0;
                xy_info->acc_off_flg = 0;
                dynamic_sensor_clr_vib();
                xy_track_set_mode(XY_TRACK_MODE3);
            }
            dynamic_debug("SMS MODE3");
        break;

        default:
            return;
        break;

    }
}

/*******************************************************************
** 函数名:     xy_track_mode_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_mode_init(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();

    if (sys_info->ref_reset_type == RESET_TYPE_POWEROFF && xy_info->mode == XY_TRACK_MODE3)
    {
        xy_track_wakeup();
    }
    
    if (xy_info->slp_sw == 0  && xy_info->mode == XY_TRACK_MODE3)
    {
        xy_info->mode = XY_TRACK_MODE2;
    }
}

/*******************************************************************
** 函数名:     xy_track_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_init(void)
{  
    xy_track_mode_init();
	dynamic_timer_start(enum_timer_track_task_timer,5 * 1000,(void*)xy_track_mode_check,NULL,FALSE);
	dynamic_timer_start(enum_timer_track_task_dataup_timer,20*1000,(void*)xy_track_dataup_task,NULL,FALSE);
}

#endif

