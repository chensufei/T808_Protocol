/********************************************************************************
**
** 文件名:     xy_main.h
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

#define XY_DEFAULT_MODE1_FREQ 10 // 模式1上报间隔 单位s
#define XY_DEFAULT_MODE2_FREQ 120 // 模式2上报间隔
#define XY_DEFAULT_SLEEP_DELAY_TIME 180 // 休眠唤醒时间 默认180秒
#define XY_DEFAULT_MOVE 200 // 位移报警半径，默认200m
#define XY_DEFAULT_SPEED 60 // 超速报警阈值 默认60km/h
#define XY_DEFAULT_SPEED_TIME 20 // 超速报警时间阀值20s
#define XY_DEFAULT_STOP_TIME 1200 // 停留报警阈值 默认1200s

#define XY_DEFAULT_VINOFF 11.5 // 主动切断外电的电压值 默认11.5V
#define XY_DEFAULT_VINON 11.8 // 主动恢复外电的电压值 默认11.8V
#define XY_DEFAULT_TIMETOLOCK 180  // ACC OFF后，自动设防延时时间 默认180s
#define XY_DEFAULT_LBV 3.7 // 备用电池低电阈值 默认3.7V


#define T808_PRODUCT_ID_LEN		(5)		/* 制造商ID长度 */
#define T808_DEV_TYPE_LEN		(20)	/* 终端型号长度 */
#define T808_DEV_ID_LEN			(7)		/* 终端 ID长度 */
#define T808_VEH_ID_LEN			(17)	/* 车辆标识ID长度 */
#define T808_AUTH_CODE_LEN		(64)	/* 鉴权码长度 */


typedef enum
{
    XY_ALM_SW_GPRS,
    XY_ALM_SW_SMS,
    XY_ALM_SW_CALL
}XY_ALM_SW_E; // 用于判断报警模式

typedef enum
{
    XY_ALM_MODE_GPRS,
    XY_ALM_MODE_SMS_GPRS,
    XY_ALM_MODE_CALL_GPRS,
    XY_ALM_MODE_SMS_GPRS_CALL,
    XY_ALM_MODE_MAX
}XY_ALM_MODE_E; // 报警方式

typedef struct _alm_mode_
{
    kal_uint8 sw;
    kal_uint16 para1; // 参数1，比如位移报警的距离,超速报警速度等
    kal_uint16 para2;
    kal_uint16 para3;
    kal_uint16 para4;
    XY_ALM_MODE_E mode; 
}XY_ALM_PARA_T;


/* 终端注册 */
typedef struct _t808_para_
{
    kal_uint8 is_regster; // 是否注册
    kal_uint16 provice_id;							/* 省域ID */
    kal_uint16 city_id;								/* 市域ID*/
    kal_uint8 product_id[T808_PRODUCT_ID_LEN+1];	/* 终端制造商编码 */
    kal_uint8 dev_model[T808_DEV_TYPE_LEN+1];	/* 终端型号,20 个字节，此终端型号由制造商自行定义，位数不足时，后补“0X00” */
    kal_uint8 dev_id[T808_DEV_ID_LEN+1];			/* 7 个字节，由大写字母和数字组成，此终端 ID 由制造商自行定义，位数不足时，后补“0X00” */
    kal_uint8 veh_color;							/* 车牌颜色 默认0*/
    kal_uint8 veh_id[T808_VEH_ID_LEN+1];			/* 车牌默认0*/
    kal_uint8 auth_code[T808_AUTH_CODE_LEN+1];		    /* 鉴权码信息 */
}T808_PARA_T;

