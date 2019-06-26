/********************************************************************************
**
** �ļ���:     xy_alm.c
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


#define XY_ACC_CHECK_CNT 3
#define XY_BREAK_CHECK_CNT 3
#define XY_BAT_ADC_CNT 8
#define XY_DEFAULT_LOW_BAT_VALUE 3700000
#define XY_ALM_OVER_SPEED_TIME 5


static kal_uint32 s_vin = 0; // ����ѹ ��λMV

/*******************************************************************
** ������:     xy_alm_check_mode
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_bool xy_alm_check_mode(XY_ALM_MODE_E mode,XY_ALM_SW_E type)
{
    kal_uint8 is_sw = KAL_FALSE;
    
    switch(type)
    {
        case XY_ALM_SW_GPRS:
            if (mode == XY_ALM_MODE_GPRS ||mode == XY_ALM_MODE_CALL_GPRS || 
                mode == XY_ALM_MODE_SMS_GPRS || mode == XY_ALM_MODE_SMS_GPRS_CALL)
            {
                is_sw = KAL_TRUE;
            }
        break;
        
        case XY_ALM_SW_SMS:
            if (mode == XY_ALM_MODE_SMS_GPRS || mode == XY_ALM_MODE_SMS_GPRS_CALL)
            {
                is_sw = KAL_TRUE;
            }

        break;
        
        case XY_ALM_SW_CALL:
            if (mode == XY_ALM_MODE_CALL_GPRS || mode == XY_ALM_MODE_SMS_GPRS_CALL)
            {
                is_sw = KAL_TRUE;
            }

        break;

        default:

        break;

    }

    return is_sw;
}

/*******************************************************************
** ������:     xy_alm_pro
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_pro(XY_ALM_TYPE_E type)
{
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint8 alm_sw = 0;
    
    switch(type)
    {
        case XY_ALM_TYPE_NONE:

        break;
    
        case XY_ALM_TYPE_ACC:
            alm_sw = xy_info->acc_p.sw;
        break;

        case XY_ALM_TYPE_POW_BREAK:
            alm_sw = xy_info->break_p.sw;
        break;  
		
		case XY_ALM_TYPE_VIB:
            alm_sw = xy_info->vib_p.sw;
		break;

		case XY_ALM_TYPE_OVER_SPEED:  
            alm_sw = xy_info->speed_p.sw;
		break;
		
		case XY_ALM_TYPE_MOVE:	
            alm_sw = xy_info->move_p.sw;
		break;
		
        case XY_ALM_TYPE_LOW_BAT:
            alm_sw = xy_info->bat_p.sw;
		break;
		
		case XY_ALM_TYPE_LOW_POW:

        break;                       
        
        default:
            dynamic_debug("xy_alm_pro err type");
            return;
        break;
    }

    if (alm_sw== 1)
    {
        if (xy_alm_check_mode(xy_info->vib_p.mode,XY_ALM_SW_SMS))
        {
            xy_sms_alm_assem(type);
        }

        if (xy_alm_check_mode(xy_info->vib_p.mode,XY_ALM_SW_GPRS))
        {
            xy_soc_alm_assem(type);
        }

        if (xy_alm_check_mode(xy_info->vib_p.mode,XY_ALM_SW_CALL))
        {
            xy_call_alm(type);
        }
    }
}


/*******************************************************************
** ������:     xy_alm_low_bat_alm
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_low_bat_alm(void)
{
    xy_alm_pro(XY_ALM_TYPE_LOW_BAT);
}

/*******************************************************************
** ������:     xy_alm_pow_break_alm
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_pow_break_alm(void)
{
     xy_alm_pro(XY_ALM_TYPE_POW_BREAK);
}

/*******************************************************************
** ������:     xy_alm_speed_alm
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_speed_alm(void)
{
    xy_alm_pro(XY_ALM_TYPE_OVER_SPEED);
}

/*******************************************************************
** ������:     xy_alm_move_alm
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_move_alm(void)
{
    xy_alm_pro(XY_ALM_TYPE_MOVE);
}

/*******************************************************************
** ������:     xy_alm_acc_alm
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_acc_alm(void)
{
    xy_alm_pro(XY_ALM_TYPE_ACC);
}

/*******************************************************************
** ������:     xy_alm_vib_alm
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_vib_alm(void)
{
    xy_alm_pro(XY_ALM_TYPE_VIB);
}

/*******************************************************************
** ������:     xy_alm_anti_thrief_position
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_anti_thrief_position(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
    
    if (xy_info->anti_thrief == 1 && (xy_info->lat == 0 || xy_info->lng == 0))
    {
        if (gps_info->valid)
        {
            xy_info->lat = gps_info->lat;
            xy_info->lng = gps_info->lng;
            xy_info_save();
        }
        else
        {
            dynamic_timer_start(enum_timer_anti_thrief_position_timer,5*1000,(void*)xy_alm_anti_thrief_position,NULL,FALSE);
        }
    }
}

/*******************************************************************
** ������:     xy_alm_antitheft_crl
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_antitheft_crl(kal_uint8 onoff)
{
    XY_INFO_T * xy_info = xy_get_info();

    xy_info->anti_thrief = onoff;
    xy_info->check_anti_thrief = 0;
    dynamic_debug("xy_alm_antitheft_crl:%d",xy_info->anti_thrief);
    if (xy_info->anti_thrief)
    {
        xy_info->lat = 0;
        xy_info->lng = 0;
        xy_alm_anti_thrief_position();
        //if (xy_info->lat == 0 || xy_info->lng == 0)
        //{
        //    dynamic_debug("���������GPS��ȡ��γ��");
        //    xy_track_wakeup();
        //}
    }
    else
    {
    }
    xy_info->vib_alm = 0;
    xy_info_save();
}

/*******************************************************************
** ������:     xy_acc_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_acc_init(void)
{
    GPIO_ModeSetup(XY_ACC_CHECK_PORT,0);   
    GPIO_InitIO(INPUT,XY_ACC_CHECK_PORT);
    GPIO_PullenSetup(XY_ACC_CHECK_PORT,1);
    GPIO_PullSelSetup(XY_ACC_CHECK_PORT,1);
}

/*******************************************************************
** ������:     xy_acc_check
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_acc_check(void)
{
    static kal_uint8 accon = 0;
    static kal_uint8 accoff =0;
    XY_INFO_T * xy_info = xy_get_info();

    
    if (GPIO_ReadIO(XY_ACC_CHECK_PORT) == 0)
    {
        accoff = 0;
        if (++accon >= XY_ACC_CHECK_CNT)
        {
            accon = 0;
            if (xy_info->acc_state == XY_ACC_OFF)
            {
                xy_info->acc_state = XY_ACC_ON;
                xy_info->acc_off_flg= 0;
                //xy_sensor_count_for_accon();
                xy_info_save();
                dynamic_debug("ACC ON!");
                xy_info->check_anti_thrief = 0;
                xy_soc_alm_assem(XY_ALM_TYPE_NONE);
                if (xy_info->anti_thrief)
                {
                    xy_alm_acc_alm();
                }
            }
        }
    }
    else
    {
        accon = 0;
        if (++accoff >= XY_ACC_CHECK_CNT)
        {
            accoff = 0;
            if (xy_info->acc_state == XY_ACC_ON)
            {
                applib_time_struct systime;
                kal_uint32 systime_sec = 0;

                dynamic_time_get_systime(&systime);
                systime_sec = dynamic_timer_time2sec(&systime);
                xy_info->accoff_sectime = systime_sec;
                xy_info->acc_off_flg = 1;

                xy_info->acc_state = XY_ACC_OFF;
                if (xy_info->anti_thrief == 0)
                {
                    xy_info->check_anti_thrief = 1;
                }
                xy_info_save();
                dynamic_gps_save_endgps();
                dynamic_debug("ACC OFF!");
                xy_soc_alm_assem(XY_ALM_TYPE_NONE);               
            }
        }
    }
}

/*******************************************************************
** ������:     xy_alm_pow_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_pow_init(void)
{
    GPIO_ModeSetup(XY_BREAK_CHECK_PORT,0);   
    GPIO_InitIO(INPUT,XY_BREAK_CHECK_PORT);
    //GPIO_PullenSetup(XY_BREAK_CHECK_PORT,1);
    //GPIO_PullSelSetup(XY_BREAK_CHECK_PORT,1);
}

/*******************************************************************
** ������:     xy_alm_pow_check
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_pow_check(void)
{
    static kal_uint8 break_cnt = 0;
    static kal_uint8 con_cnt =0;
    static kal_uint32 adc_sum = 0;
    static kal_uint32 adc_cnt = 0;
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint32 voltage;
    
    dynamic_adc_get_channel_voltage(DCL_PCBTMP_ADC_CHANNEL,&voltage);

    //dynamic_debug("voltage:%d",voltage);
    if ((GPIO_ReadIO(XY_BREAK_CHECK_PORT) != 0 ) || (voltage < 100000))
    {
        con_cnt = 0;
        if (++break_cnt >= XY_BREAK_CHECK_CNT)
        {
            break_cnt = 0;
            s_vin = 0;
            if (xy_info->break_state == XY_POW_CONNECT_STATE)
            {
                xy_info->break_state = XY_POW_BREAK_STATE;
                //xy_info->vin_alm = 0;
                dynamic_debug("pow break!");
                //xy_soc_alm_assem(XY_ALM_TYPE_POW_BREAK);
                xy_alm_pow_break_alm();
            }
        }
        adc_cnt = 0;
        adc_sum = 0;
    }
    else
    {
        break_cnt = 0;
        if (++con_cnt >= XY_BREAK_CHECK_CNT)
        {
            con_cnt = 0;
            if (xy_info->break_state == XY_POW_BREAK_STATE)
            {
                xy_info->break_state = XY_POW_CONNECT_STATE;
                dynamic_debug("pow connect!");
                xy_soc_alm_assem(XY_ALM_TYPE_NONE);
            }
        }

        if (xy_info->break_state == XY_POW_CONNECT_STATE)
        {            
            //dynamic_adc_get_channel_voltage(DCL_PCBTMP_ADC_CHANNEL,&voltage);
            if (voltage > 100000)
            {
                adc_sum += voltage;
                if (++adc_cnt >= XY_BAT_ADC_CNT)
                {
                    kal_uint32 vinoff = xy_info->vinoff * 1000;
                    kal_uint32 vinon = xy_info->vinon * 1000;
                    
                    adc_cnt = 0;
                    voltage = adc_sum/XY_BAT_ADC_CNT;
                    adc_sum = 0;
                    voltage = (double)voltage*37.451;
                    voltage = voltage/1000;
                    voltage = voltage*1.01725;

                    dynamic_debug("pow voltage:%dmv,vinoff:%d,vinon:%d",voltage,vinoff,vinon);
                    s_vin = voltage;
                    if (voltage < vinoff)
                    {
                        if (xy_info->vin_alm == 0)
                        {
                            xy_info->vin_alm = 1;
                            xy_info_save();
                        }
                    }
                    else if (voltage > vinon)
                    {
                        if (xy_info->vin_alm == 1)
                        {
                            xy_info->vin_alm = 0;
                            xy_info_save();
                        }
                    }
                }
            }
        }
    }
}

/*******************************************************************
** ������:     xy_realy_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_realy_init(void)
{
    XY_INFO_T * xy_info = xy_get_info();

    GPIO_ModeSetup(XY_REALY_CRL_PORT,0);   
    GPIO_InitIO(OUTPUT,XY_REALY_CRL_PORT);
    GPIO_WriteIO(xy_info->realy_crl,XY_REALY_CRL_PORT);
}

/*******************************************************************
** ������:     xy_alm_low_bat
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_low_bat(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    static kal_uint32 v_buf = 0;
    static kal_uint32 adc_value[XY_BAT_ADC_CNT];
    static kal_uint32 adc_cnt = 0;
    kal_uint32 voltage = 0;
    kal_uint32 i,j,temp;
    
    dynamic_adc_get_channel_voltage(DCL_VBAT_ADC_CHANNEL,&voltage);
    adc_value[adc_cnt] = voltage;
    if (++adc_cnt >= XY_BAT_ADC_CNT)
    {
        adc_cnt = 0;
        for(j=0;j<XY_BAT_ADC_CNT;j++)
        {
            for(i=0;i<(XY_BAT_ADC_CNT-1-j);i++)
            {
                if(adc_value[i]<adc_value[i+1])//����Ԫ�ش�С����������
                {
                    temp=adc_value[i];
                    adc_value[i]=adc_value[i+1];
                    adc_value[i+1]=temp;
                }
            }
        }
        voltage = adc_value[XY_BAT_ADC_CNT/2];
        if (xy_info->break_state == XY_POW_CONNECT_STATE)
        {
            if (voltage > v_buf)
            {
                xy_info->bat_value = voltage;
            }
        }
        else
        {
            if (v_buf > voltage)
            {
                xy_info->bat_value = voltage;
            }
        }
        dynamic_debug("cur bat value:%duv,break_state:%d",xy_info->bat_value,xy_info->break_state);
    }
  
    if (xy_info->bat_value == 0 || v_buf == 0)
    {
        xy_info->bat_value = voltage;
    }
    v_buf = xy_info->bat_value;

    if (xy_info->break_state == XY_POW_BREAK_STATE && xy_info->lbv > 0)
    {
        kal_uint32 lbv = (xy_info->lbv*1000000);

        if (xy_info->bat_value < lbv)
        {
            if (xy_info->bat_alm == 0)
            {
                dynamic_debug("bat_alm = 1");
                xy_info->bat_alm = 1;
                xy_info_save();
                xy_alm_low_bat_alm();                    
            }        
        }
    }
    else
    {
        if (xy_info->bat_alm == 1)
        {
            xy_info->bat_alm = 0;
            xy_info_save();
        }
    }
}


/*******************************************************************
** ������:     xy_alm_move_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_move_init(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    
    if (xy_info->anti_thrief)
    {
        xy_alm_anti_thrief_position();
    }
}

/*******************************************************************
** ������:     xy_alm_move_check
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_move_check(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
    
    if (xy_info->anti_thrief == 1 && xy_info->lat != 0 && xy_info->lng != 0)
    {
        if (gps_info->valid)
        {
            kal_uint32 distance;
            static kal_uint8 alm_cnt = 0;
            static kal_uint8 noalm_cnt = 0;
            
            distance = dynamic_gps_get_distance(gps_info->lat,gps_info->lng,xy_info->lat,xy_info->lng);
            if (xy_info->move_p.sw> 0 && distance > xy_info->move_p.para1)
            {
                noalm_cnt = 0;

                if (xy_info->move_alm == 0)
                {
                    if (++alm_cnt >= 5)
                    {
                        alm_cnt = 0;
                        xy_info->move_alm = 1;
                        xy_info_save();

                        xy_alm_move_alm();
                    }
                }
                dynamic_debug("distance:%d,xy_info->move:%d",distance,xy_info->move_p.sw);
            }
            else
            {
                alm_cnt = 0;
                if (xy_info->move_alm == 1)
                {
                    if (++noalm_cnt >= 5)
                    {
                        noalm_cnt = 0;
                        xy_info->move_alm = 0;
                        xy_info_save();
                    }
                }
            }
        }
    }
    else
    {
        if (xy_info->move_alm == 1)
        {
            xy_info->move_alm = 0;
            xy_info_save();
        }
    }
}

/*******************************************************************
** ������:     xy_alm_speed_check
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_speed_check(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
    
    if (xy_info->speed_p.sw == 1)
    {
        static kal_uint8 alm_cnt = 0;
        static kal_uint8 noalm_cnt = 0;
        
        if (xy_info->speed_p.para1 > 0 && gps_info->valid && gps_info->kmspeed > xy_info->speed_p.para1)
        {
            noalm_cnt = 0;
            if (xy_info->speed_alm == 0)
            {
                if (++alm_cnt >= XY_ALM_OVER_SPEED_TIME)
                {
                    alm_cnt = 0;
                    xy_info->speed_alm = 1;
                    xy_info_save();
                    xy_alm_speed_alm();
                }
            }
        }
        else
        {
            alm_cnt = 0;
            if (xy_info->speed_alm == 1)
            {
                if (++noalm_cnt >= XY_ALM_OVER_SPEED_TIME)
                {
                    noalm_cnt = 0;
                    xy_info->speed_alm = 0;
                    xy_info_save();
                }
            }
        }
    }
    else
    {
        if (xy_info->speed_alm == 1)
        {
            xy_info->speed_alm = 0;
            xy_info_save();
        }
    }
}

/*******************************************************************
** ������:     xy_alm_anti_thrief_check
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_anti_thrief_check(void)
{
    XY_INFO_T * xy_info = xy_get_info();
    static kal_uint16 delay_time = 0;

    if (xy_info->autolock == 1 && xy_info->anti_thrief == 0 && xy_info->acc_state == 0)
    {
        if (xy_info->check_anti_thrief == 1)
        {
            if (++delay_time >= xy_info->timetolock)
            {
                delay_time = 0;
                dynamic_debug("�Զ����");
                xy_alm_antitheft_crl(1);
            }
        }
        else
        {
            delay_time = 0;
        }
    }
    else
    {
        delay_time = 0;
        xy_info->check_anti_thrief = 0;
    }
}

/*******************************************************************
** ������:     xy_alm_task
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_task(void*str)
{
    xy_acc_check();
    xy_alm_pow_check();
    xy_alm_low_bat();
    xy_alm_move_check();
    xy_alm_speed_check();
    xy_alm_anti_thrief_check();

    dynamic_timer_start(enum_timer_alm_task_timer,1*1000,(void*)xy_alm_task,NULL,FALSE);
}

/*******************************************************************
** ������:     xy_key_cb
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_key_cb(kal_int16 key_code,kal_int16 key_type)
{
    dynamic_debug("xy_key_cb:%d,%d",key_code,key_type);
    if (key_code == KEY_VOL_UP)
    {

    }
}

/*******************************************************************
** ������:     xy_alm_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_alm_init(void)
{    
    xy_alm_pow_init();
    xy_realy_init();
    xy_alm_move_init();
	//dynamic_key_set_cb((void*)xy_key_cb); // sosӲ���ϲ�ʹ�ü��̿ڣ�Ӧ��ʹ���жϻ���ͨGPIOʵ��
    dynamic_timer_start(enum_timer_alm_task_timer,5*1000,(void*)xy_alm_task,NULL,FALSE);
}

#endif

