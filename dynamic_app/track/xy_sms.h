/********************************************************************************
**
** 文件名:     xy_sms.h
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

#ifndef __XY_SMS_H__
#define __XY_SMS_H__ 1


/*******************************************************************
** 函数名:     xy_sms_user_check
** 函数描述:   0未设置主号 1主号 2非主号
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_sms_user_check(char* num);
/*******************************************************************
** 函数名:     xy_sms_alm_assem
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_sms_alm_assem(XY_ALM_TYPE_E type);
/*******************************************************************
** 函数名:     xy_cmd_gpssnr
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int xy_cmd_gpssnr(char type,char*num,char *cmd,dynamic_custom_cmdLine *str);

#endif
#endif

