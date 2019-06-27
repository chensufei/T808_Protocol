/********************************************************************************
**
** 文件名:     dynamic_msg_at.c
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


static DYNAMIC_CMD_T s_dynamic_cmd_table[MAX_ATCMD_NUM];
static kal_uint32 s_cmd_num = 0;

#define STRING_CMD_OK "OK"

/*******************************************************************
** 函数名:     dynamic_cmd_find_cmd_mode
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
custom_cmd_mode_enum dynamic_cmd_find_cmd_mode(dynamic_custom_cmdLine *cmd_line)
{
    custom_cmd_mode_enum result;

    if (cmd_line->position < cmd_line->length)
    {
        switch (cmd_line->character[cmd_line->position])
        {
            case '?':  /* AT+...? */
            {
                cmd_line->position++;
                result = CUSTOM_READ_MODE;
                break;
            }
            case '=':  /* AT+...= */
            case ',':
            {
                cmd_line->position++;
                if ((cmd_line->position < cmd_line->length - 1 ) &&
                    (cmd_line->character[cmd_line->position] == '?'))
                {
                    cmd_line->position++;
                    result = CUSTOM_TEST_MODE;
                }
                else
                {
                    result = CUSTOM_SET_OR_EXECUTE_MODE;
                }
                break;
            }
            default:  /* AT+... */
            {
                result = CUSTOM_ACTIVE_MODE;
                break;
            }
        }
    }
    else
    {
        result = CUSTOM_ACTIVE_MODE;
    }
    return (result);
}

