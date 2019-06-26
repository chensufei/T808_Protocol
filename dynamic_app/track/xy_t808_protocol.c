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

/* ͨ��Ӧ�� */
typedef struct _xy_soc_ask_
{
	kal_int16 id; // ƽ̨�·���ָ��ID
	kal_uint16 num; // ƽ̨�·���ָ����ˮ��
	kal_uint8 result; // ���
}XY_SOC_ASK_T;


/* λ����Ϣ */
__packed struct XY_POS_INFO_T
{
	__packed union
	{
		u32	val;						/* ������־ */
		__packed struct
		{
			u32 sos:1;					/* �����������յ�Ӧ����0 */
			u32 overspeed:1;			/* ���ٱ�������־ά�����������������ʵʱ���� */
			u32 restore:2;
			u32 gpsFault:1;				/* GPS���ϣ�ʵʱ���� */
			u32 gpsAnteOpen:1;			/* GPS���߶Ͽ���ʵʱ���� */
			u32 gpsAnteClose:1;			/* GPS���߶�·��ʵʱ���� */
			u32 powerLow:1;				/* ��ԴǷѹ��ʵʱ���� */
			u32 powerOff:1;				/* ��Դ���磬ʵʱ���� */
			u32 restore1:4;
			u32 overspeedTips:1;		/* ����Ԥ����ʵʱ���� */
			u32 restore2:18;
		} bitSt;
	} alarmFlag;

	__packed union
	{
		u32 val;						/* ״̬��Ϣ */
		__packed struct
		{
			u32 acc:1;					/* ACC��0-�أ�1-�� */
			u32 gpsValid:1;				/* 0��δ��λ��1����λ */
			u32 latSta:1;				/* 0����γ��1����γ */
			u32 lonSta:1;				/* 0��������1������ */
			u32 restore:6;
			u32 oil:1;					/* 0��������·������1��������·�Ͽ� */
			u32 power:1;				/* 0��������·������1��������·�Ͽ� */
			u32 doorLock:1;				/* 0�����Ž�����1�����ż��� */
			u32 frontDoor:1;			/* 0���� 1 �أ�1���� 1 ����ǰ�ţ� */
			u32 middleDoor:1;			/* 0���� 2 �أ�1���� 2 �������ţ� */
			u32 backDoor:1;				/* 0���� 3 �أ�1���� 3 �������ţ� */
			u32 restore1:2;
			u32 gpsLoc:1;				/* 0��δʹ�� GPS ���ǽ��ж�λ��1��ʹ�� GPS ���ǽ��ж�λ */
			u32 bdLoc:1;				/* 0��δʹ�ñ������ǽ��ж�λ��1��ʹ�ñ������ǽ��ж�λ */
			u32 restore2:12;
		} bitSt;
	} status;
	u32 lat;							/* γ��,�Զ�Ϊ��λ��γ��ֵ���� 10 �� 6 �η�����ȷ�������֮һ�� */
	u32 lon;							/* ����,�Զ�Ϊ��λ�ľ���ֵ���� 10 �� 6 �η�����ȷ�������֮һ�� */
	u16 high;							/* �߳�,���θ߶ȣ���λΪ�ף�m�� */
	u16 speed;							/* �ٶ�,1/10km/h */
	u16 dir;							/* ����,0-359������Ϊ 0��˳ʱ�� */
	u8 time[6];							/* ʱ����Ϣ��YY-MM-DD-hh-mm-ss��GMT+8 ʱ�䣬����׼��֮���漰��ʱ������ô�ʱ���� */
}; 

/* �ϴ��������� */
typedef struct _xy_param_info
{
	u16 msgid;			/* ��Ϣid */
	u16 serial;			/* ƽ̨�·�����ˮ�� */
	u8 sum;				/* �ܲ��� */
	u32 *paramId;		/* ����ID�б� */
} xy_param_info;

/* �����ն����� */
typedef enum _xy_ctrl_ter_cmd_e
{
	E_CTRL_TER_POWER_OFF = 0,	/* �����ն˹ػ� */
	E_CTRL_TER_RESET,			/* �����ն˸�λ���� */
	E_CTRL_TER_FACTORY,			/* �����ն˻ָ��������� */
	E_CTRL_TER_CLOSE_DATA_LINK,	/* �����ն˹ر�����ͨ�� */
	E_CTRL_TER_CLOSE_WIRE_LINK	/* �����ն˹ر���������ͨ�� */
} xy_ctrl_ter_cmd_e;

/* T808������Ϣ */
typedef struct _xy_t808_data_info
{
	u16 serialNo;						/* ���к� */
	u8 ctrl_cmd_type;					/* ƽ̨�·�����ָ������, 3-�ػ���4-��λ������5-�ָ��������� */
} xy_t808_data_info;


/*******************************************************************
** ȫ�ֱ���
********************************************************************/
/* 808Э�����������Ϣ */
static xy_t808_data_info s_xy_t808_data = { 0, 0};

