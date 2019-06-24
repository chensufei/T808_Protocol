/********************************************************************************
**
** 文件名:     xy_soc.h
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

#ifndef __XY_SOC_H__
#define __XY_SOC_H__ 1


#define XY_MAX_SOC_DATA_NUM 100
#define XY_SOC_DATA_SIZE 1460
#define MAX_SOC_DATA_SIZE (20*1024)

typedef enum
{
	XY_SOC_SEND_RIGHT_NOW, // 直接发送
	
    XY_SOC_SEND_INSET_QUEUE // 添加到队列
}XY_SOC_SEND_TYPE_E;

typedef struct {
    kal_uint8  c_flags;     /* c_convert0 + c_convert1 = c_flags */
    kal_uint8  c_convert0;  /* c_convert0 + c_convert2 = c_convert0 */
    kal_uint8  c_convert1;
    kal_uint8  c_convert2;
}ASMRULE_T;

/*******************************************************************
** 函数: xy_soc_clear_link_info
** 描述: 清除链路信息
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_clear_link_info(void);


/*******************************************************************
** 函数: xy_soc_get_auth_ok_state
** 描述: 获取鉴权成功状态
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_get_auth_ok_state(void);
/*******************************************************************
** 函数: void xy_soc_set_ack_fail_cnt(kal_uint8 value)
** 描述: 设置响应失败的次数
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_set_ack_fail_cnt(kal_uint8 value);

/*******************************************************************
** 函数: kal_uint8 xy_soc_get_ack_fail_cnt(void)
** 描述: 获取响应失败的次数
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_get_ack_fail_cnt(void);

/*******************************************************************
** 函数名:     xy_assemble_by_rules
** 函数描述:   按规则拆分数据 自动增加头尾
** 参数:       
** 返回:       
********************************************************************/
kal_uint32 xy_assemble_by_rules(kal_uint8 *dptr, kal_uint8 *sptr, kal_uint32 len, ASMRULE_T *rule);
/*******************************************************************
** 函数名:     xy_deassmble_by_rules
** 函数描述:   按规则还原数据
** 参数:       
** 返回:       
********************************************************************/
kal_uint32 xy_deassmble_by_rules(kal_uint8 *dptr, kal_uint8 *sptr, kal_uint32 len, ASMRULE_T *rule);
/*******************************************************************
** 函数名:     xy_soc_close_delay
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_soc_close_delay(void);
/*******************************************************************
** 函数名:     xy_soc_data_in
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_bool xy_soc_data_in(kal_uint8 *data,kal_uint16 len);
/*******************************************************************
** 函数名:     xy_soc_get_socid
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_int8 xy_soc_get_socid(void);
/*******************************************************************
** 函数名:     xy_soc_data_send
** 函数描述:
** 参数:       
** 返回:       
********************************************************************/
kal_int8 xy_soc_data_send(char *buf, kal_uint32 len);
/*******************************************************************
** 函数名:     xy_soc_heart_ask
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_heart_ask(void);
/*******************************************************************
** 函数名:     xy_soc_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_task(void*ptr);
/*******************************************************************
** 函数名:     xy_soc_data_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_data_reset(void);
/*******************************************************************
** 函数名:     xy_soc_data_save
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_data_save(void);
/*******************************************************************
** 函数名:     xy_soc_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_soc_init(void);

#endif
#endif

