/********************************************************************************
**
** 文件名:     dynamic_reset.h
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
#ifndef __DYNAMIC_RESET_H__
#define __DYNAMIC_RESET_H__ 1

#include "dynamic_init.h"


typedef enum
{
    RESET_TYPE_ABNORMITY, // 异常重启
    RESET_TYPE_POWEROFF, // 关机
    RESET_TYPE_FOR_CMD, // 指令重启
    RESET_TYPE_UPGRADE, // 远程升级重启
    RESET_TYPE_24_HOUR, // 24小时自动重启
    RESET_TYPE_SOC_ERR, // SOC链接异常
    RESET_TYPE_SERVER_NOASK, // 服务器未应答
    RESET_TYPE_NODATA, // 长时间没有数据
    RESET_TYPE_NO_SIM, // 找不到SIM卡    
    RESET_TYPE_MEM_ERR, // 内存异常
    RESET_TYPE_NO_NORMAL

}RESET_TYPE_ENUM;

typedef struct
{
    RESET_TYPE_ENUM type;
    applib_time_struct time;
}dynamic_reset_info_t;

#define MAX_RESET_INFO_NUM 50
typedef struct
{
    kal_uint32 abnreset_count; // 无效开机次数计算
    RESET_TYPE_ENUM ref_reset_type;
    RESET_TYPE_ENUM reset_type; // 重启类型，只用于开机的时候判断是否异常重启
    kal_uint32 reset_cnt; // 重启次数
    dynamic_reset_info_t reset_info[MAX_RESET_INFO_NUM]; // 重启信息
    dynamic_gps_info_t end_gps; // 末次定位点
    
    
    kal_uint32 checksum; // 累加和
}DYNAMIC_SYS_T;

/*******************************************************************
** 函数名:     dynamic_sys_get_info
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
DYNAMIC_SYS_T *dynamic_sys_get_info(void);
/*******************************************************************
** 函数名:     dynamic_sys_info_save
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sys_info_save(void);
/*******************************************************************
** 函数名:     dynamic_sys_info_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_sys_info_reset(void);
/*******************************************************************
** 函数名:     dynamic_reset_delay
** 函数描述:   延时重启
** 参数:       
** 返回:       
********************************************************************/
void dynamic_reset_delay(void);
/*******************************************************************
** 函数名:     dynamic_start_reset
** 函数描述:   重启系统或关机
** 参数:       
** 返回:       
********************************************************************/
void dynamic_start_reset(RESET_TYPE_ENUM sign);
/*******************************************************************
** 函数名:     dynamic_poweroff_now
** 函数描述:   关机
** 参数:       
** 返回:       
********************************************************************/
void dynamic_poweroff_now(void);
/*******************************************************************
** 函数名:     dynamic_reset_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_reset_init(void);

#endif
