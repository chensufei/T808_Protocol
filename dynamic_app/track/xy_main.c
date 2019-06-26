/********************************************************************************
**
** �ļ���:     xy_main.c
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

/*******************************************************************
** �궨��
********************************************************************/
#define M_T808_DEF_PROVICE_ID		(44)			/* Ĭ��ʡ��ID */
#define M_T808_DEF_CITY_ID			(307)			/* Ĭ������ID */
#define M_T808_DEF_PRODUCT_ID		"19000"			/* ����ID */
#define M_T808_DEF_TERM_TYPE		"XY_DEVICE_T808"/* Ĭ���ն����� */
#define M_T808_DEF_VIN				"XY123456789012345"

/*******************************************************************
** ȫ�ֱ���
********************************************************************/
/* ϵͳ���� */
static XY_INFO_T xy_info;


/*******************************************************************
** ������:     xy_get_para_from_ascii
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_get_para_from_ascii(char *str, char *dtr,kal_uint8 num,const kal_uint8 para_len)
 {
    int paraLen = 0;

    if((str == NULL) || (dtr == NULL) || (para_len == 0))
    {
        return 0;
    }

    while((*str != 0) && (num > 0))
    {
        if(*str == ',' || *str == ']')
        {
            num--;
        }
        str++;
    }

    if((*str == 0) || (num > 0))
    {
        return 0;
    }

    while((*str != 0) && (*str != ',') && (*str != ']'))
    {   
        *dtr = *str;
        dtr++;
        str++;
        paraLen++;

        if(para_len < paraLen)
        {
            return 0;
        }
    }
    return paraLen;
 }


/*******************************************************************
** ������:     xy_get_info
** ��������:   
** ����:       
** ����:       
********************************************************************/
XY_INFO_T * xy_get_info(void)
{
    return &xy_info;
}

