/********************************************************************************
**
** 文件名:     dynamic_sms.c
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


static DYNAMIC_SMS_LIST_T s_sms_list;

void dynamic_sms_task(void * str);


/*******************************************************************
** 函数名:     dynamic_sms_list_is_empty
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sms_list_is_empty(void)
{
    if (s_sms_list.data_in != s_sms_list.data_out)
    {
        return 0;
    }
    return 1;
}

/*******************************************************************
** 函数名:     dynamic_sms_list_push
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sms_list_push(DYNAMIC_SMS_DATA_T *sms)
{
    dynamic_debug("dynamic_sms_list_push num:%s,len:%d",sms->ph_num,sms->len);
    if (strlen((char*)sms->ph_num) == 0)
    {
        return 0;
    }
    memset(&s_sms_list.info[s_sms_list.data_in],0,sizeof(DYNAMIC_SMS_DATA_T));
    memcpy(&s_sms_list.info[s_sms_list.data_in],sms,sizeof(DYNAMIC_SMS_DATA_T));

    dynamic_debug("s_sms_list in:%d,out:%d",s_sms_list.data_in,s_sms_list.data_out);
    if (++s_sms_list.data_in >= DYNAMIC_SMS_LIST_NUM)
    {
        s_sms_list.data_in = 0;
    }

    if (s_sms_list.data_in == s_sms_list.data_out)
    {
        if (++s_sms_list.data_out >= DYNAMIC_SMS_LIST_NUM)
        {
            s_sms_list.data_out = 0;
        }
    }

    dynamic_sms_save();
    return 1;
}

/*******************************************************************
** 函数名:     dynamic_sms_list_add
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sms_list_add(char*num,kal_uint16 len,char * data)
{
    DYNAMIC_SMS_DATA_T sms_data;

    if (strlen((char*)num) < 3 || len == 0)
    {
        dynamic_debug("dynamic_sms_list_add:%d,%s",len,data);
        dynamic_debug("dynamic_sms_list_add err");
        return 0;
    }

    if (len >= MAX_SMS_LEN)
    {
        len = MAX_SMS_LEN-1;
    }

    memset(&sms_data,0,sizeof(sms_data));
    memcpy(sms_data.ph_num,num,strlen((char*)num));
    memcpy(sms_data.data,data,len);
    sms_data.len = len;
    
    return dynamic_sms_list_push(&sms_data);    
}
 
/*******************************************************************
** 函数名:     dynamic_sms_list_pop
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sms_list_pop(void)
{
    dynamic_debug("dynamic_sms_list_pop\r\n");
    dynamic_debug("s_sms_list in:%d,out:%d",s_sms_list.data_in,s_sms_list.data_out);
    memset(&s_sms_list.info[s_sms_list.data_out],0,sizeof(DYNAMIC_SMS_DATA_T));

    if (s_sms_list.data_in != s_sms_list.data_out)
    {
        if (++s_sms_list.data_out >= DYNAMIC_SMS_LIST_NUM)
        {
            s_sms_list.data_out = 0;
            if (s_sms_list.data_in == s_sms_list.data_out)
            {
                if (++s_sms_list.data_in >= DYNAMIC_SMS_LIST_NUM)
                {
                    s_sms_list.data_in = 0;
                }
            }           
        }
    }

    dynamic_sms_save();
    return 1;
}

/*******************************************************************
** 函数名:     dynamic_sms_save
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sms_save(void)
{
    s_sms_list.checksum = dynamic_checksum((kal_uint8 *)&s_sms_list, (sizeof(s_sms_list)-4));
    dynamic_file_write(DYNAMIC_SMS_FILE_NAME,&s_sms_list,sizeof(s_sms_list));
    return 1;
}

/*******************************************************************
** 函数名:     dynamic_sms_list_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sms_list_reset(void)
{
    memset(&s_sms_list,0,sizeof(s_sms_list));

    return dynamic_sms_save();
}

/*******************************************************************
** 函数名:     dynamic_sms_list_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_sms_list_init(void)
{
    kal_uint32 len,checksum;
    
    memset(&s_sms_list,0,sizeof(s_sms_list));

    len = dynamic_file_read(DYNAMIC_SMS_FILE_NAME,&s_sms_list,sizeof(s_sms_list));
    checksum = dynamic_checksum((kal_uint8 *)&s_sms_list, (sizeof(s_sms_list)-4));
    if (len != sizeof(s_sms_list) || checksum != s_sms_list.checksum)
    {
        dynamic_sms_list_reset();
    }
}

/*******************************************************************
** 函数名:     dynamic_sms_send_cb
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_sms_send_cb(DYNAMIC_SMS_SEND_CB_T* send_result)
{
    kal_uint32 timer = 1000;

    if (send_result->result == 1)
    {
        dynamic_debug("sms send success");
        s_sms_list.fail_cnt = 0;
        dynamic_sms_list_pop();
    }
    else
    {
        s_sms_list.fail_cnt++;

        dynamic_debug("sms send fail:%d",s_sms_list.fail_cnt);
        if (s_sms_list.fail_cnt >= 3)
        {
            s_sms_list.fail_cnt = 0;
            dynamic_sms_list_pop();
        }
        else
        {
            timer = 6*1000;
        }
    }
    dynamic_timer_start(enum_timer_sms_timer,timer,(void *)dynamic_sms_task,NULL,0);
}
 
/*******************************************************************
** 函数名:     dynamic_sms_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_sms_task(void * str)
{
    DYNAMIC_SMS_DATA_T sms_data;
    kal_uint32 task_time = 5*1000;
    APP_CNTX_T * app_cntx = dynamic_app_cntx_get();

    if (app_cntx->gsm_state == 1 && dynamic_telephony_call_status() == CALL_RELEASE)
    {
        if (s_sms_list.data_in != s_sms_list.data_out)
        {
            dynamic_debug("dynamic_sms_task");
            memset( &sms_data, 0, sizeof(sms_data));
            memcpy( &sms_data,&s_sms_list.info[s_sms_list.data_out],sizeof(DYNAMIC_SMS_DATA_T));
            dynamic_debug("num:%s,len:%d,data:%s",sms_data.ph_num,sms_data.len,sms_data.data);
            if (sms_data.len > 0 && strlen((char*)sms_data.ph_num) >= 3)
            { 
                dynamic_telephony_sms_send((char*)sms_data.ph_num,(char*)sms_data.data,dynamic_sms_send_cb);
                task_time = 45*1000; 
            }
            else
            {
                dynamic_sms_list_pop();
            }
        }
    }

    dynamic_timer_start(enum_timer_sms_timer,task_time,(void *)dynamic_sms_task,NULL,0);
}

/*******************************************************************
** 函数名:     dynamic_sms_recv_cb
** 函数描述:   接收到新短信回调
** 参数:       num:号码，字符串格式  data:信息内容
** 返回:       
********************************************************************/
void dynamic_sms_recv_cb(char *num,char* data,kal_uint32 len)
{
    dynamic_debug("dynamic_sms_recv_cb num:%s,len:%d,%s",num,len,data);
    dynamic_cmd_parse(DYNAMIC_CMD_TYPE_SMS,num,data,len);
}

/*******************************************************************
** 函数名:     dynamic_sms_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_sms_init(void)
{
    dynamic_telephony_sms_recv_reg((SMS_CB)dynamic_sms_recv_cb);
    dynamic_sms_list_init();
    dynamic_timer_start(enum_timer_sms_timer,20*1000,(void *)dynamic_sms_task,NULL,0);
}

