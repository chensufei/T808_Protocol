/********************************************************************************
**
** 文件名:     xy_led.c
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

#define GSM_LED_CYC 30 // 3S周期
#define GPS_LED_CYC 30 // 3S周期

/*******************************************************************
** 函数名:     xy_led_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_led_task(void*str)
{
    //XY_INFO_T * xy_info = xy_get_info();
    kal_bool gsm_led = 0;
    kal_bool gps_led = 0;
    static kal_uint8 gms_led_cnt = GSM_LED_CYC;
    static kal_uint8 gps_led_cnt = GPS_LED_CYC;

    APP_CNTX_T *app_cntx = dynamic_app_cntx_get();
    if (dynamic_sim_get_valid() == 1 && app_cntx->csq > 0)
    {
        if (xy_soc_get_auth_ok_state() == 0)
        {
            gsm_led = 1;
        }
        else
        {
            if (++gms_led_cnt >= GSM_LED_CYC)
            {
                gms_led_cnt = 0;
                gsm_led = 1;
            }
        }
    }

    if (dynamic_gps_get_pow_state() == 1)
    {
        if (dynamic_gps_is_fix() == 0)
        {
            gps_led = 1;
        }
        else
        {
            if (++gps_led_cnt >= GPS_LED_CYC)
            {
                gps_led_cnt = 0;
                gps_led = 1;
            }
        }
    }

    GPIO_WriteIO(gsm_led,XY_GSM_LED_PORT);
    GPIO_WriteIO(gps_led,XY_GPS_LED_PORT);
}

/*******************************************************************
** 函数名:     xy_led_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_led_init(void)
{
    //DYNAMIC_SYS_T * sys_info = dynamic_sys_get_info();

    GPIO_ModeSetup(XY_GSM_LED_PORT,0);   
    GPIO_InitIO(OUTPUT,XY_GSM_LED_PORT);
    GPIO_WriteIO(0,XY_GSM_LED_PORT);

    GPIO_ModeSetup(XY_GPS_LED_PORT,0);   
    GPIO_InitIO(OUTPUT,XY_GPS_LED_PORT);
    GPIO_WriteIO(0,XY_GPS_LED_PORT);  

    dynamic_timer_start(enum_timer_led_timer,100,(void*)xy_led_task,NULL,TRUE);
}
#endif

