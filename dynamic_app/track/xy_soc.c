/********************************************************************************
**
** 文件名:     xy_soc.c
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

typedef struct _xy_soc_recv_
{
	kal_int32 datalen;
	kal_uint8 databuf[XY_SOC_DATA_SIZE];
}XY_SOC_RECV_T;

XY_SOC_RECV_T s_tcp_rev_data;

#define XY_WAIT_ASK_TIME 45 //等待服务器应答时间
#define XY_NOASK_CNT_FOR_CLOSE_SOC 2 // N次未收到应答，关闭SOCKET


typedef struct {
    kal_uint32 len;
    kal_uint8 *str;
} XY_SOC_DATA_T;

typedef struct 
{
    kal_int8 soc_id;
}XY_CONTEXT_T;



static ASMRULE_T s_rule = {0x7e, 0x7d, 0x02, 0x01};

static XY_CONTEXT_T s_soc_cont;
static DYNAMIC_QUEUE_T* s_soc_queue = NULL;

static kal_uint8 s_soc_nodata = 0;
static kal_uint8 s_noask_cnt = 0;
static kal_uint8 s_heart_no_ask = 0;


typedef struct _xy_soc_info_st
{
	u8 soc_connect_ok;						//socket连接平台成功标志
	u8 reg_ok;								//注册成功标志
	u8 auth_ok;								//登录鉴权成功标志
	u8 auth_fail_cnt;						//发送成功，平台应答失败的次数
	u8 send_cnt;							//报文发送成功却未应答的次数
	u8 ack_fail_cnt;						//应答失败次数
	u8 send_fail_cnt;						//发送失败的次数
	u8 close_soc_cnt;						//在连接平台到发送鉴权过程中，关闭soc的次数
} xy_soc_info_st;

void xy_soc_task(void*ptr);
void xy_soc_heart_wait_ask(void*str);
void xy_soc_heart_reset(void);
kal_uint8 xy_soc_close(void);

/*******************************************************************
** 全部变量定义 
********************************************************************/
/* socket任务相关信息 */
static xy_soc_info_st s_xy_soc_info;


/*******************************************************************
** 函数: void xy_soc_set_ack_fail_cnt(kal_uint8 value)
** 描述: 设置响应失败的次数
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_set_ack_fail_cnt(kal_uint8 value)
{
	s_xy_soc_info.send_cnt = 0;
    s_xy_soc_info.ack_fail_cnt = value;
}

/*******************************************************************
** 函数: kal_uint8 xy_soc_get_ack_fail_cnt(void)
** 描述: 获取响应失败的次数
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_get_ack_fail_cnt(void)
{
    return s_xy_soc_info.ack_fail_cnt;
}


/*******************************************************************
** 函数: xy_soc_get_reg_ok_state
** 描述: 获取注册成功状态
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_get_reg_ok_state(void)
{
    return s_xy_soc_info.reg_ok;
}

/*******************************************************************
** 函数: xy_soc_get_connect_ok_state
** 描述: 获取socket连接成功状态
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_get_connect_ok_state(void)
{
    return s_xy_soc_info.soc_connect_ok;
}

/*******************************************************************
** 函数: xy_soc_get_auth_ok_state
** 描述: 获取鉴权成功状态
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_get_auth_ok_state(void)
{
    return s_xy_soc_info.auth_ok;
}

/*******************************************************************
** 函数: xy_soc_set_auth_ok_state
** 描述: 设置鉴权成功状态
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_set_auth_ok_state(kal_uint8 flag)
{
    s_xy_soc_info.auth_ok = flag;
}

/*******************************************************************
** 函数: xy_soc_set_reg_ok_state
** 描述: 设置注册成功状态
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_set_reg_ok_state(kal_uint8 flag)
{
	s_xy_soc_info.reg_ok = flag;
}


/*******************************************************************
** 函数: xy_soc_clear_link_info
** 描述: 清除链路信息
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_clear_link_info(void)
{
	if (s_soc_cont.soc_id >= 0)
	{
		xy_soc_close();
	}
	
	s_xy_soc_info.ack_fail_cnt = 0;
	s_xy_soc_info.send_fail_cnt = 0;
	s_xy_soc_info.send_cnt = 0;
	s_xy_soc_info.auth_fail_cnt = 0;
	
	s_xy_soc_info.reg_ok = 0;
	s_xy_soc_info.auth_ok = 0;
	s_xy_soc_info.soc_connect_ok = 0;
}

/*******************************************************************
** 函数名:     xy_assemble_by_rules
** 函数描述:   按规则拆分数据 自动增加头尾
** 参数:       
** 返回:       
********************************************************************/
kal_uint32 xy_assemble_by_rules(kal_uint8 *dptr, kal_uint8 *sptr, kal_uint32 len, ASMRULE_T *rule)
{
    kal_uint8 temp = 0;
    kal_uint32 rlen = 0;
    
    if (rule == NULL) return 0;
    
    *dptr++ = rule->c_flags;
    rlen    = 1;
    for (; len > 0; len--) {
        temp = *sptr++;
        if (temp == rule->c_flags) {            /* c_flags    = c_convert0 + c_convert1 */
            *dptr++ = rule->c_convert0;
            *dptr++ = rule->c_convert1;
            rlen += 2;
        } else if (temp == rule->c_convert0) {  /* c_convert0 = c_convert0 + c_convert2 */
            *dptr++ = rule->c_convert0;
            *dptr++ = rule->c_convert2;
            rlen += 2;
        } else {
            *dptr++ = temp;
            rlen++;
        }
    }
    *dptr = rule->c_flags;
    rlen++;
    return rlen;
}

