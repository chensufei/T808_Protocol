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


#define T808_PROTOCOL_MSG_HEAD_LEN 12

/* 终端应答类型 */
typedef enum _T808LoginAckTypeE
{
	E_T808_L_ACK_SUCC = 0,					/* 成功 */
	E_T808_L_ACK_VEH_LOGINED,				/* 车辆已被注册 */
	E_T808_L_ACK_DATABASE_NO_CAR,			/* 数据库中无该车辆 */
	E_T808_L_ACK_DEV_LOGINED,				/* 终端已被注册 */
	E_T808_L_ACK_DATABASE_NO_TERM			/* 数据库中无该终端 */
} T808LoginAckTypeE;

/* 通用应答 */
typedef struct _xy_soc_ask_
{
	kal_int16 id; // 平台下发的指令ID
	kal_uint16 num; // 平台下发是指令流水号
	kal_uint8 result; // 结果
}XY_SOC_ASK_T;


/* 位置信息 */
__packed struct XY_POS_INFO_T
{
	__packed union
	{
		u32	val;						/* 报警标志 */
		__packed struct
		{
			u32 sos:1;					/* 紧急报警，收到应答清0 */
			u32 overspeed:1;			/* 超速报警，标志维持至报警条件解除，实时报警 */
			u32 restore:2;
			u32 gpsFault:1;				/* GPS故障，实时报警 */
			u32 gpsAnteOpen:1;			/* GPS天线断开，实时报警 */
			u32 gpsAnteClose:1;			/* GPS天线短路，实时报警 */
			u32 powerLow:1;				/* 电源欠压，实时报警 */
			u32 powerOff:1;				/* 电源掉电，实时报警 */
			u32 restore1:4;
			u32 overspeedTips:1;		/* 超速预警，实时报警 */
			u32 restore2:18;
		} bitSt;
	} alarmFlag;

	__packed union
	{
		u32 val;						/* 状态信息 */
		__packed struct
		{
			u32 acc:1;					/* ACC，0-关，1-开 */
			u32 gpsValid:1;				/* 0：未定位；1：定位 */
			u32 latSta:1;				/* 0：北纬；1：南纬 */
			u32 lonSta:1;				/* 0：东经；1：西经 */
			u32 restore:6;
			u32 oil:1;					/* 0：车辆油路正常；1：车辆油路断开 */
			u32 power:1;				/* 0：车辆电路正常；1：车辆电路断开 */
			u32 doorLock:1;				/* 0：车门解锁；1：车门加锁 */
			u32 frontDoor:1;			/* 0：门 1 关；1：门 1 开（前门） */
			u32 middleDoor:1;			/* 0：门 2 关；1：门 2 开（中门） */
			u32 backDoor:1;				/* 0：门 3 关；1：门 3 开（后门） */
			u32 restore1:2;
			u32 gpsLoc:1;				/* 0：未使用 GPS 卫星进行定位；1：使用 GPS 卫星进行定位 */
			u32 bdLoc:1;				/* 0：未使用北斗卫星进行定位；1：使用北斗卫星进行定位 */
			u32 restore2:12;
		} bitSt;
	} status;
	u32 lat;							/* 纬度,以度为单位的纬度值乘以 10 的 6 次方，精确到百万分之一度 */
	u32 lon;							/* 经度,以度为单位的经度值乘以 10 的 6 次方，精确到百万分之一度 */
	u16 high;							/* 高程,海拔高度，单位为米（m） */
	u16 speed;							/* 速度,1/10km/h */
	u16 dir;							/* 方向,0-359，正北为 0，顺时针 */
	u8 time[6];							/* 时间信息，YY-MM-DD-hh-mm-ss（GMT+8 时间，本标准中之后涉及的时间均采用此时区） */
}; 

/* 上传参数属性 */
typedef struct _xy_param_info
{
	u16 msgid;			/* 消息id */
	u16 serial;			/* 平台下发的流水号 */
	u8 sum;				/* 总参数 */
	u32 *paramId;		/* 参数ID列表 */
} xy_param_info;

/* 控制终端命令 */
typedef enum _xy_ctrl_ter_cmd_e
{
	E_CTRL_TER_POWER_OFF = 0,	/* 控制终端关机 */
	E_CTRL_TER_RESET,			/* 控制终端复位重启 */
	E_CTRL_TER_FACTORY,			/* 控制终端恢复出厂设置 */
	E_CTRL_TER_CLOSE_DATA_LINK,	/* 控制终端关闭数据通信 */
	E_CTRL_TER_CLOSE_WIRE_LINK	/* 控制终端关闭所有无线通信 */
} xy_ctrl_ter_cmd_e;

