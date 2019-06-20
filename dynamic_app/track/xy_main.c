/********************************************************************************
**
** 文件名:     xy_main.c
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


static XY_INFO_T xy_info;


/*******************************************************************
** 函数名:     xy_get_para_from_ascii
** 函数描述:   
** 参数:       
** 返回:       
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
** 函数名:     xy_get_info
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
XY_INFO_T * xy_get_info(void)
{
    return &xy_info;
}

/*******************************************************************
** 函数名:     xy_info_backup_save
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_info_backup_save(void)
{
    xy_info.checksum = dynamic_checksum((kal_uint8 *)&xy_info, (sizeof(XY_INFO_T)-4));
    dynamic_file_write(XY_INFO_FILE_BACKUP_NAME,&xy_info,sizeof(xy_info));
    
    return 1;
}

/*******************************************************************
** 函数名:     xy_info_save
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_info_save(void)
{
    xy_info.checksum = dynamic_checksum((kal_uint8 *)&xy_info, (sizeof(XY_INFO_T)-4));
    dynamic_file_write(XY_INFO_FILE_NAME,&xy_info,sizeof(xy_info));

    dynamic_timer_start(enum_timer_fs_backup_timer,100,(void*)xy_info_backup_save,NULL,FALSE);
    return 1;
}

/*******************************************************************
** 函数名:     xy_apn_dns_init
** 函数描述:   
** 参数:       
** 返回:       
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
** 函数名:     xy_info_reset
** 函数描述:   
** 参数:       
** 返回:       
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

    xy_info.freq = XY_DEFAULT_MODE1_FREQ; // 模式1上报间隔 默认10s
    xy_info.freq2 = XY_DEFAULT_MODE2_FREQ; // 模式2上报间隔 默认120s
    xy_info.slp_sw = 1; // 休眠使能开关，模式2持续时间结束后进入休眠 默认1
    xy_info.slp_d_t = XY_DEFAULT_SLEEP_DELAY_TIME; // 休眠延时时间 默认180秒
    
    xy_info.vib_p.sw = 0; // 震动报警开关
    xy_info.vib_p.mode = XY_ALM_MODE_GPRS;

    xy_info.acc_p.sw = 0; // 非法ACC报警开关
    xy_info.acc_p.mode = XY_ALM_MODE_GPRS;

 
    xy_info.vinoff = XY_DEFAULT_VINOFF; // 外电低电压值 默认11.5V
    xy_info.vinon = XY_DEFAULT_VINON; // 外电正常电压值 默认11.8V
    xy_info.autolock = 0; // 自动设防开关 默认0
    xy_info.timetolock = XY_DEFAULT_TIMETOLOCK; // ACC OFF后，自动设防延时时间 默认180s
    xy_info.lbv = XY_DEFAULT_LBV; // 备用电池低电阈值 默认3.7V

    
    xy_info.mode = XY_TRACK_MODE3; // // 0默认 1正常状态 2ACCOFF 3休眠状态
	xy_info.heart_time = 180;	
    
    xy_info.vibsensity = DYNAMIC_DEFAULT_VIBSENSITY; // 震动灵敏度 越大越灵敏
    
    xy_info_save();

    xy_apn_dns_init();
#ifdef __XY_MILEAGE_SUPPORT__
    xy_mil_reset();
#endif
    return 1;
}

/*******************************************************************
** 函数名:     xy_main_init
** 函数描述:   
** 参数:       
** 返回:       
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
    if (sys_info->ref_reset_type == RESET_TYPE_POWEROFF)
    {
        xy_info.reset_cnt = 0;
    }
    if (strlen((char*)xy_info.dev_num) == 0 || strncmp((char*)xy_info.dev_num,"FFFFFFFFFFFF",DEV_PHONE_NUM_DEFAULT_LEN) || strncmp((char*)xy_info.dev_num,"ffffffffffff",DEV_PHONE_NUM_DEFAULT_LEN))
    {
        memcpy(xy_info.dev_num,&app_cntx->imei[3],DEV_PHONE_NUM_DEFAULT_LEN);
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