/**************************************************************************
* ��  ��: int CommCharToDec(char *chr)
* ��  ��: ���ַ�ת����10���Ƶ���ֵ
* ��  ��: 
* ��  ��: char *chr���ַ�ֵ
* ��  ��: void
* ����ֵ: int �� -1 - ʧ��, >= 0 �ɹ�
**************************************************************************/
int CommCharToDec(char chr)
{
	if ((chr >= '0') && (chr <= '9'))
	{
		return (chr - 0x30);
	}

	if ((chr >= 'A') && (chr <= 'F'))
	{
		return ((chr - 'A') + 10);
	}

	if ((chr >= 'a') && (chr <= 'f'))
	{
		return ((chr - 'a') + 10);
	}

	return -1;
}


/**************************************************************************
* ��  ��: int CommHexStrToHex(char *hexStr, int hexlen, u8 *data)
* ��  ��: 16�����ַ���ת����16����������
* ��  ��: 123456 - 0x12 0x34 0x56
* ��  ��: char *hexStr��16�����ַ���
*	      int hexlen��16�����ַ�������
* ��  ��: u8 *data:16��������
* ����ֵ: int �� -1 - ʧ��, > 0 ����
**************************************************************************/
int CommHexStrToHex(char *hexStr, int hexlen, u8 *data)
{
	int ret = -1;
	int i = 0;
	int datalen = 0;

	do
	{
		if ((0 == hexStr) || (0 == hexlen) || (0 == data))
		{
			break;
		}

		if (0 != (hexlen % 2))
		{
			break;
		}

		for (i = 0; i < hexlen;)
		{
			data[datalen++] = (CommCharToDec(hexStr[i]) << 4) + CommCharToDec(hexStr[i + 1]);
			i += 2;
		}

		ret = datalen;
	} while(0);

	return ret;
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
    kal_uint8 veh_id[T808_VEH_ID_LEN + 1];
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
    /*if (veh_len == 0 && t808_para->veh_color == 0)
    {
        // ʹ��VIN
        strcpy((char *)veh_id, "XY123456789012345");
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
    }*/

    memcpy(&dtr[len],t808_para->veh_id,veh_len);
    len += veh_len;
    
    return len;
}

/*******************************************************************
** ����: xy_data_pack_position
** ����: ���λ����Ϣ
** ����:       
** ����: kal_uint16: ���س���      
********************************************************************/
static kal_uint16 xy_data_pack_position(kal_uint8 * dtr)
{
	u16 word = 0;
	u32 dword = 0;
	kal_uint16 len = 0;
	struct XY_POS_INFO_T pos;
	applib_time_struct time;
	nmea_data_t *gps_info = dynamic_gps_get_nmea();

	memset((void *)&pos, 0, sizeof(pos));
	pos.alarmFlag.val = 0;

	if (gps_info->tag_rmc.tag_status)
	pos.status.bitSt.gpsValid = (TRUE != dynamic_gps_is_fix()) ? 0 : 1;

	pos.lat = (u32)(gps_info->tag_rmc.tag_latitude * 1000000);
	pos.lon = (u32)(gps_info->tag_rmc.tag_longitude * 1000000);

	/* 0����γ��1����γ */
	pos.status.bitSt.latSta = ('N' == gps_info->tag_rmc.tag_north_south) ? 0 : 1;

	/* 0��������1������ */
	pos.status.bitSt.lonSta = ('E' == gps_info->tag_rmc.tag_east_west) ? 0 : 1;

	pos.status.bitSt.gpsLoc = 1;
	pos.status.bitSt.bdLoc = 1;

	pos.high = (u16)(gps_info->tag_gga.tag_altitude);
	pos.speed = (u16)(gps_info->tag_rmc.tag_ground_speed * 1.852 * 10);
	pos.dir = (u16)gps_info->tag_rmc.tag_trace_degree;

	memset(&time, 0, sizeof(time));
	dynamic_time_get_systime(&time);

	dynamic_log("time:%d/%d/%d %d:%d:%d", time.nYear, time.nMonth, time.nDay,
		time.nHour, time.nMin, time.nSec);
	
	pos.time[0] = dynamic_dec_to_bcd(time.nYear % 100);
	pos.time[1] = dynamic_dec_to_bcd(time.nMonth % 100);
	pos.time[2] = dynamic_dec_to_bcd(time.nDay % 100);
	pos.time[3] = dynamic_dec_to_bcd(time.nHour % 100);
	pos.time[4] = dynamic_dec_to_bcd(time.nMin % 100);
	pos.time[5] = dynamic_dec_to_bcd(time.nSec % 100);

	M_BIG_ENDIAN_U32(pos.alarmFlag.val, dword);
	pos.alarmFlag.val = dword;

	M_BIG_ENDIAN_U32(pos.status.val, dword);
	pos.status.val = dword;

	M_BIG_ENDIAN_U32(pos.lat, dword);
	pos.lat = dword;

	M_BIG_ENDIAN_U32(pos.lon, dword);
	pos.lon = dword;

	M_BIG_ENDIAN_U16(pos.high, word);
	pos.high = word;

	M_BIG_ENDIAN_U16(pos.speed, word);
	pos.speed = word;

	M_BIG_ENDIAN_U16(pos.dir, word);
	pos.dir = word;

	memcpy((void *)dtr, (void *)&pos, sizeof(pos));
	len += sizeof(pos);

	return len;
}


