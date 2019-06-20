/********************************************************************************
**
** 文件名:     xy_t808_protocol.c
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


/* 终端应答类型 */
typedef enum _T808LoginAckTypeE
{
	E_T808_L_ACK_SUCC = 0,					/* 成功 */
	E_T808_L_ACK_VEH_LOGINED,				/* 车辆已被注册 */
	E_T808_L_ACK_DATABASE_NO_CAR,			/* 数据库中无该车辆 */
	E_T808_L_ACK_DEV_LOGINED,				/* 终端已被注册 */
	E_T808_L_ACK_DATABASE_NO_TERM			/* 数据库中无该终端 */
} T808LoginAckTypeE;

typedef struct _xy_soc_ask_
{
	kal_int16 id; // 平台下发的指令ID
	kal_uint16 num; // 平台下发是指令流水号
	kal_uint8 result; // 结果
}XY_SOC_ASK_T; // 通用应答



static kal_uint8 s_login = 0;
static kal_uint16 s_serial_num = 0;
static kal_uint8 s_wait_login_ask = 0;



/*******************************************************************
** 函数名:     xy_soc_login_state
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_login_state(void)
{
    return s_login;
}

/*******************************************************************
** 函数名:     xy_soc_login_set_state
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_login_set_state(kal_uint8 sign)
{
    s_login = sign;
}

/*******************************************************************
** 函数名:     xy_soc_login_wait_ask
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_login_wait_ask(void)
{
    return s_wait_login_ask;
}

/*******************************************************************
** 函数名:     xy_soc_login_set_wait_ask
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_login_set_wait_ask(kal_uint8 sign)
{
    s_wait_login_ask = sign;
}

/*******************************************************************
** 函数名:     xy_data_pack_reg
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint16 xy_data_pack_reg(kal_uint8 * dtr)
{
    kal_uint16 len = 0;
    XY_INFO_T * xy_info = xy_get_info();
    T808_PARA_T * t808_para = &xy_info->t808_para;
    kal_uint8 veh_id[T808_VEH_ID_LEN];
    kal_uint16 veh_len = 0;
    
    len += dynamic_big_endian_transm_short(&dtr[len],t808_para->provice_id);
    len += dynamic_big_endian_transm_short(&dtr[len],t808_para->city_id);
    memcpy(&dtr[len],t808_para->product_id,T808_PRODUCT_ID_LEN);
    len += T808_PRODUCT_ID_LEN;

    memcpy(&dtr[len],t808_para->dev_model,T808_DEV_TYPE_LEN);
    len += T808_DEV_TYPE_LEN;

    memcpy(&dtr[len],t808_para->dev_id,T808_DEV_ID_LEN);
    len += T808_DEV_ID_LEN;

    dtr[len++] = t808_para->veh_color;


    memset(veh_id,0,sizeof(veh_id));
    veh_len = strlen((char*)t808_para->veh_id);
    if (veh_len == 0 && t808_para->veh_color == 0)
    {
        // 使用VIN
    }
    else
    {
        memcpy(veh_id,t808_para->veh_id,veh_len);
    }
    veh_len = strlen((char*)veh_id);
    if (veh_len == 0)
    {
        memcpy(veh_id,"0000",4);
        veh_len = 4;
    }

    memcpy(&dtr[len],t808_para->dev_id,veh_len);
    len += veh_len;
    
    return len;
}

/*******************************************************************
** 函数名:     xy_soc_data_pack
** 函数描述:   上报数据组包
** 参数:       type:数据类型  inset_list:0直接发送 1插入链表
** 返回:       
********************************************************************/
kal_uint8 xy_soc_data_pack(T808_PROTOCOL_MSGID_E cmd,XY_SOC_SEND_TYPE_E inset_list,kal_uint8* str)
{
    kal_uint8 *datastr = NULL;
    kal_uint16 curlen = 0;
    kal_uint16 lenpoint = 0;
    kal_uint16 headlen = 0;
    kal_bool result = KAL_TRUE;
    //APP_CNTX_T * app_cntx = dynamic_app_cntx_get();
    XY_INFO_T * xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
    
    datastr = dynamic_malloc_malloc(XY_SOC_DATA_SIZE);
    
    if (datastr == NULL)
    {
        return KAL_FALSE;
    }

    // 消息头
    datastr[curlen++] = XY_SOC_PROTOCOL_HEAD;
    curlen += dynamic_big_endian_transm_short(&datastr[curlen],cmd); // 消息ID
    lenpoint = curlen;
    datastr[curlen++] = 0X40;// 消息体属性 电源类型有源目前不考虑、分包、加密，长度后面补充
    datastr[curlen++] = 0;
    
    // 终端手机号
    {
    
        kal_uint8 dev_num[MAX_PHONE_NUM_LEN+2] = "000000000000000000000";
        kal_uint8 numlen = 0;

        numlen = strlen((char*)xy_info->dev_num);
        if (numlen < 12)
        {
            memcpy(&dev_num[MAX_PHONE_NUM_LEN-numlen],xy_info->dev_num,numlen);
            numlen = 12;
        }
        else
        {
            memcpy(dev_num,xy_info->dev_num,numlen);
        }
        if (numlen % 2)
        {
            memcpy(&dev_num[1],dev_num,numlen);
            numlen++;
            dev_num[0] = '0';
        }
        curlen += dynamic_ascii2bcd(&datastr[curlen],dev_num,numlen);
    }
    
    curlen += dynamic_big_endian_transm_short(&datastr[curlen],s_serial_num++); // 消息流水号
    headlen = curlen;
    // 消息体
    switch (cmd)
    {
        case T808_MSGID_C_GENERAL_ACK:		/* 终端通用应答 */
        {
            XY_SOC_ASK_T *ask = (XY_SOC_ASK_T *)str;

            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->id);
            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->num);
            datastr[curlen++] = ask->result;
        }
        break;
        
        case T808_MSGID_C_HEART:				/* 心跳 */

        

        break;
        
        case T808_MSGID_C_DREG:		    /* 注销 */

        

        break;

        
        case T808_MSGID_C_REG:				/* 注册 */
            curlen += xy_data_pack_reg(&datastr[curlen]);
        break;

        
        case T808_MSGID_C_AUTHEN:			/* 鉴权 */
        {
            kal_uint8 authlen = strlen((char*)t808_para->auth_code);
            if (authlen == 0)
            {
                t808_para->is_regster = 0;
                dynamic_debug("鉴权码空，重新注册");
                goto pack_err;
            }
            memcpy(&datastr[curlen],t808_para->auth_code,authlen);
            curlen += authlen;
        }
        break;

        case T808_MSGID_C_PARAM_ACK:			/* 参数汇报 */

        

        break;

        case T808_MSGID_C_ATTR_ACK:			/* 属性应答 */

        

        break;

        case T808_MSGID_C_POSITION:			/* 位置 */

        

        break;

        case T808_MSGID_C_POSITION_ACK:		/* 位置查询应答 */

        

        break;

        case T808_MSGID_C_VEH_CTRL_ACK:		/* 车辆控制应答 */

        

        break;

        case T808_MSGID_C_COMPSATE:		/* 盲区补偿 */

        

        break;

        case T808_MSGID_C_EXPEND:			/* 拓展 */

        

        break;
       
        break;
        
        default:

        break;
    }
    
    // 消息体长度
    {
        kal_uint8 len_buf[2];
        dynamic_big_endian_transm_short(len_buf,(curlen-headlen)); 
        datastr[lenpoint] |= len_buf[0];
        datastr[lenpoint+1] |= len_buf[1];
    }

    // 校验码
    datastr[curlen] = dynamic_checksum_xor(datastr,curlen);
    curlen++;
    
    dynamic_debug("xy_soc_data_pack len:%d",curlen);
    dynamic_log_hex(datastr,curlen);
    if (inset_list == 1)
    {
        result = xy_soc_data_in(datastr,curlen);
    }
    else
    {
        if (xy_soc_data_send((char*)datastr,curlen) == 0)
        {
            result = KAL_FALSE;
        }
    }
