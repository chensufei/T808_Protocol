/********************************************************************************
**
** �ļ���:     dynamic_gps.c
** ��Ȩ����:   
** �ļ�����:   gps
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   |  �޸ļ�¼
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
#define EARTH_RADIUS   6300.137 // ������ư뾶  6378.137


static kal_uint8 s_gps_log = 0;

/*******************************************************************
** ������:     dynamic_gps_set_log
** ��������:   
** ����:       
** ����:       
********************************************************************/
void dynamic_gps_set_log(kal_uint8 log)
{
    s_gps_log = log;
}

/****************************************************
������:ang_is_qujian
����:�ж�a �Ƿ���b+c,  b-c֮��
����:
int index, int c,int r
�Ƕ� 0-359
void *puser
����ֵ:
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
** ������:     dynamic_gps_check_inflection
** ��������:   
** ����:       
** ����:       
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
		if ( nDegree == 0 && pGpsInfo->kmspeed > 8)			// �յ�
		{
			 timeIndex = 0;
			 if (s_is_inflection_point == 0)
			 {
			    s_is_inflection_point = 1;
#ifdef __XY_SUPPORT__
			    dynamic_timer_start(enum_timer_track_task_dataup_timer,100,(void*)xy_track_dataup_task,NULL,FALSE);
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
** ������:     dynamic_gps_is_inflection_point
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint8 dynamic_gps_is_inflection_point(void)
{
    return s_is_inflection_point;
}

/*******************************************************************
** ������:     dynamic_gps_get_nmea
** ��������:   
** ����:       
** ����:       
********************************************************************/
double gps_get_radian(double d)  
{  
	return (d) * PI / 180.0;   //�Ƕ�1? = �� / 180  
} 

/*******************************************************************
** ������:     dynamic_gps_get_nmea
** ��������:   
** ����:       
** ����:       
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
** ������:     dynamic_gps_get_nmea
** ��������:   
** ����:       
** ����:       
********************************************************************/
nmea_data_t *dynamic_gps_get_nmea(void)
{
    return &s_gps_data;
}

/*******************************************************************
** ������:     dynamic_gps_get_pow_state
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_bool dynamic_gps_get_pow_state(void)
{
    return dynamic_gps_get_state();
}

/*******************************************************************
** ������:     dynamic_gps_is_fix
** ��������:   
** ����:       
** ����:       
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
** ������:     dynamic_gps_put_bytes
** ��������:   
** ����:       
** ����:       
********************************************************************/
kal_uint16 dynamic_gps_put_bytes(kal_uint8 *data, kal_uint32 len)
{
    return dynamic_uart_put_bytes(GPS_UART_PORT,(char*)data,len);
}

/*******************************************************************
** ������:     dynamic_gps_get_info
** ��������:   ��ȡ�ѽ�����GPS����
** ����:       
** ����:       
********************************************************************/
dynamic_gps_info_t *dynamic_gps_get_info(void)
{
    return &s_gps_info;
}

/*******************************************************************
** ������:     dynamic_gps_save_endgps
** ��������:   
** ����:       
** ����:       
********************************************************************/
void dynamic_gps_save_endgps(void)
{
    DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();

    if (sys_info->end_gps.lat != 0.0 && sys_info->end_gps.lng != 0.0)
    {
        dynamic_debug("ĩ�ζ�λ����lat:%f,lng:%f",sys_info->end_gps.lat,sys_info->end_gps.lng);
        dynamic_sys_info_save();
    }
}


/*******************************************************************
** ������:     dynamic_gps_str_to_time
** ��������:   �ַ���ʱ��ת��Ϊʮ����ʱ��
** ����:
** ����:
********************************************************************/
static void dynamic_gps_str_to_time(uct_gps_nmea_utc_time_struct *lptime, char *str)
{
    lptime->tag_hour = (str[0] - 48) * 10 + str[1] - 48;
    lptime->tag_minute = (str[2] - 48) * 10 + str[3] - 48;
    lptime->tag_second = (str[4] - 48) * 10 + str[5] - 48;
    lptime->tag_millisecond = (str[7] - 48) * 10 + str[8] - 48;
}


/*******************************************************************
** ������:     dynamic_gps_str_to_date
** ��������:   �ַ�������ת��Ϊʮ��������
** ����:
** ����:
********************************************************************/
static void dynamic_gps_str_to_date(uct_gps_nmea_utc_date_struct *lptime, char *str)
{
    lptime->tag_year = (str[4] - 48) * 10 + str[5] - 48;
    lptime->tag_month = (str[2] - 48) * 10 + str[3] - 48;
    lptime->tag_day = (str[0] - 48) * 10 + str[1] - 48;
}

