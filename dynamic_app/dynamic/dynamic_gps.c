/********************************************************************************
**
** 文件名:     dynamic_gps.c
** 版权所有:   
** 文件描述:   gps
**
*********************************************************************************
**             修改历史记录
**===============================================================================
**| 日期       | 作者   |  修改记录
**===============================================================================
**|  |  | 
*********************************************************************************/

#include "dynamic_init.h"


typedef enum
{
    ENUM_GPS_AIDING_INIT,
    ENUM_GPS_EPO_AIDING,
    ENUM_GPS_REFLOC_AIDING,
    ENUM_GPS_EPO_ERR
}ENUM_GPS_AIDING_E;

static kal_uint8 s_aiding_state = ENUM_GPS_AIDING_INIT; 
static kal_uint8 s_aid_cnt = 0;

static nmea_data_t s_gps_data;
static dynamic_gps_info_t s_gps_info;
static kal_uint8 s_is_inflection_point = 0;

#if 0
/*
static char s_test_data_buf[MAX_GPS_RECV_SIZE]={
"$GPRMC,033925.000,A,1046.8558,N,10638.8781,E,68.00,186.98,130516,,,D*6D\r\n\
$GPVTG,186.98,T,,M,0.00,N,0.00,K,D*36\r\n\
$GPGGA,033925.000,3150.8558,N,11711.8781,E,2,15,0.76,141.0,M,0.0,M,,*56\r\n\
$GPGSA,A,3,193,29,26,03,27,16,31,32,22,14,23,,1.43,0.76,1.21*35\r\n\
$GPGSV,4,1,13,26,74,341,51,16,59,257,43,31,50,060,50,14,27,146,42*76\r\n\
$GPGSV,4,2,13,23,23,317,36,27,23,184,29,03,22,270,37,32,22,150,20*7A\r\n\
$GPGSV,4,3,13,22,21,243,35,29,17,043,34,40,14,254,29,193,09,170,30*4F\r\n\
$GPGSV,4,4,13,21,02,104,*4F\r\n\
$GPGLL,3150.8558,N,11711.8781,E,033925.000,A,D*54\r\n"
};
*/
#endif


#define PI 3.1415926  
#define EARTH_RADIUS   6300.137 // 地球近似半径  6378.137


static kal_uint8 s_gps_log = 0;

/*******************************************************************
** 函数名:     dynamic_gps_set_log
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_set_log(kal_uint8 log)
{
    s_gps_log = log;
}

/****************************************************
函数名:ang_is_qujian
功能:判断a 是否在b+c,  b-c之间
参数:
int index, int c,int r
角度 0-359
void *puser
返回值:
*****************************************************/
int ang_is_qujian(int a, int b,int c)
{
	int nMin;
		
	nMin = abs(b -a);
	if (nMin > 180)
	{
		if ( a > b)
		{
			if (( b+360 - a) < c)
				return 1;
		}
		else
		{
			if (( a+360 - b) < c)
				return 1;
		}
	}
	else
	{
		if (a>=b)
		{
			if((a-b)<c)
				return 1;
		}
		else
		{
			if((b-a)<c)
				return 1;
		}
	}

	return 0;
}

