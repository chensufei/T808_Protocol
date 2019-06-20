/********************************************************************************
**
** 文件名:     xy_telephony.c
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

static kal_uint8 s_user_call = 0;
static kal_uint8 s_sos_call_cnt = 0;
static kal_uint8 s_sos_loop_call = 0;


/*******************************************************************
** 函数名:     xy_telephony_sos_call_connect
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_telephony_sos_call_connect(void)
{
    dynamic_timer_stop(enum_timer_sos_call_timer);
    s_sos_loop_call = 0;
    s_sos_call_cnt = 0;     
}

/*******************************************************************
** 函数名:     xy_get_sos_state
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_bool xy_get_sos_state(void)
{
    if (s_sos_call_cnt == 0 && s_sos_loop_call == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*******************************************************************
** 函数名:     xy_telephony_sos_call
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_telephony_sos_call(void)
{
    kal_uint32 time = 1000;
    XY_INFO_T * xy_info = xy_get_info();

    if (dynamic_telephony_call_status() == CALL_RELEASE)
    {
        s_sos_call_cnt++;
        dynamic_debug("xy_telephony_sos_call s_sos_loop_call:%d,s_sos_call_cnt:%d",s_sos_loop_call,s_sos_call_cnt);
        switch(s_sos_call_cnt)
        {
            case 1:
                if (strlen((char*)xy_info->user) > 0)
                {
                    dynamic_telephony_dial((char*)xy_info->user);
                }
            break;

            case 2:
                if (strlen((char*)xy_info->sosnum[0]) > 0)
                {
                    dynamic_telephony_dial((char*)xy_info->sosnum[0]);
                }
            break;

            case 3:
                if (strlen((char*)xy_info->sosnum[1]) > 0)
                {
                    dynamic_telephony_dial((char*)xy_info->sosnum[1]);
                }
            break;

            case 4:
                if (strlen((char*)xy_info->sosnum[2]) > 0)
                {
                    dynamic_telephony_dial((char*)xy_info->sosnum[2]);
                }
                s_sos_call_cnt = 0;
                if (++s_sos_loop_call >= 3)
                {
                    s_sos_loop_call = 0;
                    return;
                }
            break;

            default:
                s_sos_call_cnt = 0;
            break;
        }
    }
    dynamic_timer_start(enum_timer_sos_call_timer,time,(void*)xy_telephony_sos_call,NULL,FALSE);
}


/*******************************************************************
** 函数名:     xy_call_alm
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_call_alm(XY_ALM_TYPE_E type)
{
    XY_INFO_T * xy_info = xy_get_info();

    if (type == XY_ALM_TYPE_SOS)
    {
        if (strlen((char*)xy_info->user) > 0 || strlen((char*)xy_info->sosnum[0]) > 0  || strlen((char*)xy_info->sosnum[1]) > 0  || strlen((char*)xy_info->sosnum[2]) > 0 )
        {
            xy_telephony_sos_call();
        }
    }
    else
    {
        if (strlen((char *)xy_info->user) == 0)
        {
            dynamic_debug("xy_call_alm:%d  no user num",type);
            return;
        }
        
        dynamic_telephony_dial((char *)xy_info->user);
    }
}


/*******************************************************************
** 函数名:     xy_telephony_user_call
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_telephony_user_call(void)
{
}

/*******************************************************************
** 函数名:     xy_telephony_state_cb
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_telephony_state_cb(CALL_STATUS_E call_stat,kal_uint8 *num)
{
    
    XY_INFO_T * xy_info = xy_get_info();
    
    switch (call_stat)
    {

        case CALL_RELEASE: // 挂断
            if (s_user_call == 1)
            {
                s_user_call = 0;
                if (dynamic_timer_is_runing(enum_timer_call_timer))
                {
                    // 10s内挂断
                    dynamic_timer_stop(enum_timer_call_timer);
                    if (xy_info->anti_thrief)
                    {
                        xy_alm_antitheft_crl(0);
                    }
                    else
                    {
                        xy_alm_antitheft_crl(1);
                    }
                }
            }
        break;
        
        case CALL_COMING: // 来电
        {
            if (xy_sms_user_check((char*)num) == 1)
            {
                xy_track_wakeup();// 主号来电，唤醒
                s_user_call = 1;
                dynamic_timer_start(enum_timer_call_timer,10*1000,(void *) xy_telephony_user_call,NULL,KAL_FALSE);
            }
            else
            {
                dynamic_telephony_reject(); // 非主号来电拒接
            }
        }
        break;
        
        case CALL_ACCEPT: // 来电接听
            
        break;
        
        case CALL_REQ: // 呼叫请求
        
        break;
        
        case CALL_ALERT: // 呼叫开始响铃
        
        break;
        
        case CALL_CONNECT: // 呼叫被接听
            if (xy_get_sos_state())
            {
                xy_telephony_sos_call_connect();
            }
        break;

        default:

        break;

    }
}

#endif