pack_err:
    if (result == KAL_FALSE)
    {
        dynamic_debug("xy_soc_data_pack false");
        dynamic_free(datastr);
        return KAL_FALSE;
    } 
    dynamic_free(datastr);
    
    return KAL_TRUE;
}

/*******************************************************************
** 函数名:     xy_soc_regster_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_regster_up(void)
{
    dynamic_debug("xy_soc_regster_up");
    return xy_soc_data_pack(T808_MSGID_C_REG,0,0);
}

/*******************************************************************
** 函数名:     xy_soc_login_up
** 函数描述:   鉴权
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_login_up(void)
{
    dynamic_debug("xy_soc_login_up");
    return xy_soc_data_pack(T808_MSGID_C_AUTHEN,0,0);
}

/*******************************************************************
** 函数名:     xy_soc_heart_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_heart_up(void)
{
    dynamic_debug("xy_soc_heart_up");
    return xy_soc_data_pack(T808_MSGID_C_HEART,0,0);
}

/*******************************************************************
** 函数名:     xy_soc_ask_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_ask_up(kal_uint16 id,kal_uint16 num,kal_uint8 result)
{
    XY_SOC_ASK_T ask;

    ask.id = id; // 平台下发的指令ID
	ask.num = num; // 平台下发是指令流水号
	ask.result = result; // 结果
	dynamic_debug("xy_soc_ask_up");
    return xy_soc_data_pack(T808_MSGID_C_GENERAL_ACK,0,(kal_uint8*)&ask);
}

/*******************************************************************
** 函数名:	   xy_soc_alm_assem
** 函数描述:   
** 参数:	   
** 返回:	   
********************************************************************/
void xy_soc_alm_assem(XY_ALM_TYPE_E TYPE)
{
    //XY_SOC_SEND_TYPE_E inset_list = XY_SOC_SEND_INSET_QUEUE;
	
    switch(TYPE)
    {
        case XY_ALM_TYPE_NONE:

        break;
    
        case XY_ALM_TYPE_ACC:
        
        break;

        case XY_ALM_TYPE_POW_BREAK: //断电

        break;  
		
		case XY_ALM_TYPE_VIB:

		break;

		case XY_ALM_TYPE_OVER_SPEED:  

		break;
		
		case XY_ALM_TYPE_MOVE:	

		break;
		
        case XY_ALM_TYPE_LOW_BAT:

		break;
		
		case XY_ALM_TYPE_LOW_POW:

        break;                       
        
        default:
            dynamic_debug("xy_soc_alm_assem err type");
            return;
        break;
    }
    dynamic_debug("xy_soc_alm_assem:%d",TYPE);

    xy_soc_data_pack(T808_MSGID_C_POSITION,XY_SOC_SEND_INSET_QUEUE,0);
}