/*******************************************************************
** 函数名:     xy_deassmble_by_rules
** 函数描述:   按规则还原数据
** 参数:       
** 返回:       
********************************************************************/
kal_uint32 xy_deassmble_by_rules(kal_uint8 *dptr, kal_uint8 *sptr, kal_uint32 len, ASMRULE_T *rule)
{
    kal_uint32 rlen = 0;
    kal_uint8 prechar = 0;
    kal_uint8 curchar = 0;
    kal_uint8 c_convert0 = 0;
    
    if (rule == NULL) return 0;
    c_convert0 = rule->c_convert0;
    rlen = 0;
    prechar = (~c_convert0);
    for (; len > 0; len--) {
        curchar = *sptr++;
        if (prechar == c_convert0) {
            if (curchar == rule->c_convert1) {            /* c_convert0 + c_convert1 = c_flags */
                prechar = (~c_convert0);
                *dptr++ = rule->c_flags;
                rlen++;
            } else if (curchar == rule->c_convert2) {     /* c_convert0 + c_convert2 = c_convert0 */
                prechar = (~c_convert0);
                *dptr++ = c_convert0;
                rlen++;
            } else {
                return 0;
            }
        } else {
            if ((prechar = curchar) != c_convert0) {
                *dptr++ = curchar;
                rlen++;
            }
        }
    }
    return rlen;
}

