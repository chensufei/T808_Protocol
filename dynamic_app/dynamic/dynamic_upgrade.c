/********************************************************************************
**
** 文件名:     dynamic_upgrade.c
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

#define DYNAMIC_UPGRADE_URL "http://39.104.208.59:4004"
#define DYNAMIC_UPGRADE_PID "5509253"

#define URL_TEST "http://39.104.208.59:4004/api/upgrade/software?SV=3E00_2019/03/01&HV=T920_20190301&IMEI=868893045509253//IMEI&SIM=93045509253&PID=5509253&IP1=222.243.128.10:6977&ICCID=89860405191871807566"

#define MAX_HTTP_ERR_CNT 20 // 下载失败重连次数


typedef struct _dynamic_zip_file
{		
		char tag_flage[8];
		kal_int32 tag_filelen;
		kal_int32 tag_type_info_len;
		kal_int32 tag_gps_data_len;
		kal_uint32 tag_crc;
		char tag_ver[4];
		char tag_buffer[1];
		char temp[3];
}dynamic_zip_file;  // bin压缩文件


typedef enum 
{
	UPGRADE_STATE_REP_VER,
	UPGRADE_STATE_RECV_ASK,
	UPGRADE_STATE_DOWNLOADING,
	UPGRADE_STATE_FINISH
}UPGRADE_STATE_E;

#define MAX_UPGRADE_ERR_CNT 6

UPGRADE_STATE_E s_upgrade_state = UPGRADE_STATE_REP_VER;
static kal_uint8 s_upgrade_err = 0;
static kal_uint8 * s_http_str = NULL;
static kal_uint8 s_http_err = 0;
static char *s_recv_str = NULL;
static char *s_url_str = NULL;
static char s_eskey[32]={"*"};


void dynamic_http_download_cb(int result,void *user,int downLoadLen,int total );
enum_http_result dynamic_http_start(char*url,kal_uint32 star);
void dynamic_http_upgrade_task(void*str);



/*******************************************************************
** 函数名:     dynamic_http_state
** 函数描述:  
** 参数:       
** 返回:       0空闲状态  1下载中
********************************************************************/
kal_uint8 dynamic_http_state(void)
{
    if (s_http_str == NULL)
    {
        return 0;
    }
    return 1;
}

/*******************************************************************
** 函数名:     dynamic_check_app
** 函数描述:   校验升级包是否正常
** 参数:       
** 返回:       
********************************************************************/
static int dynamic_check_app(kal_uint8 *data,kal_uint32 len)
{
	dynamic_zip_file *zip_head = 0;
	unsigned long  crc = 0;
	int nLen =0;
	zip_head = (dynamic_zip_file *)data;
	nLen = zip_head->tag_gps_data_len +  sizeof(dynamic_zip_file);
	
	if((zip_head->tag_gps_data_len +  sizeof(dynamic_zip_file)-4) == ( unsigned int)zip_head->tag_filelen)
	{    
		crc = dynamic_app_crc32(zip_head->tag_buffer, zip_head->tag_gps_data_len);//获取crc32值				
	}

	dynamic_debug(" file len = %d,  crc = %x, nLen = %d, cal file crc = %x", zip_head->tag_filelen, zip_head->tag_crc, nLen, crc);
	
	if(crc == zip_head->tag_crc && crc != 0)
	{
		return zip_head->tag_filelen;
	}
	else
	{
		dynamic_error(" err zip_head->tag_crc=%d crc = %d",zip_head->tag_crc,crc);
        return 0;
	}
}

