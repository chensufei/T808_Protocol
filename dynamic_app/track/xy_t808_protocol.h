/********************************************************************************
**
** 文件名:     xy_t808_protocol.h
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
#ifndef __XY_T808_PROTOCOL_H__
#define __XY_T808_PROTOCOL_H__


#define XY_SOC_PROTOCOL_MSG_HEAD_LEN 21

#define XY_SOC_PROTOCOL_HEAD 0x7e
#define XY_SOC_PROTOCOL_END 0x7e


/* 消息ID */
typedef enum
{
	/* 终端上传 */
	T808_MSGID_C_GENERAL_ACK = 0x0001,		/* 终端通用应答 */
	T808_MSGID_C_HEART = 0x0002,			/* 心跳 */
	T808_MSGID_C_DREG = 0x0003,			    /* 注销 */
	T808_MSGID_C_REG = 0x0100,				/* 注册 */
	T808_MSGID_C_AUTHEN = 0x0102,			/* 鉴权 */
	T808_MSGID_C_PARAM_ACK = 0x0104,		/* 参数汇报 */
	T808_MSGID_C_ATTR_ACK = 0x0107,			/* 属性应答 */
	T808_MSGID_C_POSITION = 0x0200,			/* 位置 */
	T808_MSGID_C_POSITION_ACK = 0x0201,		/* 位置查询应答 */
	T808_MSGID_C_VEH_CTRL_ACK = 0x0500,		/* 车辆控制应答 */
	T808_MSGID_C_COMPSATE = 0x0704,			/* 盲区补偿 */
	T808_MSGID_C_EXPEND = 0x0900,			/* 拓展 */

	/* 平台下发 */
	T808_MSGID_S_GENERAL_ACK = 0x8001,		/* 平台的通用应答 */
	T808_MSGID_S_REG_ACK = 0x8100,			/* 注册应答 */
	T808_MSGID_S_SET_PARAM = 0x8103,		/* 平台设置参数 */
	T808_MSGID_S_QUERY_PARAM = 0x8104,		/* 平台查询参数 */
	T808_MSGID_S_CTRL_CMD = 0x8105,			/* 平台控制指令 */
	T808_MSGID_S_QUERY_SPE_PARAM = 0x8106,	/* 查询指定参数 */
	T808_MSGID_S_QUERY_ATTR = 0x8107,		/* 查询终端属性 */
	T808_MSGID_S_QUERY_POSITION = 0x8201,	/* 平台点名操作 */
	T808_MSGID_S_TEMP_POS_TRACKING = 0x8202,/* 临时位置跟踪控制 */
	T808_MSGID_S_ALARM_ACK = 0x8203,		/* 确认某个报警 */
	T808_MSGID_S_SEND_TEXT = 0x8300,		/* 文本信息下发 */
	T808_MSGID_S_CAR_CTRL = 0x8500,			/* 车辆控制 */
	T808_MSGID_S_EXPEND = 0x8900			/* 透传信息 */

	
} T808_PROTOCOL_MSGID_E;

/*******************************************************************
** 函数名:     xy_soc_data_pack
** 函数描述:   上报数据组包
** 参数:       type:数据类型  inset_list:0直接发送 1插入链表
** 返回:       
********************************************************************/
kal_uint8 xy_soc_data_pack(T808_PROTOCOL_MSGID_E cmd,XY_SOC_SEND_TYPE_E inset_list,kal_uint8* str);
/*******************************************************************
** 函数名:     xy_soc_send
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_int8 xy_soc_send(char *buf, kal_uint32 len);
/*******************************************************************
** 函数名:     xy_soc_regster_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_regster_up(void);
/*******************************************************************
** 函数名:     xy_soc_login_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_login_up(void);
/*******************************************************************
** 函数名:     xy_soc_heart_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_heart_up(void);
/*******************************************************************
** 函数名:     xy_soc_ask_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_ask_up(kal_uint16 id,kal_uint16 num,kal_uint8 result);
/*******************************************************************
** 函数名:	   xy_soc_alm_assem
** 函数描述:   
** 参数:	   
** 返回:	   
********************************************************************/
void xy_soc_alm_assem(XY_ALM_TYPE_E TYPE);
/*******************************************************************
** 函数名:     xy_soc_location_up
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_location_up(void);
/*******************************************************************
** 函数名:     xy_soc_data_deal
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_data_deal(kal_uint8*data,kal_uint32 len);


#endif



