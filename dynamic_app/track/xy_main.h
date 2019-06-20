/********************************************************************************
**
** �ļ���:     xy_main.h
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
#include "dynamic_init.h"
#ifdef __XY_SUPPORT__

#ifndef __XY_MAIN_H__
#define __XY_MAIN_H__ 1



#define MAX_SOS_NUM_CNT 3
#define MAX_PB_NUM_CNT 10


#define DEV_PHONE_NUM_DEFAULT_LEN 11

#define XY_URL_LEN 128
#define XY_USERID_LEN 6   //
#define XY_MAX_SMS_CMD_LEN 10
#define XY_PASSWORD_LEN 6

#define XY_DEFAULT_PORT 8800 
#define XY_DEFAULT_SERVER "121.201.110.106"


#define XY_DEFAULT_CRL_PSW "123456"

#define XY_DEFAULT_MODE1_FREQ 10 // ģʽ1�ϱ���� ��λs
#define XY_DEFAULT_MODE2_FREQ 120 // ģʽ2�ϱ����
#define XY_DEFAULT_SLEEP_DELAY_TIME 180 // ���߻���ʱ�� Ĭ��180��
#define XY_DEFAULT_MOVE 200 // λ�Ʊ����뾶��Ĭ��200m
#define XY_DEFAULT_SPEED 60 // ���ٱ�����ֵ Ĭ��60km/h
#define XY_DEFAULT_SPEED_TIME 20 // ���ٱ���ʱ�䷧ֵ20s
#define XY_DEFAULT_STOP_TIME 1200 // ͣ��������ֵ Ĭ��1200s

#define XY_DEFAULT_VINOFF 11.5 // �����ж����ĵ�ѹֵ Ĭ��11.5V
#define XY_DEFAULT_VINON 11.8 // �����ָ����ĵ�ѹֵ Ĭ��11.8V
#define XY_DEFAULT_TIMETOLOCK 180  // ACC OFF���Զ������ʱʱ�� Ĭ��180s
#define XY_DEFAULT_LBV 3.7 // ���õ�ص͵���ֵ Ĭ��3.7V


#define T808_PRODUCT_ID_LEN		(5)		/* ������ID���� */
#define T808_DEV_TYPE_LEN		(20)	/* �ն��ͺų��� */
#define T808_DEV_ID_LEN			(7)		/* �ն� ID���� */
#define T808_VEH_ID_LEN			(17)	/* ������ʶID���� */
#define T808_AUTH_CODE_LEN		(64)	/* ��Ȩ�볤�� */


typedef enum
{
    XY_ALM_SW_GPRS,
    XY_ALM_SW_SMS,
    XY_ALM_SW_CALL
}XY_ALM_SW_E; // �����жϱ���ģʽ

typedef enum
{
    XY_ALM_MODE_GPRS,
    XY_ALM_MODE_SMS_GPRS,
    XY_ALM_MODE_CALL_GPRS,
    XY_ALM_MODE_SMS_GPRS_CALL,
    XY_ALM_MODE_MAX
}XY_ALM_MODE_E; // ������ʽ

typedef struct _alm_mode_
{
    kal_uint8 sw;
    kal_uint16 para1; // ����1������λ�Ʊ����ľ���,���ٱ����ٶȵ�
    kal_uint16 para2;
    kal_uint16 para3;
    kal_uint16 para4;
    XY_ALM_MODE_E mode; 
}XY_ALM_PARA_T;


/* �ն�ע�� */
typedef struct _t808_para_
{
    kal_uint8 is_regster; // �Ƿ�ע��
    kal_uint16 provice_id;							/* ʡ��ID */
    kal_uint16 city_id;								/* ����ID*/
    kal_uint8 product_id[T808_PRODUCT_ID_LEN+1];	/* �ն������̱��� */
    kal_uint8 dev_model[T808_DEV_TYPE_LEN+1];	/* �ն��ͺ�,20 ���ֽڣ����ն��ͺ������������ж��壬λ������ʱ���󲹡�0X00�� */
    kal_uint8 dev_id[T808_DEV_ID_LEN+1];			/* 7 ���ֽڣ��ɴ�д��ĸ��������ɣ����ն� ID �����������ж��壬λ������ʱ���󲹡�0X00�� */
    kal_uint8 veh_color;							/* ������ɫ Ĭ��0*/
    kal_uint8 veh_id[T808_VEH_ID_LEN+1];			/* ����Ĭ��0*/
    kal_uint8 auth_code[T808_AUTH_CODE_LEN+1];		    /* ��Ȩ����Ϣ */
}T808_PARA_T;

