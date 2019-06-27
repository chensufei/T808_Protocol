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

/*******************************************************************
** 宏定义
********************************************************************/
#define M_T808_DEF_PROVICE_ID		(44)			/* 默认省域ID */
#define M_T808_DEF_CITY_ID			(307)			/* 默认市域ID */
#define M_T808_DEF_PRODUCT_ID		"19000"			/* 厂商ID */
#define M_T808_DEF_TERM_TYPE		"XY_DEVICE_T808"/* 默认终端类型 */
#define M_T808_DEF_VIN				"XY123456789012345"

/*******************************************************************
** 全局变量
********************************************************************/
/* 系统参数 */
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
** 函数名:     xy_t808_param_reset
** 函数描述:   初始化参数
** 参数:       
** 返回:       
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

	xy_info.t808_para.tcp_ack_overtime = 60;	/* TCP超时时候 */
	xy_info.t808_para.tcp_resend_cnt = 3;		/* tcp重传次数 */
	xy_info.t808_para.report_type = 0;			/* 位置汇报策略,位置信息上报方式，0：定时汇报；1：定距汇报；2：定时和定距汇报 */
	xy_info.t808_para.report_way = 0;			/* 位置汇报方案, 0：根据 ACC 状态； 1：根据登录状态和 ACC 状态，先判断登录状态，若登录再根据 ACC 状态 */

	xy_info.t808_para.sleep_freq = 3600;
	xy_info.t808_para.sos_freq = 5;				/* 紧急报警时间，默认是5s */
	xy_info.t808_para.def_distance = 300;		/* 缺省距离汇报间隔,默认是300m */
	xy_info.t808_para.sos_distance = 50;		/* 紧急距离汇报间隔,默认是50m */
	xy_info.t808_para.degree = 7;				/* 拐点补传角度, <180° */
	xy_info.t808_para.degree_switch =1;         /* 拐点补传开关, 默认开启 */
	xy_info.t808_para.degree_freq = 5;
	xy_info.t808_para.fencing_radius = 300;		/* 电子围栏半径（非法位移阈值），单位为米，默认300m */

	memset(xy_info.t808_para.txt_number, 0, sizeof(xy_info.t808_para.txt_number));		/* 文本号码，接收短信/报警 */

	//报警开关默认都打开
	xy_info.t808_para.alarm_sw.val = 0;				/* 报警开关字节，对应位置信息里面的报警位，1-代表屏蔽 */

	//短信报警默认都关闭
	xy_info.t808_para.alarm_sms_sw.val = 1;			/* 报警发送文本 SMS 开关，与位置信息汇报消息中的报警标志相对应，相应位为 1 则相应报警时发送文本 SMS */

	xy_info.t808_para.over_speed = 120;				/* 最高速度，单位为公里每小时(km/h)默认120km/h */
	xy_info.t808_para.speed_keep_time = 10;			/* 超速持续时间，单位为秒（s）默认10s */

	xy_info.t808_para.over_speed_pre_val = 110;		/* 超速持续时间，单位为秒（s），默认110km/h */

	xy_info.t808_para.hit_param_val = 0x0a01;		/* 碰撞报警参数设置：
	  												b7-b0： 碰撞时间，单位 4ms；
	  												b15-b8：碰撞加速度，单位 0.1g，设置范围在：0-79 之间，默认为10。 */
	xy_info.t808_para.roll_over_param_val = 30;		/* 侧翻报警参数设置:侧翻角度，单位 1 度，默认为 30 度。 */

	xy_info.t808_para.gpsMode = 1;   /* GPS定位模式，0 - 不支持GPS定位，1-支持GPS定位 */
	xy_info.t808_para.bdMode = 1;    /* bd定位模式，0 - 不支持GPS定位，1-支持GPS定位 */
}

/*******************************************************************
** 函数名:     xy_info_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_info_reset(void)
{
    APP_CNTX_T * app_cntx = dynamic_app_cntx_get();
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
    
    memset(dev_num, 0, sizeof(dev_num));
    memcpy(dev_num, xy_info.dev_num,strlen((char*)xy_info.dev_num));
	
    memset(&xy_info,0,sizeof(xy_info));
    memcpy(xy_info.server,server,strlen((char*)server));
    xy_info.port = port;

	if (strlen((char *)dev_num) > 0)
	{
    	memcpy(xy_info.dev_num, dev_num, strlen((char*)dev_num));
	}
	else
	{
		memcpy(xy_info.dev_num, &app_cntx->imei[4], DEV_PHONE_NUM_DEFAULT_LEN);
	}
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

    
    xy_info.mode = XY_TRACK_MODE1; // // 0默认 1正常状态 2ACCOFF 3休眠状态
	xy_info.heart_time = 180;	
    
    xy_info.vibsensity = DYNAMIC_DEFAULT_VIBSENSITY; // 震动灵敏度 越大越灵敏

    xy_info.tzone = 8; 
    xy_info.acc_check_mode = 0;
	xy_info.acc_check_switch = 1;
	xy_info.lockmode = 1;
	xy_t808_param_reset();
	
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
    dynamic_debug("xy_info:%d,%s",len,xy_info.server);
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
        dynamic_error("服务器异常");
        xy_info_reset();
    }
    
    if (sys_info->ref_reset_type == RESET_TYPE_POWEROFF)
    {
        xy_info.reset_cnt = 0;
    }
    if (0 == strlen((char*)xy_info.dev_num))
    {
    	memset(xy_info.dev_num, 0, sizeof(xy_info.dev_num));
        memcpy(xy_info.dev_num, &app_cntx->imei[4], DEV_PHONE_NUM_DEFAULT_LEN);
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