/*******************************************************************
** 函数名:    dynamic_cmd_uart_write
** 函数描述:  
** 输入参数:  
** 输出参数:
** 返回:       
********************************************************************/
void dynamic_cmd_uart_write(char *str,kal_uint16 len)
{
	dynamic_log("%s",str);
}
/*******************************************************************
** 函数名:     dynamic_cmd_ask
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_cmd_ask(char type,char*num,char* askdata,kal_uint16 asklen)
{
    if (asklen == 0)
    {
        return;
    }
	switch (type)
	{
	    case DYNAMIC_CMD_TYPE_SERVER:

	    break;

	    case DYNAMIC_CMD_TYPE_SMS:
            dynamic_sms_list_add(num,asklen,askdata);
	    break;

        case DYNAMIC_CMD_TYPE_UART:
	    default:
            dynamic_cmd_uart_write(askdata,asklen);
	    break;
	}
}
/*******************************************************************
** 函数名:     dynamic_cmd_reset
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_reset(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    //memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:

        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;
            
		case CUSTOM_ACTIVE_MODE:
			asklen = sprintf(askdata,"%s:%s\r\n",cmd,STRING_CMD_OK);
			dynamic_cmd_ask(type,num,askdata,asklen);
			if (type == DYNAMIC_CMD_TYPE_UART)
			{
                dynamic_start_reset(RESET_TYPE_FOR_CMD);
			}
			else
			{
                dynamic_timer_start(enum_timer_start_reset_later,30*1000, (void *)dynamic_reset_delay,0,0);
			}
			ret = DYNAMIC_AT_RET;
        break;

        default:

        break;

    }

    dynamic_free(askdata);
    return ret;
}

/*******************************************************************
** 函数名:     dynamic_cmd_debug
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_debug(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    kal_uint8 debug_lv = 0;
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    //memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:
		    if (sscanf(&str->character[str->position],"%hhd",&debug_lv) == 1)
		    {
		        if (debug_lv < DYNAMIC_LOG_END)
		        {		
                    dynamic_log_set_lvl(debug_lv);
        			asklen = sprintf(askdata,"%s:%s\r\n",cmd,STRING_CMD_OK);
        			dynamic_cmd_ask(type,num,askdata,asklen);
        			ret = DYNAMIC_AT_RET;
		        }
		    }
        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
            
            
        break;

        default:

        break;

    }

	dynamic_free(askdata);
    return ret;
}
/*******************************************************************
** 函数名:     dynamic_cmd_atd
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_atd(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    char callnum[22];
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    //memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:
            memset(callnum,0,sizeof(callnum));
		    if (sscanf(&str->character[str->position],"%s",callnum) == 1)
		    {
                dynamic_telephony_dial(callnum);
    			asklen = sprintf(askdata,"%s:%s\r\n",cmd,STRING_CMD_OK);
    			dynamic_cmd_ask(type,num,askdata,asklen);
    			ret = DYNAMIC_AT_RET;
		    }
        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:

        break;

        default:

        break;

    }
    dynamic_free(askdata);
    return ret;
}


/*******************************************************************
** 函数名:     dynamic_cmd_gpslog
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_gpslog(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    kal_uint8 gps_log = 0;
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    //memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:
		    if (sscanf(&str->character[str->position],"%hhd",&gps_log) == 1)
		    {
                dynamic_gps_set_log(gps_log);
    			asklen = sprintf(askdata,"%s:%s\r\n",cmd,STRING_CMD_OK);
    			dynamic_cmd_ask(type,num,askdata,asklen);
		    }
		    ret = DYNAMIC_AT_RET;	
        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
            
            
        break;

        default:

        break;

    }
    dynamic_free(askdata);
    return ret;
}


/*******************************************************************
** 函数名:     dynamic_cmd_sr
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_sr(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    //memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:

        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
        {
            DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
            APP_CNTX_T * app_cntx = dynamic_app_cntx_get();			

			if (DYNAMIC_CMD_TYPE_UART == type)
			{
			    dynamic_log("ver:%s\r\n",DYNAMIC_SOFT_VER);
                dynamic_log("build date time:%s\r\n",dynamic_time_get_build_date_time());
                dynamic_log("space total:%lld,%lld\r\n",dynamic_fs_get_total_space(),dynamic_fs_get_free_space());
                dynamic_log("Server:%s,%d\r\n",xy_info->server, xy_info->port);
                dynamic_log("DevID:%s\r\n", xy_info->dev_num);
                dynamic_log("Tid:%s\r\n",xy_info->t808_para.dev_id);
                dynamic_log("plmn:%s\r\n",app_cntx->sim_plmn);
                dynamic_log("imei:%s\r\n",app_cntx->imei);
                dynamic_log("csq:%d\r\n",app_cntx->csq);

                dynamic_log("mode:%d\r\n",xy_info->mode);
                dynamic_log("acc_state:%d\r\n",xy_info->acc_state);
                dynamic_log("sport_state:%d\r\n",xy_info->sport_state);
                
                dynamic_log("ref_reset_type:%d\r\n",sys_info->ref_reset_type);
                dynamic_log("end gps lat:%f,lng:%f\r\n",sys_info->end_gps.lat,sys_info->end_gps.lng);
				asklen = 0;
			}
			else if (DYNAMIC_CMD_TYPE_SMS == type)
			{
				asklen += sprintf(&askdata[asklen],"ver:%s\r\n",DYNAMIC_SOFT_VER);
	    	    asklen += sprintf(&askdata[asklen],"build time:%s\r\n",dynamic_time_get_build_date_time());
	    	    asklen += sprintf(&askdata[asklen],"space:%lld,%lld\r\n",dynamic_fs_get_total_space(),dynamic_fs_get_free_space());
				asklen += sprintf(&askdata[asklen],"Ip:%s,%d\r\n", xy_info->server, xy_info->port);
				asklen += sprintf(&askdata[asklen],"Id:%s\r\n", xy_info->dev_num);
				asklen += sprintf(&askdata[asklen],"plmn:%s\r\n",app_cntx->sim_plmn);
	            asklen += sprintf(&askdata[asklen],"imei:%s\r\n",app_cntx->imei);
	            asklen += sprintf(&askdata[asklen],"csq:%d\r\n",app_cntx->csq);
	            asklen += sprintf(&askdata[asklen],"reset:%d\r\n",sys_info->ref_reset_type);
	            asklen += sprintf(&askdata[asklen],"end gps lat:%f,lng:%f\r\n",sys_info->end_gps.lat,sys_info->end_gps.lng);
			}
            dynamic_cmd_ask(type,num,askdata,asklen);
            ret = DYNAMIC_AT_RET;	
        }
        break;

        default:

        break;

    }
    dynamic_free(askdata);
    return ret;
}

/*******************************************************************
** 函数名:     dynamic_cmd_resetinfo
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_resetinfo(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    custom_cmd_mode_enum result;

    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:

        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
        {
            DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
            kal_uint16 i;

            dynamic_log("reset info\r\n");
            for (i=0;i<sys_info->reset_cnt;i++)
            {
                dynamic_log("%d:type:%d ",i,sys_info->reset_info[i].type);
                switch(sys_info->reset_info[i].type)
                {
                    case RESET_TYPE_ABNORMITY: // 异常重启
                        dynamic_log("异常重启        ");
                    break;
                    
                    case RESET_TYPE_POWEROFF: // 关机
                        dynamic_log("关机            ");
                    break;
                    
                    case RESET_TYPE_FOR_CMD: // 指令重启
                        dynamic_log("指令重启        ");
                    break;
                    
                    case RESET_TYPE_UPGRADE: // 远程升级重启
                        dynamic_log("远程升级重启    ");
                    break;
                    
                    case RESET_TYPE_24_HOUR: // 24小时自动重启
                        dynamic_log("24小时自动重启  ");
                    break;
                    
                    case RESET_TYPE_SOC_ERR: // SOC链接异常
                        dynamic_log("SOC链接异常     ");
                    break;
                    
                    case RESET_TYPE_SERVER_NOASK: // 服务器未应答
                        dynamic_log("长时间没有数据   ");
                    break;
                    
                    case RESET_TYPE_NODATA: // 服务器未应答
                        dynamic_log("服务器未应答    ");
                    break;                    
                    
                    case RESET_TYPE_NO_SIM: // 找不到SIM卡
                        dynamic_log("找不到SIM卡     ");
                    break;
                    
                    case RESET_TYPE_MEM_ERR: // 内存异常
                        dynamic_log("内存异常        ");
                    break;

                    default:

                    break;
                }
                dynamic_log("[%d-%02d-%02d %02d:%02d:%02d]\r\n",sys_info->reset_info[i].time.nYear,sys_info->reset_info[i].time.nMonth,
                            sys_info->reset_info[i].time.nDay,sys_info->reset_info[i].time.nHour,
                            sys_info->reset_info[i].time.nMin,sys_info->reset_info[i].time.nSec);
                            
            }

            ret = DYNAMIC_AT_OK;
        }
        break;

        default:

        break;
    }
    return ret;
}

/*******************************************************************
** 函数名:     dynamic_cmd_format
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_format(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_OK;
	custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    //memset(askdata,0,DYNAMIC_CMD_ASK_LEN);

	result = dynamic_cmd_find_cmd_mode(str);
	switch (result)
	{
		case CUSTOM_ACTIVE_MODE:	
			dynamic_sys_info_reset();
			dynamic_sms_list_reset();
#ifdef __XY_SUPPORT__
            {
                XY_INFO_T * xy_info = xy_get_info();

                memset(xy_info,0,sizeof(XY_INFO_T)); // 全部初始化
                xy_info_reset();

				xy_soc_data_clear();
            }
#endif
			asklen = sprintf(askdata,"%s:%s\r\n",cmd,STRING_CMD_OK);
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
			
		break;

       default:

		break;

	}
	dynamic_free(askdata);
    return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_server
** 函数描述:   server? / server=xxx.xxx.xxx.xxx,port
** 参数:	   
** 返回:	   
********************************************************************/
int dynamic_cmd_server(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	XY_INFO_T * xy_info = xy_get_info();

	if (askdata == NULL)
	{
		return ret;
	}
	//memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			char ip[32] = {0};
			char strport[8] = {0};
			u16 port = 0;

			
			if (2 == sscanf(&str->character[str->position],"%[^,],%s", ip, strport))
		    {
		    	port = (u16)atoi(strport);
		    	if ((strlen(ip) > 0) && (port > 0))
		    	{
					memset(xy_info->server, 0, sizeof(xy_info->server));
					memcpy((void *)xy_info->server, (void *)ip, strlen(ip));
					xy_info->port = port;
					xy_info_save();

					asklen += sprintf(&askdata[asklen],"Set Server Info Ok\r\n");
		            dynamic_cmd_ask(type,num,askdata,asklen);

					xy_soc_clear_link_info();

					xy_soc_data_clear();

					ret = DYNAMIC_AT_RET;
				}				
		    }
			
		}
		break;

		case CUSTOM_READ_MODE://读参数
		{
			asklen += sprintf(&askdata[asklen],"Server:%s,%d\r\n", xy_info->server, xy_info->port);
            dynamic_cmd_ask(type,num,askdata,asklen);
			
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
    return ret;
}