/*******************************************************************
** ������:     xy_info_backup_save
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_info_backup_save(void)
{
    xy_info.checksum = dynamic_checksum((kal_uint8 *)&xy_info, (sizeof(XY_INFO_T)-4));
    dynamic_file_write(XY_INFO_FILE_BACKUP_NAME,&xy_info,sizeof(xy_info));
    
    return 1;
}

/*******************************************************************
** ������:     xy_info_save
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_info_save(void)
{
    xy_info.checksum = dynamic_checksum((kal_uint8 *)&xy_info, (sizeof(XY_INFO_T)-4));
    dynamic_file_write(XY_INFO_FILE_NAME,&xy_info,sizeof(xy_info));

    dynamic_timer_start(enum_timer_fs_backup_timer,100,(void*)xy_info_backup_save,NULL,FALSE);
    return 1;
}

/*******************************************************************
** ������:     xy_apn_dns_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_apn_dns_init(void)
{
    if (strlen((char*)xy_info.apn) > 0)
    {
        dynamic_apn_set_sturct * apn_dns = dynamic_gprs_get_apn_dns();

        memcpy(apn_dns->tag_apn,xy_info.apn,MAX_GPRS_APN_LEN);
        memcpy(apn_dns->tag_user_name,xy_info.user_name,MAX_GPRS_USER_NAME_LEN);
        memcpy(apn_dns->tag_password,xy_info.password,MAX_GPRS_PASSWORD_LEN);

        memcpy(apn_dns->tag_dns1,xy_info.dns1,MAX_GPRS_IP_ADDR);
        memcpy(apn_dns->tag_dns2,xy_info.dns2,MAX_GPRS_IP_ADDR);
    }
}

/*******************************************************************
** ������:     xy_t808_param_reset
** ��������:   ��ʼ������
** ����:       
** ����:       
********************************************************************/
void xy_t808_param_reset(void)
{
	xy_info.t808_para.provice_id = M_T808_DEF_PROVICE_ID;
	xy_info.t808_para.city_id = M_T808_DEF_CITY_ID;
	
	memcpy(xy_info.t808_para.product_id, (u8 *)M_T808_DEF_PRODUCT_ID, T808_PRODUCT_ID_LEN);
	memcpy(xy_info.t808_para.dev_model, (u8 *)M_T808_DEF_TERM_TYPE, strlen(M_T808_DEF_TERM_TYPE));
	memcpy(xy_info.t808_para.dev_id, (u8 *)"XY_001", strlen("XY_001"));

	xy_info.t808_para.veh_color = 0;
	
	memcpy(xy_info.t808_para.veh_id, (u8 *)M_T808_DEF_VIN, strlen(M_T808_DEF_VIN));

	xy_info.t808_para.tcp_ack_overtime = 60;	/* TCP��ʱʱ�� */
	xy_info.t808_para.tcp_resend_cnt = 3;		/* tcp�ش����� */
	xy_info.t808_para.report_type = 0;			/* λ�û㱨����,λ����Ϣ�ϱ���ʽ��0����ʱ�㱨��1������㱨��2����ʱ�Ͷ���㱨 */
	xy_info.t808_para.report_way = 0;			/* λ�û㱨����, 0������ ACC ״̬�� 1�����ݵ�¼״̬�� ACC ״̬�����жϵ�¼״̬������¼�ٸ��� ACC ״̬ */

	xy_info.t808_para.sos_freq = 5;				/* ��������ʱ�䣬Ĭ����5s */
	xy_info.t808_para.def_distance = 300;		/* ȱʡ����㱨���,Ĭ����300m */
	xy_info.t808_para.sos_distance = 50;		/* ��������㱨���,Ĭ����50m */
	xy_info.t808_para.degree = 7;				/* �յ㲹���Ƕ�, <180�� */
	xy_info.t808_para.fencing_radius = 300;		/* ����Χ���뾶���Ƿ�λ����ֵ������λΪ�ף�Ĭ��300m */

	memset(xy_info.t808_para.txt_number, 0, sizeof(xy_info.t808_para.txt_number));		/* �ı����룬���ն���/���� */

	//��������Ĭ�϶���
	xy_info.t808_para.alarm_sw.val = 0;				/* ���������ֽڣ���Ӧλ����Ϣ����ı���λ��1-�������� */

	//���ű���Ĭ�϶��ر�
	xy_info.t808_para.alarm_sms_sw.val = 1;			/* ���������ı� SMS ���أ���λ����Ϣ�㱨��Ϣ�еı�����־���Ӧ����ӦλΪ 1 ����Ӧ����ʱ�����ı� SMS */

	xy_info.t808_para.over_speed = 120;				/* ����ٶȣ���λΪ����ÿСʱ(km/h)Ĭ��120km/h */
	xy_info.t808_para.speed_keep_time = 10;			/* ���ٳ���ʱ�䣬��λΪ�루s��Ĭ��10s */

	xy_info.t808_para.over_speed_pre_val = 110;		/* ���ٳ���ʱ�䣬��λΪ�루s����Ĭ��110km/h */

	xy_info.t808_para.hit_param_val = 0x0a01;		/* ��ײ�����������ã�
	  												b7-b0�� ��ײʱ�䣬��λ 4ms��
	  												b15-b8����ײ���ٶȣ���λ 0.1g�����÷�Χ�ڣ�0-79 ֮�䣬Ĭ��Ϊ10�� */
	xy_info.t808_para.roll_over_param_val = 30;		/* �෭������������:�෭�Ƕȣ���λ 1 �ȣ�Ĭ��Ϊ 30 �ȡ� */

	xy_info.t808_para.gpsMode = 1;   /* GPS��λģʽ��0 - ��֧��GPS��λ��1-֧��GPS��λ */
	xy_info.t808_para.bdMode = 1;    /* bd��λģʽ��0 - ��֧��GPS��λ��1-֧��GPS��λ */
}