/*******************************************************************
** 函数名:     dynamic_save_app
** 函数描述:   保存升级包并重启
** 参数:       
** 返回:       
********************************************************************/
int dynamic_save_app(kal_uint8 *data,kal_uint32 len)
{
    kal_uint32 space_size = 0;
    kal_int32 fiel_size = 0;
    
    if(len > 0 && dynamic_check_app(data,len) > 0)
	{
		space_size = dynamic_fs_get_free_space(); 
        fiel_size = dynamic_fs_get_size(DYNAMIC_APP_FILE_NAME);	
        dynamic_debug("cur app size = %d",fiel_size);
        if (fiel_size > 0)
        {
            space_size += fiel_size;
        }
        
		dynamic_debug("download app size = %d,cur app size = %d, space_size = %d",len, space_size);
		if (space_size >= len)
		{
            dynamic_file_dele(DYNAMIC_APP_FILE_NAME);

            if (dynamic_file_write(DYNAMIC_APP_FILE_NAME,data,len) > 0)
            {
                dynamic_log("download app success");
                dynamic_start_reset(RESET_TYPE_UPGRADE);
                return 1;
            }
            else
            {
                dynamic_error("ERROR! write file err");
            }
		}
		else
		{
			dynamic_error(" ERROR! download app size = %d, but space = %d",len, space_size);
		}
	}

	return 0;
}


