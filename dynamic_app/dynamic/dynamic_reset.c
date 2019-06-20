/********************************************************************************
**
** �ļ���:     dynamic_reset.c
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

#define MAX_RESET_CNT 20



static DYNAMIC_SYS_T s_sys_info;

/*******************************************************************
** ������:     dynamic_sys_get_info
** ��������:   
** ����:       
** ����:       
********************************************************************/
DYNAMIC_SYS_T *dynamic_sys_get_info(void)
{
    return &s_sys_info;
}

/*******************************************************************
** ������:     dynamic_sys_info_save
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sys_info_save(void)
{
    s_sys_info.checksum = dynamic_checksum((kal_uint8 *)&s_sys_info, (sizeof(s_sys_info)-4));
    dynamic_file_write(DYNAMIC_SYS_FILE_NAME,&s_sys_info,sizeof(s_sys_info));
    return 1;
}

/*******************************************************************
** ������:     dynamic_sys_info_reset
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sys_info_reset(void)
{
    dynamic_debug("dynamic_sys_info_reset");
    memset(&s_sys_info,0,sizeof(s_sys_info));
    dynamic_sys_info_save();
    return 1;
}
/*******************************************************************
** ������:     dynamic_reset_now
** ��������:   �ػ�������
** ����:       
** ����:       
********************************************************************/
void dynamic_reset_now(void)
{
    if (s_sys_info.reset_type == RESET_TYPE_POWEROFF)
    {
        dynamic_sys_shutdown(0);
    }
    else
    {
	    //dynamic_sys_reset();
		dynamic_sys_shutdown(0);
	    dynamic_log("dynamic_sys_reset......\r\n");
    }
}

/*******************************************************************
** ������:     dynamic_poweroff_now
** ��������:   �ػ�
** ����:       
** ����:       
********************************************************************/
void dynamic_poweroff_now(void)
{
    dynamic_start_reset(RESET_TYPE_POWEROFF);
}

/*******************************************************************
** ������:     dynamic_start_reset_wait
** ��������:   ��ʱ����
** ����:       
** ����:       
********************************************************************/
void dynamic_start_reset_wait(void)
{
    dynamic_start_reset(s_sys_info.reset_type);
}

/*******************************************************************
** ������:     dynamic_reset_delay
** ��������:   ��ʱ����
** ����:       
** ����:       
********************************************************************/
void dynamic_reset_delay(void)
{
    dynamic_start_reset(RESET_TYPE_FOR_CMD);
}

/*******************************************************************
** ������:     dynamic_start_reset
** ��������:   ����ϵͳ��ػ�
** ����:       
** ����:       
********************************************************************/
void dynamic_start_reset(RESET_TYPE_ENUM sign)
{
    static kal_uint8 poweroff_status = 0;

    dynamic_log("dynamic_start_reset:%d\r\n",sign);

    if (dynamic_telephony_call_status() != CALL_RELEASE)
    {
        dynamic_info("ͨ���У�������\r\n");
        s_sys_info.reset_type = sign;
        dynamic_timer_start(enum_timer_start_reset_timer,10*1000, (void *)dynamic_start_reset_wait,0,0);
        return;
    }

    if (poweroff_status == 1)
    {
        return;
    }
    
    s_sys_info.reset_type = sign;
    poweroff_status = 1;
    dynamic_sys_info_save();
    dynamic_sms_save();
#ifdef __XY_SUPPORT__
    xy_soc_data_save();
    if (sign == RESET_TYPE_FOR_CMD)
    {
        xy_track_wakeup(); // ָ������������
    }
    xy_info_save();
#endif
    dynamic_timer_start(enum_timer_start_reset_timer,2*1000, (void *)dynamic_reset_now,0,0);
}

