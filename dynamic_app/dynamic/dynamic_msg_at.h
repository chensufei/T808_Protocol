/********************************************************************************
**
** 文件名:     dynamic_msg_at.h
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
#ifndef __DYNAMIC_MSG_AT_H__
#define __DYNAMIC_MSG_AT_H__ 1

#include "dynamic_init.h"


#define COMMAND_LINE_SIZE    (100)
#define NULL_TERMINATOR_LENGTH (1)
#define MAX_CMD_LEN	200 
#define MAX_ATCMD_NUM 120
#define DYNAMIC_CMD_ASK_LEN 200

typedef struct 
{
	short  position;
	short  length;
	char   type;
	char   character[COMMAND_LINE_SIZE + NULL_TERMINATOR_LENGTH];
} dynamic_custom_cmdLine;

typedef struct _dynamic_cmd_
{
	char *sstr;
	int (*entryproc)(char type,char*num,char*cmd,dynamic_custom_cmdLine * str);
}DYNAMIC_CMD_T;

typedef enum
{
    DYNAMIC_CMD_TYPE_SERVER,
    DYNAMIC_CMD_TYPE_SMS,
    DYNAMIC_CMD_TYPE_UART
}DYNAMIC_CMD_TYPE_E;

/*******************************************************************
** 函数名:     dynamic_cmd_ask
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_cmd_ask(char type,char*num,char* askdata,kal_uint16 asklen);
/*******************************************************************
** 函数名:     dynamic_cmd_find_cmd_mode
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
custom_cmd_mode_enum dynamic_cmd_find_cmd_mode(dynamic_custom_cmdLine *cmd_line);
/*******************************************************************
** 函数名:     dynamic_cmd_parse
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_parse(DYNAMIC_CMD_TYPE_E type,char *num,char* str,kal_uint32 len);
/*******************************************************************
** 函数名:     dynamic_msg_at_init
** 函数描述:   msg at初始化
** 参数:       
** 返回:       
********************************************************************/
void dynamic_msg_at_init(void);

#endif