typedef struct _xy_info_
{
    kal_uint8 server[XY_URL_LEN+1];
    kal_uint16 port;
    kal_uint8 dev_num[MAX_PHONE_NUM_LEN+1]; // 设备手机号 默认使用IMEI 4~14位的值
    kal_uint8 user[MAX_PHONE_NUM_LEN+1]; // 用户手机号
    kal_uint8 sosnum[MAX_SOS_NUM_CNT][MAX_PHONE_NUM_LEN+1];		
    kal_uint8 psw[XY_PASSWORD_LEN+1]; // 密码

    kal_uint8 apn[MAX_GPRS_APN_LEN]; //APN asc码
    kal_uint8 user_name[MAX_GPRS_USER_NAME_LEN];//用户名(可选)
    kal_uint8 password[MAX_GPRS_PASSWORD_LEN];//密码(可选)
    kal_uint8 dns1[MAX_GPRS_IP_ADDR]; //十进制
    kal_uint8 dns2[MAX_GPRS_IP_ADDR]; //十进制

    T808_PARA_T t808_para;
    
    kal_uint8 reset_cnt; // 检测不到SIM卡重启次数
    kal_uint32 heart_time;//心跳时间
    kal_uint32 freq; // 模式1上报间隔 默认10s
    kal_uint32 freq2; // 模式2上报间隔 默认120s
    kal_uint8 slp_sw; // 休眠使能开关，模式2持续时间结束后进入休眠
    kal_uint32 slp_d_t; // 休眠延时时间 默认180秒

    kal_uint8 mode; // 0默认 1正常状态 2ACCOFF 3休眠状态
    kal_uint8 anti_thrief; // 是否设防
    kal_uint8 check_anti_thrief; // 开始检测自动设防
    kal_uint8 autolock; // 自动设防开关 默认0
    kal_uint32 timetolock; // ACC OFF后，自动设防延时时间 默认180s
    double lat; // 设防后的经纬度
    double lng;
    
    XY_ALM_PARA_T vib_p; // 震动报警参数
    kal_uint8 vib_alm; // 震动报警
    kal_uint8 vibsensity; // 震动灵敏度 0~2 低中高 越大越灵敏
    
    XY_ALM_PARA_T acc_p; // 非法ACC报警参数
    kal_uint8 acc_alm; // ACC报警
    kal_uint8 acc_state; // ACC状态
    
    XY_ALM_PARA_T move_p; // 位移报警参数
    kal_uint8 move_alm; // 位移报警

    XY_ALM_PARA_T speed_p; // 超速报警参数
    kal_uint8 speed_alm; // 超速报警

    XY_ALM_PARA_T break_p; // 外电断开报警参数
    kal_uint8 break_alm; // 外电断开报警
    kal_uint8 break_state; // 剪线状态

    XY_ALM_PARA_T bat_p; // 电池报警参数
    kal_uint8 bat_alm; // 电池低电报警
    kal_uint32 bat_value; // 电池电压值 单位uv
    float lbv; // 备用电池低电阈值 默认3.7V
    
    float vinoff; // 外电低电压值 默认11.5V
    float vinon; // 外电电压恢复值 默认11.8V
    kal_uint8 vin_alm; // 外电低报警

    kal_uint8 realy_crl; // 继电器控制
    kal_uint32 slp_sectime; // 休眠时的时间
    kal_uint8 acc_off_flg; // ACC 从on变为off
    kal_uint8 sport_state; // 运动状态
    kal_uint32 sport_sectime; // 最后检测到运动的时间秒
    kal_uint32 accoff_sectime; // 熄火的时间
    kal_uint32 mileage; // 里程
    
    kal_uint32 temp[10];
    
    kal_uint32 checksum; // 累加和
} XY_INFO_T;

/*******************************************************************
** 函数名:     xy_get_para_from_ascii
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_get_para_from_ascii(char *str, char *dtr,kal_uint8 num,const kal_uint8 para_len);
/*******************************************************************
** 函数名:     xy_get_info
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
XY_INFO_T * xy_get_info(void);
/*******************************************************************
** 函数名:     xy_info_save
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_info_save(void);
/*******************************************************************
** 函数名:     xy_info_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_info_reset(void);

/*******************************************************************
** 函数名:     xy_main_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_main_init(void);
#endif
#endif
