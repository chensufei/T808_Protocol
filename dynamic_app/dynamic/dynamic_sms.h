/********************************************************************************
**
** �ļ���:     dynamic_sms.h
** ��Ȩ����:   
** �ļ�����:   ��ʱ��
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   |  �޸ļ�¼
**===============================================================================
**|  |  | 
*********************************************************************************/
#ifndef __DYNAMIC_SMS_H__
#define __DYNAMIC_SMS_H__ 1

#include "dynamic_init.h"

#define MAX_SMS_LEN 200
#define DYNAMIC_SMS_LIST_NUM 10

typedef struct _sms_data_
{
    kal_uint8 ph_num[MAX_PHONE_NUM_LEN];
    srv_sms_dcs_enum dcs; // δ��Ч
    kal_uint16 len;
    kal_uint8 data[MAX_SMS_LEN];
    
}DYNAMIC_SMS_DATA_T;

typedef struct __sms_list_
{
	kal_uint8 fail_cnt; // ��ǰ����ʧ�ܴ���
	kal_uint8 data_in; // ��������
	kal_uint8 data_out; // ȡ����������
	DYNAMIC_SMS_DATA_T info[DYNAMIC_SMS_LIST_NUM];
	kal_uint32 checksum;
}DYNAMIC_SMS_LIST_T;


/*******************************************************************
** ������:     dynamic_sms_list_is_empty
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sms_list_is_empty(void);
/*******************************************************************
** ������:     dynamic_sms_list_push
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sms_list_push(DYNAMIC_SMS_DATA_T *sms);
/*******************************************************************
** ������:     dynamic_sms_list_add
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sms_list_add(char*num,kal_uint16 len,char * data);
/*******************************************************************
** ������:     dynamic_sms_list_pop
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sms_list_pop(void);
/*******************************************************************
** ������:     dynamic_sms_save
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sms_save(void);
/*******************************************************************
** ������:     dynamic_sms_list_reset
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_sms_list_reset(void);
/*******************************************************************
** ������:     dynamic_sms_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void dynamic_sms_init(void);


#endif