/*******************************************************************
** 函数名:     dynamic_gps_check_inflection
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_check_inflection( void)
{
	 dynamic_gps_info_t *pGpsInfo = dynamic_gps_get_info();
	 static unsigned char timeIndex = 0;
	 static dynamic_gps_info_t stLastGpsInfo = {0};
	 int nDegree = 0;

	if ( dynamic_gps_is_fix() != TRUE)
	{
		s_is_inflection_point = 0;
		memset( &stLastGpsInfo, 0, sizeof( dynamic_gps_info_t));	
		return ;
	}

	if(dynamic_gps_is_fix())
	{
		nDegree = ang_is_qujian( stLastGpsInfo.degree,	pGpsInfo->degree,  7);
		if ( nDegree == 0 && pGpsInfo->kmspeed > 8)			// 拐点
		{
			 timeIndex = 0;
			 if (s_is_inflection_point == 0)
			 {
			    s_is_inflection_point = 1;
#ifdef __XY_SUPPORT__
			    dynamic_timer_start(enum_timer_track_task_timer,100,(void*)xy_track_dataup_task,NULL,FALSE);
#endif
			 }
		}
		else
		{
			 timeIndex++;
			 if(timeIndex > 4)
			 {	
				if(s_is_inflection_point == 1)
				//dynamic_debug(" %s, %s, %d, InflectionPoint end \r\n", VFILE_FUN_LINE);
				s_is_inflection_point = 0;
				timeIndex = 0;
			 }		
		}
	}
	memset( &stLastGpsInfo, 0, sizeof( dynamic_gps_info_t));
	memcpy( &stLastGpsInfo, pGpsInfo, sizeof( dynamic_gps_info_t));
}

/*******************************************************************
** 函数名:     dynamic_gps_is_inflection_point
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_gps_is_inflection_point(void)
{
    return s_is_inflection_point;
}

/*******************************************************************
** 函数名:     dynamic_gps_get_nmea
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
double gps_get_radian(double d)  
{  
	return (d) * PI / 180.0;   //角度1? = π / 180  
} 

/*******************************************************************
** 函数名:     dynamic_gps_get_nmea
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint32 dynamic_gps_get_distance(double lat1,double lon1, double lat2, double lon2)
{	
	double radLat1 =0.00000000000;  
	double radLat2 = 0.00000000000;  
	double a = 0.00000000000;  
	double b = 0.00000000000; 
	double dst = 0.00000000000; 
	kal_uint32 distance1 = 0;
	radLat1 = gps_get_radian(lat1);  
	radLat2 = gps_get_radian(lat2);  
	if(radLat1 > radLat2)
		a = radLat1 - radLat2;  
	else
	{
		a = radLat2 - radLat1;  
	}
	if(lon1 > lon2)
		b = gps_get_radian(lon1) - gps_get_radian(lon2); 
	else
	{				
		b = gps_get_radian(lon2) - gps_get_radian(lon1); 
	}
	
	dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));  
	dst = dst * EARTH_RADIUS*1000;  
	distance1 = (kal_uint32)dst;

	return distance1;  
}

/*******************************************************************
** 函数名:     dynamic_gps_get_nmea
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
nmea_data_t *dynamic_gps_get_nmea(void)
{
    return &s_gps_data;
}

/*******************************************************************
** 函数名:     dynamic_gps_get_pow_state
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_bool dynamic_gps_get_pow_state(void)
{
    return dynamic_gps_get_state();
}

/*******************************************************************
** 函数名:     dynamic_gps_is_fix
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_bool dynamic_gps_is_fix(void)
{
    if (dynamic_gps_get_pow_state() == 1 && s_gps_info.valid == 1)
    {
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************
** 函数名:     dynamic_gps_put_bytes
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint16 dynamic_gps_put_bytes(kal_uint8 *data, kal_uint32 len)
{
    return dynamic_uart_put_bytes(GPS_UART_PORT,(char*)data,len);
}

/*******************************************************************
** 函数名:     dynamic_gps_get_info
** 函数描述:   获取已解析的GPS数据
** 参数:       
** 返回:       
********************************************************************/
dynamic_gps_info_t *dynamic_gps_get_info(void)
{
    return &s_gps_info;
}

