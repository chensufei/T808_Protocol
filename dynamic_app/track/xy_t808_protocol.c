/********************************************************************************
**
** �ļ���:     xy_t808_protocol.c
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


/* �ն�Ӧ������ */
typedef enum _T808LoginAckTypeE
{
	E_T808_L_ACK_SUCC = 0,					/* �ɹ� */
	E_T808_L_ACK_VEH_LOGINED,				/* �����ѱ�ע�� */
	E_T808_L_ACK_DATABASE_NO_CAR,			/* ���ݿ����޸ó��� */
	E_T808_L_ACK_DEV_LOGINED,				/* �ն��ѱ�ע�� */
	E_T808_L_ACK_DATABASE_NO_TERM			/* ���ݿ����޸��ն� */
} T808LoginAckTypeE;

typedef struct _xy_soc_ask_
{
	kal_int16 id; // ƽ̨�·���ָ��ID
	kal_uint16 num; // ƽ̨�·���ָ����ˮ��
	kal_uint8 result; // ���
}XY_SOC_ASK_T; // ͨ��Ӧ��



static kal_uint8 s_login = 0;
static kal_uint16 s_serial_num = 0;
static kal_uint8 s_wait_login_ask = 0;



/*******************************************************************
** ������:     xy_soc_login_state
** ��������:
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_soc_login_state(void)
{
    return s_login;
}

/*******************************************************************
** ������:     xy_soc_login_set_state
** ��������:
** ����:       
** ����:       
********************************************************************/
void xy_soc_login_set_state(kal_uint8 sign)
{
    s_login = sign;
}

/*******************************************************************
** ������:     xy_soc_login_wait_ask
** ��������:
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_soc_login_wait_ask(void)
{
    return s_wait_login_ask;
}

/*******************************************************************
** ������:     xy_soc_login_set_wait_ask
** ��������:
** ����:       
** ����:       
********************************************************************/
void xy_soc_login_set_wait_ask(kal_uint8 sign)
{
    s_wait_login_ask = sign;
}

/*******************************************************************
** ������:     xy_data_pack_reg
** ��������:   
** ����:       
** ����:       
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
        // ʹ��VIN
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
** ������:     xy_soc_data_pack
** ��������:   �ϱ��������
** ����:       type:��������  inset_list:0ֱ�ӷ��� 1��������
** ����:       
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

    // ��Ϣͷ
    datastr[curlen++] = XY_SOC_PROTOCOL_HEAD;
    curlen += dynamic_big_endian_transm_short(&datastr[curlen],cmd); // ��ϢID
    lenpoint = curlen;
    datastr[curlen++] = 0X40;// ��Ϣ������ ��Դ������ԴĿǰ�����ǡ��ְ������ܣ����Ⱥ��油��
    datastr[curlen++] = 0;
    
    // �ն��ֻ���
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
    
    curlen += dynamic_big_endian_transm_short(&datastr[curlen],s_serial_num++); // ��Ϣ��ˮ��
    headlen = curlen;
    // ��Ϣ��
    switch (cmd)
    {
        case T808_MSGID_C_GENERAL_ACK:		/* �ն�ͨ��Ӧ�� */
        {
            XY_SOC_ASK_T *ask = (XY_SOC_ASK_T *)str;

            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->id);
            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->num);
            datastr[curlen++] = ask->result;
        }
        break;
        
        case T808_MSGID_C_HEART:				/* ���� */

        

        break;
        
        case T808_MSGID_C_DREG:		    /* ע�� */

        

        break;

        
        case T808_MSGID_C_REG:				/* ע�� */
            curlen += xy_data_pack_reg(&datastr[curlen]);
        break;

        
        case T808_MSGID_C_AUTHEN:			/* ��Ȩ */
        {
            kal_uint8 authlen = strlen((char*)t808_para->auth_code);
            if (authlen == 0)
            {
                t808_para->is_regster = 0;
                dynamic_debug("��Ȩ��գ�����ע��");
                goto pack_err;
            }
            memcpy(&datastr[curlen],t808_para->auth_code,authlen);
            curlen += authlen;
        }
        break;

        case T808_MSGID_C_PARAM_ACK:			/* �����㱨 */

        

        break;

        case T808_MSGID_C_ATTR_ACK:			/* ����Ӧ�� */

        

        break;

        case T808_MSGID_C_POSITION:			/* λ�� */

        

        break;

        case T808_MSGID_C_POSITION_ACK:		/* λ�ò�ѯӦ�� */

        

        break;

        case T808_MSGID_C_VEH_CTRL_ACK:		/* ��������Ӧ�� */

        

        break;

        case T808_MSGID_C_COMPSATE:		/* ä������ */

        

        break;

        case T808_MSGID_C_EXPEND:			/* ��չ */

        

        break;
       
        break;
        
        default:

        break;
    }
    
    // ��Ϣ�峤��
    {
        kal_uint8 len_buf[2];
        dynamic_big_endian_transm_short(len_buf,(curlen-headlen)); 
        datastr[lenpoint] |= len_buf[0];
        datastr[lenpoint+1] |= len_buf[1];
    }

    // У����
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
** ������:     xy_soc_regster_up
** ��������:  
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_soc_regster_up(void)
{
    dynamic_debug("xy_soc_regster_up");
    return xy_soc_data_pack(T808_MSGID_C_REG,0,0);
}