/*******************************************************************
** 函数名:     xy_soc_get_socid
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_int8 xy_soc_get_socid(void)
{
    return s_soc_cont.soc_id;
}

/*******************************************************************
** 函数名:     xy_soc_close
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_close(void)
{

    dynamic_debug("user xy_soc_close");

	dynamic_soc_close(s_soc_cont.soc_id);
	s_soc_cont.soc_id = -1;

	s_xy_soc_info.ack_fail_cnt = 0;
	s_xy_soc_info.auth_fail_cnt = 0;
	s_xy_soc_info.auth_ok = 0;
	s_xy_soc_info.close_soc_cnt = 0;
	s_xy_soc_info.reg_ok = 0;
	s_xy_soc_info.send_cnt = 0;
	s_xy_soc_info.send_fail_cnt = 0;
	s_xy_soc_info.soc_connect_ok = 0;

	return 0;
}

/*******************************************************************
** 函数名:     xy_soc_close_delay
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_close_delay(void)
{
    dynamic_debug("user xy_soc_close_delay");
    return xy_soc_close();
}

/*******************************************************************
** 函数名:     xy_soc_send
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_int8 xy_soc_send(kal_int8 socket, char *buf, kal_uint32 len)
{
    kal_uint32 result = 0;
    kal_uint8 * data_dtr = dynamic_malloc_malloc(XY_SOC_DATA_SIZE+100);
    kal_uint16 dtr_len = 0;

    if (data_dtr == NULL)
    {
        return result;
    }
    dtr_len = xy_assemble_by_rules(data_dtr,(kal_uint8*)buf,len,&s_rule);
    result = dynamic_soc_send(socket,(char*)data_dtr,dtr_len);
    if (result == dtr_len)
    {
        dynamic_debug("send ok");
        result = 1;
        xy_soc_heart_reset();
    }
    return result;
}

/*******************************************************************
** 函数名:     xy_soc_data_send
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_int8 xy_soc_data_send(char *buf, kal_uint32 len)
{
    return xy_soc_send(s_soc_cont.soc_id,buf,len);
}

/*******************************************************************
** 函数名:     xy_soc_data_empty
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_data_empty(void)
{
    XY_SOC_DATA_T * data_str;
    
    data_str = dynamic_queue_peek(s_soc_queue);
    if (data_str != NULL)
    {
        return 0;
    }
    
    return 1;
}

/*******************************************************************
** 函数名:     xy_soc_data_pop
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_bool xy_soc_data_pop(void)
{
    dynamic_free(dynamic_queue_pop(s_soc_queue));
    return 1;
}

/*******************************************************************
** 函数名:     xy_soc_data_in
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_bool xy_soc_data_in(kal_uint8 *data,kal_uint16 len)
{
    XY_SOC_DATA_T * data_str;
    
    if (s_soc_queue == NULL)
    {
        dynamic_error("xy_soc_data_in s_soc_queue == NULL");
        dynamic_start_reset(RESET_TYPE_MEM_ERR);
        return KAL_FALSE;
    }

    data_str = dynamic_malloc_malloc(sizeof(XY_SOC_DATA_T)+len+1);
    if (data_str == 0)
    {
        dynamic_error("xy_soc_data_in data_str == NULL");
        return KAL_FALSE;
    }

    if (s_soc_queue->size >= s_soc_queue->limit)
    {
        dynamic_debug("s_soc_queue->size >= s_soc_queue->limit:%d,%d",s_soc_queue->size,s_soc_queue->limit);
        xy_soc_data_pop();
    }
    data_str->str = (kal_uint8*)(data_str+1);
    data_str->len = len;
    memcpy(data_str->str,data,len);
    
    dynamic_debug("xy_soc_data_in queue size:%d,len:%d,%s",s_soc_queue->size,data_str->len,data_str->str);
    
    if (dynamic_queue_push(s_soc_queue,data_str) != 1)
    {
        dynamic_error("xy_soc_data_in dynamic_queue_push err");
        dynamic_free(data_str);
        return KAL_FALSE;
    }
    
    return KAL_TRUE;
}

/*******************************************************************
** 函数名:     xy_soc_list_data_send
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_list_data_send(void)
{
    XY_SOC_DATA_T * data_str;
    kal_int32 ret;
    XY_INFO_T * xy_info = xy_get_info();

    data_str = dynamic_queue_peek(s_soc_queue);
    if (data_str != NULL)
    {
        char * cmdstr=NULL;
        
        if (s_noask_cnt == XY_NOASK_CNT_FOR_CLOSE_SOC)
        {
            s_noask_cnt++;
            xy_soc_close();
            return 1;
        }
        else if (s_noask_cnt >= (XY_NOASK_CNT_FOR_CLOSE_SOC*2))
        {
            dynamic_debug("多次未收到应答，删除该条数据");
            xy_soc_data_pop();
            s_noask_cnt = 0;
            return 1;
        }
        
        ret = xy_soc_send(s_soc_cont.soc_id,(char*)data_str->str,data_str->len);
        
        dynamic_debug("xy_soc_list_data_send:%d,%s",data_str->len,data_str->str);
        if (ret != data_str->len)
        {
            dynamic_debug("xy_soc_list_data_send false:%d",ret);
            return 0;
        }

        cmdstr = strstr((char*)data_str->str,",T19,");
        if (strlen((char*)xy_info->user) == 0 || cmdstr != NULL || data_str->len == 0)
        {
            xy_soc_data_pop();
            return 1;
        }
        else
        {
            s_noask_cnt++;
            dynamic_debug("数据发送成功，等待应答:%d",s_noask_cnt);
            return 2;
        }
    }
    else
    {
        s_noask_cnt = 0;
    }
    
    return 1;
}




/*******************************************************************
** 函数名:	   xy_soc_recv_task
** 函数描述:   
** 参数:	   
** 返回:	   
********************************************************************/
void xy_soc_recv_task(void*str)
{
	if (s_tcp_rev_data.datalen > 0)
	{
        kal_uint8 *temp;
        kal_uint32 len;        
        kal_uint8 *str_head = 0;
        kal_uint8 *str_end = 0;
        kal_uint32 len_count = 0;
        kal_uint32 cur_len = 0;

        kal_uint8 data_dtr[XY_SOC_DATA_SIZE];
        kal_uint16 dtr_len = 0;

        memset(data_dtr,0,sizeof(data_dtr));
        //dynamic_log_hex(s_tcp_rev_data.databuf,s_tcp_rev_data.datalen); 
        dtr_len = xy_deassmble_by_rules(data_dtr,s_tcp_rev_data.databuf,s_tcp_rev_data.datalen,&s_rule);
        temp = data_dtr;
        len = dtr_len;
        dynamic_debug("-----------------c26_soc_read_task--------------------");
        dynamic_log_hex(temp,len);         
        while(temp != NULL)
		{
            str_head = dynamic_find_byte_pos(temp,XY_SOC_PROTOCOL_HEAD,(len-len_count));
            
            if (str_head != NULL)
            {                
                str_end = dynamic_find_byte_pos((str_head+1),XY_SOC_PROTOCOL_END,(len-len_count-1));

                if (str_end != NULL)
                {
                    cur_len = (str_end - str_head + 1);
                    if (cur_len > (len-len_count))
                    {
                        dynamic_debug("-----------------xy_soc_read_task err len ,break:%d,%d--------------------",cur_len,(len-len_count));
                        break;
                    }
                }
                else
                {
                    dynamic_debug("str_end == NULL");
                    break;
                }
            }
            else
            {
                dynamic_debug("-----------------str_head == NULL--------------------");
                break;
            }

            len_count += cur_len;
            dynamic_debug("len:%d,%d,%d",len,len_count,cur_len);
            temp = str_head+cur_len;
            xy_soc_data_deal((str_head+1),(cur_len-2));   
            
            if (len_count >= len)
            {
                break;
            }
		}
		memset(s_tcp_rev_data.databuf,0,sizeof(s_tcp_rev_data.databuf));
		s_tcp_rev_data.datalen = 0;
	}
	
    //dynamic_timer_start(enum_timer_xy_soc_read_timer,100,(void*)xy_soc_read_task,NULL,FALSE); 

}

