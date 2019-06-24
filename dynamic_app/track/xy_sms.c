/********************************************************************************
**
** 文件名:     xy_sms.c
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

#define MIN_PHONE_NUM_LEN 7

/*******************************************************************
** 函数名:     xy_sms_user_check
** 函数描述:   0未设置主号 1主号 2非主号
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_sms_user_check(char* num)
{
    XY_INFO_T * xy_info = xy_get_info();
    kal_uint8 userlen;
    kal_uint8 numlen;
    kal_uint8 result = 2;

    userlen = strlen((char*)xy_info->user);
    numlen = strlen(num);
    
    if (userlen == 0)
    {
        result = 0;
    }
    else
    {
        if (userlen >= MIN_PHONE_NUM_LEN && numlen >= MIN_PHONE_NUM_LEN)
        {
            if (strncmp((char*)&xy_info->user[userlen - MIN_PHONE_NUM_LEN],&num[numlen - MIN_PHONE_NUM_LEN],MIN_PHONE_NUM_LEN) == 0)
            {
                dynamic_debug("xy_sms_user_check:%s,%s",&xy_info->user[userlen - MIN_PHONE_NUM_LEN],&num[numlen - MIN_PHONE_NUM_LEN]);
                result = 1;
            }
        }
        else
        {
            if (strncmp((char*)xy_info->user,num,userlen) == 0)
            {
                result = 1;
            }
        }
    }

    dynamic_debug("xy_sms_user_check result:%d",result);
    return result;
}

/*******************************************************************
** 函数名:     xy_sms_link_google_map
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 xy_sms_link_google_map(char* buf,dynamic_gps_info_t *gps)
{
    kal_uint8 len;
    
    memset(buf,0,sizeof(buf));
	len = sprintf(buf,"<http://maps.google.com/?q=%f%c,%f%c>",
         		gps->lat,gps->d_lat,gps->lng,gps->d_lng);
         		
    return len;
}

/*******************************************************************
** 函数名:     xy_sms_alm_assem
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_sms_alm_assem(XY_ALM_TYPE_E type)
{
	XY_INFO_T * xy_info = xy_get_info();
	DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
	applib_time_struct systime;
	kal_uint8 datalen = 0;
	char *asc_buf = NULL;

	asc_buf = dynamic_malloc_malloc(200);

    if (asc_buf == NULL)
    {
        dynamic_debug("xy_sms_alm_assem malloc err");
        return;
    }

	if (strlen((char *)xy_info->user) == 0)
	{
		dynamic_debug("xy_sms_alm_assem:%d	no user num",type);
		return;
	}
	dynamic_debug("xy_sms_alm_assem:%d",type);

	memset(asc_buf,0,sizeof(asc_buf));
	dynamic_time_get_systime(&systime);
	switch(type)
	{
		case XY_ALM_TYPE_ACC:
			datalen = sprintf(asc_buf, "Illegal ACC alarm <%02d:%02d:%02d %02d/%02d/%d>. ",
					systime.nHour,systime.nMin,systime.nSec,systime.nDay,systime.nMonth,systime.nYear);

			if (dynamic_gps_is_fix())
			{
				datalen += xy_sms_link_google_map(&asc_buf[datalen],dynamic_gps_get_info()); 			   
			}
			else if (sys_info->end_gps.lat != 0 && sys_info->end_gps.lng != 0)
			{
				applib_time_struct * gpstime = &sys_info->end_gps.mtime;
				
				datalen += xy_sms_link_google_map(&asc_buf[datalen],&sys_info->end_gps);
				datalen += sprintf(&asc_buf[datalen], " luc <%02d:%02d:%02d %02d/%02d/%d>",
								gpstime->nHour,gpstime->nMin,gpstime->nSec,gpstime->nDay,gpstime->nMonth,gpstime->nYear);
			}
			else
			{
				datalen += sprintf(&asc_buf[datalen], "never tracking succesful");
			}
		break;

		case XY_ALM_TYPE_MOVE:
		{
			dynamic_gps_info_t gps;
			
			datalen = sprintf(asc_buf, "Displacement alarm! <%02d:%02d:%02d %02d/%02d/%d>. ",
					systime.nHour,systime.nMin,systime.nSec,systime.nDay,systime.nMonth,systime.nYear);
					
			if (dynamic_gps_is_fix())
			{
				memcpy(&gps,dynamic_gps_get_info(),sizeof(dynamic_gps_info_t));
			}
			else if (sys_info->end_gps.lat != 0 && sys_info->end_gps.lng != 0)
			{
				memcpy(&gps,&sys_info->end_gps,sizeof(dynamic_gps_info_t));
			} 
			
			datalen += xy_sms_link_google_map(&asc_buf[datalen],&gps);		 
		}	 
		break;

		case XY_ALM_TYPE_OVER_SPEED:
		{
			dynamic_gps_info_t gps;
			
			datalen = sprintf(asc_buf, "Overspeed alarm! <%02d:%02d:%02d %02d/%02d/%d>. ",
					systime.nHour,systime.nMin,systime.nSec,systime.nDay,systime.nMonth,systime.nYear);
					
			if (dynamic_gps_is_fix())
			{
				memcpy(&gps,dynamic_gps_get_info(),sizeof(dynamic_gps_info_t));
			}
			else if (sys_info->end_gps.lat != 0 && sys_info->end_gps.lng != 0)
			{
				memcpy(&gps,&sys_info->end_gps,sizeof(dynamic_gps_info_t));
			} 
			
			datalen += xy_sms_link_google_map(&asc_buf[datalen],&gps);		 
		}
		break;		  
		
		
		case XY_ALM_TYPE_POW_BREAK:
			datalen = sprintf(asc_buf, "Power failure alarm! <%02d:%02d:%02d %02d/%02d/%d>. ",
					systime.nHour,systime.nMin,systime.nSec,systime.nDay,systime.nMonth,systime.nYear);

			if (dynamic_gps_is_fix())
			{
				datalen += sprintf(&asc_buf[datalen], "current pos ");
				datalen += xy_sms_link_google_map(&asc_buf[datalen],dynamic_gps_get_info()); 			   
			}
			else if (sys_info->end_gps.lat != 0 && sys_info->end_gps.lng != 0)
			{
				applib_time_struct * gpstime = &sys_info->end_gps.mtime;
				
				datalen += sprintf(&asc_buf[datalen], "Tracking unsuccessfull but in the past tracking successful:");
				datalen += xy_sms_link_google_map(&asc_buf[datalen],&sys_info->end_gps);
				datalen += sprintf(&asc_buf[datalen], " <%02d:%02d:%02d %02d/%02d/%d>",
								gpstime->nHour,gpstime->nMin,gpstime->nSec,gpstime->nDay,gpstime->nMonth,gpstime->nYear);
			}
			else
			{
				datalen += sprintf(&asc_buf[datalen], "never tracking succesful.");
			}
		break;
		
		case XY_ALM_TYPE_LOW_BAT:
			datalen += sprintf(&asc_buf[datalen], "Backup battery voltage is under 3.7V");
		break;	   

		case XY_ALM_TYPE_LOW_POW:
			datalen += sprintf(&asc_buf[datalen], "External voltage is under parameter cut off external voltage.");
		break;			

		case XY_ALM_TYPE_POW_RECOVER:
			datalen += sprintf(&asc_buf[datalen], "Voltage recovery to external voltage.");
		break;		   

		case XY_ALM_TYPE_REQUEST_POS:		
		break;	 
		   
		case XY_ALM_TYPE_REQUEST_POS2:
		break;		

		case XY_ALM_TYPE_VIB:
			datalen = sprintf(asc_buf, "Illegal vibration <%02d:%02d:%02d %02d/%02d/%d>. ",
					systime.nHour,systime.nMin,systime.nSec,systime.nDay,systime.nMonth,systime.nYear);

			if (dynamic_gps_is_fix())
			{
				datalen += sprintf(&asc_buf[datalen], "current pos ");
				datalen += xy_sms_link_google_map(&asc_buf[datalen],dynamic_gps_get_info()); 			   
			}
			else if (sys_info->end_gps.lat != 0 && sys_info->end_gps.lng != 0)
			{
				applib_time_struct * gpstime = &sys_info->end_gps.mtime;
				
				datalen += sprintf(&asc_buf[datalen], "Tracking unsuccessfull but in the past tracking successful:");
				datalen += xy_sms_link_google_map(&asc_buf[datalen],&sys_info->end_gps);
				datalen += sprintf(&asc_buf[datalen], "<%02d:%02d:%02d %02d/%02d/%d>",
								gpstime->nHour,gpstime->nMin,gpstime->nSec,gpstime->nDay,gpstime->nMonth,gpstime->nYear);
			}
			else
			{
				datalen += sprintf(&asc_buf[datalen], "never tracking succesful");
			}
		break;

		default:
			dynamic_debug("xy_sms_alm_assem err type");
			return;
		break;
	}
	dynamic_debug("asc_buf:%s",asc_buf);
    if (strlen((char *)xy_info->user) > 0)
    {
    	dynamic_sms_list_add((char*)xy_info->user,datalen,asc_buf);
    }
	dynamic_free(asc_buf);
}


/*******************************************************************
** 函数名:     xy_cmd_gpssnr
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int xy_cmd_gpssnr(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_ACTIVE_MODE:
        {
            nmea_data_t *nmea_str = dynamic_gps_get_nmea();
            applib_time_struct systime;
            kal_uint32 systime_sec = 0;
            XY_INFO_T * xy_info = xy_get_info();
            dynamic_gps_info_t *gps_info = dynamic_gps_get_info();

            dynamic_time_get_systime(&systime);
            systime_sec = dynamic_timer_time2sec(&systime);
            xy_info->accoff_sectime = systime_sec;
            xy_info->acc_off_flg = 1;
            xy_info->acc_state = XY_ACC_OFF;

            if (gps_info->valid)
            {
                asklen = sprintf(askdata,"%s:%d-%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                    cmd,
                    nmea_str->tag_gsv.tag_sates_in_view,
                    nmea_str->tag_gsv.tag_rsv[0].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[1].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[2].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[3].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[4].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[5].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[6].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[7].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[8].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[9].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[10].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[11].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[12].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[13].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[14].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[15].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[16].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[17].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[18].tag_snr,
                    nmea_str->tag_gsv.tag_rsv[19].tag_snr);
                    
    			dynamic_cmd_ask(type,num,askdata,asklen);
    			ret = DYNAMIC_AT_RET;           
            }

            ret = DYNAMIC_AT_OK;
        }
        break;

        default:

        break;
    }
    return ret;
}


#endif

