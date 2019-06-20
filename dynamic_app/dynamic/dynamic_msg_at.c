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
	switch (type)
	{
	    case DYNAMIC_CMD_TYPE_SERVER:

	    break;

	    case DYNAMIC_CMD_TYPE_SMS:
            dynamic_sms_list_add(num,asklen,askdata);
            dynamic_timer_start(enum_timer_sms_reset_timer,30*1000, (void *)dynamic_reset_delay,0,0);
	    break;

        case DYNAMIC_CMD_TYPE_UART:
	    default:
            dynamic_cmd_uart_write(askdata,asklen);
            dynamic_start_reset(RESET_TYPE_FOR_CMD);
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
    memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
    result = dynamic_cmd_find_cmd_mode(str); 	
    switch (result)
    {
        case CUSTOM_SET_OR_EXECUTE_MODE:
		    if (sscanf(&str->character[str->position],"%lld",&debug_lv) == 1)
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
    memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
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
    memset(askdata,0,DYNAMIC_CMD_ASK_LEN);
    
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
            APP_CNTX_T * app_cntx = dynamic_app_cntx_get();			
			
            asklen += sprintf(&askdata[asklen],"ver:%s\r\n",DYNAMIC_SOFT_VER);
    	    asklen += sprintf(&askdata[asklen],"build date time:%s\r\n",dynamic_time_get_build_date_time());
    	    asklen += sprintf(&askdata[asklen],"space total:%lld,%lld\r\n",dynamic_fs_get_total_space(),dynamic_fs_get_free_space());
            asklen += sprintf(&askdata[asklen],"plmn:%s\r\n",app_cntx->sim_plmn);
            asklen += sprintf(&askdata[asklen],"imei:%s\r\n",app_cntx->imei);
            asklen += sprintf(&askdata[asklen],"csq:%d\r\n",app_cntx->csq);
            asklen += sprintf(&askdata[asklen],"ref_reset_type:%d\r\n",sys_info->ref_reset_type);
            asklen += sprintf(&askdata[asklen],"end gps lat:%f,lng:%f\r\n",sys_info->end_gps.lat,sys_info->end_gps.lng);
            
            dynamic_cmd_ask(type,num,askdata,asklen);
            ret = DYNAMIC_AT_RET;	
        }
        break;

        default:

        break;

    }
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
    memset(askdata,0,DYNAMIC_CMD_ASK_LEN);

	result = dynamic_cmd_find_cmd_mode(str);
	switch (result)
	{
		case CUSTOM_ACTIVE_MODE:	
			dynamic_sys_info_reset();
			dynamic_sms_list_reset();

			asklen = sprintf(askdata,"%s:%s\r\n",cmd,STRING_CMD_OK);
			dynamic_cmd_ask(type,num,askdata,asklen);
			ret = DYNAMIC_AT_RET;
			
		break;

       default:

		break;

	}
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
** 函数名:    dynamic_cmd_uart_callback
** 函数描述:  
** 输入参数:  
** 输出参数:
** 返回:       
********************************************************************/
int dynamic_cmd_uart_callback(unsigned char *str,int len)
{
	return dynamic_cmd_parse(DYNAMIC_CMD_TYPE_SMS,NULL,(char*)str,len);
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

    cmd_table[s_cmd_num].sstr = "RESET"; cmd_table[s_cmd_num++].entryproc = dynamic_cmd_reset;
    cmd_table[s_cmd_num].sstr = "DEBUG"; cmd_table[s_cmd_num++].entryproc = dynamic_cmd_debug;    
    cmd_table[s_cmd_num].sstr = "ATD"; cmd_table[s_cmd_num++].entryproc = dynamic_cmd_atd; 
    cmd_table[s_cmd_num].sstr = "GPSLOG"; cmd_table[s_cmd_num++].entryproc = dynamic_cmd_gpslog; 
    cmd_table[s_cmd_num].sstr = "SR"; cmd_table[s_cmd_num++].entryproc = dynamic_cmd_sr; 
    cmd_table[s_cmd_num].sstr = "RESETINFO"; cmd_table[s_cmd_num++].entryproc = dynamic_cmd_resetinfo;
    cmd_table[s_cmd_num].sstr = "FORMAT"; cmd_table[s_cmd_num++].entryproc = dynamic_cmd_format;

#ifdef __XY_SUPPORT__

    cmd_table[s_cmd_num].sstr = "GPSSNR"; cmd_table[s_cmd_num++].entryproc = xy_cmd_gpssnr;

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