/*******************************************************************
** ������:     xy_info_reset
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 xy_info_reset(void)
{
    //APP_CNTX_T * app_cntx = dynamic_app_cntx_get();
    kal_uint8 server[XY_URL_LEN];
    kal_uint16 port = 0;
    kal_uint8 user[MAX_PHONE_NUM_LEN+1];
    kal_uint8 dev_num[MAX_PHONE_NUM_LEN+1];

    dynamic_debug("xy_info_reset");
    memset(server,0,sizeof(server));
    
    if (strlen((char*)xy_info.server) > 7 && xy_info.port > 0)
    {
        memcpy(server,xy_info.server,strlen((char*)xy_info.server));
        port = xy_info.port;
    }
    else
    {
        memcpy(server,XY_DEFAULT_SERVER,strlen(XY_DEFAULT_SERVER));
        port = XY_DEFAULT_PORT;
    }

    memset(user,0,sizeof(user));
    memcpy(user,xy_info.user,strlen((char*)xy_info.user));
    
    memset(dev_num,0,sizeof(dev_num));
    memcpy(dev_num,xy_info.dev_num,strlen((char*)xy_info.dev_num));
	
    memset(&xy_info,0,sizeof(xy_info));
    memcpy(xy_info.server,server,strlen((char*)server));
    xy_info.port = port;
    
    memcpy(xy_info.dev_num,dev_num,strlen((char*)dev_num));
    memcpy(xy_info.user,user,strlen((char*)user));
    
    memcpy(xy_info.psw,XY_DEFAULT_CRL_PSW,strlen(XY_DEFAULT_CRL_PSW));

    xy_info.freq = XY_DEFAULT_MODE1_FREQ; // ģʽ1�ϱ���� Ĭ��10s
    xy_info.freq2 = XY_DEFAULT_MODE2_FREQ; // ģʽ2�ϱ���� Ĭ��120s
    xy_info.slp_sw = 1; // ����ʹ�ܿ��أ�ģʽ2����ʱ�������������� Ĭ��1
    xy_info.slp_d_t = XY_DEFAULT_SLEEP_DELAY_TIME; // ������ʱʱ�� Ĭ��180��
    
    xy_info.vib_p.sw = 0; // �𶯱�������
    xy_info.vib_p.mode = XY_ALM_MODE_GPRS;

    xy_info.acc_p.sw = 0; // �Ƿ�ACC��������
    xy_info.acc_p.mode = XY_ALM_MODE_GPRS;

 
    xy_info.vinoff = XY_DEFAULT_VINOFF; // ���͵�ѹֵ Ĭ��11.5V
    xy_info.vinon = XY_DEFAULT_VINON; // ���������ѹֵ Ĭ��11.8V
    xy_info.autolock = 0; // �Զ�������� Ĭ��0
    xy_info.timetolock = XY_DEFAULT_TIMETOLOCK; // ACC OFF���Զ������ʱʱ�� Ĭ��180s
    xy_info.lbv = XY_DEFAULT_LBV; // ���õ�ص͵���ֵ Ĭ��3.7V

    
    xy_info.mode = XY_TRACK_MODE3; // // 0Ĭ�� 1����״̬ 2ACCOFF 3����״̬
	xy_info.heart_time = 180;	
    
    xy_info.vibsensity = DYNAMIC_DEFAULT_VIBSENSITY; // �������� Խ��Խ����

	xy_t808_param_reset();
	
    xy_info_save();

    xy_apn_dns_init();
#ifdef __XY_MILEAGE_SUPPORT__
    xy_mil_reset();
#endif
    return 1;
}

/*******************************************************************
** ������:     xy_main_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_main_init(void)
{
    APP_CNTX_T * app_cntx = dynamic_app_cntx_get();
    DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
    kal_uint32 checksum;
    kal_uint32 len;

    dynamic_debug("xy_main_init");
    memset(&xy_info,0,sizeof(xy_info));
    len = dynamic_file_read(XY_INFO_FILE_NAME,&xy_info,sizeof(xy_info));
    checksum = dynamic_checksum((kal_uint8 *)&xy_info, (sizeof(XY_INFO_T)-4));
    if (len != sizeof(xy_info) || checksum != xy_info.checksum)
    {
        dynamic_debug("XY_INFO_FILE_NAME read err len:%d,%d,crc:%d,%d",len,sizeof(xy_info),checksum,xy_info.checksum);
        memset(&xy_info,0,sizeof(xy_info));
        len = dynamic_file_read(XY_INFO_FILE_BACKUP_NAME,&xy_info,sizeof(xy_info));
        checksum = dynamic_checksum((kal_uint8 *)&xy_info, (sizeof(XY_INFO_T)-4));
        if (len != sizeof(xy_info) || checksum != xy_info.checksum)
        {
            dynamic_debug("XY_INFO_FILE_BACKUP_NAME read err len:%d,%d,crc:%d,%d",len,sizeof(xy_info),checksum,xy_info.checksum);
            xy_info_reset();
        }
    }
    
    if (strlen((char*)xy_info.server) < 3)
    {
        dynamic_error("�������쳣");
        xy_info_reset();
    }
    
    if (sys_info->ref_reset_type == RESET_TYPE_POWEROFF)
    {
        xy_info.reset_cnt = 0;
    }
    if (strlen((char*)xy_info.dev_num) == 0 || strncmp((char*)xy_info.dev_num,"FFFFFFFFFFFF",DEV_PHONE_NUM_DEFAULT_LEN) || strncmp((char*)xy_info.dev_num,"ffffffffffff",DEV_PHONE_NUM_DEFAULT_LEN))
    {
        memcpy(xy_info.dev_num,&app_cntx->imei[4], DEV_PHONE_NUM_DEFAULT_LEN);
    }
#ifdef __XY_MILEAGE_SUPPORT__
    xy_mil_init(xy_info.mileage);
#endif    

    xy_apn_dns_init();
    xy_led_init();
    xy_alm_init();
    xy_soc_init();
    xy_track_init();
    
    xy_watchdog_init();
}


#endif