/*******************************************************************
** 函数名:     dynamic_http_download_continue
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_http_download_continue(void* str)
{
    DYNAMIC_HTTP_T * http_info = dynamic_http_get_info();
    enum_http_result http_result;
    
    dynamic_debug("http download continue:%d",s_http_err);
    if (++s_http_err >= MAX_HTTP_ERR_CNT)
    {
        s_http_err = 0;
        memset(http_info,0,sizeof(DYNAMIC_HTTP_T));
        dynamic_debug("http download err");
        dynamic_free(s_http_str);
        return;
    }
    
    http_result = dynamic_http_get_file((char*)http_info->url,(char*)&s_http_str[http_info->cur_len],MAX_IMAGE_SIZE,(void *)dynamic_http_download_cb,NULL,http_info->cur_len,http_info->file_size);
    if (http_result != enum_http_result_ok)
    {
        dynamic_timer_start(enum_timer_http_continue_timer,5*1000,(void*)dynamic_http_download_continue,NULL,FALSE);    
    }
}

/*******************************************************************
** 函数名:     dynamic_http_download_cb
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_http_download_cb(int result,void *user,int downLoadLen,int total )
{
    DYNAMIC_HTTP_T * http_info = dynamic_http_get_info();
    

    if (http_info == NULL || s_http_str == NULL)
    {
        dynamic_debug("dynamic_http_download_cb http_info == NULL || s_http_str == NULL");
        return;
    }
    dynamic_debug("dynamic_http_download_cb ");
    dynamic_debug("result:%d",result);
    dynamic_debug("url:%s,%s:%d",http_info->url,http_info->host,http_info->port);
    dynamic_debug("downLoadLen:%d",downLoadLen);
    dynamic_debug("total:%d",total);

    if ((downLoadLen < total) || (result != enum_http_result_download_ok && result != enum_http_result_exited))
    {
        dynamic_timer_start(enum_timer_http_continue_timer,3*1000,(void*)dynamic_http_download_continue,NULL,FALSE);
    }
    else
    {
        s_http_err = 0;
        dynamic_debug("http download success");

		if (strncmp((char*)http_info->buffer,"{\"R\"",4) == 0)
        {
			if (http_info->file_size < MAX_HTTP_FEAME_SIZE)
			{
				if (s_recv_str == NULL)
				{
					s_recv_str = dynamic_malloc_malloc(MAX_HTTP_FEAME_SIZE+1);
				}
				
				if (s_recv_str != NULL)
				{
					memcpy(s_recv_str,(char*)http_info->buffer,http_info->file_size);
					s_upgrade_state = UPGRADE_STATE_RECV_ASK;
					dynamic_timer_start(enum_timer_http_task_timer,100,(void*)dynamic_http_upgrade_task,NULL,KAL_FALSE);
				}
			}
        }
		else
        {            
			dynamic_save_app(http_info->buffer,http_info->file_size);
        }
        memset(http_info,0,sizeof(DYNAMIC_HTTP_T));
        dynamic_free(s_http_str);
    }
}

/*******************************************************************
** 函数名:     dynamic_http_start
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
enum_http_result dynamic_http_start(char*url,kal_uint32 star)
{
    enum_http_result result;

    if (s_http_str == NULL)
    {
        s_http_str = dynamic_malloc_malloc(MAX_IMAGE_SIZE);
        s_http_err = 0;
    }
    result = dynamic_http_get_file(url,(char*)s_http_str,MAX_IMAGE_SIZE,(void *)dynamic_http_download_cb,NULL,star,0);
    dynamic_debug("dynamic_http_start:%d,start:%u",result,star);
    if (result != enum_http_result_ok && result != enum_http_result_exited)
    {
        dynamic_free(s_http_str);
    }
    return result;
}

/*******************************************************************
** 函数名:     dynamic_http_upgrage_rep_ver
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
enum_http_result dynamic_http_upgrage_rep_ver(void)
{
	enum_http_result ret;
	char url[MAX_HTTP_FEAME_SIZE];
	kal_uint16 len = 0;
	APP_CNTX_T * app_cntx = dynamic_app_cntx_get();
	applib_time_struct nowTime;

	memset(url,0,sizeof(url));
	len += sprintf(url,"%s",DYNAMIC_UPGRADE_URL); // 升级服务器地址
	len += sprintf(&url[len],"/hadware/upgrade?SV=%s&HV=%s",M_DYNAMIC_SOFTWARE_VER,M_DYNAMIC_HARDWARE_VER); // 软硬件版本
    dynamic_time_get_systime(&nowTime);
	len += sprintf(&url[len],"&UTC=%d%02d%02d%02d%02d%02d",nowTime.nYear,nowTime.nMonth,nowTime.nDay,
	                nowTime.nHour,nowTime.nMin,nowTime.nSec); // UTC
	len += sprintf(&url[len],"&TZONE=+%d",8); // 设备时区
	len += sprintf(&url[len],"&IMEI=%s",app_cntx->imei); // IMEI
	len += sprintf(&url[len],"&ICCID=%s",app_cntx->iccid); // ICCID
	len += sprintf(&url[len],"&PID=%s",DYNAMIC_UPGRADE_PID); // PID
	len += sprintf(&url[len],"&ESKEY=%s",s_eskey); // ESKEY
	
	ret = dynamic_http_start(url,0);

	return ret;
}


/*******************************************************************
** 函数名:     dynamic_http_ver_ask
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
kal_uint8 dynamic_http_ver_ask(char* data)
{
	
	kal_uint8 r = 0;
	char key[32];
	char sdurl[MAX_HTTP_URL_LEN];
	char utc[32];
	char ca[128];	
	kal_uint8 ret;
	
	memset(key,0,sizeof(key));
	memset(sdurl,0,sizeof(sdurl));
	memset(utc,0,sizeof(utc));
	memset(ca,0,sizeof(ca));
    dynamic_debug("dynamic_http_ver_ask:%s",data);

    ret = sscanf(data, "\"Result\":%hhd,\"ESKEY\":\"%s\",\"SoftDowloadURL\":\"%s\",\"UTC\":\"%s\",\"ConnectAdress\":\"%s\"", //
            &r,key,sdurl,utc,ca);	

	dynamic_debug("ret:%d",ret);
	dynamic_debug("Result:%d",r);
	dynamic_debug("ESKEY:%s",key);
	dynamic_debug("SoftDowloadURL:%s",sdurl);
	dynamic_debug("UTC:%s",utc);
	dynamic_debug("ConnectAdress:%s",ca);

	if (ret == 5)
	{
		if (r == 0 && strlen(sdurl) > 10)
		{
			if (s_url_str == NULL)
			{
				s_url_str = dynamic_malloc_malloc(MAX_HTTP_URL_LEN);
			}

			if (s_url_str != NULL)
			{
				memcpy(s_url_str,sdurl,strlen(sdurl));
				return 1;
			}
		}
	}
	else
	{
		dynamic_debug("数据解析失败");
	}
	
	return 0;
}


/*******************************************************************
** 函数名:     dynamic_http_upgrade_task
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
void dynamic_http_upgrade_task(void*str)
{
	kal_uint32 time = 10*1000;
	APP_CNTX_T * app_cntx = dynamic_app_cntx_get();
	enum_http_result ret;

	dynamic_debug("dynamic_http_upgrade_task:%d",s_upgrade_state);
	switch (s_upgrade_state)
	{
		case UPGRADE_STATE_REP_VER:
			if (app_cntx->gsm_state == 1)
			{
				ret = dynamic_http_upgrage_rep_ver();
				if (ret != enum_http_result_ok)
				{
					dynamic_debug("上报软件版本失败 ret:%d,%d",ret,s_upgrade_err);
				}
				else
				{
					dynamic_debug("软件版本上报中:%d",ret);
					time = 60*1000;
				}
				
				if (++s_upgrade_err > MAX_UPGRADE_ERR_CNT)
				{
					s_upgrade_err = 0;
					s_upgrade_state = UPGRADE_STATE_FINISH;
					time = 100;
					dynamic_debug("上报软件版本失败，退出升级");
				}
			}
		break;
			
		case UPGRADE_STATE_RECV_ASK:
			dynamic_debug("收到服务器应答");
			s_upgrade_err = 0;
			if (dynamic_http_ver_ask(s_recv_str) == 0)
			{
				dynamic_debug("不需要升级");
				s_upgrade_state = UPGRADE_STATE_FINISH;
			}
			else
			{
				s_upgrade_state = UPGRADE_STATE_DOWNLOADING;
			}
			dynamic_free(s_recv_str);
			time = 100;
		break;

		case UPGRADE_STATE_DOWNLOADING:
			if (s_url_str)
			{
				ret = dynamic_http_start(s_url_str,0);
				//dynamic_http_start("http://39.104.208.59:4004/File/20190307/3E01.gz",0); // 开始下载升级包
				if (ret == enum_http_result_ok)
				{
					dynamic_debug("开始下载升级包");
					s_upgrade_state = UPGRADE_STATE_FINISH;
					time = 100;
				} 
				else if (ret == enum_http_result_exited)
				{
					dynamic_debug("已在下载中");
					s_upgrade_state = UPGRADE_STATE_FINISH;
					time = 100;
				}
				else
				{
					dynamic_debug("准备下载升级包:%d",s_upgrade_state);
					if (++s_upgrade_err > MAX_UPGRADE_ERR_CNT)
					{
						s_upgrade_err = 0;
						s_upgrade_state = UPGRADE_STATE_FINISH;
						time = 100;
						dynamic_debug("启动下载失败，退出升级");
					}
				}
			}
			else
			{
				dynamic_debug("升级链接异常");
				s_upgrade_state = UPGRADE_STATE_FINISH;
				time = 100;
			}
		break;	
		
		case UPGRADE_STATE_FINISH:
			s_upgrade_err = 0;
			s_upgrade_state = UPGRADE_STATE_REP_VER;
			if (s_url_str != NULL)
			{
				dynamic_free(s_url_str);
			}
		break;

		default:

		break;

	}

	dynamic_timer_start(enum_timer_http_task_timer,time,(void*)dynamic_http_upgrade_task,NULL,KAL_FALSE);
}

/*******************************************************************
** 函数名:     dynamic_http_init
** 函数描述:  
** 参数:       
** 返回:       
********************************************************************/
void dynamic_http_init(void)
{
	dynamic_timer_start(enum_timer_http_task_timer,20*1000,(void*)dynamic_http_upgrade_task,NULL,KAL_FALSE);
}


void dynamic_http_test(void)
{
    dynamic_http_start(URL_TEST,0);
}


