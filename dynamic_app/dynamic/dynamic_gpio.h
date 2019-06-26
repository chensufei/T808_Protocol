/********************************************************************************
**
** 文件名:     dynamic_gpio.h
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
#ifndef __DYNAMIC_GPIO_H__
#define __DYNAMIC_GPIO_H__ 1

#include "dynamic_init.h"



#ifdef __XY_SUPPORT__

#define XY_ACC_CHECK_PORT 0 // 0 acc on  1 acc off
#define XY_BREAK_CHECK_PORT 1 // 断外电检测 0接外电  1断外电
#define XY_WATCHDOG_PORT 15
#define XY_REALY_CRL_PORT 21 // 1断油路

#define XY_GSM_LED_PORT 30
#define XY_GPS_LED_PORT 29





#endif


#endif

