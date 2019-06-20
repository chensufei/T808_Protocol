/********************************************************************************
**
** 文件名:     xy_alm.h
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

#ifndef __XY_ALM_H__
#define __XY_ALM_H__ 1


typedef enum
{
    XY_ACC_OFF,
    XY_ACC_ON
}XY_ACC_STATE_E;

typedef enum
{
    XY_TRACK_MODE1 = 1,
    XY_TRACK_MODE2 = 2,
    XY_TRACK_MODE3 = 3
}XY_TRACK_MODE_E;

typedef enum
{
    XY_POW_CONNECT_STATE,
    XY_POW_BREAK_STATE
}XY_POW_STATE;



typedef enum
{
    XY_ALM_TYPE_NONE,
    XY_ALM_TYPE_ACC,
    XY_ALM_TYPE_MOVE,
    XY_ALM_TYPE_OVER_SPEED,
    XY_ALM_TYPE_POW_BREAK,
    XY_ALM_TYPE_LOW_BAT,
    XY_ALM_TYPE_LOW_POW,
    XY_ALM_TYPE_POW_RECOVER,
    XY_ALM_TYPE_VIB,
    XY_ALM_TYPE_REQUEST_POS,
    XY_ALM_TYPE_REQUEST_POS2,

    XY_ALM_TYPE_SOS,
    
    XY_ALM_TYPE_MAX
}XY_ALM_TYPE_E;


/*******************************************************************
** 函数名:     xy_alm_check_mode
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_bool xy_alm_check_mode(XY_ALM_MODE_E mode,XY_ALM_SW_E type);
/*******************************************************************
** 函数名:     xy_alm_vib_alm
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_alm_vib_alm(void);
/*******************************************************************
** 函数名:     xy_alm_antitheft_crl
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_alm_antitheft_crl(kal_uint8 onoff);
/*******************************************************************
** 函数名:     xy_track_set_mode
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_set_mode(kal_uint8 mode);
/*******************************************************************
** 函数名:     xy_track_sms_set_mode
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_track_sms_set_mode(XY_TRACK_MODE_E mode);
/*******************************************************************
** 函数名:     xy_alm_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_alm_init(void);

#endif
#endif