/*******************************************************************
** ������:     dynamic_reset_check_pro
** ��������:   reset��ʱ��⣬�����ڼ������쳣���������ϵͳ(ĿǰΪ24Сʱ����������������SOC��)
** ����:       
** ����:       
********************************************************************/
void dynamic_reset_check_pro(void)
{
    static kal_uint32 work_time = 0;

    if (++work_time >= 24*60*60)
    {
        if (dynamic_sensor_vib_in_3min() == 0) // ����24Сʱ��3������û���𶯣�����
        {
            dynamic_start_reset(RESET_TYPE_24_HOUR);
        }
        else
        {
            dynamic_timer_start(enum_timer_reset_timer,5*60*1000, (void *)dynamic_reset_check_pro,0,0);
        }
    }
}

/*******************************************************************
** ������:     dynamic_reset_delay_check
** ��������:   ������ʱ���ڲ������ŵ�����Ч����
** ����:       
** ����:       
********************************************************************/
void dynamic_reset_delay_check(void)
{
    s_sys_info.abnreset_count = 0;
    dynamic_sys_info_save();
}
/*******************************************************************
** ������:     dynamic_reset_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void dynamic_reset_init(void)
{
    applib_time_struct curtime;
    kal_uint32 checksum;
    kal_uint32 len;

    dynamic_time_get_systime(&curtime);
        
    memset(&s_sys_info,0,sizeof(s_sys_info));
    len = dynamic_file_read(DYNAMIC_SYS_FILE_NAME,&s_sys_info,sizeof(s_sys_info));
    checksum = dynamic_checksum((kal_uint8 *)&s_sys_info, (sizeof(s_sys_info)-4));
    if (len != sizeof(s_sys_info) || checksum != s_sys_info.checksum)
    {
        dynamic_sys_info_reset();
    }
    s_sys_info.abnreset_count++;
    if (s_sys_info.abnreset_count > MAX_RESET_CNT)
    {
        //s_sys_info.abnreset_count = 0;
        //dynamic_log("===============err app,dele==============\r\n");
        //dynamic_file_dele(DYNAMIC_APP_FILE_NAME);
        //dynamic_start_reset(RESET_TYPE_UPGRADE);
    }
    dynamic_debug("reset_type:%d,pwron reason:%d",s_sys_info.reset_type,dynamic_sys_get_power_on_reason());
    if (dynamic_sys_get_power_on_reason() != ABNRESET)
    {
        if (s_sys_info.reset_type == RESET_TYPE_ABNORMITY || s_sys_info.reset_type == RESET_TYPE_POWEROFF)
        { 
            s_sys_info.reset_type = RESET_TYPE_POWEROFF;
        }
    }
    if (s_sys_info.reset_cnt >= MAX_RESET_INFO_NUM) // ��¼���50������ԭ��
    {
        s_sys_info.reset_cnt = MAX_RESET_INFO_NUM;
        memcpy(&s_sys_info.reset_info[0],&s_sys_info.reset_info[1],((MAX_RESET_INFO_NUM-1)*sizeof(dynamic_reset_info_t)));
        s_sys_info.reset_info[(MAX_RESET_INFO_NUM-1)].type = s_sys_info.reset_type;
        memcpy(&s_sys_info.reset_info[(MAX_RESET_INFO_NUM-1)].time,&curtime,sizeof(applib_time_struct));
    }
    else
    {
        s_sys_info.reset_info[s_sys_info.reset_cnt].type = s_sys_info.reset_type;
        memcpy(&s_sys_info.reset_info[s_sys_info.reset_cnt].time,&curtime,sizeof(applib_time_struct));
    }

    if (s_sys_info.reset_cnt < MAX_RESET_INFO_NUM)
    {
        s_sys_info.reset_cnt++;
    } 

    s_sys_info.ref_reset_type = s_sys_info.reset_type;
    s_sys_info.reset_type = RESET_TYPE_ABNORMITY;

    if (s_sys_info.ref_reset_type == RESET_TYPE_POWEROFF)
    {
        
    }
    dynamic_sys_info_save();
    //dynamic_timer_start(enum_timer_reset_delay_check_timer,30*1000, (void *)dynamic_reset_delay_check,0,0);
    dynamic_timer_start(enum_timer_reset_timer,1*1000, (void *)dynamic_reset_check_pro,0,0);
}