/*******************************************************************
** 函数名:     xy_soc_recv_data
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_recv_data(kal_uint8*data,kal_uint32 len)
{    
    if (s_tcp_rev_data.datalen + len < sizeof(s_tcp_rev_data.databuf))
    {
        memcpy(&s_tcp_rev_data.databuf[s_tcp_rev_data.datalen],data,len);
        s_tcp_rev_data.datalen += len;
    }
    else
    {
        dynamic_debug("-----------------xy_soc_recv err len:%d,%d--------------------",len,s_tcp_rev_data.datalen + len);
    }
    dynamic_timer_start(enum_timer_soc_read_timer,100,(void*)xy_soc_recv_task,NULL,FALSE); 
}

/*******************************************************************
** 函数名:     xy_soc_cb
** 函数描述:   soc回调
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_cb(DYNAMIC_SOC_CB_E type,kal_int8 socket,void *user_data, void *data, int len)
{
    DYNAMIC_SOC_CONNECTION_T *p_connection;
    
    p_connection = dynamic_soc_find_connection(socket);
                    
    switch (type)
    {
        case DYNAMIC_SOC_HOST_OK: // 解析域名成功
            dynamic_debug("xy_soc_cb DYNAMIC_SOC_HOST_OK");
        break;
        
        case DYNAMIC_SOC_HOST_FAILED: // 解析域名失败
            dynamic_debug("xy_soc_cb DYNAMIC_SOC_HOST_FAILED");
            s_soc_cont.soc_id = -1;
			s_xy_soc_info.soc_connect_ok = 0;
        break;
        
        case DYNAMIC_SOC_CONNECT_OK: // 连接成功
            dynamic_debug("xy_soc_cb DYNAMIC_SOC_CONNECT_OK");
			s_xy_soc_info.soc_connect_ok = 1;
			s_xy_soc_info.reg_ok = 0;
			s_xy_soc_info.auth_ok = 0;
            dynamic_timer_start(enum_timer_soc_task_timer,5000,(void*)xy_soc_task,NULL,FALSE);
        break;
        
        case DYNAMIC_SOC_CONNECT_FAILED: // 连接失败
            dynamic_debug("xy_soc_cb DYNAMIC_SOC_CONNECT_FAILED");
            s_soc_cont.soc_id = -1;
			s_xy_soc_info.soc_connect_ok = 0;
        break;
        
        case DYNAMIC_SOC_CLOSE: // 连接被关闭
        {
            dynamic_debug("xy_soc_cb DYNAMIC_SOC_CLOSE");
            s_soc_cont.soc_id = -1;
			s_xy_soc_info.soc_connect_ok = 0;
        }    
        break;
        
        case DYNAMIC_SOC_RECV: // 收到SOC数据
            dynamic_debug("xy_soc_cb DYNAMIC_SOC_RECV:%d,%s",len,data);
			dynamic_log_hex(data,len);
            s_soc_nodata = 0;
            xy_soc_recv_data(data,len);
        break;
        
        case DYNAMIC_SOC_WRITE: // 保留
            dynamic_debug("xy_soc_cb DYNAMIC_SOC_WRITE");
            
        break;

        default:

        break;
    }
}
/*******************************************************************
** 函数名:     xy_soc_heart_ask
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_heart_ask(void)
{
    s_heart_no_ask = 0;
}
/*******************************************************************
** 函数名:     xy_soc_heart_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_heart_task(void*str)
{
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint32 heart_time = 30;

    if (xy_info->heart_time != 0)
    {
        heart_time = xy_info->heart_time;
    }

    if ((1 == s_xy_soc_info.auth_ok) && (xy_info->heart_time != 0))
    {
        dynamic_debug("xy_soc_heart_task");
        if (xy_soc_heart_up() == 1)
        {
            if (dynamic_timer_is_runing(enum_timer_track_heart_wait_ask_timer) == 0)
            {
                dynamic_timer_start(enum_timer_track_heart_wait_ask_timer,40*1000,(void*)xy_soc_heart_wait_ask,NULL,FALSE);
            }
        }
    }
    dynamic_timer_start(enum_timer_track_heart_timer,heart_time*1000,(void*)xy_soc_heart_task,NULL,FALSE); 
}

/*******************************************************************
** 函数名:     xy_soc_heart_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_heart_reset(void)
{
    kal_uint32 heart_time = 30;
	XY_INFO_T * xy_info = xy_get_info();

    if (xy_info->heart_time != 0)
    {
        heart_time = xy_info->heart_time;
    }

    dynamic_timer_stop(enum_timer_track_heart_wait_ask_timer);
    dynamic_timer_start(enum_timer_track_heart_timer,heart_time*1000,(void*)xy_soc_heart_task,NULL,FALSE); 
}

/*******************************************************************
** 函数名:     xy_soc_heart_wait_ask
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_heart_wait_ask(void*str)
{
    s_heart_no_ask++;
    dynamic_debug("心跳未应答:%d",s_heart_no_ask);
    if (s_heart_no_ask == 2)
    {
        xy_soc_close();
    }
    if (s_heart_no_ask >= 3)
    {
        s_heart_no_ask = 0;
        dynamic_start_reset(RESET_TYPE_SERVER_NOASK);
    }
    
    dynamic_timer_start(enum_timer_track_heart_timer,20*1000,(void*)xy_soc_heart_task,NULL,FALSE); 
}

/*******************************************************************
** 函数名:     xy_soc_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_task(void*ptr)
{
    kal_uint32 tasktime = 5*1000;
#if 1
	static u8 gsm_net_fail_cnt = 0;
#else
	static kal_uint16 err_cnt = 0; // 连续多次判断socket连接未成功，关闭SOCKET
    static kal_uint16 err_reset = 0; // 连续多次异常，重启设备
    static kal_uint16 err_gsm_state = 0;
    static kal_uint8 send_false = 0;
#endif
    XY_INFO_T * xy_info = xy_get_info();
    APP_CNTX_T * app_cntx = dynamic_app_cntx_get();
    DYNAMIC_SOC_CONNECTION_T *p_connection = dynamic_soc_find_connection(s_soc_cont.soc_id);

    if (dynamic_telephony_call_status() == CALL_ACCEPT || dynamic_telephony_call_status() == CALL_CONNECT)
    {
        tasktime = 2*1000;
        s_soc_nodata = 0;
        dynamic_timer_start(enum_timer_soc_task_timer,tasktime,(void*)xy_soc_task,NULL,FALSE);
        return;
    }

#if 1
	if (p_connection && (DYNAMIC_SOC_CONNECTED == p_connection->status))
	{

		dynamic_log("connet:%d, reg:%d, auth:%d\r\n", s_xy_soc_info.soc_connect_ok, s_xy_soc_info.reg_ok, s_xy_soc_info.auth_ok);
		dynamic_log("auth fail cnt:%d, send cnt:%d, ack fail cnt:%d, send fail cnt:%d close soc cnt:%d\r\n",
			s_xy_soc_info.auth_fail_cnt, s_xy_soc_info.send_cnt, s_xy_soc_info.ack_fail_cnt, s_xy_soc_info.send_fail_cnt,
			s_xy_soc_info.close_soc_cnt);
	
		/* 已连接上平台 */
		if (1 == s_xy_soc_info.soc_connect_ok)
		{
			/* =======鉴权通过 */
			if (1 == s_xy_soc_info.auth_ok)
			{
				/* 位置包的发送 */
				tasktime = 5*1000;

				goto cycle;
			}

			/* 一直未应答或发送失败 */
			if ((s_xy_soc_info.send_cnt > 3) || (s_xy_soc_info.send_fail_cnt > 3))
			{
				/* 2种情况去关闭socket重新连接：
				1.发送成功三次，但是一直未应答的情况.
				2.连续三次发送失败
				*/
				dynamic_debug("send data cnt[%d], send fail cnt[%d] close socket[%d]\r\n",
				s_xy_soc_info.send_cnt, s_xy_soc_info.send_fail_cnt, s_soc_cont.soc_id);

				/* 需要重新进行连接设备 */
				s_xy_soc_info.ack_fail_cnt = 0;
				s_xy_soc_info.send_fail_cnt = 0;
				s_xy_soc_info.send_cnt = 0;
				s_xy_soc_info.auth_fail_cnt = 0;
				
				s_xy_soc_info.reg_ok = 0;
				s_xy_soc_info.auth_ok = 0;
				s_xy_soc_info.soc_connect_ok = 0;
				
				s_xy_soc_info.close_soc_cnt++;
				dynamic_soc_close(s_soc_cont.soc_id);

				/* 10s后重连 */
				tasktime = 10 * 1000;
				goto cycle;
			}
		
			/* ========注册成功, 鉴权失败 */
			if (1 == s_xy_soc_info.reg_ok)
			{
				if (s_xy_soc_info.ack_fail_cnt >= 3)
				{
					/* 连续应答鉴权失败三次，上传的鉴权信息问题导致，重新发送注册信息，获取鉴权信息 */
					s_xy_soc_info.reg_ok = 0;
					s_xy_soc_info.ack_fail_cnt = 0;
					s_xy_soc_info.send_fail_cnt = 0;
					s_xy_soc_info.send_cnt = 0;

					dynamic_debug("ack auth fail cnt[%d]\r\n", s_xy_soc_info.auth_fail_cnt + 1);
					if (++s_xy_soc_info.auth_fail_cnt >= 3)
					{
						s_xy_soc_info.auth_fail_cnt = 0;
						s_xy_soc_info.soc_connect_ok = 0;
						s_xy_soc_info.close_soc_cnt++;
						dynamic_soc_close(s_soc_cont.soc_id);

						/* 10s后重连 */
						tasktime = 10 * 1000;
					}
					else
					{
						/* 5s后重发送注册信息 */
						tasktime = 5 * 1000;
					}
					goto cycle;
				}

				if (KAL_TRUE == xy_soc_login_up())
				{
					s_xy_soc_info.send_cnt++;
					s_xy_soc_info.send_fail_cnt = 0;

					/* 等待45s后，判断是否鉴权通过 */
					tasktime = XY_WAIT_ASK_TIME * 1000;
				}
				else
				{
					s_xy_soc_info.send_fail_cnt++;

					/* 5s后重发 */
					tasktime = 5 * 1000;
				}

				goto cycle;
			}

			/* =========还未注册成功 */
			/* 发送成功注册报文，连续三次应答失败，注册信息不对，导致平台应答失败，重启一次 */
			if (s_xy_soc_info.ack_fail_cnt >= 3)
			{
				s_xy_soc_info.ack_fail_cnt = 0;
				s_xy_soc_info.send_fail_cnt = 0;
				s_xy_soc_info.send_cnt = 0;

				dynamic_debug("send reg data succ, ack fail 3 times, close socket[%d]\r\n",
								s_soc_cont.soc_id);

				s_xy_soc_info.reg_ok = 0;
				s_xy_soc_info.soc_connect_ok = 0;
				s_xy_soc_info.close_soc_cnt++;
				dynamic_soc_close(s_soc_cont.soc_id);

				/* 10s后重连 */
				tasktime = 10 * 1000;
				goto cycle;
			}

			/* 发送注册报文 */
			if (KAL_TRUE == xy_soc_regster_up())
            {
                tasktime = XY_WAIT_ASK_TIME*1000; // 发送成功，等待应答,45s
                s_xy_soc_info.send_cnt++;
				s_xy_soc_info.send_fail_cnt = 0;
            }
            else
            {
                s_xy_soc_info.send_fail_cnt++;

				/* 5s之后，重新进行发送 */
				tasktime = 5*1000;
            }
			goto cycle;
		}

		/* 但是connect OK 标志有误,等待10s后去重连平台 */
		if (s_soc_cont.soc_id >= 0)
		{
			s_xy_soc_info.close_soc_cnt++;
			p_connection->status = DYNAMIC_SOC_CLOSING;
			dynamic_soc_close(s_soc_cont.soc_id);
		}
		tasktime = 10 *1000;
		goto cycle;
	}

	/* 开始连接平台 */
	if (!p_connection)
	{
		s_xy_soc_info.close_soc_cnt = 0;

		if (1 == app_cntx->gsm_state)//搜索到网络了
        {
			kal_uint8 server[XY_URL_LEN + 1] = {0};
            kal_uint16 port = 0;

			gsm_net_fail_cnt = 0;
            memset(server,0,sizeof(server));
            if (strlen((char*)xy_info->server) > 7 && xy_info->port > 0)
            {
                memcpy(server, xy_info->server, strlen((char*)xy_info->server));
                port = xy_info->port;
            }
            else
            {
                dynamic_log("err server:%s,port:%d\r\n",xy_info->server,xy_info->port);
                memcpy(server,XY_DEFAULT_SERVER,strlen((char*)XY_DEFAULT_SERVER));
                port = XY_DEFAULT_PORT;
            }

			/* 用TCP的方式去连接平台 */
			dynamic_log("connect server: %s, %d\r\n", server, port);
			s_soc_cont.soc_id = dynamic_soc_connect(0, (kal_int8*)server, port,(void*)xy_soc_cb, NULL);

			tasktime = 15*1000;
            goto cycle;
		}

		/* 未注册上网络，100s未检测搜索都网络，就重启 */
		if (++gsm_net_fail_cnt >= 20)
		{
			gsm_net_fail_cnt = 0;
			dynamic_log("soc err err_gsm_state,start reset\r\n");
	        if (dynamic_sim_get_valid() == 0)
	        {
	            if (xy_info->reset_cnt < 3)
	            {
	                xy_info->reset_cnt++;
	                dynamic_start_reset(RESET_TYPE_NO_SIM);
	            }
	            dynamic_log("xy_info->reset_cnt:%d\r\n", xy_info->reset_cnt);
	        }
	        else
	        {
	            xy_info->reset_cnt = 0;
	            dynamic_start_reset(RESET_TYPE_SOC_ERR);
	        }

			tasktime = 5*1000;
		}
	}
	
