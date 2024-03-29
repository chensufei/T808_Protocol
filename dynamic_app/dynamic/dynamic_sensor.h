/********************************************************************************
**
** 文件名:     dynamic_sensor.h
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

#ifndef __DYNAMIC_SENSOR_H__
#define __DYNAMIC_SENSOR_H__ 1

#define DYNAMIC_MAX_VIBSENSITY 2
#define DYNAMIC_DEFAULT_VIBSENSITY 1 // 默认震动灵敏度 0~2 低中高 越大越灵敏


/*******************************************************************
** 函数名:     dynamic_sensor_clear_check_cnt
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_clear_check_cnt(void);
/*******************************************************************
** 函数名:     dynamic_sensor_vib_in_3min
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
kal_uint8 dynamic_sensor_vib_in_3min(void);
/*******************************************************************
** 函数名:     dynamic_sensor_count_for_accon
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_count_for_accon(void);
/*******************************************************************
** 函数名:     dynamic_sensor_get_id
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
kal_uint32 dynamic_sensor_get_id(void);
/*******************************************************************
** 函数名:     dynamic_sensor_clr_vib
** 函数描述:   
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_clr_vib(void);
/*******************************************************************
** 函数名:     dynamic_sensor_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_sensor_init(void);


#endif