/*******************************************************************
** ������:     xy_soc_login_up
** ��������:   ��Ȩ
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_soc_login_up(void)
{
    dynamic_debug("xy_soc_login_up");
    return xy_soc_data_pack(T808_MSGID_C_AUTHEN,0,0);
}

/*******************************************************************
** ������:     xy_soc_heart_up
** ��������:  
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_soc_heart_up(void)
{
    dynamic_debug("xy_soc_heart_up");
    return xy_soc_data_pack(T808_MSGID_C_HEART,0,0);
}

/*******************************************************************
** ������:     xy_soc_ask_up
** ��������:  
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_soc_ask_up(kal_uint16 id,kal_uint16 num,kal_uint8 result)
{
    XY_SOC_ASK_T ask;

    ask.id = id; // ƽ̨�·���ָ��ID
	ask.num = num; // ƽ̨�·���ָ����ˮ��
	ask.result = result; // ���
	dynamic_debug("xy_soc_ask_up");
    return xy_soc_data_pack(T808_MSGID_C_GENERAL_ACK,0,(kal_uint8*)&ask);
}

/*******************************************************************
** ������:	   xy_soc_alm_assem
** ��������:   
** ����:	   
** ����:	   
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

        case XY_ALM_TYPE_POW_BREAK: //�ϵ�

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
** ������:     xy_soc_location_up
** ��������:  
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_soc_location_up(void)
{
    return 1;
}
/*******************************************************************
** ������:     xy_soc_data_deal
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_soc_data_deal(kal_uint8*data,kal_uint32 len)
{
    kal_uint16 cmd = 0;
    kal_uint16 serialnum = 0;
    kal_uint8 *info = &data[XY_SOC_PROTOCOL_MSG_HEAD_LEN]; // ��Ϣ��
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
        case T808_MSGID_S_GENERAL_ACK:		/* ƽ̨��ͨ��Ӧ�� */
        {
            kal_uint16 ask_id;
            
            
            ask_id = dynamic_big_endian_pack_short(&info[2]);
            ask_result = info[4];
            dynamic_debug("ͨ��Ӧ�� �ն���ϢID:0x%04x,ask_result:%d",ask_id,ask_result);
            switch(ask_id)
            {
                case T808_MSGID_C_AUTHEN:
                    if (s_wait_login_ask == 1)
                    {
                        s_wait_login_ask = 0;
                        dynamic_debug("��ȨӦ��");
                        s_login = 1;
                        xy_soc_heart_ask();
                        dynamic_timer_start(enum_timer_soc_task_timer,300,(void*)xy_soc_task,NULL,FALSE);
                    }
                break;

                

                default:
                    dynamic_debug("ͨ��Ӧ��");
                    xy_soc_heart_ask();
                    dynamic_timer_stop(enum_timer_track_heart_wait_ask_timer);

                break;
            }
        }
        break;

        case T808_MSGID_S_REG_ACK:		/* ע��Ӧ�� */
            ask_result = info[2];
            if (len > (XY_SOC_PROTOCOL_MSG_HEAD_LEN+4) && (len - (XY_SOC_PROTOCOL_MSG_HEAD_LEN+4)) <= T808_AUTH_CODE_LEN)
            {
                memset(t808_para->auth_code,0,sizeof(t808_para->auth_code));
                memcpy(t808_para->auth_code,&info[3],(len - XY_SOC_PROTOCOL_MSG_HEAD_LEN - 4));
            }
            dynamic_debug("�ն�ע��Ӧ��:%d,auth_code:%s",ask_result,t808_para->auth_code);
        break;
        
        case T808_MSGID_S_SET_PARAM:			/* ƽ̨���ò��� */
        {
            kal_uint8 para_cnt = 0;
            kal_uint8 i;
            kal_uint8 paralen = 0;
            kal_uint16 para_type;
            kal_uint16 pos = 0;
            kal_uint8 set_server = 0;
            
            para_cnt = info[pos++];
            dynamic_debug("�����ն˲��� para_cnt:%d",para_cnt);
            for(i=0;i<para_cnt;i++)
            {
                para_type = dynamic_big_endian_pack_short(&data[pos]);
                pos += 2;
                paralen = data[pos++];
                
                switch (para_type)
                {
                    case 0x0001: // �������ͼ��
                    {
                        kal_uint32 heart_time;
                        
                        heart_time = dynamic_big_endian_pack_int(&data[pos]);
                        dynamic_debug("�������ͼ��:%d",heart_time);
                        xy_info->heart_time = heart_time;
                        xy_info_save();
                    }
                    break;

                    case 0x0013: // ��������ַ
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
                        dynamic_debug("���÷�����:%d,%s",paralen,server);                   
                    }
                    break;

                    case 0x0029: // �ն�λ���ϱ����
                    {
                        kal_uint32 track_sports;
                        
                        track_sports = dynamic_big_endian_pack_int(&data[pos]);
                        dynamic_debug("λ���ϱ����:%d",track_sports);
                        xy_info->freq = track_sports;
                        xy_info_save();
                    }
                    break;

                    case 0x0018: // TCP�˿�
                    {
                        kal_uint16 port;
                        
                        port = dynamic_big_endian_pack_short(&data[pos]);
                        dynamic_debug("TCP�˿�:%d",port);
                        xy_info->port = port;
                        xy_info_save();
                        set_server = 1;
                    }
                    break;

                    case 0x0019: // UDP�˿�
                    {
                        // ��ʱ����UDP

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
        
        case T808_MSGID_S_QUERY_PARAM:		/* ƽ̨��ѯ���� */

        break;
        
        case T808_MSGID_S_CTRL_CMD:		    /* ƽ̨����ָ�� */

        break;
        
        case T808_MSGID_S_QUERY_SPE_PARAM:	/* ��ѯָ������ */

        break;
        
        case T808_MSGID_S_QUERY_ATTR:		/* ��ѯ�ն����� */

        break;
        
        case T808_MSGID_S_CTRL_VEH:			/* �·����Ƴ��� */

        break;
        
        case T808_MSGID_S_QUERY_POSITION:	/* ƽ̨�������� */

        break;
        
        case T808_MSGID_S_ALARM_ACK:			/* ȷ��ĳ������ */

        break;
        
        case T808_MSGID_S_SEND_TEXT:			/* �ı���Ϣ�·� */

        break;
        
        case T808_MSGID_S_EXPEND:			/* ͸����Ϣ */
        

        default:

        break;
    }

}