cycle:

	/* 在连接平台到发送鉴权过程中，关闭soc的次数大于10次，就重启设备 */
	if (s_xy_soc_info.close_soc_cnt >= 10)
	{
		dynamic_log("over 10 times, close soc ,start reset......\r\n");
        dynamic_start_reset(RESET_TYPE_SOC_ERR);
	}
	
	dynamic_timer_start(enum_timer_soc_task_timer, tasktime,(void*)xy_soc_task,NULL,FALSE);
#else
    if (p_connection == NULL) // SOC未连接
    {
        err_cnt = 0;
        xy_soc_login_set_wait_ask(0);
        if (app_cntx->gsm_state == 1)
        {
            kal_uint8 server[XY_URL_LEN];
            kal_uint16 port;

            memset(server,0,sizeof(server));
            err_gsm_state = 0;
            if (strlen((char*)xy_info->server) > 7 && xy_info->port > 0)
            {
                memcpy(server,xy_info->server,strlen((char*)xy_info->server));
                port = xy_info->port;
            }
            else
            {
                dynamic_log("err server:%s,port:%d\r\n",xy_info->server,xy_info->port);
                memcpy(server,XY_DEFAULT_SERVER,strlen((char*)XY_DEFAULT_SERVER));
                port = XY_DEFAULT_PORT;
            }
            s_soc_cont.soc_id = dynamic_soc_connect(0,(kal_int8*)server,port,(void*)xy_soc_cb,NULL);
            tasktime = 15*1000;
            send_false++;
        }
        else if (++err_gsm_state >= 20)
        {
            err_gsm_state = 0;
            dynamic_log("soc err err_gsm_state,start reset\r\n");
            if (dynamic_sim_get_valid() == 0)
            {
                if (xy_info->reset_cnt < 3)
                {
                    xy_info->reset_cnt++;
                    dynamic_start_reset(RESET_TYPE_SOC_ERR);
                }
                dynamic_log("xy_info->reset_cnt:%d\r\n",xy_info->reset_cnt);
            }
            else
            {
                xy_info->reset_cnt = 0;
                dynamic_start_reset(RESET_TYPE_SOC_ERR);
            }
        }
    }
    else
    {
        if (p_connection->status == DYNAMIC_SOC_CONNECTED)
        {
            err_cnt = 0;
            if (xy_info->t808_para.is_regster == 0)
            {
                if (xy_soc_regster_up() == KAL_TRUE)
                {
                    tasktime = XY_WAIT_ASK_TIME*1000; // 发送成功，等待应答
                    send_false = 0;
                }
                else
                {
                    send_false++;
                }
            }
            else if (xy_soc_get_auth_ok_state() == 0)
            {
                if (xy_soc_login_wait_ask() == 1)
                {
                    dynamic_debug("登陆未应答");
                    xy_soc_login_set_wait_ask(0);
                    xy_soc_close();
                    if (strlen((char*)xy_info->user) > 0)
                    {
                        err_reset++;
                    }
                }
                else if (xy_soc_login_up() == KAL_TRUE)
                {
                    xy_soc_login_set_wait_ask(1);
                    tasktime = XY_WAIT_ASK_TIME*1000; // 发送成功，等待应答
                    send_false = 0;
                }
                else
                {
                    send_false++;
                }
            }    
            else 
            {
                kal_uint8 send_result = xy_soc_list_data_send();
                if (send_result == 0)
                {
                    send_false++;
                }
                else
                {
                    send_false = 0;
                    err_reset = 0;
                    if (send_result == 2)
                    {
                        tasktime = XY_WAIT_ASK_TIME*1000;
                    }
                }                
            }
        }
        else
        {
            // 超时异常处理
            if (++err_cnt >= 6)
            {
                dynamic_debug("xy_soc_task err_cnt:%d",err_cnt);
                err_cnt = 0;
                dynamic_soc_close(s_soc_cont.soc_id);
                err_reset++;
            }
        }
    }
    
    if (send_false >= 3) // 连续发送失败，关闭socket
    {
        dynamic_debug("send_false err cnt:%d",err_reset);
        send_false = 0;
        dynamic_soc_close(s_soc_cont.soc_id);
        err_reset++;
    }
    
    if (err_reset >= 8)
    {
        err_reset = 0;
        dynamic_log("soc err ,start reset\r\n");
        dynamic_start_reset(RESET_TYPE_SOC_ERR);
    }

    dynamic_timer_start(enum_timer_soc_task_timer, tasktime, (void*)xy_soc_task,NULL,FALSE);
