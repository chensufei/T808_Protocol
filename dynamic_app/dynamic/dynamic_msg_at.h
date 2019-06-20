/********************************************************************************
**
** �ļ���:     dynamic_msg_at.h
** ��Ȩ����:   
** �ļ�����:   
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   |  �޸ļ�¼
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
** ������:     dynamic_cmd_ask
** ��������:   
** ����:       
** ����:       
********************************************************************/
void dynamic_cmd_ask(char type,char*num,char* askdata,kal_uint16 asklen);
/*******************************************************************
** ������:     dynamic_cmd_find_cmd_mode
** ��������:   
** ����:       
** ����:       
********************************************************************/
custom_cmd_mode_enum dynamic_cmd_find_cmd_mode(dynamic_custom_cmdLine *cmd_line);
/*******************************************************************
** ������:     dynamic_cmd_parse
** ��������:   
** ����:       
** ����:       
********************************************************************/
int dynamic_cmd_parse(DYNAMIC_CMD_TYPE_E type,char *num,char* str,kal_uint32 len);
/*******************************************************************
** ������:     dynamic_msg_at_init
** ��������:   msg at��ʼ��
** ����:       
** ����:       
********************************************************************/
void dynamic_msg_at_init(void);

#endif