/*******************************************************************
** 函数名:     dynamic_cmd_factoy
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_factoy(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
    int ret = DYNAMIC_AT_ERROR;
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    //memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:

        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
        {
			dynamic_file_dele(XY_INFO_FILE_NAME);
			dynamic_file_dele(XY_INFO_FILE_BACKUP_NAME);

			xy_info_reset();
			xy_soc_clear_link_info();
			xy_soc_data_clear();

            asklen += sprintf(&askdata[asklen],"Restore Factory Settings Ok\r\n");
            
            dynamic_cmd_ask(type,num,askdata,asklen);
            ret = DYNAMIC_AT_RET;	
        }
        break;

        default:

        break;

    }
    dynamic_free(askdata);
    return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_devid
** 函数描述:   
** 参数:	devid=13025082345 - 写指令  / devid? - 读指令
** 返回:	   
********************************************************************/
static int dynamic_cmd_devid(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

	if (askdata == NULL)
	{
		return ret;
	}
	//memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			char devid[13] = {0};
			int len = strlen(&str->character[str->position]);

			if (len > 0)
			{
				memset(devid, 0, sizeof(devid));
				if (len > 12)
				{
					memcpy(devid, &str->character[str->position], 12);
				}
				else
				{
					memcpy(devid, &str->character[str->position], len);
				}

				memset(xy_info->dev_num, 0, sizeof(xy_info->dev_num));
				memcpy(xy_info->dev_num, devid, strlen(devid));
				xy_info_save();
				
				asklen += sprintf(&askdata[asklen],"Set Device Id Ok\r\n");
				dynamic_cmd_ask(type,num,askdata,asklen);

				xy_soc_clear_link_info();

				xy_soc_data_clear();

				ret = DYNAMIC_AT_RET;
			}
		}
		break;

		case CUSTOM_READ_MODE:
		{
			asklen += sprintf(&askdata[asklen],"Get Device Id [%s] Ok\r\n", xy_info->dev_num);
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
	return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_tid
** 函数描述:   
** 参数:	tid=1302508 - 写指令  / tid? - 读指令
** 返回:	   
********************************************************************/
static dynamic_cmd_tid(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

	if (askdata == NULL)
	{
		return ret;
	}
	//memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			char tid[8] = {0};
			int len = strlen(&str->character[str->position]);

			if (len > 0)
			{
				
				memset(tid, 0, sizeof(tid));
				if (len > 7)
				{
					memcpy(tid, &str->character[str->position], 7);
					len = 7;
				}
				else
				{
					memcpy(tid, &str->character[str->position], len);
				}

				memset(xy_info->t808_para.dev_id, 0, sizeof(xy_info->t808_para.dev_id));
				memcpy(xy_info->t808_para.dev_id, tid, len);
				xy_info_save();
				
				asklen += sprintf(&askdata[asklen],"Set Terminal Id Ok\r\n");
				dynamic_cmd_ask(type,num,askdata,asklen);

				xy_soc_clear_link_info();
				ret = DYNAMIC_AT_RET;
			}
		}
		break;

		case CUSTOM_READ_MODE:
		{
			asklen += sprintf(&askdata[asklen],"Get Terminal Id [%s] Ok\r\n", xy_info->t808_para.dev_id);
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
	return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_report_mode
** 函数描述:   
** 参数:	   reportmode=0/1/2   上报模式，0-定时上报，1-定距上报，2-定时和定距上报 / reportmode?
** 返回:	   
********************************************************************/
int dynamic_cmd_report_mode(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

	if (askdata == NULL)
	{
		return ret;
	}
	//memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			int len = strlen(&str->character[str->position]);
			char val = str->character[str->position];

			if ((1 == len) && ((0x30 == val) || (0x31 == val) || (0x32 == val)))
			{
				xy_info->t808_para.report_type = val - 0x30;
				xy_info_save();

				asklen += sprintf(&askdata[asklen],"Set Report Mode Ok\r\n");
				dynamic_cmd_ask(type,num,askdata,asklen);
				ret = DYNAMIC_AT_RET;
			}
		}
		break;

		case CUSTOM_READ_MODE:
		{
			asklen += sprintf(&askdata[asklen],"Get Report Mode [%d] Ok\r\n", xy_info->t808_para.report_type);
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
	return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_timer
** 函数描述:   
** 参数:	   timer=time  >= 5 - 上传时间间隔 / timer?
** 返回:	   
********************************************************************/
int dynamic_cmd_timer(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

	if (askdata == NULL)
	{
		return ret;
	}
	//memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			int cycle = atoi(&str->character[str->position]);

			if (cycle >= 5)
			{
				xy_info->freq = cycle;
				xy_info_save();

				asklen += sprintf(&askdata[asklen],"Set timer Ok\r\n");
				dynamic_cmd_ask(type,num,askdata,asklen);
				ret = DYNAMIC_AT_RET;
			}
		}
		break;

		case CUSTOM_READ_MODE:
		{
			asklen += sprintf(&askdata[asklen],"Get Report timer [%d] Ok\r\n", xy_info->freq);
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
	return ret;
}
/*******************************************************************
** 函数名:	   dynamic_cmd_hbt
** 函数描述:   
** 参数:	   hbt=hbt  >= 60 - 上传时间间隔 / timer?
** 返回:	   
********************************************************************/
int dynamic_cmd_hbt(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

	if (askdata == NULL)
	{
		return ret;
	}
	memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			int hbt = atoi(&str->character[str->position]);

			if (hbt >= 60)
			{
				xy_info->heart_time = hbt;
				xy_info_save();

				asklen += sprintf(&askdata[asklen],"Set heart_time Ok\r\n");
				dynamic_cmd_ask(type,num,askdata,asklen);
				ret = DYNAMIC_AT_RET;
			}
		}
		break;

		case CUSTOM_READ_MODE:
		{
			asklen += sprintf(&askdata[asklen],"Get heart_time [%d] Ok\r\n", xy_info->heart_time);
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
	return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_distance
** 函数描述:   
** 参数:	   distance=distance  >= 50 - 上传时间间隔 / timer?
** 返回:	   
********************************************************************/
int dynamic_cmd_distance(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

	if (askdata == NULL)
	{
		return ret;
	}
	memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			int distance = atoi(&str->character[str->position]);

			if (distance >= 50)
			{
				xy_info->t808_para.def_distance = distance;
				xy_info_save();

				asklen += sprintf(&askdata[asklen],"Set distance Ok\r\n");
				dynamic_cmd_ask(type,num,askdata,asklen);
				ret = DYNAMIC_AT_RET;
			}
		}
		break;

		case CUSTOM_READ_MODE:
		{
			asklen += sprintf(&askdata[asklen],"Get distance [%d] Ok\r\n", xy_info->t808_para.def_distance );
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
	return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_anglerep
** 函数描述:   
** 参数:	   anglerep,<X>[,A][,B]  
    X=0/1 开启/关闭 默认开启
    A=5～180度，偏转角度 默认7度
    B=1～5秒，检测时间 默认1s
** 返回:	   
********************************************************************/
int dynamic_cmd_anglerep(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

	if (askdata == NULL)
	{
		return ret;
	}
	memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			char deg_switch[12] = {0};
			char degree[12] = {0};
			char deg_freq[12] = {0};

			if (3 == sscanf(&str->character[str->position],"%[^,],%[^,],%s", deg_switch, degree, deg_freq))
			{
				int sw = atoi(deg_switch);
				int degreeVal = atoi(degree);
				int freq = atoi(deg_freq);
			
				if ((sw >= 0)&& (degreeVal > 0) && (freq > 0))
				{
					dynamic_debug("degree_switch:%s\r\n", deg_switch);

					xy_info->t808_para.degree_switch = sw;
					xy_info->t808_para.degree = degreeVal;
					xy_info->t808_para.degree_freq = freq;
					xy_info_save();

					if (xy_info->t808_para.degree_switch)
					{
					   asklen += sprintf(&askdata[asklen],"Allows the device judge degree,degree=%d,deg_freq=%d\r\n",xy_info->t808_para.degree,xy_info->t808_para.degree_freq );
					}
					else
					{
						asklen += sprintf(&askdata[asklen],"Not allows the device judge degree\r\n");
					}	
					dynamic_cmd_ask(type,num,askdata,asklen);
					ret = DYNAMIC_AT_RET;
				}
				


			}
			
		}
		break;

		case CUSTOM_READ_MODE:
		{
		    asklen += sprintf(&askdata[asklen],"Get degree parameter Ok, deg_switch:%d,degree:%d,deg_freq:%d\r\n",
					xy_info->t808_para.degree_switch,xy_info->t808_para.degree,xy_info->t808_para.degree_freq );
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
	return ret;
}

/*******************************************************************
** 函数名:	   dynamic_cmd_sends
** 函数描述:   
** 参数:	   SENDS,<A>
           A=0-300分钟
** 返回:	   
********************************************************************/
int dynamic_cmd_sends(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	   int ret = DYNAMIC_AT_ERROR;
		custom_cmd_mode_enum result;
		kal_int16 asklen = 0;
		XY_INFO_T * xy_info = xy_get_info();
		char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	
		if (askdata == NULL)
		{
			return ret;
		}
		memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
		
		result = dynamic_cmd_find_cmd_mode(str);	
		switch (result)
		{
			case CUSTOM_SET_OR_EXECUTE_MODE:
			{
				int sends = atoi(&str->character[str->position]);
	
				if (sends == 0)
				{
					xy_info->slp_sw = 0; //休眠开关禁止使能，设备不进入休眠
					xy_info_save();

					asklen += sprintf(&askdata[asklen],"Set slp_sw Ok %d\r\n",xy_info->slp_sw);
				}
				else
				{
					xy_info->slp_sw = 1; //休眠开关使能,设备进入休眠
					xy_info->slp_d_t = sends;
					xy_info_save();

					asklen += sprintf(&askdata[asklen],"Set sends time Ok %d\r\n",sends);
						
				}
				   dynamic_cmd_ask(type,num,askdata,asklen);
					ret = DYNAMIC_AT_RET;
			}
			break;
	
			case CUSTOM_READ_MODE:
			{
				asklen += sprintf(&askdata[asklen],"Get sleep_time [%d] Ok\r\n", xy_info->slp_d_t);
				dynamic_cmd_ask(type,num,askdata,asklen);
				ret = DYNAMIC_AT_RET;
			}
			break;
	
			case CUSTOM_TEST_MODE:
	
			break;
	
			case CUSTOM_ACTIVE_MODE:
			
			break;
	
			default:
	
			break;
	
		}
		dynamic_free(askdata);
		return ret;

}



/*******************************************************************
** 函数名:     dynamic_cmd_parse
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_parse(DYNAMIC_CMD_TYPE_E type,char *num,char* str,kal_uint32 len)
{
    char buffer[MAX_SMS_LEN+1];
    char *cmd_name;
    char emptynum[2] = {'\0'};
    kal_uint8 index = 0; 
    kal_uint16 length;
    kal_uint16 i;
    int ret = 0;
    dynamic_custom_cmdLine command_line;

    cmd_name = buffer;

    length = strlen(str);
    length = length > MAX_SMS_LEN ? MAX_SMS_LEN : length;    
    while ((str[index] != ',' ) &&  //might be TEST command or EXE command
        (str[index] != '?' ) && // might be READ command
        (str[index] != '=' ) &&
        (str[index] != 0x0d ) && //carriage return
        index < length)  
    {
        cmd_name[index] = str[index];
        if(cmd_name[index] >= 'a' &&  cmd_name[index] <='z')
        {
            cmd_name[index]  =  'A'+cmd_name[index] -'a';
        }
        index ++;
    }
    cmd_name[index] = '\0' ;    

    for(i = 0;i<s_cmd_num;i++)
    {
    	if (strcmp(cmd_name,s_dynamic_cmd_table[i].sstr) == 0)
    	{
            strncpy(command_line.character, str, COMMAND_LINE_SIZE + NULL_TERMINATOR_LENGTH);
            command_line.character[COMMAND_LINE_SIZE] = '\0';
            command_line.length = strlen(command_line.character);
            command_line.position = index;
#ifdef __XY_SUPPORT__
            if (num != NULL)
            {
                kal_uint8 usernum_state;
                
                usernum_state = xy_sms_user_check((char*)num);
                if (usernum_state == 0)
                {
                    if (strncmp(cmd_name,"START",5) == 0 || (length > 17 && 
                        ((strncmp(str,"SET,",4) == 0 && strncmp(&str[11],"USER=",5) == 0) ||
                        (strncmp(str,"set,",4) == 0 && strncmp(&str[11],"user=",5) == 0) ||
                        (strncmp(str,"Set,",4) == 0 && strncmp(&str[11],"user=",5) == 0) )))
                    {

                    }
                    else
                    {
                        // 请设置主号
                        #define STRING_NO_USER_NUM "Please set the user number."
					    dynamic_sms_list_add(num,strlen(STRING_NO_USER_NUM),STRING_NO_USER_NUM);
					    return KAL_FALSE;
                    }
                }
                else if (usernum_state == 2)
                {
                    if (strncmp(cmd_name,"XYFORMAT",8) != 0)
                    {
                        return KAL_FALSE;
                    }
                }
            }
#endif
            if (num == NULL)
            {
                num = emptynum;
            }
            ret = s_dynamic_cmd_table[i].entryproc(type,num,cmd_name,&command_line);
            
            return KAL_TRUE;
    	}
    }
	return KAL_FALSE;
}

/*******************************************************************
** 函数名:     dynamic_cmd_version
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_version(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
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
        case CUSTOM_SET_OR_EXECUTE_MODE:

        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
        {
            asklen += sprintf(&askdata[asklen],"ver:%s\r\n",DYNAMIC_SOFT_VER);
    	    asklen += sprintf(&askdata[asklen],"build date time:%s\r\n",dynamic_time_get_build_date_time());
            dynamic_cmd_ask(type,num,askdata,asklen);
            ret = DYNAMIC_AT_RET;	
        }
        break;

        default:

        break;

    }
	dynamic_free(askdata);
    return ret;
}

/*******************************************************************
** 函数名:     dynamic_cmd_status
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_status(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
    custom_cmd_mode_enum result;
    kal_int16 asklen = 0;
	XY_INFO_T * xy_info = xy_get_info();
	nmea_data_t *gps = dynamic_gps_get_nmea();
    char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);

    if (askdata == NULL)
    {
        return ret;
    }
    memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:

        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
        {
            APP_CNTX_T * app_cntx = dynamic_app_cntx_get();			

			if (DYNAMIC_CMD_TYPE_UART == type)
			{
				dynamic_log("ACC:%d\r\n", xy_info->acc_state);
				dynamic_log("csp:%d\r\n", app_cntx->csq);
			   	dynamic_log("login:%d\r\n", xy_soc_get_auth_ok_state());
				dynamic_log("gps:%d\r\n", gps->tag_gga.tag_sat_in_fix);//GPS卫星数
				dynamic_log("pow:%0.3f\r\n", xy_get_power_vol()/1000.0);//外电电压
			 	dynamic_log("bat:%0.3f\r\n", xy_get_battery_vol()/1000000.0);//电池电压
			  	dynamic_log("vibalm:%d\r\n", xy_info->vib_alm);
			   	dynamic_log("speedalm:%d\r\n", xy_info->speed_alm);
			}
			else
			{
	            asklen += sprintf(&askdata[asklen],"ACC:%d;", xy_info->acc_state);
				asklen += sprintf(&askdata[asklen],"csp:%d;", app_cntx->csq);
			   	asklen += sprintf(&askdata[asklen],"login:%d;", xy_soc_get_auth_ok_state());
				asklen += sprintf(&askdata[asklen],"gps:%d;", gps->tag_gga.tag_sat_in_fix);//GPS卫星数
				asklen += sprintf(&askdata[asklen],"pow:%0.3f;", xy_get_power_vol()/1000.0);//外电电压
			 	asklen += sprintf(&askdata[asklen],"bat:%0.3f;", xy_get_battery_vol()/1000000.0);//电池电压
			  	asklen += sprintf(&askdata[asklen],"vibalm:%d;", xy_info->vib_alm);
			   	asklen += sprintf(&askdata[asklen],"speedalm:%d;", xy_info->speed_alm);

				if (asklen <= DYNAMIC_CMD_ASK_LEN)
				{
	            	dynamic_cmd_ask(type,num,askdata,asklen);
					ret = DYNAMIC_AT_RET;
				}
			}
        }
        break;

        default:

        break;

    }
	dynamic_free(askdata);
    return ret;

}

/*******************************************************************
** 函数名:     dynamic_cmd_where
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_where(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
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
        case CUSTOM_SET_OR_EXECUTE_MODE:

        break;

        case CUSTOM_READ_MODE:

        break;

        case CUSTOM_TEST_MODE:

        break;

        case CUSTOM_ACTIVE_MODE:
        {
            DYNAMIC_SYS_T *sys_info = dynamic_sys_get_info();
			
       		if(sys_info->end_gps.lat==0 ||sys_info->end_gps.lng==0)
       			asklen += sprintf(&askdata[asklen],"not fixed!");
			else
				asklen += sprintf(&askdata[asklen],"http://maps.google.com/maps?q:%f,%f\r\n=",sys_info->end_gps.lat,sys_info->end_gps.lng);
            dynamic_cmd_ask(type,num,askdata,asklen);
            ret = DYNAMIC_AT_RET;	
        }
        break;

        default:

        break;

    }
	dynamic_free(askdata);
    return ret;

}

/*******************************************************************
** 函数名:     dynamic_cmd_APN
** 函数描述:   
** 参数:       APN,<网络名>[,用户名][,用户密码]
** 返回:       
********************************************************************/
int dynamic_cmd_apn(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	    int ret = DYNAMIC_AT_ERROR;
    custom_cmd_mode_enum result;
    kal_uint8 temp;
		kal_int16 asklen = 0;
		char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	XY_INFO_T * xy_info = xy_get_info();

	if (askdata == NULL)
    {
        return ret;
    }

    dynamic_debug("dynamic_cmd_apn");
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:
        {
            dynamic_apn_set_sturct * apn_dns = dynamic_gprs_get_apn_dns();
            dynamic_apn_set_sturct apn_dns_buf;

            memset(&apn_dns_buf,0,sizeof(apn_dns_buf));
            temp = sscanf(&str->character[str->position],"%[^,],%[^,],%s",
                &apn_dns_buf.tag_apn,&apn_dns_buf.tag_user_name,&apn_dns_buf.tag_password);

            dynamic_debug("set apn:%s,%s,%s",apn_dns_buf.tag_apn,apn_dns_buf.tag_user_name,apn_dns_buf.tag_password);
            
            memcpy(xy_info->apn,apn_dns_buf.tag_apn,MAX_GPRS_APN_LEN);
            memcpy(xy_info->user_name,apn_dns_buf.tag_user_name,MAX_GPRS_USER_NAME_LEN);
            memcpy(xy_info->password,apn_dns_buf.tag_password,MAX_GPRS_PASSWORD_LEN);
			xy_info_save();

            memcpy(apn_dns->tag_apn,apn_dns_buf.tag_apn,MAX_GPRS_APN_LEN);
            memcpy(apn_dns->tag_user_name,apn_dns_buf.tag_user_name,MAX_GPRS_USER_NAME_LEN);
            memcpy(apn_dns->tag_password,apn_dns_buf.tag_password,MAX_GPRS_PASSWORD_LEN);
		   asklen += sprintf(&askdata[asklen],"set apn:%s,%s,%s", apn_dns_buf.tag_apn,apn_dns_buf.tag_user_name,apn_dns_buf.tag_password);
            dynamic_cmd_ask(type,num,askdata,asklen);
            ret = DYNAMIC_AT_OK;
        }
        break;

        case CUSTOM_READ_MODE:
        {
            
            char buffer[40+1];
                        
            sprintf(buffer,"\r\nAPN:%s\r\nUSER NAME:%s\r\nPASSWORD:%s\r\n",
                xy_info->apn,xy_info->user_name,xy_info->password);                        
            dynamic_log(buffer, strlen(buffer));
            ret = DYNAMIC_AT_OK;
        }
        break;

        case CUSTOM_TEST_MODE:
        break;

        case CUSTOM_ACTIVE_MODE:

        break;

        default:

        break;
    }
	dynamic_free(askdata);
    return ret;

}


/*******************************************************************
** 函数名:     dynamic_cmd_sos
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
int dynamic_cmd_sos(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_OK;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	DYNAMIC_SMS_DATA_T sms_data;	
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	XY_INFO_T * xy_info = xy_get_info();

	if (askdata == NULL)
    {
        return ret;
    }
    memset(&sms_data,0,sizeof(sms_data));
	result = dynamic_cmd_find_cmd_mode(str);	
	dynamic_debug("dynamic_cmd_sos"); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:
        {
			char *parastr = &str->character[str->position];
			  char sosnum[MAX_SOS_NUM_CNT][MAX_PHONE_NUM_LEN+1];
			  
			  memset(sosnum,0,sizeof(sosnum));
			  xy_get_para_from_ascii(parastr,sosnum[0],0,MAX_PHONE_NUM_LEN);
			  xy_get_para_from_ascii(parastr,sosnum[1],1,MAX_PHONE_NUM_LEN);
			  xy_get_para_from_ascii(parastr,sosnum[2],2,MAX_PHONE_NUM_LEN);
			  
			  memset(xy_info->sosnum,0,sizeof(xy_info->sosnum));
			  memcpy(xy_info->sosnum,sosnum,sizeof(xy_info->sosnum));
			  xy_info_save();
			 asklen += sprintf(&askdata[asklen],"SUCCESS. SOS:%s,%s,%s", xy_info->sosnum[0],xy_info->sosnum[1],xy_info->sosnum[2]);
            dynamic_cmd_ask(type,num,askdata,asklen);  
            ret = DYNAMIC_AT_OK;

        }
        break;

        case CUSTOM_READ_MODE:
			 asklen += sprintf(&askdata[asklen],"SOS:%s,%s,%s", xy_info->sosnum[0],xy_info->sosnum[1],xy_info->sosnum[2]);
            dynamic_cmd_ask(type,num,askdata,asklen); 
		    ret = DYNAMIC_AT_OK;

        break;

        case CUSTOM_TEST_MODE:
        break;

        case CUSTOM_ACTIVE_MODE:

        break;

        default:

        break;
    }
	dynamic_free(askdata);
    return ret;

}

/*******************************************************************
** 函数名:	   dynamic_cmd_acc
** 函数描述:    ACC,<A>[,M]
** 参数:	    A:0/1 0：无acc判断，上传acc状态为OFF。1：按M值方式（默认）
			M为ACC　ON/OFF判断方式
			M=0:ACC线检测点火/熄火 默认为0
			M=3：G-sensor检测方式；
** 返回:	   
********************************************************************/
int dynamic_cmd_acc(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	int ret = DYNAMIC_AT_ERROR;
	custom_cmd_mode_enum result;
	kal_int16 asklen = 0;
	char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	XY_INFO_T * xy_info = xy_get_info();

	if (askdata == NULL)
	{
		return ret;
	}
	memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
	
	result = dynamic_cmd_find_cmd_mode(str);	
	switch (result)
	{
		case CUSTOM_SET_OR_EXECUTE_MODE:
		{
			char acc_sw[4] = {0};
			char acc_mode[4] = {0};
			
			if (2 == sscanf(&str->character[str->position],"%[^,],%s", acc_sw, acc_mode))
		    {
				int acc_check_switch = atoi(acc_sw);
				int acc_check_mode = atoi(acc_mode);
			
		    	dynamic_debug("acc_check_switch=%d,acc_check_mode=%d\r\n",acc_check_switch,acc_check_mode); 

				if((acc_check_switch != 1 && acc_check_switch != 0) ||(acc_check_mode !=0 && acc_check_mode != 3))
					break;
				if(acc_check_switch==0)
				{
					xy_info->acc_state =0;
					xy_info_save();
					asklen += sprintf(&askdata[asklen],"Not allows the device judge acc_state\r\n");
				}
				else
				{
					xy_info->acc_check_mode =	acc_check_mode;
					xy_info_save();
					asklen += sprintf(&askdata[asklen],"Allows the device judge acc_state,set check_mode=%d\r\n",acc_check_mode);
				}
		
	            dynamic_cmd_ask(type,num,askdata,asklen);

				ret = DYNAMIC_AT_RET;
			}				
		}
		break;

		case CUSTOM_READ_MODE://读参数
		{
			asklen += sprintf(&askdata[asklen],"acc_check_switch:%d,acc_check_mode:%d\r\n", xy_info->acc_check_switch, xy_info->acc_check_mode);
            dynamic_cmd_ask(type,num,askdata,asklen);
			
			ret = DYNAMIC_AT_RET;
		}
		break;

		case CUSTOM_TEST_MODE:

		break;

		case CUSTOM_ACTIVE_MODE:
		
		break;

		default:

		break;

	}
	dynamic_free(askdata);
    return ret;
}
/*******************************************************************
** 函数名:	   dynamic_cmd_defmode
** 函数描述:   
** 参数:	  DEFMODE[,X]
          X=0~1; 设防模式，0 自动设防/撤防，1 手动撤防撤防;默认X=0；
          DEFMODE 查询当前设防模式
** 返回:	   
********************************************************************/
int dynamic_cmd_defmode(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	   int ret = DYNAMIC_AT_ERROR;
		custom_cmd_mode_enum result;
		kal_int16 asklen = 0;
		XY_INFO_T * xy_info = xy_get_info();
		
		char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	
		if (askdata == NULL)
		{
			return ret;
		}
		memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
		
		result = dynamic_cmd_find_cmd_mode(str);	
		switch (result)
		{
			case CUSTOM_SET_OR_EXECUTE_MODE:
			{
				int lockmode = atoi(&str->character[str->position]);
	            xy_info->lockmode = lockmode;
				xy_info_save();

				if (xy_info->lockmode)
				{
					asklen += sprintf(&askdata[asklen],"Set HM lockmode Ok\r\n");
				}
				else
				{
					asklen += sprintf(&askdata[asklen],"Set auto lockmode Ok\r\n");
				}
			   	dynamic_cmd_ask(type,num,askdata,asklen);
				ret = DYNAMIC_AT_RET;
			}
			break;
	
			case CUSTOM_READ_MODE:
			{
				asklen += sprintf(&askdata[asklen],"Get lockmode [%d] Ok\r\n", xy_info->lockmode);
				dynamic_cmd_ask(type,num,askdata,asklen);
				ret = DYNAMIC_AT_RET;
			}
			break;
	
			case CUSTOM_TEST_MODE:
	
			break;
	
			case CUSTOM_ACTIVE_MODE:
			
			break;
	
			default:
	
			break;
	
		}
		dynamic_free(askdata);
		return ret;

}

/*******************************************************************
** 函数名:	   dynamic_cmd_111
** 函数描述:   
** 参数:	  
** 返回:	   
********************************************************************/
int dynamic_cmd_111(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	   int ret = DYNAMIC_AT_ERROR;
		custom_cmd_mode_enum result;
		kal_int16 asklen = 0;
		XY_INFO_T * xy_info = xy_get_info();
		
		char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	
		if (askdata == NULL)
		{
			return ret;
		}
		memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
		
		result = dynamic_cmd_find_cmd_mode(str);	
		switch (result)
		{
			case CUSTOM_SET_OR_EXECUTE_MODE:
			{
			  
			}
			break;
	
			case CUSTOM_READ_MODE:
			{
				if(xy_info->lockmode)
				{
					 xy_info->vib_p.sw = 1; // 震动报警打开
		              xy_info->move_p.sw = 1;//位移报警开关
				      xy_info->anti_thrief = 1;
					  xy_info_save();
					asklen += sprintf(&askdata[asklen],"HM lock Ok\r\n");
					dynamic_cmd_ask(type,num,askdata,asklen);
					ret = DYNAMIC_AT_RET;
				}
	
			}
			break;
	
			case CUSTOM_TEST_MODE:
	
			break;
	
			case CUSTOM_ACTIVE_MODE:
			
			break;
	
			default:
	
			break;
	
		}
		dynamic_free(askdata);
		return ret;

}
/*******************************************************************
** 函数名:	   dynamic_cmd_000
** 函数描述:   
** 参数:	  
** 返回:	   
********************************************************************/
int dynamic_cmd_000(char type,char*num,char *cmd,dynamic_custom_cmdLine *str)
{
	   int ret = DYNAMIC_AT_ERROR;
		custom_cmd_mode_enum result;
		kal_int16 asklen = 0;
		XY_INFO_T * xy_info = xy_get_info();
		
		char *askdata = dynamic_malloc_malloc(DYNAMIC_CMD_ASK_LEN);
	
		if (askdata == NULL)
		{
			return ret;
		}
		memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
		
		result = dynamic_cmd_find_cmd_mode(str);	
		switch (result)
		{
			case CUSTOM_SET_OR_EXECUTE_MODE:
			{
			  
			}
			break;
	
			case CUSTOM_READ_MODE:
			{
				if(xy_info->lockmode)
				{
					xy_info->vib_p.sw = 0; // 震动报警打开
		            xy_info->move_p.sw = 0;//位移报警开关
				    xy_info->anti_thrief = 0;
					xy_info_save();
					asklen += sprintf(&askdata[asklen],"HM lock Ok\r\n");
					dynamic_cmd_ask(type,num,askdata,asklen);
					ret = DYNAMIC_AT_RET;

				}

			}
			break;
	
			case CUSTOM_TEST_MODE:
	
			break;
	
			case CUSTOM_ACTIVE_MODE:
			
			break;
	
			default:
	
			break;
	
		}
		dynamic_free(askdata);
		return ret;

}

//*************************zack*****************//
/*******************************************************************
** 函数名:    dynamic_cmd_uart_callback
** 函数描述:  
** 输入参数:  
** 输出参数:
** 返回:       
********************************************************************/
int dynamic_cmd_uart_callback(unsigned char *str,int len)
{
	return dynamic_cmd_parse(DYNAMIC_CMD_TYPE_UART,NULL,(char*)str,len);
}

/*******************************************************************
** 函数名:     dynamic_cmd_init
** 函数描述:   短信和串口指令
** 参数:       
** 返回:       
********************************************************************/
void dynamic_cmd_init(void)
{
    DYNAMIC_CMD_T * cmd_table = s_dynamic_cmd_table;

    cmd_table[s_cmd_num].sstr = "RESET"; 		cmd_table[s_cmd_num++].entryproc = dynamic_cmd_reset;
    cmd_table[s_cmd_num].sstr = "DEBUG"; 		cmd_table[s_cmd_num++].entryproc = dynamic_cmd_debug;    
    cmd_table[s_cmd_num].sstr = "ATD"; 			cmd_table[s_cmd_num++].entryproc = dynamic_cmd_atd; 
    cmd_table[s_cmd_num].sstr = "GPSLOG"; 		cmd_table[s_cmd_num++].entryproc = dynamic_cmd_gpslog; 
    cmd_table[s_cmd_num].sstr = "SR"; 			cmd_table[s_cmd_num++].entryproc = dynamic_cmd_sr; 
    cmd_table[s_cmd_num].sstr = "RESETINFO"; 	cmd_table[s_cmd_num++].entryproc = dynamic_cmd_resetinfo;
    cmd_table[s_cmd_num].sstr = "FORMAT"; 		cmd_table[s_cmd_num++].entryproc = dynamic_cmd_format;
	cmd_table[s_cmd_num].sstr = "SERVER"; 			cmd_table[s_cmd_num++].entryproc = dynamic_cmd_server;
	cmd_table[s_cmd_num].sstr = "FACTORY"; 		cmd_table[s_cmd_num++].entryproc = dynamic_cmd_factoy;
	cmd_table[s_cmd_num].sstr = "DEVID"; 		cmd_table[s_cmd_num++].entryproc = dynamic_cmd_devid;
	cmd_table[s_cmd_num].sstr = "TID"; 			cmd_table[s_cmd_num++].entryproc = dynamic_cmd_tid;
	cmd_table[s_cmd_num].sstr = "REPORTMODE"; 	cmd_table[s_cmd_num++].entryproc = dynamic_cmd_report_mode;
	cmd_table[s_cmd_num].sstr = "TIMER"; 		cmd_table[s_cmd_num++].entryproc = dynamic_cmd_timer;
	cmd_table[s_cmd_num].sstr = "HBT";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_hbt;
	cmd_table[s_cmd_num].sstr = "DISTANCE";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_distance;
	cmd_table[s_cmd_num].sstr = "ANGLEREP";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_anglerep;
	cmd_table[s_cmd_num].sstr = "SENDS";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_sends;
	
		
#ifdef __XY_SUPPORT__
  cmd_table[s_cmd_num].sstr = "GPSSNR"; cmd_table[s_cmd_num++].entryproc = xy_cmd_gpssnr;
//**************Zack******************************//
cmd_table[s_cmd_num].sstr = "VERSION";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_version;
cmd_table[s_cmd_num].sstr = "STATUS";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_status;
cmd_table[s_cmd_num].sstr = "WHERE";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_where;
cmd_table[s_cmd_num].sstr = "APN";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_apn;
cmd_table[s_cmd_num].sstr = "SOS";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_sos;

cmd_table[s_cmd_num].sstr = "ACC";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_acc;
cmd_table[s_cmd_num].sstr = "DEFMODE";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_defmode;
cmd_table[s_cmd_num].sstr = "111";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_111;
cmd_table[s_cmd_num].sstr = "000";	  cmd_table[s_cmd_num++].entryproc = dynamic_cmd_000;


//**************Zack******************************//

#endif
    
    if (s_cmd_num > MAX_ATCMD_NUM)
    {
        dynamic_error("dynamic_at_app_init err,no enough space for at cmd!!");
    } 
}

/*******************************************************************
** 函数名:     dynamic_msg_at_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_msg_at_init(void)
{
    dynamic_cmd_init();
    dynamic_uart_debug_port_callback_reg((void*)dynamic_cmd_uart_callback);

}