#endif

}

/*******************************************************************
** 函数名:     xy_soc_err_check
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_err_check(void)
{
#if 0
    XY_INFO_T * xy_info = xy_get_info();

    if (xy_info->mode == XY_TRACK_MODE3)
    {
        s_soc_nodata = 0;
    }
    else
    {
        dynamic_debug("s_soc_nodata:%d",s_soc_nodata);
    }
#endif
    if (++s_soc_nodata >= 15*6) //90s没有收到平台下发数据重启
    {
        s_soc_nodata = 0;
        dynamic_start_reset(RESET_TYPE_NODATA);
    }

    dynamic_timer_start(enum_timer_soc_err_check_timer,10*1000,(void*)xy_soc_err_check,NULL,FALSE); 
}

/*******************************************************************
** 函数名:     xy_soc_data_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_data_init(void)
{
    char * data = NULL;
    kal_int32 datalen,i;

    data = dynamic_malloc_malloc(MAX_SOC_DATA_SIZE+1);

    if (data != NULL)
    {
        char * str = NULL;
        char * dtr = NULL;
        kal_uint32 curlen;
        
        datalen = dynamic_file_read(XY_SOC_DATA_FILE_NAME,data,MAX_SOC_DATA_SIZE);
        dynamic_debug("xy_soc_data_init datalen:%d",datalen);
        for (i=0;i<datalen;i++)
        {
            if (data[i] == XY_SOC_PROTOCOL_HEAD)
            {
                str = &data[i];
            }
            else if (data[i] == XY_SOC_PROTOCOL_END)
            {
                if (str == NULL)
                {
                    dynamic_debug("err heard str");
                }
                else
                {
                    dtr = &data[i];
                    curlen = dtr-str+1;
                    dynamic_debug("curlen:%d",curlen);
                    if (curlen < XY_SOC_DATA_SIZE)
                    {
                        xy_soc_data_in((kal_uint8*)str,curlen);
                    }
                    else
                    {
                        dynamic_debug("err curlen:%d,%d",curlen,XY_SOC_DATA_SIZE);
                    }
                    dtr = NULL;
                    str = NULL;
                }
            }
        }
        dynamic_free(data);
        dynamic_file_dele(XY_SOC_DATA_FILE_NAME);
    }
}

/*******************************************************************
** 函数名:     xy_soc_data_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_data_reset(void)
{
    XY_SOC_DATA_T * data_str;

    while (s_soc_queue->size > 0)
    {
        data_str = dynamic_queue_peek(s_soc_queue);
        if (data_str != NULL)
        {
            xy_soc_data_pop();
        }
        else
        {
            break;
        }
    }
    dynamic_debug("xy_soc_data_reset");
}

/*******************************************************************
** 函数名:     xy_soc_data_save
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_data_save(void)
{
    char * data = NULL;
    kal_int32 datalen = 0;
    kal_uint16 i;

    data = dynamic_malloc_malloc(MAX_SOC_DATA_SIZE+1);

    if (data != NULL)
    {
        for (i=0;i<XY_MAX_SOC_DATA_NUM;i++)
        {
            XY_SOC_DATA_T *data_str = NULL;
            
            data_str = dynamic_queue_peek(s_soc_queue);
            if (data_str == NULL)
            {
                break;
            }
            else
            {
                memcpy(&data[datalen],data_str->str,data_str->len);
                datalen += data_str->len;
                xy_soc_data_pop();
            }
        }
        if (datalen > 0)
        {
            dynamic_file_write(XY_SOC_DATA_FILE_NAME,data,datalen);
        }
        dynamic_free(data);
    }
}

/*******************************************************************
** 函数名:     xy_soc_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_init(void)
{
    memset(&s_tcp_rev_data,0,sizeof(s_tcp_rev_data));
    memset(&s_soc_cont,0,sizeof(s_soc_cont));
    s_soc_cont.soc_id = -1;
    
    s_soc_queue = dynamic_queue_create(XY_MAX_SOC_DATA_NUM);//缓存100条
    if (s_soc_queue == NULL)
    {
        dynamic_error("xy_soc_init queue create err");
    }
    else
    {
        xy_soc_data_init();//读文件，把保存到文件的盲区数据读到队列中
    }

    dynamic_timer_start(enum_timer_soc_task_timer,15*1000,(void*)xy_soc_task,NULL,FALSE); 
    dynamic_timer_start(enum_timer_soc_err_check_timer,10*1000,(void*)xy_soc_err_check,NULL,FALSE); 
    dynamic_timer_start(enum_timer_track_heart_timer,30*1000,(void*)xy_soc_heart_task,NULL,FALSE);
}

#endif