/*******************************************************************
** ������:     dynamic_gps_format_latlng
** ��������:   11808.0945ת��Ϊ118.1349
** ����:
** ����:
********************************************************************/
static double dynamic_gps_format_latlng(double laglong)
{
	int ret = (int)laglong / 100;
	double fen = ((int)laglong % 100) / 60.0;
	double miao = ((int)(laglong * 10000) % 10000) / 600000.00;

	return ret + fen + miao;
}

/*****************************************************************************
 * ������
 *  dynamic_gps_parse_rmc
 * ����
4�� Recommended Minimum Specific GPS/TRANSIT Data��RMC���Ƽ���λ��Ϣ
$GPRMC,060206.00,A,2236.91218,N,11403.24719,E,0.015,,130214,,,D*7E
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh<CR><LF>
<1> UTCʱ�䣬hhmmss��ʱ���룩��ʽ
<2> ��λ״̬��A=��Ч��λ��V=��Ч��λ
<3> γ��ddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<4> γ�Ȱ���N�������򣩻�S���ϰ���
<5> ����dddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<6> ���Ȱ���E����������W��������
<7> �������ʣ�000.0~999.9�ڣ�ǰ���0Ҳ�������䣩
<8> ���溽��000.0~359.9�ȣ����汱Ϊ�ο���׼��ǰ���0Ҳ�������䣩
<9> UTC���ڣ�ddmmyy�������꣩��ʽ
<10> ��ƫ�ǣ�000.0~180.0�ȣ�ǰ���0Ҳ�������䣩
<11> ��ƫ�Ƿ���E��������W������
<12> ģʽָʾ����NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч��

 GPRMC - �Ƽ���С��GNSS�������
typedef�ṹ
{
    ˫tag_latitude;  γ��
    ˫tag_longitude;  ����
    ��tag_ground_speed;  �Ե��ٶȣ�����
    ��tag_trace_degree;  ����ģʽ�ȣ�����Ϊ0
    ��tag_magnetic;
    uct_gps_nmea_utc_time_struct tag_utc_time;   UTCʱ��
    uct_gps_nmea_utc_date_struct tag_utc_date;   UTC����
    �����ַ�tag_status;  ״̬��V=�������ջ���
    �����ַ�tag_north_south;   N��S
    �����ַ�tag_east_west;   E��W
    �����ַ�tag_magnetic_e_w;  ��E��
    �����ַ�tag_cmode;  ģʽ
    �����ַ�tag_nav_status;  ����״̬
} uct_gps_nmea_rmc_struct;
 *  void
 *����ֵ
 *  void
 *****************************************************************************/