/* T808数据信息 */
typedef struct _xy_t808_data_info
{
	u16 serialNo;						/* 序列号 */
	u8 ctrl_cmd_type;					/* 平台下发控制指令类型, 3-关机，4-复位重启，5-恢复出厂设置 */
} xy_t808_data_info;

/*******************************************************************
** 全局变量
********************************************************************/
/* 808协议相关数据信息 */
static xy_t808_data_info s_xy_t808_data = { 0, 0};

/**************************************************************************
* 函  数: int CommCharToDec(char *chr)
* 描  述: 将字符转换成10进制的数值
* 举  例: 
* 入  参: char *chr：字符值
* 出  参: void
* 返回值: int ： -1 - 失败, >= 0 成功
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
* 函  数: int CommHexStrToHex(char *hexStr, int hexlen, u8 *data)
* 描  述: 16进制字符串转换成16进制数据流
* 举  例: 123456 - 0x12 0x34 0x56
* 入  参: char *hexStr：16进制字符串
*	      int hexlen：16进制字符串长度
* 出  参: u8 *data:16进制数据
* 返回值: int ： -1 - 失败, > 0 长度
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
        // 使用VIN
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
** 函数: xy_data_pack_position
** 描述: 组包位置信息
** 参数:       
** 返回: kal_uint16: 返回长度      
********************************************************************/
static kal_uint16 xy_data_pack_position(kal_uint8 * dtr)
{
	u16 word = 0;
	u32 dword = 0;
	kal_uint16 len = 0;
	struct XY_POS_INFO_T pos;
	applib_time_struct time;
	XY_INFO_T * xy_info = xy_get_info();
	DYNAMIC_SYS_T *sys = dynamic_sys_get_info();
	nmea_data_t *gps_info = dynamic_gps_get_nmea();

	memset((void *)&pos, 0, sizeof(pos));
	pos.alarmFlag.val = 0;

	pos.status.bitSt.acc = (0 != xy_info->acc_state) ? 1 : 0;

	if (dynamic_gps_is_fix())
	{
		pos.status.bitSt.gpsValid = 1;

		pos.lat = (u32)(gps_info->tag_rmc.tag_latitude * 1000000);
		pos.lon = (u32)(gps_info->tag_rmc.tag_longitude * 1000000);

		/* 0：北纬；1：南纬 */
		pos.status.bitSt.latSta = ('N' == gps_info->tag_rmc.tag_north_south) ? 0 : 1;

		/* 0：东经；1：西经 */
		pos.status.bitSt.lonSta = ('E' == gps_info->tag_rmc.tag_east_west) ? 0 : 1;

		pos.high = (u16)(gps_info->tag_gga.tag_altitude);
		pos.speed = (u16)(gps_info->tag_rmc.tag_ground_speed * 1.852 * 10);
		pos.dir = (u16)gps_info->tag_rmc.tag_trace_degree;
	}
	else
	{
		pos.status.bitSt.gpsValid = 0;
		pos.lat = (u32)(sys->end_gps.lat * 1000000);
		pos.lon = (u32)(sys->end_gps.lng * 1000000);

		/* 0：北纬；1：南纬 */
		pos.status.bitSt.latSta = ('N' == sys->end_gps.d_lat) ? 0 : 1;

		/* 0：东经；1：西经 */
		pos.status.bitSt.lonSta = ('E' == sys->end_gps.d_lng) ? 0 : 1;
	}

	pos.status.bitSt.gpsLoc = 1;
	pos.status.bitSt.bdLoc = 1;

	memset(&time, 0, sizeof(time));
	dynamic_time_get_local_time(&time);

	dynamic_debug("time:%d/%d/%d %d:%d:%d", time.nYear, time.nMonth, time.nDay,
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
** 函数: xy_data_pack_get_one_param_info
** 描述: 组包一个参数信息
** 参数: kal_uint8 * data : 输出组包内容
*        u32 paramId : 参数ID
** 返回: kal_uint16: 返回长度	   
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
		case 0x0001:/* 心跳 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->heart_time;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0002:/* TCP应答超时时间 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->tcp_ack_overtime;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0003:/* TCP重传次数 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->tcp_resend_cnt;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0010:/* APN接入点 */
		{
			paramlen = strlen((char *)xy_info->apn);
			data[datalen++] = paramlen;

			memcpy(&data[datalen], (u8 *)xy_info->apn, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0013:/* IP/域名 */
		{
			paramlen = strlen((char *)xy_info->server);
			data[datalen++] = paramlen;

			memcpy(&data[datalen], (u8 *)xy_info->server, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0018:/* 服务器 TCP 端口 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->port;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0020:/* 位置汇报策略,位置信息上报方式，0：定时汇报；1：定距汇报；2：定时和定距汇报 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->report_type;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0021:/* 位置汇报方案, 0：根据 ACC 状态； 1：根据登录状态和 ACC 状态，先判断登录状态，若登录再根据 ACC 状态 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->report_way;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0028:/* 紧急报警时汇报时间间隔 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->sos_freq;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0029:/* 缺省时间汇报间隔 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->freq;//直接取上报时间
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x002C:/* 缺省距离汇报间隔,单位为米（m），>0 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->def_distance;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x002F:/* 紧急报警时汇报距离间隔,单位为米（m），>0 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->sos_distance;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0030:/* 拐点补传角度，<180° */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = t808_para->degree;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0031:/* 电子围栏半径（非法位移阈值），单位为米 */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;

			M_BIG_ENDIAN_U16(t808_para->fencing_radius, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0044:/* 接收终端 SMS 文本报警号码 */
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
		case 0x0050:/* 报警屏蔽字，与位置信息汇报消息中的报警标志相对应，相应位为 1则相应报警被屏蔽 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->alarm_sw.val;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0051:/* 报警发送文本 SMS 开关，与位置信息汇报消息中的报警标志相对应，相应位为 1 则相应报警时发送文本 SMS */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->alarm_sms_sw.val;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0055:/* 最高速度，单位为公里每小时（km/h） */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->over_speed;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0056:/* 超速持续时间，单位为秒（s） */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;
	
			paramval = t808_para->speed_keep_time;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x005B:/* 超速报警预警差值，单位为 1/10Km/h */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;
	
			M_BIG_ENDIAN_U16(t808_para->over_speed_pre_val/10, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x005D:/* 碰撞报警参数设置：
						b7-b0： 碰撞时间，单位 4ms；
						b15-b8：碰撞加速度，单位 0.1g，设置范围在：0-79 之间，默认为10。
						*/
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;
	
			M_BIG_ENDIAN_U16(t808_para->hit_param_val, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x005E:/* 侧翻报警参数设置：侧翻角度，单位 1 度，默认为 30 度。 */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;
	
			M_BIG_ENDIAN_U16(t808_para->roll_over_param_val, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0080:/* 车辆里程表读数 */
		{
			paramlen = sizeof(u32);
			data[datalen++] = paramlen;

			paramval = xy_info->mileage;
			M_BIG_ENDIAN_U32(paramval, dword);
			memcpy(&data[datalen], (u8 *)&dword, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0081:/* 车辆所在的省域 ID */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;

			M_BIG_ENDIAN_U16(t808_para->provice_id, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0082:/* 车辆所在的市域 ID */
		{
			paramlen = sizeof(u16);
			data[datalen++] = paramlen;

			M_BIG_ENDIAN_U16(t808_para->city_id, word);
			memcpy(&data[datalen], (u8 *)&word, paramlen);
			datalen += paramlen;
		}
		break;
		case 0x0084:/* 车牌颜色 */
		{
			paramlen = sizeof(u8);
			data[datalen++] = paramlen;

			data[datalen++] = t808_para->veh_color;
		}
		break;
		case 0x0090:/* GNSS 定位模式 */
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
** 函数: xy_data_pack_set_one_param_info
** 描述: 设置一个参数信息
** 参数: u32 paramId : 参数ID
*		 u8 len : 长度
*		 void *data : 值
** 返回: void
********************************************************************/
static void xy_data_pack_set_one_param_info(u32 paramId, u8 len, void *data)
{
	u16 word = 0;
	u32 dword = 0;
	XY_INFO_T *xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
	kal_uint8 set_server = 0;

	dynamic_debug("xy_data_pack_set_one_param_info, param ID:%x, len:%d\r\n", paramId, len);
	dynamic_log_hex((kal_uint8 *)data, len); 

	switch(paramId)
	{
		case 0x0001:/* 心跳 */
		{
			memcpy((void *)&xy_info->heart_time, data, len);
			dynamic_debug("heart time:%d\r\n", xy_info->heart_time);
		}
		break;
		case 0x0002:/* TCP应答超时时间 */
		{
			memcpy((void *)&xy_info->t808_para.tcp_ack_overtime, data, len);
		}
		break;
		case 0x0003:/* TCP重传次数 */
		{
			memcpy((void *)&xy_info->t808_para.tcp_resend_cnt, data, len);
		}
		break;
		case 0x0010:/* APN接入点 */
		{
			if (len < sizeof(xy_info->apn))
			{
				memset((void *)xy_info->apn, 0, sizeof(xy_info->apn));
				memcpy((void *)xy_info->apn, data, len);
			}
		}
		break;
		case 0x0013:/* IP/域名 */
		{
			if (len < sizeof(xy_info->server))
			{
				memset((void *)xy_info->server, 0, sizeof(xy_info->server));
				memcpy((void *)xy_info->server, data, len);
				set_server = 1;
			}
		}
		break;
		case 0x0018:/* 服务器 TCP 端口 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->port = (u16)(dword & 0xffff);
			set_server = 1;
		}
		break;
		case 0x0020:/* 位置汇报策略,位置信息上报方式，0：定时汇报；1：定距汇报；2：定时和定距汇报 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.report_type = (u8)(dword & 0xff);
		}
		break;
		case 0x0021:/* 位置汇报方案, 0：根据 ACC 状态； 1：根据登录状态和 ACC 状态，先判断登录状态，若登录再根据 ACC 状态 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.report_way = (u8)(dword & 0xff);
		}
		break;
		case 0x0027:/* 休眠时汇报时间间隔 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.sleep_freq = dword;

			dynamic_debug("sleep_freq:%d\r\n", xy_info->t808_para.sleep_freq);
		}
		break;
		case 0x0028:/* 紧急报警时汇报时间间隔 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.sos_freq = dword;

			dynamic_debug("sos_freq:%d\r\n", xy_info->t808_para.sos_freq);
		}
		break;
		case 0x0029:/* 缺省时间汇报间隔 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->freq = dword;

			dynamic_debug("default_freq:%d\r\n", xy_info->freq);
		}
		break;
		case 0x002C:/* 缺省距离汇报间隔,单位为米（m），>0 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.def_distance = dword;
		}
		break;
		case 0x002F:/* 紧急报警时汇报距离间隔,单位为米（m），>0 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.sos_distance = dword;
		}
		break;
		case 0x0030:/* 拐点补传角度，<180° */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.degree = word;
		}
		break;
		case 0x0031:/* 电子围栏半径（非法位移阈值），单位为米 */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.fencing_radius = word;
		}
		break;
		case 0x0044:/* 接收终端 SMS 文本报警号码 */
		{
			if (len < sizeof(xy_info->t808_para.txt_number))
			{
				memset((void *)xy_info->t808_para.txt_number, 0, sizeof(xy_info->t808_para.txt_number));
				memcpy((void *)xy_info->t808_para.txt_number, data, len);
			}
		}
		break;
		case 0x0050:/* 报警屏蔽字，与位置信息汇报消息中的报警标志相对应，相应位为 1则相应报警被屏蔽 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.alarm_sw.val = dword;
		}
		break;
		case 0x0051:/* 报警发送文本 SMS 开关，与位置信息汇报消息中的报警标志相对应，相应位为 1 则相应报警时发送文本 SMS */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.alarm_sms_sw.val = dword;
		}
		break;
		case 0x0055:/* 最高速度，单位为公里每小时（km/h） */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.over_speed = dword;
		}
		break;
		case 0x0056:/* 超速持续时间，单位为秒（s） */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.speed_keep_time = dword;
		}
		break;
		case 0x005B:/* 超速报警预警差值，单位为 1/10Km/h */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.over_speed_pre_val = word * 10;
		}
		break;
		case 0x005D:/* 碰撞报警参数设置：
						b7-b0： 碰撞时间，单位 4ms；
						b15-b8：碰撞加速度，单位 0.1g，设置范围在：0-79 之间，默认为10。
						*/
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.hit_param_val = dword;
		}
		break;
		case 0x005E:/* 侧翻报警参数设置：侧翻角度，单位 1 度，默认为 30 度。 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.roll_over_param_val = dword;
		}
		break;
		case 0x0080:/* 车辆里程表读数 */
		{
			memcpy((void *)&dword, data, len);
			xy_info->mileage = dword;
		}
		break;
		case 0x0081:/* 车辆所在的省域 ID */
		{
			memcpy((void *)&word, data, len);
			xy_info->t808_para.provice_id = word;
		}
		break;
		case 0x0082:/* 车辆所在的市域 ID */
		{
			memcpy((void *)&dword, data, len);
			xy_info->t808_para.city_id = word;
		}
		break;
		case 0x0084:/* 车牌颜色 */
		{
			u8 *val = (u8 *)data;
			t808_para->veh_color = *val;
		}
		break;
		case 0x0090:/* GNSS 定位模式 */
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
** 函数: xy_data_pack_all_param_info
** 描述: 组包参数信息
** 参数: kal_uint8 * dtr : 输出组包内容
** 返回: kal_uint16: 返回长度	   
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

	/* 查询所有参数 */
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
** 函数: xy_data_pack_all_param_info
** 描述: 组包参数信息
** 参数: kal_uint8 * dtr : 输出组包内容
** 返回: kal_uint16: 返回长度	   
********************************************************************/
static kal_uint16 xy_data_pack_spec_param_info(kal_uint8 * dtr, u8 sum, u32 *paramId)
{
	kal_uint16 ret = 0;
	kal_uint16 len = 0;
	u8 i = 0;

	/* 查询所有参数 */
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
** 函数: xy_data_pack_attr_info
** 描述: 组包属性应答信息
** 参数: kal_uint8 * dtr : 输出组包内容
** 返回: kal_uint16: 返回长度	   
********************************************************************/
static kal_uint16 xy_data_pack_attr_info(kal_uint8 * dtr)
{
	int ret = 0;
	int datalen = 0;
	APP_CNTX_T *app_cntx = dynamic_app_cntx_get();
	XY_INFO_T *xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
	u16 word = 0;

	/* 适应于所有的车和设备 */
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

	/* 硬件版本 */
	dtr[datalen++] = (u8)strlen(M_DYNAMIC_HARDWARE_VER);
	memcpy(&dtr[datalen], (u8 *)M_DYNAMIC_HARDWARE_VER, strlen(M_DYNAMIC_HARDWARE_VER));
	datalen += strlen(M_DYNAMIC_HARDWARE_VER);

	/* 硬件版本 */
	dtr[datalen++] = (u8)strlen(M_DYNAMIC_SOFTWARE_VER);
	memcpy(&dtr[datalen], (u8 *)M_DYNAMIC_SOFTWARE_VER, strlen(M_DYNAMIC_SOFTWARE_VER));
	datalen += strlen(M_DYNAMIC_SOFTWARE_VER);

	dtr[datalen++] = ((0 != t808_para->gpsMode) ? 0x01 : 0x00)
					  | ((0 != t808_para->gpsMode) ? 0x02 : 0x00);;
	dtr[datalen++] = 0x01;

	return datalen;
}


/*******************************************************************
** 函数名:     xy_soc_data_pack
** 函数描述:   上报数据组包
** 参数:       type:数据类型  inset_list:0直接发送 1插入链表
** 返回:       
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

    // 消息头
    //datastr[curlen++] = XY_SOC_PROTOCOL_HEAD;
    curlen += dynamic_big_endian_transm_short(&datastr[curlen],cmd); // 消息ID
    lenpoint = curlen;
    datastr[curlen++] = 0X00;// 消息体属性 电源类型有源目前不考虑、不分包、加密，长度后面补充
    datastr[curlen++] = 0;
    
    // 终端手机号
    {
    
        kal_uint8 dev_num[MAX_PHONE_NUM_LEN + 1] = {0};
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
    
    curlen += dynamic_big_endian_transm_short(&datastr[curlen], s_xy_t808_data.serialNo++); // 消息流水号
    headlen = curlen;
    // 消息体
    switch (cmd)
    {
        case T808_MSGID_C_GENERAL_ACK:		/* 终端通用应答 */
        {
            XY_SOC_ASK_T *ask = (XY_SOC_ASK_T *)str;

			dynamic_debug("终端通用应答报文");

            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->id);
            curlen += dynamic_big_endian_transm_short(&datastr[curlen],ask->num);
            datastr[curlen++] = ask->result;
        }
        break;
        
        case T808_MSGID_C_HEART:	/* 心跳 */
        case T808_MSGID_C_DREG:		/* 注销 */
			dynamic_debug("心跳报文");
        break;
        
        case T808_MSGID_C_REG:		/* 注册 */
			dynamic_debug("注册报文");
            curlen += xy_data_pack_reg(&datastr[curlen]);
        break;

        case T808_MSGID_C_AUTHEN:	/* 鉴权 */
        {
			dynamic_debug("鉴权报文");
			
            if (0 == t808_para->auth_code_len)
            {
                dynamic_debug("鉴权码空，重新注册");
                goto pack_err;
            }
            memcpy(&datastr[curlen],t808_para->auth_code, t808_para->auth_code_len);
            curlen += t808_para->auth_code_len;
        }
        break;

        case T808_MSGID_C_PARAM_ACK:	/* 参数汇报 */
		{
			xy_param_info *param = (xy_param_info *)str;

			M_BIG_ENDIAN_U16(param->serial, word);
			memcpy(&datastr[curlen], (u8 *)&word, sizeof(u16));
			curlen += sizeof(u16);

			if (T808_MSGID_S_QUERY_PARAM == param->msgid)
			{
				dynamic_debug("所有参数报文");
				curlen += xy_data_pack_all_param_info(&datastr[curlen]);
			}
			else if (T808_MSGID_S_QUERY_SPE_PARAM == param->msgid)
			{
				dynamic_debug("指定参数报文");
				curlen += xy_data_pack_spec_param_info(&datastr[curlen], param->sum, param->paramId);
			}
		}
        break;

        case T808_MSGID_C_ATTR_ACK:		/* 属性应答 */
		{
			dynamic_debug("属性应答报文");
			curlen += xy_data_pack_attr_info(&datastr[curlen]);
		}
        break;

        case T808_MSGID_C_POSITION:		/* 位置 */
		{
			dynamic_debug("位置报文");
			curlen += xy_data_pack_position(&datastr[curlen]);
		}
        break;

        case T808_MSGID_C_POSITION_ACK:	/* 位置查询应答 */
		{
			kal_uint16 *recv_serial = (kal_uint16 *)str;

			dynamic_debug("点名应答位置报文");

			M_BIG_ENDIAN_U16(*recv_serial, word);
			memcpy(&datastr[curlen], (u8 *)&word, sizeof(u16));
			curlen += sizeof(u16);

			curlen += xy_data_pack_position(&datastr[curlen]);
		}
        break;

        case T808_MSGID_C_VEH_CTRL_ACK:	/* 车辆控制应答 */
		{
						
		}
        break;

        case T808_MSGID_C_COMPSATE:		/* 盲区补偿 */
		{
						
		}
        break;

        case T808_MSGID_C_EXPEND:		/* 拓展 */
		{
						
		}
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

	//datastr[curlen++] = XY_SOC_PROTOCOL_END;
    
    dynamic_debug("xy_soc_data_pack len:%d",curlen);
    dynamic_log_hex(datastr,curlen);
    if (inset_list == 1)
    {
        result = xy_soc_data_in(datastr,curlen);
    }
    else
    {
        if (xy_soc_send((char*)datastr,curlen) == 0)
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
** 函数描述:   登录鉴权
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
** 函数描述:  心跳
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
    dynamic_debug("xy_soc_location_up");
    return xy_soc_data_pack(T808_MSGID_C_POSITION, 1, 0);
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
    kal_uint8 *info = &data[T808_PROTOCOL_MSG_HEAD_LEN]; // 消息体
    kal_int8 ask_result;
    XY_INFO_T * xy_info = xy_get_info();
    T808_PARA_T *t808_para = &xy_info->t808_para;
    
    if (len < T808_PROTOCOL_MSG_HEAD_LEN)
    {
        dynamic_debug("xy_soc_data_deal error data len:%d", len);
        return;
    }

    cmd = dynamic_big_endian_pack_short(&data[0]);
    serialnum = dynamic_big_endian_pack_short(&data[T808_PROTOCOL_MSG_HEAD_LEN-2]);
    
    dynamic_debug("xy_soc_data_deal cmd:0x%04x,serialnum:0x%04x",cmd,serialnum);
    dynamic_log_hex(data,len);  
    xy_soc_heart_reset();
    switch (cmd)
    {        
        case T808_MSGID_S_GENERAL_ACK:		/* 平台的通用应答 */
        {
            kal_uint16 ask_id = 0;

            ask_id = dynamic_big_endian_pack_short(&info[2]);
            ask_result = info[4];
            dynamic_debug("通用应答 终端消息ID:0x%04x,ask_result:%d",ask_id,ask_result);
            switch(ask_id)
            {
                case T808_MSGID_C_AUTHEN:
                {
					kal_uint8 cnt = xy_soc_get_ack_fail_cnt();

                    dynamic_debug("鉴权应答, result:%d", ask_result);

					if (!ask_result)
					{
						xy_soc_set_auth_ok_state(1);
						xy_soc_set_ack_fail_cnt(0);
					}
					else
					{
						xy_soc_set_ack_fail_cnt(++cnt);
					}

					dynamic_timer_stop(enum_timer_soc_task_timer);
					dynamic_timer_start(enum_timer_soc_task_timer, 1000, (void*)xy_soc_task,NULL,FALSE);
                }
                break;
				case T808_MSGID_C_HEART:
				{
					dynamic_debug("心跳应答, result:%d", ask_result);
					if (!ask_result)
					{
						xy_soc_heart_ask();
						dynamic_timer_stop(enum_timer_track_heart_wait_ask_timer);
					}
				}
				break;
				case T808_MSGID_C_POSITION:
				{
					dynamic_debug("位置应答, result:%d", ask_result);
					if (!ask_result)
					{
						//从队列中删除已成功发送的数据
						xy_soc_clear_data_flag();
						dynamic_timer_start(enum_timer_soc_task_timer, 100,(void*)xy_soc_task,NULL,FALSE);
					}
				}
				break;
                default:
                    dynamic_debug("通用应答");
                break;
            }
        }
        break;

        case T808_MSGID_S_REG_ACK:		/* 注册应答 */
        {
			kal_uint8 cnt = xy_soc_get_ack_fail_cnt();

			ask_result = info[2];
            if (len > (T808_PROTOCOL_MSG_HEAD_LEN + 4) && (len - (T808_PROTOCOL_MSG_HEAD_LEN+4)) <= T808_AUTH_CODE_LEN)
            {
            	if (!ask_result)
            	{
            		xy_soc_set_ack_fail_cnt(0);
                	memset(t808_para->auth_code, 0, sizeof(t808_para->auth_code));
					t808_para->auth_code_len = (len - T808_PROTOCOL_MSG_HEAD_LEN - 4);
                	memcpy(t808_para->auth_code, &info[3], t808_para->auth_code_len);
					xy_soc_set_ack_fail_cnt(0);
					xy_soc_set_reg_ok_state(1);

					dynamic_debug("终端注册应答:%d,auth_code:%s", ask_result, t808_para->auth_code);
					dynamic_log_hex(t808_para->auth_code, t808_para->auth_code_len);
				}
				else
				{
					xy_soc_set_ack_fail_cnt(++cnt);
				}
				dynamic_timer_stop(enum_timer_soc_task_timer);
				dynamic_timer_start(enum_timer_soc_task_timer, 1000, (void*)xy_soc_task,NULL,FALSE);
            }
			else
			{
				xy_soc_set_ack_fail_cnt(++cnt);
			}
		}
        break;
        
        case T808_MSGID_S_SET_PARAM:			/* 平台设置参数 */
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
					word = (u16)((u16)info[pos] << 8) | (u16)info[pos + 1];
					xy_data_pack_set_one_param_info(paramId, paramlen, (void *)&word);
				}
				else if (sizeof(u32) == paramlen)
				{
					dword = (u32)((u32)info[pos] << 24) | (u32)((u32)info[pos + 1] << 16)
						       | (u32)((u32)info[pos + 2] << 8) | (u32)info[pos + 3];
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
        break;
        
        case T808_MSGID_S_QUERY_PARAM:		/* 平台查询参数 */
		{
			xy_param_info param = {0};

			memset(&param, 0, sizeof(param));
			param.msgid = T808_MSGID_S_QUERY_PARAM;
			param.serial = serialnum;
			param.sum = 0;
			xy_soc_data_pack(T808_MSGID_C_PARAM_ACK, 0, (u8 *)&param);
        }
        break;
        
        case T808_MSGID_S_CTRL_CMD:		    /* 平台控制指令 */
		{
			s_xy_t808_data.ctrl_cmd_type = info[0];
			if (0x64 == s_xy_t808_data.ctrl_cmd_type)
			{
				dynamic_debug("断开油路");
			}
			else if (0x65 == s_xy_t808_data.ctrl_cmd_type)
			{
				dynamic_debug("恢复油路");
			}
			else if (0x04 == s_xy_t808_data.ctrl_cmd_type)
			{
				dynamic_debug("重启设备");
			}
			else if (0x05 == s_xy_t808_data.ctrl_cmd_type)
			{
				dynamic_debug("恢复出厂设置");
			}

			xy_soc_ask_up(T808_MSGID_S_CTRL_CMD, serialnum, 0);
		}
        break;
        
        case T808_MSGID_S_QUERY_SPE_PARAM:	/* 查询指定参数 */
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

			xy_soc_data_pack(T808_MSGID_C_PARAM_ACK, 0, (u8 *)&param);

			dynamic_free(param.paramId);
			param.paramId = 0;
        }
        break;
        
        case T808_MSGID_S_QUERY_ATTR:		/* 查询终端属性 */
		{
			xy_soc_data_pack(T808_MSGID_C_ATTR_ACK, 0, 0);
		}
        break;
       
        case T808_MSGID_S_QUERY_POSITION:	/* 平台点名操作 */
		{
			kal_uint16 serial = 0;

			serial = ((u16)info[0] << 8) | (u16)info[1];
			xy_soc_data_pack(T808_MSGID_C_POSITION_ACK, 0, (kal_uint8 *)&serial);
		}
        break;

		case T808_MSGID_S_TEMP_POS_TRACKING:/* 临时位置跟踪控制 */
		{
			t808_para->tracking_cylce = ((u16)info[0] << 8) | (u16)info[1];
			t808_para->tracking_time_sec = ((u32)info[2] << 24) | ((u32)info[3] << 16) | ((u32)info[4] << 8) | (u32)info[5];

			dynamic_debug("临时位置跟踪的信息:cycle-%d, sec-%d", t808_para->tracking_cylce,
				t808_para->tracking_time_sec);

			xy_soc_ask_up(T808_MSGID_S_TEMP_POS_TRACKING, serialnum, 0);
		}
		break;
		
        case T808_MSGID_S_ALARM_ACK:		/* 确认某个报警 */
		{
			//针对需要人工确认的报警
			//kal_uint16 recv_serial = ((u16)info[0] << 8) | (u16)info[1];
			kal_uint32 comfirm_alarm = ((u32)info[2] << 24) | ((u32)info[3] << 16) | ((u32)info[4] << 8) | (u32)info[5];

			dynamic_debug("confirm alarm:%x\r\n", comfirm_alarm);

			if (M_GET_BIT(comfirm_alarm, 0))
			{
				/* 确定紧急报警 */
			}

			if (M_GET_BIT(comfirm_alarm, 3))
			{
				/* 确认危险预警 */
			}

			if (M_GET_BIT(comfirm_alarm, 20))
			{
				/* 确认进出区域报警 */
			}

			if (M_GET_BIT(comfirm_alarm, 21))
			{
				/* 确认进出路线报警 */
			}

			if (M_GET_BIT(comfirm_alarm, 27))
			{
				/* 确认车辆非法点火报警 */
			}

			if (M_GET_BIT(comfirm_alarm, 28))
			{
				/* 确认车辆非法位移报警 */
			}

			xy_soc_ask_up(T808_MSGID_S_ALARM_ACK, serialnum, 0);
		}
        break;
        
        case T808_MSGID_S_SEND_TEXT:	/* 文本信息下发 */
		{
			int datalen = len - T808_PROTOCOL_MSG_HEAD_LEN - 2;
			char *serverStr = NULL;

			dynamic_debug("标志:%x, textlem:%d\r\n", info[0], datalen);

			xy_soc_ask_up(T808_MSGID_S_SEND_TEXT, serialnum, 0);

			serverStr = dynamic_malloc(1024);
			if (!serverStr)
			{
				break;
			}

			memset(serverStr, 0, 1024);
			memcpy((void *)serverStr, (void *)&info[1], datalen);
			dynamic_cmd_parse(DYNAMIC_CMD_TYPE_SERVER, NULL, serverStr, datalen);

			dynamic_free(serverStr);
			serverStr = NULL;
		}
        break;

		case T808_MSGID_S_CAR_CTRL:		/* 车辆控制 */
		{
			if (M_GET_BIT(info[0], 0))
			{
				/* 车门加锁 */
			}
			else
			{
				/* 车门解锁 */
			}
		}
		break;

        case T808_MSGID_S_EXPEND:		/* 透传信息 */
        {
			//电路操作
			if (0x01 == info[0])
			{
				//01 fd 04 01 fe - 断开电路
				//01 fd 00 01 fe - 恢复电路
				u32 dword = ((u32)info[1] << 24) | ((u32)info[2] << 16) | ((u32)info[3] << 8) | (u32)info[4];
				if (0xfd0401fe == dword)
				{
					dynamic_debug("断开电路");
				}
				else if (0xfd0001fe == dword)
				{
					dynamic_debug("恢复电路");
				}
			}
        }
		break;

        default:
        break;
    }

}