typedef struct _xy_info_
{
    kal_uint8 server[XY_URL_LEN+1];
    kal_uint16 port;
    kal_uint8 dev_num[MAX_PHONE_NUM_LEN+1]; // �豸�ֻ��� Ĭ��ʹ��IMEI 4~14λ��ֵ
    kal_uint8 user[MAX_PHONE_NUM_LEN+1]; // �û��ֻ���
    kal_uint8 sosnum[MAX_SOS_NUM_CNT][MAX_PHONE_NUM_LEN+1];		
    kal_uint8 psw[XY_PASSWORD_LEN+1]; // ����

    kal_uint8 apn[MAX_GPRS_APN_LEN]; //APN asc��
    kal_uint8 user_name[MAX_GPRS_USER_NAME_LEN];//�û���(��ѡ)
    kal_uint8 password[MAX_GPRS_PASSWORD_LEN];//����(��ѡ)
    kal_uint8 dns1[MAX_GPRS_IP_ADDR]; //ʮ����
    kal_uint8 dns2[MAX_GPRS_IP_ADDR]; //ʮ����

    T808_PARA_T t808_para;
    
    kal_uint8 reset_cnt; // ��ⲻ��SIM����������
    kal_uint32 heart_time;//����ʱ��
    kal_uint32 freq; // ģʽ1�ϱ���� Ĭ��10s
    kal_uint32 freq2; // ģʽ2�ϱ���� Ĭ��120s
    kal_uint8 slp_sw; // ����ʹ�ܿ��أ�ģʽ2����ʱ��������������
    kal_uint32 slp_d_t; // ������ʱʱ�� Ĭ��180��

    kal_uint8 mode; // 0Ĭ�� 1����״̬ 2ACCOFF 3����״̬
    kal_uint8 anti_thrief; // �Ƿ����
    kal_uint8 check_anti_thrief; // ��ʼ����Զ����
    kal_uint8 autolock; // �Զ�������� Ĭ��0
    kal_uint32 timetolock; // ACC OFF���Զ������ʱʱ�� Ĭ��180s
    double lat; // �����ľ�γ��
    double lng;
    
    XY_ALM_PARA_T vib_p; // �𶯱�������
    kal_uint8 vib_alm; // �𶯱���
    kal_uint8 vibsensity; // �������� 0~2 ���и� Խ��Խ����
    
    XY_ALM_PARA_T acc_p; // �Ƿ�ACC��������
    kal_uint8 acc_alm; // ACC����
    kal_uint8 acc_state; // ACC״̬
    
    XY_ALM_PARA_T move_p; // λ�Ʊ�������
    kal_uint8 move_alm; // λ�Ʊ���

    XY_ALM_PARA_T speed_p; // ���ٱ�������
    kal_uint8 speed_alm; // ���ٱ���

    XY_ALM_PARA_T break_p; // ���Ͽ���������
    kal_uint8 break_alm; // ���Ͽ�����
    kal_uint8 break_state; // ����״̬

    XY_ALM_PARA_T bat_p; // ��ر�������
    kal_uint8 bat_alm; // ��ص͵籨��
    kal_uint32 bat_value; // ��ص�ѹֵ ��λuv
    float lbv; // ���õ�ص͵���ֵ Ĭ��3.7V
    
    float vinoff; // ���͵�ѹֵ Ĭ��11.5V
    float vinon; // ����ѹ�ָ�ֵ Ĭ��11.8V
    kal_uint8 vin_alm; // ���ͱ���

    kal_uint8 realy_crl; // �̵�������
    kal_uint32 slp_sectime; // ����ʱ��ʱ��
    kal_uint8 acc_off_flg; // ACC ��on��Ϊoff
    kal_uint8 sport_state; // �˶�״̬
    kal_uint32 sport_sectime; // ����⵽�˶���ʱ����
    kal_uint32 accoff_sectime; // Ϩ���ʱ��
    kal_uint32 mileage; // ���
    
    kal_uint32 temp[10];
    
    kal_uint32 checksum; // �ۼӺ�
} XY_INFO_T;

/*******************************************************************
** ������:     xy_get_para_from_ascii
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_get_para_from_ascii(char *str, char *dtr,kal_uint8 num,const kal_uint8 para_len);
/*******************************************************************
** ������:     xy_get_info
** ��������:   
** ����:       
** ����:       
********************************************************************/
XY_INFO_T * xy_get_info(void);
/*******************************************************************
** ������:     xy_info_save
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_info_save(void);
/*******************************************************************
** ������:     xy_info_reset
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_info_reset(void);

/*******************************************************************
** ������:     xy_main_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_main_init(void);
#endif
#endif