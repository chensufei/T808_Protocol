/********************************************************************************
**
** 文件名:     dynamic_file.h
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
#ifndef __DYNAMIC_FILE_H__
#define __DYNAMIC_FILE_H__ 1

#include "dynamic_init.h"

#define DYNAMIC_APP_FILE_NAME "dynamic_app.gz"
#define DYNAMIC_SYS_FILE_NAME "dynamic_app_sys_cfg"
#define DYNAMIC_SMS_FILE_NAME "dynamic_sms_info"
#ifdef __XY_SUPPORT__
#define XY_INFO_FILE_NAME "xy_info"
#define XY_INFO_FILE_BACKUP_NAME "xy_info_backup"
#define XY_SOC_DATA_FILE_NAME "xy_soc_data"
#endif

/*******************************************************************
** 函数名:     dynamic_fs_delete
** 函数描述:   删除文件
** 参数:       
** 返回:       
********************************************************************/
kal_int32 dynamic_file_dele(char* file_name);
/*******************************************************************
** 函数名:     dynamic_file_read
** 函数描述:   读取文件
** 参数:       
** 返回:       
********************************************************************/
kal_int32 dynamic_file_read(char* file_name, void *buffer,kal_int32 len);
/*******************************************************************
** 函数名:     dynamic_file_write
** 函数描述:   写文件，文件不存在则新建文件再写
** 参数:       
** 返回:       
********************************************************************/
kal_int32 dynamic_file_write(char* file_name, void *buffer,kal_int32 len);



#endif