/*******************************************************************
** 函数名:     xy_soc_location_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_location_up(void)
{
    return 1;
}
/*******************************************************************
** 函数名:     xy_soc_data_deal
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_data_deal(kal_uint8*data,kal_uint32 len)
{
    kal_uint16 cmd = 0;
    kal_uint16 serialnum = 0;
    kal_uint8 *info = &data[XY_SOC_PROTOCOL_MSG_HEAD_LEN]; // 消息体
    kal_int8 ask_result;
    XY_INFO_T * xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
    
    if (len < XY_SOC_PROTOCOL_MSG_HEAD_LEN)
    {
        dynamic_debug("xy_soc_data_deal error data len:%d",len);
        return;
    }

    cmd = dynamic_big_endian_pack_short(&data[1]);
    serialnum = dynamic_big_endian_pack_short(&data[XY_SOC_PROTOCOL_MSG_HEAD_LEN-2]);
    
    dynamic_debug("xy_soc_data_deal cmd:0x%04x,serialnum:0x%04x",cmd,serialnum);
    dynamic_log_hex(data,len);  
    
    switch (cmd)
    {        
        case T808_MSGID_S_GENERAL_ACK:		/* 平台的通用应答 */
        {
            kal_uint16 ask_id;
            
            
            ask_id = dynamic_big_endian_pack_short(&info[2]);
            ask_result = info[4];
            dynamic_debug("通用应答 终端消息ID:0x%04x,ask_result:%d",ask_id,ask_result);
            switch(ask_id)
            {
                case T808_MSGID_C_AUTHEN:
                    if (s_wait_login_ask == 1)
                    {
                        s_wait_login_ask = 0;
                        dynamic_debug("鉴权应答");
                        s_login = 1;
                        xy_soc_heart_ask();
                        dynamic_timer_start(enum_timer_soc_task_timer,300,(void*)xy_soc_task,NULL,FALSE);
                    }
                break;

                

                default:
                    dynamic_debug("通用应答");
                    xy_soc_heart_ask();
                    dynamic_timer_stop(enum_timer_track_heart_wait_ask_timer);

                break;
            }
        }
        break;

        case T808_MSGID_S_REG_ACK:		/* 注册应答 */
            ask_result = info[2];
            if (len > (XY_SOC_PROTOCOL_MSG_HEAD_LEN+4) && (len - (XY_SOC_PROTOCOL_MSG_HEAD_LEN+4)) <= T808_AUTH_CODE_LEN)
            {
                memset(t808_para->auth_code,0,sizeof(t808_para->auth_code));
                memcpy(t808_para->auth_code,&info[3],(len - XY_SOC_PROTOCOL_MSG_HEAD_LEN - 4));
            }
            dynamic_debug("终端注册应答:%d,auth_code:%s",ask_result,t808_para->auth_code);
        break;
        
        case T808_MSGID_S_SET_PARAM:			/* 平台设置参数 */
        {
            kal_uint8 para_cnt = 0;
            kal_uint8 i;
            kal_uint8 paralen = 0;
            kal_uint16 para_type;
            kal_uint16 pos = 0;
            kal_uint8 set_server = 0;
            
            para_cnt = info[pos++];
            dynamic_debug("设置终端参数 para_cnt:%d",para_cnt);
            for(i=0;i<para_cnt;i++)
            {
                para_type = dynamic_big_endian_pack_short(&data[pos]);
                pos += 2;
                paralen = data[pos++];
                
                switch (para_type)
                {
                    case 0x0001: // 心跳发送间隔
                    {
                        kal_uint32 heart_time;
                        
                        heart_time = dynamic_big_endian_pack_int(&data[pos]);
                        dynamic_debug("心跳发送间隔:%d",heart_time);
                        xy_info->heart_time = heart_time;
                        xy_info_save();
                    }
                    break;

                    case 0x0013: // 服务器地址
                    {
                        kal_uint8 server[XY_URL_LEN+1];

                        if (paralen >= 7 && paralen <= XY_URL_LEN)
                        {
                            memset(server,0,sizeof(server));
                            memcpy(server,&data[pos],paralen);
                            memset(xy_info->server,0,sizeof(xy_info->server));
                            memcpy(xy_info->server,server,paralen);
                            xy_info_save();
                            set_server = 1;
                        }
                        dynamic_debug("设置服务器:%d,%s",paralen,server);                   
                    }
                    break;

                    case 0x0029: // 终端位置上报间隔
                    {
                        kal_uint32 track_sports;
                        
                        track_sports = dynamic_big_endian_pack_int(&data[pos]);
                        dynamic_debug("位置上报间隔:%d",track_sports);
                        xy_info->freq = track_sports;
                        xy_info_save();
                    }
                    break;

                    case 0x0018: // TCP端口
                    {
                        kal_uint16 port;
                        
                        port = dynamic_big_endian_pack_short(&data[pos]);
                        dynamic_debug("TCP端口:%d",port);
                        xy_info->port = port;
                        xy_info_save();
                        set_server = 1;
                    }
                    break;

                    case 0x0019: // UDP端口
                    {
                        // 暂时不加UDP

                    }
                    break;

                    default:

                    break;
                    
    
                }
                pos += paralen;
            }
            xy_soc_ask_up(cmd,serialnum,0);
            if (set_server == 1)
            {
                dynamic_timer_start(enum_timer_xy_soc_close_timer,5*1000,(void*)xy_soc_close_delay,NULL,FALSE); 
            }
        }
        break;
        
        case T808_MSGID_S_QUERY_PARAM:		/* 平台查询参数 */

        break;
        
        case T808_MSGID_S_CTRL_CMD:		    /* 平台控制指令 */

        break;
        
        case T808_MSGID_S_QUERY_SPE_PARAM:	/* 查询指定参数 */

        break;
        
        case T808_MSGID_S_QUERY_ATTR:		/* 查询终端属性 */

        break;
        
        case T808_MSGID_S_CTRL_VEH:			/* 下发控制车辆 */

        break;
        
        case T808_MSGID_S_QUERY_POSITION:	/* 平台点名操作 */

        break;
        
        case T808_MSGID_S_ALARM_ACK:			/* 确认某个报警 */

        break;
        
        case T808_MSGID_S_SEND_TEXT:			/* 文本信息下发 */

        break;
        
        case T808_MSGID_S_EXPEND:			/* 透传信息 */
        

        default:

        break;
    }

}



