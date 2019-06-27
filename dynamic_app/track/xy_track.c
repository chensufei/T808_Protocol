/********************************************************************************
**
** �ļ���:     xy_track.c
** ��Ȩ����:   
** �ļ�����:   
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   |  �޸ļ�¼
**===============================================================================
**|  |  | 
*********************************************************************************/

#include "dynamic_init.h"
#ifdef __XY_SUPPORT__


/*******************************************************************
** ������:     xy_track_dataup_task
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_track_dataup_task(void*ptr)
{
    XY_INFO_T * xy_info = xy_get_info();
	T808_PARA_T *t808_para = &(xy_info->t808_para);
	dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
	kal_int32 task_time = 5;
	kal_uint32 distance = 0;
	static double last_pos_lat = 0;
    static double last_pos_lng = 0;

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
            	/* ����ÿ��24Сʱ�ϴ�һ��λ������ */
                xy_info->slp_sectime = systime_sec;				
                //xy_soc_location_up(); // ģʽ3 24Сʱ�ϴ�һ������
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
        dynamic_error("xy_track_dataup_task �쳣����");
        task_time= 40;
    }

	dynamic_timer_start(enum_timer_track_task_dataup_timer,task_time*1000,(void*)xy_track_dataup_task,NULL,FALSE);	
#else
	
	dynamic_log("time_sec:%d, cycle:%d\r\n",
				t808_para->tracking_time_sec, t808_para->tracking_cylce);
	
	dynamic_log("type:%d, way:%d, mode:%d, freq:%d\r\n",
					t808_para->report_type, t808_para->report_way, xy_info->mode, xy_info->freq);
	/* ������·���ʱ�����豸 */
	if ((t808_para->tracking_time_sec > 0) && (t808_para->tracking_cylce > 0))
	{
		xy_soc_location_up();

		last_pos_lat = gps_info->lat;
		last_pos_lng = gps_info->lng;
		
		if (t808_para->tracking_time_sec <= t808_para->tracking_cylce)
		{
			/* �����ٵ�ʱ��С�ڸ���ʱ��������ֻ�ϴ����һ���� */
			t808_para->tracking_cylce = 0;
			t808_para->tracking_time_sec = 0;
		}

		task_time = t808_para->tracking_cylce;

		dynamic_timer_start(enum_timer_track_task_dataup_timer, task_time * 1000,(void*)xy_track_dataup_task,NULL,FALSE);	
		return;
	}

	/* �����ϱ���ʽ����ʱ�ϱ����Ƕ������ϱ������Ƕ�ʱ�Ͷ����ϱ� */
	if (0 == t808_para->report_type) //��ʱ�ϱ�
	{
		//����ǽ���״̬��

		if (0 == t808_para->report_way)
		{
			//ֱ�Ӹ���accȥ�ж�
			if (XY_TRACK_MODE1 == xy_info->mode)
			{
				xy_soc_location_up();

				last_pos_lat = gps_info->lat;
				last_pos_lng = gps_info->lng;

				dynamic_log("cycle:%d\r\n", xy_info->freq);
				task_time = xy_info->freq;
			}
			else
			{
				task_time = 5;
			}
		}
		else if (1 == t808_para->report_way)
		{
			//���жϵ�¼״̬���ٸ���ACCȥ�ж�
			if (xy_soc_get_auth_ok_state())
			{
				if (XY_TRACK_MODE1 == xy_info->mode)
				{
					xy_soc_location_up();

					last_pos_lat = gps_info->lat;
					last_pos_lng = gps_info->lng;
					task_time = xy_info->freq;
				}
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

	if (1 == t808_para->report_type)//�����ϱ�
	{
		if (XY_TRACK_MODE1 == xy_info->mode)
		{
			//ֱ�Ӹ���accȥ�ж�
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
					/* ������ģʽ�����Ļ���������ϱ�һ���� */
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

	if (2 == t808_para->report_type)//��ʱ�����ϱ�
	{
		
		return;
	}
#endif
}

/*******************************************************************
** ������:     xy_track_wakeup
** ��������:   ����x����
** ����: 
** ����:       
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
** ������:     xy_track_mode_check
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_track_mode_check(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint8 gps_onoff = 0;
    applib_time_struct systime;
    kal_uint32 systime_sec = 0;

    dynamic_time_get_systime(&systime);
    systime_sec = dynamic_timer_time2sec(&systime); 
        
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
                xy_info->sport_sectime = systime_sec;
                xy_info->sport_state = 0;
                dynamic_debug("�л�Ϊ��ֹ״̬");
                xy_info_save();
            }
        }
    }

    if (xy_info->vib_alm == 1)
    {
        if ((systime_sec - xy_info->sport_sectime) >= 300) // �𶯱���180sһ��
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
        if (xy_info->acc_off_flg)//��ON->OFF
        {
            if ((xy_info->mode != XY_TRACK_MODE2) && (XY_TRACK_MODE3 != xy_info->mode))
            {
            	//�����ߵ������
                xy_track_set_mode(XY_TRACK_MODE2);
            }

            if (systime_sec > xy_info->accoff_sectime)
            {
                kal_uint32 sleeptime = xy_info->slp_d_t;

                if ((systime_sec - xy_info->accoff_sectime) >=  sleeptime)//Ϩ�����������
                {
                    xy_info->acc_off_flg = 0;
                    xy_track_set_mode(XY_TRACK_MODE3);//����
                }
            }
            else
            {
                xy_info->accoff_sectime = systime_sec;
            }
        }
        else//��OFF->ON�����
        {
            if (xy_info->sport_state == 1)//�г�����
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
** ������:     xy_track_set_mode
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_track_set_mode(kal_uint8 mode)
{
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint8 pre_mode = xy_info->mode;

    if (xy_info->mode != mode)
    {
        dynamic_debug("�л�����ģʽ:%d,%d",pre_mode,mode);
        xy_info->mode = mode;
        xy_info_save();
        dynamic_timer_start(enum_timer_track_task_dataup_timer,1000,(void*)xy_track_dataup_task,NULL,FALSE);
    }
}

/*******************************************************************
** ������:     xy_track_sms_set_mode
** ��������:   
** ����:       
** ����:       
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
** ������:     xy_track_mode_init
** ��������:   
** ����:       
** ����:       
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
** ������:     xy_track_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_track_init(void)
{  
    xy_track_mode_init();
	dynamic_timer_start(enum_timer_track_task_timer,1000,(void*)xy_track_mode_check,NULL,FALSE);
	dynamic_timer_start(enum_timer_track_task_dataup_timer,20*1000,(void*)xy_track_dataup_task,NULL,FALSE);
}

#endif