/*******************************************************************
** 函数名:     dynamic_gps_save_endgps
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_save_endgps(void)
{
    DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();

    if (sys_info->end_gps.lat != 0.0 && sys_info->end_gps.lng != 0.0)
    {
        dynamic_debug("末次定位保存lat:%f,lng:%f",sys_info->end_gps.lat,sys_info->end_gps.lng);
        dynamic_sys_info_save();
    }
}

/*******************************************************************
** 函数名:     dynamic_gps_recv_cb
** 函数描述:   gps数据接收回调，默认1s更新一次数据
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_recv_cb (kal_uint8* data,kal_uint16 len)
{
    DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
    static kal_uint8 gps_filter = 0;
    static kal_uint8 satnum_cnt = 0;
    static kal_uint16 invalid = 0;
    static kal_uint8 check_time = 0;
	kal_uint32 systime_sec =0;
    kal_uint32 utc_sec = 0;
    applib_time_struct systime; 
    
    if (s_gps_log)
    {
        dynamic_log("%s",data);
    }

    memcpy(&s_gps_data,dynamic_gps_get_info_src(),sizeof(nmea_data_t));
    memcpy(&s_gps_info,dynamic_gps_get_gps_info(),sizeof(dynamic_gps_info_t));
    
    // 更新GPS数据
    if (dynamic_gps_get_pow_state() && s_gps_data.tag_rmc.tag_status == 'A')
    {
        if (gps_filter < 3)
        {
            gps_filter++;
            return;
        }

        if (s_gps_data.tag_gga.tag_sat_in_fix <= 5)
        {
            satnum_cnt = 0;
        }

        if (sys_info->end_gps.lat != 0 && sys_info->end_gps.lng != 0)
        {
            kal_uint32 distance;
            kal_uint32 time_temp,time1,time2;
            applib_time_struct gpstime;
            
            distance = dynamic_gps_get_distance(s_gps_data.tag_rmc.tag_latitude,s_gps_data.tag_rmc.tag_longitude,sys_info->end_gps.lat,sys_info->end_gps.lng);
            
        	gpstime.nYear = s_gps_data.tag_rmc.tag_utc_date.tag_year+2000;
        	gpstime.nMonth = s_gps_data.tag_rmc.tag_utc_date.tag_month;
        	gpstime.nDay = s_gps_data.tag_rmc.tag_utc_date.tag_day;	
        	gpstime.nHour = s_gps_data.tag_rmc.tag_utc_time.tag_hour;
        	gpstime.nMin = s_gps_data.tag_rmc.tag_utc_time.tag_minute;
        	gpstime.nSec = s_gps_data.tag_rmc.tag_utc_time.tag_second;
            time1 = dynamic_timer_time2sec(&sys_info->end_gps.mtime);
            time2 = dynamic_timer_time2sec(&gpstime);
            //dynamic_debug("distance:%d,time2:%d,time1:%d",distance,time2,time1);
            
            if (time2 >= time1)
            {
                time_temp = time2 - time1;
                if (time_temp == 0)
                {
                    time_temp = 1;
                }
                
                //dynamic_debug("time_temp:%d",time_temp);
                if (distance/time_temp > 70)
                {
                    if (s_gps_data.tag_gga.tag_sat_in_fix > 5)
                    {
                        if (satnum_cnt < 3)
                        {
                            satnum_cnt++;
                            dynamic_debug("\r\nerr speed 1\r\n");
                            return;
                        }
                    }
                    else
                    {
                        satnum_cnt = 0;
                        if (invalid < 300)
                        {
                            invalid++;
                            dynamic_debug("\r\nerr speed 2\r\n");
                            return;
                        }
                    }
                }
            }
        }
        else
        {
            dynamic_debug("第一个点");
        }
        invalid = 0;
        satnum_cnt = 0;
        
        memset(&s_gps_info,0,sizeof(s_gps_info));

        // UTC 时间
    	s_gps_info.mtime.nYear = s_gps_data.tag_rmc.tag_utc_date.tag_year+2000;
    	s_gps_info.mtime.nMonth = s_gps_data.tag_rmc.tag_utc_date.tag_month;
    	s_gps_info.mtime.nDay = s_gps_data.tag_rmc.tag_utc_date.tag_day;	
    	s_gps_info.mtime.nHour = s_gps_data.tag_rmc.tag_utc_time.tag_hour;
    	s_gps_info.mtime.nMin = s_gps_data.tag_rmc.tag_utc_time.tag_minute;
    	s_gps_info.mtime.nSec = s_gps_data.tag_rmc.tag_utc_time.tag_second;

    	s_gps_info.altitude = s_gps_data.tag_gga.tag_altitude; // 海拔高度
    	s_gps_info.sat_num = s_gps_data.tag_gga.tag_sat_in_fix; // 卫星数
    	s_gps_info.hdop = s_gps_data.tag_gga.tag_h_precision; // 水平精度
    	memcpy(s_gps_info.lat_str,s_gps_data.tag_rmc.tag_lat_str,sizeof(s_gps_data.tag_rmc.tag_lat_str));// 字符串度分格式
    	memcpy(s_gps_info.lng_str,s_gps_data.tag_rmc.tag_lng_str,sizeof(s_gps_data.tag_rmc.tag_lng_str));
    	s_gps_info.lat= s_gps_data.tag_rmc.tag_latitude; // 纬度 度格式
    	s_gps_info.lng= s_gps_data.tag_rmc.tag_longitude; // 经度 度格式
    	s_gps_info.milespeed = s_gps_data.tag_rmc.tag_ground_speed; // 速度 海里
    	s_gps_info.kmspeed = (unsigned int)(s_gps_data.tag_rmc.tag_ground_speed*1.852); // 速度 公里
    	s_gps_info.degree = s_gps_data.tag_rmc.tag_trace_degree; // 角度
    	s_gps_info.d_lat = s_gps_data.tag_rmc.tag_north_south; // 北半球，南半球
    	s_gps_info.d_lng= s_gps_data.tag_rmc.tag_east_west; // 东经，西经
    	s_gps_info.valid = 1;

        memcpy(&sys_info->end_gps,&s_gps_info,sizeof(s_gps_info));
        sys_info->end_gps.milespeed = 0;
        sys_info->end_gps.kmspeed = 0;
        sys_info->end_gps.sat_num = 0;
        
        dynamic_gps_check_inflection();
        
#ifdef __XY_MILEAGE_SUPPORT__
        {
            XY_INFO_T * xy_info = xy_get_info();
            
            if (xy_info->acc_state)
            {
                xy_mil_sum();
            }    
        }
#endif
        if (check_time == 0)
        {
            check_time = 1;
            dynamic_time_get_systime(&systime);
            systime_sec = dynamic_timer_time2sec(&systime);
            utc_sec = dynamic_timer_time2sec(&s_gps_info.mtime);

            if (abs(systime_sec-utc_sec) > 60)
            {
                dynamic_debug("systime_sec:%d,server_sec:%d",systime_sec,utc_sec);
                dynamic_time_set_systime(&s_gps_info.mtime);
            }
        }
    }
    else
    {
        satnum_cnt = 0;
        invalid = 0;
        gps_filter = 0;
        s_gps_info.valid = 0;
        s_gps_info.degree = 0;
        s_gps_info.milespeed = 0;
        s_gps_info.kmspeed = 0;
        s_gps_info.sat_num = 0;        
    }
#if 0
    dynamic_debug("mtime:%02d-%02d-%02d %02d:%02d:%02d",s_gps_info.mtime.nYear,s_gps_info.mtime.nMonth,
        s_gps_info.mtime.nDay,s_gps_info.mtime.nHour,s_gps_info.mtime.nMin,s_gps_info.mtime.nSec);
    dynamic_debug("altitude:%d",s_gps_info.altitude);
    dynamic_debug("sat_num:%d",s_gps_info.sat_num);
    dynamic_debug("hdop:%f",s_gps_info.hdop);
    dynamic_debug("lat_str:%s",s_gps_info.lat_str);
    dynamic_debug("lng_str:%s",s_gps_info.lng_str);
    dynamic_debug("lat:%lf",s_gps_info.lat);
    dynamic_debug("lng:%lf",s_gps_info.lng);
    dynamic_debug("milespeed:%d",s_gps_info.milespeed);
    dynamic_debug("kmspeed:%d",s_gps_info.kmspeed);
    dynamic_debug("d_lat:%c",s_gps_info.d_lat);
    dynamic_debug("d_lng:%c",s_gps_info.d_lng);
    dynamic_debug("valid:%d",s_gps_info.valid);
#endif
}

/*******************************************************************
** 函数名:     dynamic_gps_aid_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_aid_task(void*str)
{
    kal_uint32 timevalue = 1000;
    applib_time_struct cur_time;
    dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
    //DYNAMIC_SYS_T * sys_info = dynamic_sys_get_info();
    
    dynamic_time_get_systime(&cur_time);

    if (dynamic_gps_get_state() == 0 && gps_info->valid == 1)
    {
        return;
    }

    if (cur_time.nYear >= 2019)
    {
        switch(s_aiding_state)
        {
            case ENUM_GPS_AIDING_INIT:
                if (dynamic_epo_is_valid() == 0)
                {
                    dynamic_gps_epo_req();
                    dynamic_debug("请求下载EPO");
                }
                dynamic_gps_time_aid(cur_time);
                dynamic_debug("时间辅助");
                
                s_aiding_state = ENUM_GPS_EPO_AIDING;
            break;
            
            case ENUM_GPS_EPO_AIDING:
            {
                kal_bool aid_res;

                if (dynamic_epo_is_valid())
                {
                    dynamic_debug("EPO辅助");
                    aid_res = dynamic_gps_epo_aiding();
                    dynamic_debug("EPO:%d",aid_res);
                    
                    s_aiding_state = ENUM_GPS_REFLOC_AIDING;
                    timevalue = 500;
                }
                else
                {
                    timevalue = 1000;
                    if (++s_aid_cnt >= 150)
                    {
                        s_aid_cnt = 0;
                        s_aiding_state = ENUM_GPS_AIDING_INIT;
                        dynamic_debug("EPO失败");
                        return;
                    }
                }
            }
            break;

            case ENUM_GPS_REFLOC_AIDING:
			#if 0
                sys_info->end_gps.lat = 22.56602;
                sys_info->end_gps.lng = 113.94841;
                if (sys_info->end_gps.lat != 0.0 && sys_info->end_gps.lng != 0.0)
                {
                    dynamic_debug("位置辅助");
                    dynamic_gps_refloc_aid(sys_info->end_gps.lat,sys_info->end_gps.lng,sys_info->end_gps.altitude);
                }
			#endif
                s_aiding_state = ENUM_GPS_AIDING_INIT;
                return;
            break;

            default:
                return;
            break;
        }
    }

    dynamic_timer_start(enum_timer_gps_aid_timer,timevalue,(void*)dynamic_gps_aid_task,NULL,FALSE);
}


/*******************************************************************
** 函数名:     dynamic_gps_pow_ctrl
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_pow_ctrl(kal_uint8 onoff)
{
    static kal_uint8 cur_state = 0xff;

    if (cur_state == onoff)
    {
        return;
    }

    cur_state = onoff;

    dynamic_debug("dynamic_gps_pow_ctrl:%d",onoff);
    if (onoff == 1)
    {
        if (dynamic_gps_get_state() == 0)
        {
            s_aid_cnt = 0;
            s_aiding_state = ENUM_GPS_AIDING_INIT;
            dynamic_timer_start(enum_timer_gps_aid_timer,1*1000,(void*)dynamic_gps_aid_task,NULL,FALSE);
        }
        dynamic_gps_pow_crl(1);
        dynamic_sleep_disable();
    }
    else
    {
#ifdef __XY_SUPPORT__
        XY_INFO_T * xy_info = xy_get_info();
        applib_time_struct systime;
        kal_uint32 systime_sec = 0;

        dynamic_time_get_systime(&systime);
        systime_sec = dynamic_timer_time2sec(&systime);
        xy_info->slp_sectime = systime_sec;
        xy_info_save();
#endif
        s_gps_info.valid = 0;
        s_gps_info.degree = 0;
        s_gps_info.milespeed = 0;
        s_gps_info.kmspeed = 0;
        s_gps_info.sat_num = 0;   
    
        dynamic_gps_pow_crl(0);
        dynamic_sleep_enable();
        
    }
}

/*******************************************************************
** 函数名:     dynamic_gps_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_task(void *str)
{
    static kal_uint8 onoff_buf = 0;
    static kal_uint8 save_endgps = 0; // 保存末次定位点
    dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
    DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
 
    if (onoff_buf != dynamic_gps_get_state())
    {
        onoff_buf = dynamic_gps_get_state();
        save_endgps = 1;
    }
    
    if (onoff_buf == 1)
    {
        if (gps_info->valid == 1)
        {
            memset(&sys_info->end_gps,0,sizeof(dynamic_gps_info_t));
            memcpy(&sys_info->end_gps,gps_info,sizeof(dynamic_gps_info_t));
        }
    }

    if (save_endgps == 1)
    {
        if (sys_info->end_gps.lat != 0.0 && sys_info->end_gps.lng != 0.0)
        {
            save_endgps = 0;
            dynamic_debug("末次定位保存lat:%f,lng:%f,altitude:%f",sys_info->end_gps.lat,sys_info->end_gps.lng,sys_info->end_gps.altitude);
            dynamic_sys_info_save();
        }
    }

    {
        //APP_CNTX_T *app_cntx = dynamic_app_cntx_get();
        //dynamic_debug("CSQ:%d",app_cntx->csq);
    }
    dynamic_timer_start(enum_timer_gps_timer,1000,(void*)dynamic_gps_task,NULL,FALSE);
}

/*******************************************************************
** 函数名:     dynamic_gps_pow_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_pow_init(void)
{
#ifdef __XY_SUPPORT__
    XY_INFO_T * xy_info = xy_get_info();
    
    if (xy_info->mode == XY_TRACK_MODE1)
    {
        dynamic_gps_pow_ctrl(1);
    }
    else
    {
        dynamic_gps_pow_ctrl(0);
    }
#else
    dynamic_gps_pow_ctrl(0);
#endif
}

/*******************************************************************
** 函数名:     dynamic_gps_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_gps_init(void)
{
    DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
    
    dynamic_gps_recv_cb_reg(dynamic_gps_recv_cb);
    if (sys_info->ref_reset_type == RESET_TYPE_POWEROFF)
    {
        applib_time_struct curtime;
        kal_uint32 curtime_sec,gpstime_sec;

        dynamic_time_get_systime(&curtime);
        curtime_sec = dynamic_timer_time2sec(&curtime);
        gpstime_sec = dynamic_timer_time2sec(&sys_info->end_gps.mtime);

        if ((curtime_sec < gpstime_sec) || (curtime_sec > gpstime_sec && (curtime_sec-gpstime_sec) > 3600))
        {
            dynamic_debug("reset end gps data:%d,%d",curtime_sec,gpstime_sec);
            memset(&sys_info->end_gps,0,sizeof(sys_info->end_gps));
            dynamic_sys_info_save();
        }
    }

    dynamic_gps_pow_init();
}