/*******************************************************************
** ����: xy_data_pack_get_one_param_info
** ����: ���һ��������Ϣ
** ����: kal_uint8 * data : ����������
*        u32 paramId : ����ID
** ����: kal_uint16: ���س���	   
********************************************************************/
static kal_uint16 xy_data_pack_get_one_param_info(kal_uint8 * data, u32 paramId)
{
	int datalen = 0;
	u8 paramlen = 0;
	u32 paramval = 0;
	u16 word = 0;
	u32 dword = 0;
	XY_INFO_T *xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;

	M_BIG_ENDIAN_U32(paramId, dword);
	paramlen = sizeof(u32);
	memcpy(data, (u8 *)&dword, paramlen);
	datalen += paramlen;
		
	switch(paramId)
	{
		case 0x0001:/* ���� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->heart_time;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0002:/* TCPӦ��ʱʱ�� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->tcp_ack_overtime;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0003:/* TCP�ش����� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->tcp_resend_cnt;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0010:/* APN����� */
		{
			paramlen = strlen((char *)xy_info->apn);
			data[datalen++] = paramlen;

			memcpy(&data[datalen], (u8 *)xy_info->apn, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0013:/* IP/���� */
		{
			paramlen = strlen((char *)xy_info->server);
			data[datalen++] = paramlen;

			memcpy(&data[datalen], (u8 *)xy_info->server, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0018:/* ������ TCP �˿� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->port;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0020:/* λ�û㱨����,λ����Ϣ�ϱ���ʽ��0����ʱ�㱨��1������㱨��2����ʱ�Ͷ���㱨 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->report_type;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0021:/* λ�û㱨����, 0������ ACC ״̬�� 1�����ݵ�¼״̬�� ACC ״̬�����жϵ�¼״̬������¼�ٸ��� ACC ״̬ */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->report_way;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0028:/* ��������ʱ�㱨ʱ���� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->sos_freq;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0029:/* ȱʡʱ��㱨��� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->freq;//ֱ��ȡ�ϱ�ʱ��
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x002C:/* ȱʡ����㱨���,��λΪ�ף�m����>0 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->def_distance;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x002F:/* ��������ʱ�㱨������,��λΪ�ף�m����>0 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->sos_distance;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0030:/* �յ㲹���Ƕȣ�<180�� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->degree;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0031:/* ����Χ���뾶���Ƿ�λ����ֵ������λΪ�� */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;

			M_BIG_ENDIAN_U16(t808_para->fencing_radius, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0044:/* �����ն� SMS �ı��������� */
		{
			paramlen = strlen(t808_para->txt_number);
			data[datalen++] = paramlen;
	
			if (paramlen > 0)
			{
				memcpy(&data[datalen], (u8 *)t808_para->txt_number, paramlen);
				datalen += paramlen;
			}
		}
		break;
		case 0x0050:/* ���������֣���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1����Ӧ���������� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->alarm_sw.val;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0051:/* ���������ı� SMS ���أ���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1 ����Ӧ����ʱ�����ı� SMS */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->alarm_sms_sw.val;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0055:/* ����ٶȣ���λΪ����ÿСʱ��km/h�� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->over_speed;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0056:/* ���ٳ���ʱ�䣬��λΪ�루s�� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->speed_keep_time;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x005B:/* ���ٱ���Ԥ����ֵ����λΪ 1/10Km/h */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;
	
			M_BIG_ENDIAN_U16(t808_para->over_speed_pre_val/10, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x005D:/* ��ײ�����������ã�
						b7-b0�� ��ײʱ�䣬��λ 4ms��
						b15-b8����ײ���ٶȣ���λ 0.1g�����÷�Χ�ڣ�0-79 ֮�䣬Ĭ��Ϊ10��
						*/
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;
	
			M_BIG_ENDIAN_U16(t808_para->hit_param_val, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x005E:/* �෭�����������ã��෭�Ƕȣ���λ 1 �ȣ�Ĭ��Ϊ 30 �ȡ� */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;
	
			M_BIG_ENDIAN_U16(t808_para->roll_over_param_val, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0080:/* ������̱���� */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->mileage;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0081:/* �������ڵ�ʡ�� ID */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;

			M_BIG_ENDIAN_U16(t808_para->provice_id, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0082:/* �������ڵ����� ID */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;

			M_BIG_ENDIAN_U16(t808_para->city_id, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0084:/* ������ɫ */
		{
			paramlen = sizeof(u8);
			data[datalen++] = paramlen;

			data[datalen++] = t808_para->veh_color;
		}
		break;
		case 0x0090:/* GNSS ��λģʽ */
		{
			paramlen = sizeof(u8);
			data[datalen++] = paramlen;

			data[datalen++] = ((0 != t808_para->gpsMode) ? 0x01 : 0x00)
								| ((0 != t808_para->gpsMode) ? 0x02 : 0x00);
		}
		break;
		default:
		{
			return 0;
		}
	}

	return datalen;
}

/*******************************************************************
** ����: xy_data_pack_set_one_param_info
** ����: ����һ��������Ϣ
** ����: u32 paramId : ����ID
*		 u8 len : ����
*		 void *data : ֵ
** ����: void
********************************************************************/
static void xy_data_pack_set_one_param_info(u32 paramId, u8 len, void *data)
{
	u16 word = 0;
	u32 dword = 0;
	XY_INFO_T *xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
	kal_uint8 set_server = 0;

	switch(paramId)
	{
		case 0x0001:/* ���� */
		{
			memcpy((void *)&xy_info->heart_time, data, len);
		}
		break;
		case 0x0002:/* TCPӦ��ʱʱ�� */
		{
			memcpy((void *)&xy_info->t808_para.tcp_ack_overtime, data, len);
		}
		break;
		case 0x0003:/* TCP�ش����� */
		{
			memcpy((void *)&xy_info->t808_para.tcp_resend_cnt, data, len);
		}
		break;
		case 0x0010:/* APN����� */
		{
			if (len < sizeof(xy_info->apn))
			{
				memset((void *)xy_info->apn, 0, sizeof(xy_info->apn));
				memcpy((void *)xy_info->apn, data, len);
			}
		}
		break;
		case 0x0013:/* IP/���� */
		{
			if (len < sizeof(xy_info->server))
			{
				memset((void *)xy_info->server, 0, sizeof(xy_info->server));
				memcpy((void *)xy_info->server, data, len);
				set_server = 1;
			}
		}
		break;
		case 0x0018:/* ������ TCP �˿� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->port = (u16)(dword & 0xffff);
			set_server = 1;
		}
		break;
		case 0x0020:/* λ�û㱨����,λ����Ϣ�ϱ���ʽ��0����ʱ�㱨��1������㱨��2����ʱ�Ͷ���㱨 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.report_type = (u8)(dword & 0xff);
		}
		break;
		case 0x0021:/* λ�û㱨����, 0������ ACC ״̬�� 1�����ݵ�¼״̬�� ACC ״̬�����жϵ�¼״̬������¼�ٸ��� ACC ״̬ */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.report_way = (u8)(dword & 0xff);
		}
		break;
		case 0x0028:/* ��������ʱ�㱨ʱ���� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.sos_freq = dword;
		}
		break;
		case 0x0029:/* ȱʡʱ��㱨��� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->freq = dword;
		}
		break;
		case 0x002C:/* ȱʡ����㱨���,��λΪ�ף�m����>0 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.def_distance = dword;
		}
		break;
		case 0x002F:/* ��������ʱ�㱨������,��λΪ�ף�m����>0 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.sos_distance = dword;
		}
		break;
		case 0x0030:/* �յ㲹���Ƕȣ�<180�� */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.degree = word;
		}
		break;
		case 0x0031:/* ����Χ���뾶���Ƿ�λ����ֵ������λΪ�� */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.fencing_radius = word;
		}
		break;
		case 0x0044:/* �����ն� SMS �ı��������� */
		{
			if (len < sizeof(xy_info->t808_para.txt_number))
			{
				memset((void *)xy_info->t808_para.txt_number, 0, sizeof(xy_info->t808_para.txt_number));
				memcpy((void *)xy_info->t808_para.txt_number, data, len);
			}
		}
		break;
		case 0x0050:/* ���������֣���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1����Ӧ���������� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.alarm_sw.val = dword;
		}
		break;
		case 0x0051:/* ���������ı� SMS ���أ���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1 ����Ӧ����ʱ�����ı� SMS */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.alarm_sms_sw.val = dword;
		}
		break;
		case 0x0055:/* ����ٶȣ���λΪ����ÿСʱ��km/h�� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.over_speed = dword;
		}
		break;
		case 0x0056:/* ���ٳ���ʱ�䣬��λΪ�루s�� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.speed_keep_time = dword;
		}
		break;
		case 0x005B:/* ���ٱ���Ԥ����ֵ����λΪ 1/10Km/h */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.over_speed_pre_val = word * 10;
		}
		break;
		case 0x005D:/* ��ײ�����������ã�
						b7-b0�� ��ײʱ�䣬��λ 4ms��
						b15-b8����ײ���ٶȣ���λ 0.1g�����÷�Χ�ڣ�0-79 ֮�䣬Ĭ��Ϊ10��
						*/
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.hit_param_val = dword;
		}
		break;
		case 0x005E:/* �෭�����������ã��෭�Ƕȣ���λ 1 �ȣ�Ĭ��Ϊ 30 �ȡ� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.roll_over_param_val = dword;
		}
		break;
		case 0x0080:/* ������̱���� */
		{
			memcpy((void *)&dword, data, len);
			xy_info->mileage = dword;
		}
		break;
		case 0x0081:/* �������ڵ�ʡ�� ID */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.provice_id = word;
		}
		break;
		case 0x0082:/* �������ڵ����� ID */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.city_id = word;
		}
		break;
		case 0x0084:/* ������ɫ */
		{
			u8 *val = (u8 *)data;
			t808_para->veh_color = *val;
		}
		break;
		case 0x0090:/* GNSS ��λģʽ */
		{
			u8 *val = (u8 *)data;
			t808_para->veh_color = *val;
		}
		break;
		default:
		break;
	}

	if (1 == set_server)
    {
        dynamic_timer_start(enum_timer_xy_soc_close_timer, 5*1000,(void*)xy_soc_close_delay,NULL,FALSE); 
    }
}


/*******************************************************************
** ����: xy_data_pack_all_param_info
** ����: ���������Ϣ
** ����: kal_uint8 * dtr : ����������
** ����: kal_uint16: ���س���	   
********************************************************************/
static kal_uint16 xy_data_pack_all_param_info(kal_uint8 * dtr)
{
	u8 sumIds = 0;
	kal_uint16 ret = 0;
	kal_uint16 len = 0;
	u32 i = 0;
	u32 count = 0;
	u16 paramIdS[] = {  0x0001, 0x0002, 0x0003, 0x0010, 0x0013,
						0x0018, 0x0020, 0x0021, 0x0028, 0x0029,
						0x002C, 0x002F, 0x0030, 0x0031, 0x0044,
						0x0050, 0x0051, 0x0055, 0x0056, 0x005B,
						0x005D, 0x005E, 0x0080, 0x0081, 0x0082,
						0x0084, 0x0090};

	/* ��ѯ���в��� */
	len++;
	count = sizeof(paramIdS)/sizeof(paramIdS[0]);
	for (i = 0; i < count; i++)
	{
		ret = xy_data_pack_get_one_param_info(&dtr[len], paramIdS[i]);
		if (0 != ret)
		{
			len += ret;
			sumIds++;
		}
	}

	dtr[0] = sumIds;

	return len;
}

/*******************************************************************
** ����: xy_data_pack_all_param_info
** ����: ���������Ϣ
** ����: kal_uint8 * dtr : ����������
** ����: kal_uint16: ���س���	   
********************************************************************/
static kal_uint16 xy_data_pack_spec_param_info(kal_uint8 * dtr, u8 sum, u32 *paramId)
{
	kal_uint16 ret = 0;
	kal_uint16 len = 0;
	u8 i = 0;

	/* ��ѯ���в��� */
	dtr[len++] = sum;
	for (i = 0; i < sum; i++)
	{
		ret = xy_data_pack_get_one_param_info(&dtr[len], paramId[i]);
		if (0 != ret)
		{
			len += ret;
		}
	}

	return len;
}

/*******************************************************************
** ����: xy_data_pack_attr_info
** ����: �������Ӧ����Ϣ
** ����: kal_uint8 * dtr : ����������
** ����: kal_uint16: ���س���	   
********************************************************************/
static kal_uint16 xy_data_pack_attr_info(kal_uint8 * dtr)
{
	int ret = 0;
	int datalen = 0;
	APP_CNTX_T *app_cntx = dynamic_app_cntx_get();
	XY_INFO_T *xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
	u16 word = 0;

	/* ��Ӧ�����еĳ����豸 */
	M_BIG_ENDIAN_U16(0x00FF, word);
	memcpy(dtr, (u8 *)&word, sizeof(u16));
	datalen += sizeof(u16);
	
	memcpy(&dtr[datalen], t808_para->product_id, T808_PRODUCT_ID_LEN);
	datalen += T808_PRODUCT_ID_LEN;
	
	memcpy(&dtr[datalen], t808_para->dev_model, T808_DEV_TYPE_LEN);
	datalen += T808_DEV_TYPE_LEN;
	
	memcpy(&dtr[datalen], t808_para->dev_id, T808_DEV_ID_LEN);
	datalen += T808_DEV_ID_LEN;

	ret = CommHexStrToHex((char *)app_cntx->iccid, strlen((char *)app_cntx->iccid), &dtr[datalen]);
	if (-1 == ret)
	{
		return 0;
	}

	datalen += (DYNAMIC_ICCID_MAX_LEN / 2);

	/* Ӳ���汾 */
	dtr[datalen++] = (u8)strlen(M_DYNAMIC_HARDWARE_VER);
	memcpy(&dtr[datalen], (u8 *)M_DYNAMIC_HARDWARE_VER, strlen(M_DYNAMIC_HARDWARE_VER));
	datalen += strlen(M_DYNAMIC_HARDWARE_VER);

	/* Ӳ���汾 */
	dtr[datalen++] = (u8)strlen(M_DYNAMIC_SOFTWARE_VER);
	memcpy(&dtr[datalen], (u8 *)M_DYNAMIC_SOFTWARE_VER, strlen(M_DYNAMIC_SOFTWARE_VER));
	datalen += strlen(M_DYNAMIC_SOFTWARE_VER);

	dtr[datalen++] = ((0 != t808_para->gpsMode) ? 0x01 : 0x00)
					  | ((0 != t808_para->gpsMode) ? 0x02 : 0x00);;
	dtr[datalen++] = 0x01;

	return datalen;
}


/*******************************************************************
** ������:     xy_soc_data_pack
** ��������:   �ϱ��������
** ����:       type:��������  inset_list:0ֱ�ӷ��� 1��������
** ����:       
********************************************************************/
kal_uint8 xy_soc_data_pack(T808_PROTOCOL_MSGID_E cmd,XY_SOC_SEND_TYPE_E inset_list,kal_uint8* str)
{
	kal_uint16 word = 0;
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
    //datastr[curlen++] = XY_SOC_PROTOCOL_HEAD;
    curlen += dynamic_big_endian_transm_short(&datastr[curlen],cmd); // ��ϢID
    lenpoint = curlen;
    datastr[curlen++] = 0X00;// ��Ϣ������ ��Դ������ԴĿǰ�����ǡ����ְ������ܣ����Ⱥ��油��
    datastr[curlen++] = 0;
    
    // �ն��ֻ���
    {
    
        kal_uint8 dev_num[DEV_PHONE_NUM_DEFAULT_LEN + 1] = {0};
        kal_uint8 numlen = 0;

		memset(dev_num, '0', sizeof(dev_num));
        numlen = strlen((char*)xy_info->dev_num);
        if (numlen < 12)
        {
            memcpy(&dev_num[12 - numlen], xy_info->dev_num, numlen);
            
        }
        else
        {
            memcpy(dev_num,xy_info->dev_num, 12);
        }

		numlen = 12;
        curlen += dynamic_ascii2bcd(&datastr[curlen], dev_num, numlen);
    }
    
    curlen += dynamic_big_endian_transm_short(&datastr[curlen], s_xy_t808_data.serialNo++); // ��Ϣ��ˮ��
    headlen = curlen;
    // ��Ϣ��
    switch (cmd)
    {
        case T808_MSGID_C_GENERAL_ACK:		/* �ն�ͨ��Ӧ�� */
        {
            XY_SOC_ASK_T *ask = (XY_SOC_ASK_T *)str;

			dynamic_debug("�ն�ͨ��Ӧ����");

            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->id);
            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->num);
            datastr[curlen++] = ask->result;
        }
        break;
        
        case T808_MSGID_C_HEART:	/* ���� */
        case T808_MSGID_C_DREG:		/* ע�� */
			dynamic_debug("��������");
        break;
        
        case T808_MSGID_C_REG:		/* ע�� */
			dynamic_debug("ע�ᱨ��");
            curlen += xy_data_pack_reg(&datastr[curlen]);
        break;

        case T808_MSGID_C_AUTHEN:	/* ��Ȩ */
        {
			dynamic_debug("��Ȩ����");
			
            if (0 == t808_para->auth_code_len)
            {
                dynamic_debug("��Ȩ��գ�����ע��");
                goto pack_err;
            }
            memcpy(&datastr[curlen],t808_para->auth_code, t808_para->auth_code_len);
            curlen += t808_para->auth_code_len;
        }
        break;

        case T808_MSGID_C_PARAM_ACK:	/* �����㱨 */
		{
			xy_param_info *param = (xy_param_info *)str;

			M_BIG_ENDIAN_U16(param->serial, word);
			memcpy(&datastr[curlen], (u8 *)&word, sizeof(u16));
			curlen += sizeof(u16);

			if (T808_MSGID_S_QUERY_PARAM == param->msgid)
			{
				dynamic_debug("���в�������");
				curlen += xy_data_pack_all_param_info(&datastr[curlen]);
			}
			else if (T808_MSGID_S_QUERY_SPE_PARAM == param->msgid)
			{
				dynamic_debug("ָ����������");
				curlen += xy_data_pack_spec_param_info(&datastr[curlen], param->sum, param->paramId);
			}
		}
        break;

        case T808_MSGID_C_ATTR_ACK:		/* ����Ӧ�� */
		{
			dynamic_debug("����Ӧ����");
			curlen += xy_data_pack_attr_info(&datastr[curlen]);
		}
        break;

        case T808_MSGID_C_POSITION:		/* λ�� */
		{
			dynamic_debug("λ�ñ���");
			curlen += xy_data_pack_position(&datastr[curlen]);
		}
        break;

        case T808_MSGID_C_POSITION_ACK:	/* λ�ò�ѯӦ�� */
		{
			kal_uint16 *recv_serial = (kal_uint16 *)str;

			dynamic_debug("����Ӧ��λ�ñ���");

			M_BIG_ENDIAN_U16(*recv_serial, word);
			memcpy(&datastr[curlen], (u8 *)&word, sizeof(u16));
			curlen += sizeof(u16);

			curlen += xy_data_pack_position(&datastr[curlen]);
		}
        break;

        case T808_MSGID_C_VEH_CTRL_ACK:	/* ��������Ӧ�� */
		{
						
		}
        break;

        case T808_MSGID_C_COMPSATE:		/* ä������ */
		{
						
		}
        break;

        case T808_MSGID_C_EXPEND:		/* ��չ */
		{
						
		}
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

	//datastr[curlen++] = XY_SOC_PROTOCOL_END;
    
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
** ��������:   ��¼��Ȩ
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
** ��������:  ����
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
    dynamic_debug("xy_soc_location_up");
    return xy_soc_data_pack(T808_MSGID_C_POSITION, 1, 0);
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
    kal_uint8 *info = &data[DEV_PHONE_NUM_DEFAULT_LEN + 1]; // ��Ϣ��
    kal_int8 ask_result;
    XY_INFO_T * xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
    
    if (len < DEV_PHONE_NUM_DEFAULT_LEN)
    {
        dynamic_debug("xy_soc_data_deal error data len:%d", len);
        return;
    }

    cmd = dynamic_big_endian_pack_short(&data[0]);
    serialnum = dynamic_big_endian_pack_short(&data[DEV_PHONE_NUM_DEFAULT_LEN-1]);
    
    dynamic_debug("xy_soc_data_deal cmd:0x%04x,serialnum:0x%04x",cmd,serialnum);
    dynamic_log_hex(data,len);  
    
    switch (cmd)
    {        
        case T808_MSGID_S_GENERAL_ACK:		/* ƽ̨��ͨ��Ӧ�� */
        {
            kal_uint16 ask_id = 0;

            ask_id = dynamic_big_endian_pack_short(&info[2]);
            ask_result = info[4];
            dynamic_debug("ͨ��Ӧ�� �ն���ϢID:0x%04x,ask_result:%d",ask_id,ask_result);
            switch(ask_id)
            {
                case T808_MSGID_C_AUTHEN:
                {
					kal_uint8 cnt = xy_soc_get_ack_fail_cnt();

                    dynamic_debug("��ȨӦ��, result:%d", ask_result);

					if (!ask_result)
					{
						xy_soc_set_auth_ok_state(1);
						xy_soc_set_ack_fail_cnt(0);
						dynamic_timer_start(enum_timer_soc_task_timer, 300, (void*)xy_soc_task,NULL,FALSE);
					}
					else
					{
						xy_soc_set_ack_fail_cnt(++cnt);
					}
                }
                break;
				case T808_MSGID_C_HEART:
				{
					dynamic_debug("����Ӧ��, result:%d", ask_result);
					if (!ask_result)
					{
						xy_soc_heart_ask();
						dynamic_timer_stop(enum_timer_track_heart_wait_ask_timer);
					}
				}
				break;
				case T808_MSGID_C_POSITION:
				{
					dynamic_debug("λ��Ӧ��, result:%d", ask_result);
					if (!ask_result)
					{
						//�Ӷ�����ɾ���ѳɹ����͵�����
						xy_soc_clear_data_flag();
						 dynamic_timer_start(enum_timer_soc_task_timer, 100,(void*)xy_soc_task,NULL,FALSE);
					}
				}
				break;
                default:
                    dynamic_debug("ͨ��Ӧ��");
                break;
            }
        }
        break;

        case T808_MSGID_S_REG_ACK:		/* ע��Ӧ�� */
        {
			kal_uint8 cnt = xy_soc_get_ack_fail_cnt();

			ask_result = info[2];
            if (len > (DEV_PHONE_NUM_DEFAULT_LEN + 5) && (len - (DEV_PHONE_NUM_DEFAULT_LEN+5)) <= T808_AUTH_CODE_LEN)
            {
            	if (!ask_result)
            	{
            		xy_soc_set_ack_fail_cnt(0);
                	memset(t808_para->auth_code, 0, sizeof(t808_para->auth_code));
					t808_para->auth_code_len = (len - DEV_PHONE_NUM_DEFAULT_LEN - 5);
                	memcpy(t808_para->auth_code, &info[3], t808_para->auth_code_len);
					xy_soc_set_ack_fail_cnt(0);
					xy_soc_set_reg_ok_state(1);

					dynamic_debug("�ն�ע��Ӧ��:%d,auth_code:%s", ask_result, t808_para->auth_code);
					dynamic_log_hex(t808_para->auth_code, t808_para->auth_code_len);

					dynamic_timer_start(enum_timer_soc_task_timer, 100,(void*)xy_soc_task,NULL,FALSE);
					break;
				}
            }
			xy_soc_set_ack_fail_cnt(++cnt);
		}
        break;
        
        case T808_MSGID_S_SET_PARAM:			/* ƽ̨���ò��� */
		{
			kal_uint8 cnt = 0;
			kal_uint16 pos = 0;
			kal_uint8 i = 0;
			kal_uint32 paramId = 0;
			kal_uint8 paramlen = 0;
			kal_uint16 word = 0;
			kal_uint32 dword = 0;

			cnt = info[pos++];
			for (i = 0; i < cnt; i++)
			{
				paramId = dynamic_big_endian_pack_int(&info[pos]);
				pos += sizeof(u32);
				paramlen =  info[pos];
				pos += sizeof(u8);

				if (sizeof(u16) == paramlen)
				{
					word = dynamic_big_endian_pack_short(&info[pos]);
					xy_data_pack_set_one_param_info(paramId, paramlen, (void *)&word);
				}
				else if (sizeof(u32) == paramlen)
				{
					dword = dynamic_big_endian_pack_short(&info[pos]);
					xy_data_pack_set_one_param_info(paramId, paramlen, (void *)&dword);
				}
				else
				{
					xy_data_pack_set_one_param_info(paramId, paramlen, (void *)&info[pos]);
				}

				pos += paramlen;
			}

			xy_soc_ask_up(cmd, serialnum, 0);
		}
#if 0
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
#endif
        break;
        
        case T808_MSGID_S_QUERY_PARAM:		/* ƽ̨��ѯ���� */
		{
			xy_param_info param = {0};

			memset(&param, 0, sizeof(param));
			param.msgid = T808_MSGID_S_QUERY_PARAM;
			param.serial = serialnum;
			param.sum = 0;
			xy_soc_data_pack(T808_MSGID_S_QUERY_PARAM, 0, (u8 *)&param);
        }
        break;
        
        case T808_MSGID_S_CTRL_CMD:		    /* ƽ̨����ָ�� */
		{
			s_xy_t808_data.ctrl_cmd_type = info[0];

			xy_soc_ask_up(T808_MSGID_S_CTRL_CMD, serialnum, 0);
		}
        break;
        
        case T808_MSGID_S_QUERY_SPE_PARAM:	/* ��ѯָ������ */
		{
			u8 i = 0;
			xy_param_info param = {0};

			memset(&param, 0, sizeof(param));
			param.msgid = T808_MSGID_S_QUERY_SPE_PARAM;
			param.serial = serialnum;
			param.sum = info[0];

			param.paramId = (u32 *)dynamic_malloc(param.sum * sizeof(u32));
			if (!param.paramId)
			{
				break;
			}
			for (i = 0; i < param.sum; i++)
			{
				param.paramId[i] = (u32)info[1 + 4 * i] << 24 | (u32)info[2 + 4 * i] << 16
					                 | (u32)info[3 + 4 * i] << 8 | (u32)info[4 + 4 * i];
			}

			xy_soc_data_pack(T808_MSGID_S_QUERY_SPE_PARAM, 0, (u8 *)&param);

			dynamic_free(param.paramId);
			param.paramId = 0;
        }
        break;
        
        case T808_MSGID_S_QUERY_ATTR:		/* ��ѯ�ն����� */
		{
			xy_soc_data_pack(T808_MSGID_C_ATTR_ACK, 0, 0);
		}
        break;
       
        case T808_MSGID_S_QUERY_POSITION:	/* ƽ̨�������� */
		{
			kal_uint16 serial = 0;

			serial = ((u16)info[0] << 8) | (u16)info[1];
			xy_soc_data_pack(T808_MSGID_C_POSITION_ACK, 0, (kal_uint8 *)&serial);
		}
        break;

		case T808_MSGID_S_TEMP_POS_TRACKING:/* ��ʱλ�ø��ٿ��� */
		{
			t808_para->tracking_cylce = ((u16)info[0] << 8) | (u16)info[1];
			t808_para->tracking_time_sec = ((u32)info[2] << 24) | ((u32)info[3] << 16) | ((u32)info[4] << 8) | (u32)info[5];

			dynamic_log("��ʱλ�ø��ٵ���Ϣ:cycle-%d, sec-%d", t808_para->tracking_cylce,
				t808_para->tracking_time_sec);

			xy_soc_ask_up(T808_MSGID_S_TEMP_POS_TRACKING, serialnum, 0);
		}
		break;
		
        case T808_MSGID_S_ALARM_ACK:		/* ȷ��ĳ������ */
		{
			//�����Ҫ�˹�ȷ�ϵı���
			//kal_uint16 recv_serial = ((u16)info[0] << 8) | (u16)info[1];
			kal_uint32 comfirm_alarm = ((u32)info[2] << 24) | ((u32)info[3] << 16) | ((u32)info[4] << 8) | (u32)info[5];

			if (M_GET_BIT(comfirm_alarm, 0))
			{
				/* ȷ���������� */
			}

			if (M_GET_BIT(comfirm_alarm, 3))
			{
				/* ȷ��Σ��Ԥ�� */
			}

			if (M_GET_BIT(comfirm_alarm, 20))
			{
				/* ȷ�Ͻ������򱨾� */
			}

			if (M_GET_BIT(comfirm_alarm, 21))
			{
				/* ȷ�Ͻ���·�߱��� */
			}

			if (M_GET_BIT(comfirm_alarm, 27))
			{
				/* ȷ�ϳ����Ƿ���𱨾� */
			}

			if (M_GET_BIT(comfirm_alarm, 28))
			{
				/* ȷ�ϳ����Ƿ�λ�Ʊ��� */
			}

			xy_soc_ask_up(T808_MSGID_S_ALARM_ACK, serialnum, 0);
		}
        break;
        
        case T808_MSGID_S_SEND_TEXT:	/* �ı���Ϣ�·� */
		{
			
		}
        break;

		case T808_MSGID_S_CAR_CTRL:		/* �������� */
		{
			if (M_GET_BIT(info[0], 0))
			{
				/* ���ż��� */
			}
			else
			{
				/* ���Ž��� */
			}
		}
		break;

        case T808_MSGID_S_EXPEND:		/* ͸����Ϣ */
        {
			
		}
		break;

        default:
        break;
    }

}