int  dynamic_gps_parse_rmc(uct_gps_nmea_rmc_struct *lpg_rmc, char *str_data, int strlen1)
{
    char *pstr = 0;
    char strtime[10] = {0};
    char strdate[10] = {0};

    pstr = strstr(str_data, "RMC,");
    if(lpg_rmc)
    {
        if(pstr)
        {
            int i = 0;
            char *tokens[12];
            char temp = 0;

            pstr += 4;

            while (pstr && i < 12)
            {
                if (*pstr == ',')
                {
                    tokens[i++] = &temp;
                }
                else
                {
                    tokens[i++] = pstr;
                }

                pstr = strchr(pstr, ',');
                if (pstr)
                {
                    pstr++;
                }
            }

            if (i == 12)
            {
                sscanf(tokens[0], "%9c", strtime);
                lpg_rmc->tag_status = *tokens[1];
                sscanf(tokens[2], "%lf", &lpg_rmc->tag_latitude);
                lpg_rmc->tag_north_south = *tokens[3];
                sscanf(tokens[4], "%lf", &lpg_rmc->tag_longitude);
                lpg_rmc->tag_east_west = *tokens[5];
                sscanf(tokens[6], "%f", &lpg_rmc->tag_ground_speed);
                sscanf(tokens[7], "%f", &lpg_rmc->tag_trace_degree);
                sscanf(tokens[8], "%6c", strdate);
                sscanf(tokens[9], "%f", &lpg_rmc->tag_magnetic);
                lpg_rmc->tag_magnetic_e_w = *tokens[10];
                lpg_rmc->tag_cmode = *tokens[11];

                sprintf(lpg_rmc->tag_lat_str, "%f", lpg_rmc->tag_latitude);
                sprintf(lpg_rmc->tag_lng_str, "%f", lpg_rmc->tag_longitude);

                lpg_rmc->tag_latitude = dynamic_gps_format_latlng(lpg_rmc->tag_latitude);
                lpg_rmc->tag_longitude = dynamic_gps_format_latlng(lpg_rmc->tag_longitude);
                if(*strtime)
                    dynamic_gps_str_to_time(&lpg_rmc->tag_utc_time, strtime);

                if(*strdate)
                    dynamic_gps_str_to_date(&lpg_rmc->tag_utc_date, strdate);
            }
            else
            {
                lpg_rmc->tag_latitude = 0;
                lpg_rmc->tag_longitude = 0;
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

#if 0
    dynamic_debug("latitude:%f,longitude:%f",s_gps_data.tag_rmc.tag_latitude,s_gps_data.tag_rmc.tag_longitude);
    dynamic_debug("tag_lat_str:%s,tag_lng_str:%s",s_gps_data.tag_rmc.tag_lat_str,s_gps_data.tag_rmc.tag_lng_str);
    dynamic_debug("speed:%f*1.852=%f,degree:%f,magnetic:%f",s_gps_data.tag_rmc.tag_ground_speed,(s_gps_data.tag_rmc.tag_ground_speed*1.852),
                    s_gps_data.tag_rmc.tag_trace_degree,s_gps_data.tag_rmc.tag_magnetic);
    dynamic_debug("utc_date:%02d,%02d,%02d",s_gps_data.tag_rmc.tag_utc_date.tag_year,
        s_gps_data.tag_rmc.tag_utc_date.tag_month,s_gps_data.tag_rmc.tag_utc_date.tag_day);
    dynamic_debug("utc_time:%02d,%02d,%02d",s_gps_data.tag_rmc.tag_utc_time.tag_hour,
        s_gps_data.tag_rmc.tag_utc_time.tag_minute,s_gps_data.tag_rmc.tag_utc_time.tag_second);
    dynamic_debug("status:%c,north_south:%c,east_west:%c,magnetic_e_w:%c",s_gps_data.tag_rmc.tag_status,
        s_gps_data.tag_rmc.tag_north_south,s_gps_data.tag_rmc.tag_east_west,s_gps_data.tag_rmc.tag_magnetic_e_w);
    dynamic_debug("cmod:%c,nav_status:%c",s_gps_data.tag_rmc.tag_cmode,s_gps_data.tag_rmc.tag_nav_status);
#endif
    
    return 1;
}

/*****************************************************************************
 * ������
 *  dynamic_gps_parse_gga
 * ����
 *1�� Global Positioning System Fix Data��GGA��GPS��λ��Ϣ
$GPGGA,060206.00,2236.91218,N,11403.24719,E,2,10,0.88,118.2,M,-2.4,M,,0000*45
$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*hh<CR><LF>
<1> UTCʱ�䣬hhmmss��ʱ���룩��ʽ
<2> γ��ddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<3> γ�Ȱ���N�������򣩻�S���ϰ���
<4> ����dddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<5> ���Ȱ���E����������W��������
<6> GPS״̬��0=δ��λ��1=�ǲ�ֶ�λ��2=��ֶ�λ��6=���ڹ���
<7> ����ʹ�ý���λ�õ�����������00~12����ǰ���0Ҳ�������䣩
<8> HDOPˮƽ�������ӣ�0.5~99.9��
<9> ���θ߶ȣ�-9999.9~99999.9��
<10> ������������Դ��ˮ׼��ĸ߶�
<11> ���ʱ�䣨�����һ�ν��յ�����źſ�ʼ��������������ǲ�ֶ�λ��Ϊ�գ�
<12> ���վID��0000~1023��ǰ���0Ҳ�������䣬������ǲ�ֶ�λ��Ϊ�գ�
 * ����˵��
typedef�ṹ
{
    ˫tag_latitude ; / *γ����< 0��> 0
    ˫tag_longitude ; / *������< 0��> 0
    ��tag_h_precision ; / *����ˮƽϡ��
    ˫tag_altitude ; / *���߸߶ȸ���/���ھ�ֵ��ƽ�棨���ˮ׼�棩
    ��tag_unit_of_altitude ;���߸߶�/ *��λ����
    ��tag_geoidal ; / *���ˮ׼����룬��WGS -84���֮�������
    ��tag_unit_of_geoidal ;���ˮ׼�����/ *��λ����
    ��tag_gps_age ; / *���GPS���ݣ�ʱ������Ϊ��λ��ȥ��SC104����
    �޷��Ŷ�tag_station_id ; / *��ֻ�׼վID �� 0000-1023
    �޷����ַ�tag_sat_in_fix ; / *���ǵ�ʹ������
    uct_gps_nmea_utc_time_struct tag_utc_time ; / *ʱ�䣨UTC ��
    �����ַ�tag_north_south ; / *�ϱ�
    �����ַ�tag_east_west ; / *�򶫻�����
    �����ַ�tag_quality ; / * GPS������ָ��
} uct_gps_nmea_gga_struct
 *  void
 *����ֵ
 *  void
 *****************************************************************************/
int  dynamic_gps_parse_gga(uct_gps_nmea_gga_struct *lpgga, char *str_gps_data, int strlen1)
{
    char *pstr;
    char strtime[20];

    memset(strtime, 0, sizeof(strtime));

    pstr = strstr(str_gps_data, "GGA,");

    if(lpgga && pstr)
    {
        int i = 0;
        char *tokens[13];
        char temp = 0;
        
        pstr += 4;

        while (pstr && i < 13)
        {
            if (*pstr == ',')
            {
                tokens[i++] = &temp;
            }
            else
            {
                tokens[i++] = pstr;
            }
            pstr = strchr(pstr, ',');
            if (pstr)
                pstr++;
        }

        if (i == 13)
        {
            sscanf(tokens[0], "%9c", strtime);
            sscanf(tokens[1], "%lf", &lpgga->tag_latitude);
            lpgga->tag_north_south = *tokens[2];
            sscanf(tokens[3], "%lf", &lpgga->tag_longitude);
            lpgga->tag_east_west = *tokens[4];
            lpgga->tag_quality = atoi(tokens[5]);
            lpgga->tag_sat_in_fix = atoi(tokens[6]);
            sscanf(tokens[7], "%f", &lpgga->tag_h_precision);
            sscanf(tokens[8], "%lf", &lpgga->tag_altitude);
            // *lpgga->tag_unit_of_altitude = *tokens[9];
            sscanf(tokens[10], "%f", &lpgga->tag_geoidal);
            // *lpgga->tag_unit_of_geoidal = tokens[11];
            sscanf(tokens[12], "%f", &lpgga->tag_gps_age);

            lpgga->tag_latitude = dynamic_gps_format_latlng(lpgga->tag_latitude);
            lpgga->tag_longitude = dynamic_gps_format_latlng(lpgga->tag_longitude);
            if(*strtime)
                dynamic_gps_str_to_time(&lpgga->tag_utc_time, strtime);
        }
        else
        {
            lpgga->tag_latitude = 0;
            lpgga->tag_longitude = 0;
            return 0;
        }
    }
    else
    {
        return 0;
    }
    
#if 0
    dynamic_debug("sat_in_fix:%d,h_precision:%f,altitude:%lf",s_gps_data.tag_gga.tag_sat_in_fix,
            s_gps_data.tag_gga.tag_h_precision,s_gps_data.tag_gga.tag_altitude);
#endif
    return 1;

}

/*****************************************************************************
 * ������
 *  dynamic_gps_parse_gsa
 * ����
 *2�� GPS DOP and Active Satellites��GSA����ǰ������Ϣ
$GPGSA,<1>,<2>,<3>,<3>,<3>,<3>,<3>,<3>,<3>,<3>,<3>,<3>,<3>,<3>,<4>,<5>,<6>*hh<CR><LF>
<1> ģʽ��M=�ֶ���A=�Զ�
<2> ��λ���ͣ�1=û�ж�λ��2=2D��λ��3=3D��λ
<3> PRN�루α��������룩���������ڽ���λ�õ����Ǻţ�01~32��ǰ���0Ҳ�������䣩��
<4> PDOPλ�þ������ӣ�0.5~99.9��
<5> HDOPˮƽ�������ӣ�0.5~99.9��
<6> VDOP��ֱ�������ӣ�0.5~99.9��
 * ����˵��
	 float   tag_pdop;       PDOP in meters
    float   tag_hdop;       HDOP in meters
    float   tag_vdop;       VDOP in meters
    kal_uint16     tag_sate_id[12]; ID of satellites
    signed char     tag_op_mode;      Selection mode: A=auto M=manual
    signed char      tag_fix_mode;     Mode
 *  void
 *����ֵ
 *  void
 *****************************************************************************/
int  dynamic_gps_parse_gsa(uct_gps_nmea_gsa_struct *lpgsa, char *str_gps_data, int strlen1)
{
    char *pstr = 0;
    kal_uint8 i = 0,fix_num = 0;
    kal_uint8 gsa_num = 0;

    if (lpgsa == NULL)
    {
        return 0;
    }
    
    pstr = strstr(str_gps_data, "GSA,");
    while(pstr)
    {
        memset(lpgsa, 0, sizeof(uct_gps_nmea_gsa_struct)*4);
        
        pstr += 4;
        sscanf(pstr, "%c,%c,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%f,%f,%f", //
            &lpgsa->tag_op_mode,
            &lpgsa->tag_fix_mode,
            &lpgsa->tag_sate_id[0],
            &lpgsa->tag_sate_id[1],
            &lpgsa->tag_sate_id[2],
            &lpgsa->tag_sate_id[3],
            &lpgsa->tag_sate_id[4],
            &lpgsa->tag_sate_id[5],
            &lpgsa->tag_sate_id[6],
            &lpgsa->tag_sate_id[7],
            &lpgsa->tag_sate_id[8],
            &lpgsa->tag_sate_id[9],
            &lpgsa->tag_sate_id[10],
            &lpgsa->tag_sate_id[11],
            &lpgsa->tag_pdop,
            &lpgsa->tag_hdop,
            &lpgsa->tag_vdop);

        for(i = 0; i < 12; i++)
        {
            if(lpgsa->tag_sate_id[i] > 0)
                fix_num++;
        }

        pstr = strstr(pstr, "GSA,");
        gsa_num++;
        lpgsa += 1;
    }
#if 1
    dynamic_debug("GSA NUM:%d,FIX NUM:%d",gsa_num,fix_num);
#endif

    return 1;
}

kal_uint16 CharToInt(char *src, kal_uint8 len)
{
	kal_uint16 Idata;
	kal_uint8 i;
	
	Idata =0;
	for(i =0; i<len; i++)
	{
		if(src[i] == 0x0a || src[i] == 0x0d || src[i] == 0x20 || src[i] == 0x09)
			continue;
		if(src[i] < '0' || src[i] > '9')
			break;
		Idata *=10;
		Idata +=src[i] - '0';
	}
	return Idata;
}

/*****************************************************************************
 * ������
 *  dynamic_gps_parse_gsv
 * ����
3�� GPS Satellites in View��GSV���ɼ�������Ϣ
$GPGSV,<1>,<2>,<3>,<4>,<5>,<6>,<7>,��<4>,<5>,<6>,<7>*hh<CR><LF>
<1> GSV��������
<2> ����GSV�ı��
<3> �ɼ����ǵ�������00~12��ǰ���0Ҳ�������䣩
<4> PRN�루α��������룩��01~32��ǰ���0Ҳ�������䣩
<5> �������ǣ�00~90�ȣ�ǰ���0Ҳ�������䣩
<6> ���Ƿ�λ�ǣ�000~359�ȣ�ǰ���0Ҳ�������䣩
<7> ����ȣ�00~99dB��û�и��ٵ�����ʱΪ�գ�ǰ���0Ҳ�������䣩
ע��<4>,<5>,<6>,<7>��Ϣ������ÿ�����ǽ���ѭ����ʾ��ÿ��GSV�����������ʾ4�����ǵ���Ϣ��
����������Ϣ������һ���е�NMEA0183����������
GPGSV -- GNSS Satellites in View
typedef�ṹ
{
    ǩ�����tag_msg_sum;  ��Ϣ������
    ǩ�����tag_msg_index;  ��Ϣ��
    ǩ�����tag_sates_in_view;  ��������
    ǩ�����tag_max_snr;  ��������
    ǩ�����tag_min_snr;   ��СSNR
    �ṹ
    {
        �޷����ַ�tag_sate_id;  ����ID
        �޷����ַ�tag_elevation;  ̧�߶�
        �޷����ַ�tag_azimuth;  ��λ�Զ�Ϊtrue
        �޷����ַ�tag_snr;  ��dB�����
    } tag_rsv[DE_GPS_NMEA_MAX_SVVIEW];
} uct_gps_nmea_gsv_struct;
 *  void
 *����ֵ
 *  void
 *****************************************************************************/
int  dynamic_gps_parse_gsv(uct_gps_nmea_gsv_struct *lpgsv, char *str_gps_data, int strlen1)
{
    char *pstr = NULL;
    short     tag_msg_sum = 0;              /*total number of messages*/
    short     tag_msg_index = 0;            /*message number*/
    short     tag_sates_in_view = 0;              /*Max snr*/
    kal_uint8 i,j;
    
    if (lpgsv == NULL)
    {
        return 0;
    }
    
    pstr = strstr(str_gps_data, "GSV,");

    if(pstr)
    {
        memset(lpgsv,0,sizeof(uct_gps_nmea_gsv_struct));
        pstr = pstr + 4;
        sscanf(pstr, "%hd,%hd,%hd", //
            &tag_msg_sum,
            &tag_msg_index,
            &tag_sates_in_view);
        
        
        lpgsv->tag_msg_sum = tag_msg_sum;
        lpgsv->tag_msg_index = tag_msg_index;
        lpgsv->tag_sates_in_view = tag_sates_in_view;
        
        for (i=0;i<lpgsv->tag_msg_sum;i++)
        {
            char * estr = NULL;
            char * cstr = NULL;
            char * nstr = NULL;
            kal_uint16 len;
            
            estr = strstr(pstr, "*");

            if (estr == NULL)
            {
                break;
            }

            cstr = strstr(pstr, ",");
            if (cstr == NULL)
            {
                break;
            }
            cstr++;
            cstr = strstr(cstr, ",");
            if (cstr == NULL)
            {
                break;
            }
            cstr++;

            cstr = strstr(cstr, ",");
            if (cstr == NULL)
            {
                break;
            }
            cstr++;

            for(j =0; j < 4; j++)
            {
        		// <4>  ���Ǳ��
        		if ((nstr = strchr(cstr, ',')) == NULL)
        			break;
        		len = (kal_uint8)(nstr - cstr);
        		if(len)
        		{
        			lpgsv->tag_rsv[i*4+j].tag_sate_id = (kal_uint16)CharToInt(cstr, len);
        		}
        		else
        		{
        			lpgsv->tag_rsv[i*4+j].tag_sate_id = 0;
        		}
        		cstr = nstr + 1;
        		
        		// <5>  ����
        		if ((nstr = strchr(cstr, ',')) == NULL)
        			break;
        		len = (kal_uint8)(nstr - cstr);
        		if(len)
        		{
        			lpgsv->tag_rsv[i*4+j].tag_elevation = (kal_int16)CharToInt(cstr, len);
        		}
        		else
        		{
        			lpgsv->tag_rsv[i*4+j].tag_elevation = 0;
        		}
        		cstr = nstr + 1;

        		// <6>  ��λ��
        		if ((nstr = strchr(cstr, ',')) == NULL)
        			break;
        		len = (kal_uint8)(nstr - cstr);
        		if(len)
        		{
        			lpgsv->tag_rsv[i*4+j].tag_azimuth = (kal_int16)CharToInt(cstr, len);
        		}
        		else
        		{
        			lpgsv->tag_rsv[i*4+j].tag_azimuth = 0;
        		}
        		cstr = nstr + 1;
        		
        		// <7>  �����
        		nstr = strchr(cstr, ',');
        		if (nstr == NULL || nstr > estr)
        		{
        		    nstr = (estr-1);
        			len = (kal_uint8)(nstr - cstr);
        			if(len)
        			{
        			    lpgsv->tag_rsv[i*4+j].tag_snr	= (kal_uint8)CharToInt(cstr, len);
        			}
        			else
        			{
        				lpgsv->tag_rsv[i*4+j].tag_snr = 0;
        			}
        			cstr = nstr + 1;
                    break;
        		}

    			len = (kal_uint8)(nstr - cstr);
    			if(len)
    			{
    			    lpgsv->tag_rsv[i*4+j].tag_snr	= (kal_uint8)CharToInt(cstr, len);
    			}
    			else
    			{
    				lpgsv->tag_rsv[i*4+j].tag_snr = 0;
    			}
    			cstr = nstr + 1;
            }
#if 0
            dynamic_debug("tag_msg_sum:%d,tag_msg_index:%d,tag_sates_in_view:%d",tag_msg_sum,tag_msg_index,tag_sates_in_view);
            dynamic_debug("%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd",
                lpgsv->tag_rsv[i*4].tag_sate_id,
                lpgsv->tag_rsv[i*4].tag_elevation,
                lpgsv->tag_rsv[i*4].tag_azimuth,
                lpgsv->tag_rsv[i*4].tag_snr,
                lpgsv->tag_rsv[i*4+1].tag_sate_id,
                lpgsv->tag_rsv[i*4+1].tag_elevation,
                lpgsv->tag_rsv[i*4+1].tag_azimuth,
                lpgsv->tag_rsv[i*4+1].tag_snr,
                lpgsv->tag_rsv[i*4+2].tag_sate_id,
                lpgsv->tag_rsv[i*4+2].tag_elevation,
                lpgsv->tag_rsv[i*4+2].tag_azimuth,
                lpgsv->tag_rsv[i*4+2].tag_snr,
                lpgsv->tag_rsv[i*4+3].tag_sate_id,
                lpgsv->tag_rsv[i*4+3].tag_elevation,
                lpgsv->tag_rsv[i*4+3].tag_azimuth,
                lpgsv->tag_rsv[i*4+3].tag_snr);
#endif
            pstr = strstr(estr, "GSV,");
            
            if(pstr == NULL)
            {
                break;
            }
            pstr = pstr + 4;
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

/*****************************************************************************
 * ������
 *  dynamic_gps_parse_vtg
 * ����
5�� Track Made Good and Ground Speed��VTG�������ٶ���Ϣ
$GPVTG,,T,,M,0.015,N,0.029,K,D*29
$GPVTG,<1>,T,<2>,M,<3>,N,<4>,K,<5>*hh<CR><LF>
<1> ���汱Ϊ�ο���׼�ĵ��溽��000~359�ȣ�ǰ���0Ҳ�������䣩
<2> �Դű�Ϊ�ο���׼�ĵ��溽��000~359�ȣ�ǰ���0Ҳ�������䣩
<3> �������ʣ�000.0~999.9�ڣ�ǰ���0Ҳ�������䣩
<4> �������ʣ�0000.0~1851.8����/Сʱ��ǰ���0Ҳ�������䣩
<5> ģʽָʾ����NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч��
 GPVTG -- VTG Data
typedef�ṹ
{
    ��tag_true_heading; / *���ٶ�
    ��tag_mag_heading; / *�ŵ���
    ��tag_hspeed_knot; / *�ٶȽ�
    ��tag_hspeed_km; / *�ٶ�ÿСʱ����
    �����ַ�tag_mode; / *ģʽ
} uct_gps_nmea_vtg_struct;

 *  void
 *����ֵ
 *  void
 *****************************************************************************/
int  dynamic_gps_parse_vtg(uct_gps_nmea_vtg_struct *lpg_vtg, char *str_data, int strlen1)
{
    char *pstr = 0;
    char strtime[10];

    memset(strtime, 0, sizeof(strtime));

    pstr = strstr(str_data, "VTG,");
    if(lpg_vtg)
    {
        if(pstr)
        {
            pstr = pstr + 4;
            sscanf(pstr, "%f,%f,%f,%f,%c", //
                &lpg_vtg->tag_true_heading,// 1
                &lpg_vtg->tag_mag_heading,// 2
                &lpg_vtg->tag_hspeed_knot,// 3
                &lpg_vtg->tag_hspeed_km,// 4
                &lpg_vtg->tag_mode);// 5
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
#if 0
    dynamic_debug("hspeed_knot:%d,hspeed_km:%d",lpg_vtg->tag_hspeed_knot,&lpg_vtg->tag_hspeed_km);
#endif
    return 1;
}

/*****************************************************************************
 * ������
 *  dynamic_gps_parse_gll
 * ����
6�� Geographic Position��GLL����λ������Ϣ
$GPGLL,2236.91218,N,11403.24719,E,060206.00,A,D*66
$GPGLL,<1>,<2>,<3>,<4>,<5>,<6>,<7>*hh<CR><LF>
<1> γ��ddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<2> γ�Ȱ���N�������򣩻�S���ϰ���
<3> ����dddmm.mmmm���ȷ֣���ʽ��ǰ���0Ҳ�������䣩
<4> ���Ȱ���E����������W��������
<5> UTCʱ�䣬hhmmss��ʱ���룩��ʽ
<6> ��λ״̬��A=��Ч��λ��V=��Ч��λ
<7> ģʽָʾ����NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч
  GPGLL - ����λ�� - γ��/����*
typedef�ṹ
{
    ˫tag_latitude;  γ��
    ˫tag_longitude;  ����
    uct_gps_nmea_utc_time_struct tag_utc_time;   UTCʱ��
    �����ַ�tag_north_south;   N��S
    �����ַ�tag_east_west;   E��W
    �����ַ�tag_status;  ״̬A  - ������Ч��V - ������Ч
    �����ַ�tag_mode;  ģʽ
} uct_gps_nmea_gll_struct;

 *  void
 *����ֵ
 *  void
 *****************************************************************************/
int  dynamic_gps_parse_gll(uct_gps_nmea_gll_struct *lpggll, char *str_data, int strlen1)
{
    char *pstr = 0;
    short len = 20;
    char strtime[20];

    memset(strtime, 0, sizeof(strtime));
    memcpy(strtime, &len, 2);
    pstr = strstr(str_data, "GLL,");
    if(lpggll)
    {
        if(pstr)
        {
            pstr = pstr + 4;
            sscanf(pstr, "%lf,%c,%lf,%c,%9c,%c,%c", //
                &lpggll->tag_latitude,// 1
                &lpggll->tag_north_south,// 2
                &lpggll->tag_longitude,// 3
                &lpggll->tag_east_west,
                strtime,// 5
                &lpggll->tag_status,// 6
                &lpggll->tag_mode);// 7

            lpggll->tag_latitude = dynamic_gps_format_latlng(lpggll->tag_latitude);
            lpggll->tag_longitude = dynamic_gps_format_latlng(lpggll->tag_longitude);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}


/*******************************************************************
** ������:     dynamic_gps_recv_cb
** ��������:   gps���ݽ��ջص���Ĭ��1s����һ������
** ����:       
** ����:       
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
        dynamic_log("%s", (char *)data);
    }

    memset(&s_gps_data, 0, sizeof(s_gps_data));

    dynamic_gps_parse_gga(&s_gps_data.tag_gga, (char*)data, len);
    dynamic_gps_parse_rmc(&s_gps_data.tag_rmc, (char*)data, len);
    //dynamic_gps_parse_gsa(&s_gps_data.tag_gsa[0], (char*)data, len);
    dynamic_gps_parse_gsv(&s_gps_data.tag_gsv, (char*)data, len);
    //dynamic_gps_parse_vtg(&s_gps_data.tag_vtg, (char*)data, len);
    //dynamic_gps_parse_gll(&s_gps_data.tag_gll, (char*)data, len);

    // ����GPS����
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
            dynamic_debug("��һ����");
        }
        invalid = 0;
        satnum_cnt = 0;
        memset(&s_gps_info,0,sizeof(s_gps_info));

        // UTC ʱ��
    	s_gps_info.mtime.nYear = s_gps_data.tag_rmc.tag_utc_date.tag_year+2000;
    	s_gps_info.mtime.nMonth = s_gps_data.tag_rmc.tag_utc_date.tag_month;
    	s_gps_info.mtime.nDay = s_gps_data.tag_rmc.tag_utc_date.tag_day;	
    	s_gps_info.mtime.nHour = s_gps_data.tag_rmc.tag_utc_time.tag_hour;
    	s_gps_info.mtime.nMin = s_gps_data.tag_rmc.tag_utc_time.tag_minute;
    	s_gps_info.mtime.nSec = s_gps_data.tag_rmc.tag_utc_time.tag_second;

    	s_gps_info.altitude = s_gps_data.tag_gga.tag_altitude; // ���θ߶�
    	s_gps_info.sat_num = s_gps_data.tag_gga.tag_sat_in_fix; // ������
    	s_gps_info.hdop = s_gps_data.tag_gga.tag_h_precision; // ˮƽ����
    	memcpy(s_gps_info.lat_str,s_gps_data.tag_rmc.tag_lat_str,sizeof(s_gps_data.tag_rmc.tag_lat_str));// �ַ����ȷָ�ʽ
    	memcpy(s_gps_info.lng_str,s_gps_data.tag_rmc.tag_lng_str,sizeof(s_gps_data.tag_rmc.tag_lng_str));
    	s_gps_info.lat= s_gps_data.tag_rmc.tag_latitude; // γ�� �ȸ�ʽ
    	s_gps_info.lng= s_gps_data.tag_rmc.tag_longitude; // ���� �ȸ�ʽ
    	s_gps_info.milespeed = s_gps_data.tag_rmc.tag_ground_speed; // �ٶ� ����
    	s_gps_info.kmspeed = (unsigned int)(s_gps_data.tag_rmc.tag_ground_speed*1.852); // �ٶ� ����
    	s_gps_info.degree = s_gps_data.tag_rmc.tag_trace_degree; // �Ƕ�
    	s_gps_info.d_lat = s_gps_data.tag_rmc.tag_north_south; // �������ϰ���
    	s_gps_info.d_lng= s_gps_data.tag_rmc.tag_east_west; // ����������
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
** ������:     dynamic_gps_aid_task
** ��������:   
** ����:       
** ����:       
********************************************************************/
void dynamic_gps_aid_task(void*str)
{
    kal_uint32 timevalue = 1000;
    applib_time_struct cur_time;
    dynamic_gps_info_t *gps_info = dynamic_gps_get_info();
    
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
                    dynamic_debug("��������EPO");
                }
                dynamic_gps_time_aid(cur_time);
                dynamic_debug("ʱ�丨��");
                
                s_aiding_state = ENUM_GPS_EPO_AIDING;
            break;
            
            case ENUM_GPS_EPO_AIDING:
            {
                kal_bool aid_res;

                if (dynamic_epo_is_valid())
                {
                    dynamic_debug("EPO����");
                    aid_res = dynamic_gps_epo_aiding();
                    dynamic_debug("EPO:%d",aid_res);
                     
                    s_aiding_state = ENUM_GPS_REFLOC_AIDING;
                    timevalue = 1000;
                }
                else
                {
                    timevalue = 1000;
                    if (++s_aid_cnt >= 150)
                    {
                        s_aid_cnt = 0;
                        s_aiding_state = ENUM_GPS_AIDING_INIT;
                        dynamic_debug("EPOʧ��");
                        return;
                    }
                }
            }
            break;

            case ENUM_GPS_REFLOC_AIDING:
            {
			    DYNAMIC_SYS_T * sys_info = dynamic_sys_get_info();

                //sys_info->end_gps.lat = 22.56602;
                //sys_info->end_gps.lng = 113.94841;
                if (sys_info->end_gps.lat != 0.0 && sys_info->end_gps.lng != 0.0)
                {
                    dynamic_debug("λ�ø���");
                    dynamic_gps_refloc_aid(sys_info->end_gps.lat,sys_info->end_gps.lng,sys_info->end_gps.altitude);
                }
                s_aiding_state = ENUM_GPS_AIDING_INIT;
                return;
            }
            break;

            default:
                return;
            break;
        }
    }

    dynamic_timer_start(enum_timer_gps_aid_timer,timevalue,(void*)dynamic_gps_aid_task,NULL,FALSE);
}


/*******************************************************************
** ������:     dynamic_gps_pow_ctrl
** ��������:   
** ����:       
** ����:       
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
        dynamic_sleep_disable();
        dynamic_gps_pow_crl(1);
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
** ������:     dynamic_gps_task
** ��������:   
** ����:       
** ����:       
********************************************************************/
void dynamic_gps_task(void *str)
{
    static kal_uint8 onoff_buf = 0;
    static kal_uint8 save_endgps = 0; // ����ĩ�ζ�λ��
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
            dynamic_debug("ĩ�ζ�λ����lat:%f,lng:%f,altitude:%f",sys_info->end_gps.lat,sys_info->end_gps.lng,sys_info->end_gps.altitude);
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
** ������:     dynamic_gps_init
** ��������:   
** ����:       
** ����:       
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
			sys_info->end_gps.d_lat = 'N';
			sys_info->end_gps.d_lng = 'E';
            dynamic_sys_info_save();
        }
    }
}

