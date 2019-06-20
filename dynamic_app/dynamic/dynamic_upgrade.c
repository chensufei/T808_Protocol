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


static kal_uint8 * s_http_str = NULL;
static kal_uint8 s_http_err = 0;

void dynamic_http_download_cb(int result,void *user,int downLoadLen,int total );
enum_http_result dynamic_http_start(char*url,kal_uint32 star);

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
** 函数名:     dynamic_http_ver_ask
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_http_ver_ask(char* data)
{
    dynamic_debug("dynamic_http_ver_ask:%s",data);

    // 解析
    /*
    {"R":0,   //为0收到成功
    "KEY":"*",//密码  没有为*
    "SDURL":"http://39.104.208.59:4004/File/20190307/3E01.gz",//升级地址，没有为　*
    "UTC":"20190422184519",  //对时 UTC时间
    "CA":"*"}  //是否修改业务服务器IP   * 表示不修改     110。1。1。1，3434
    */

    //dynamic_http_start("http://39.104.208.59:4004/File/20190307/3E01.gz",0); // 开始下载升级包
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
    char *ver_info = NULL;

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

        if (dynamic_save_app(http_info->buffer,http_info->file_size) == 0)
        {            
            if (strncmp((char*)http_info->buffer,"{\"R\"",4) == 0)
            {
                if (http_info->file_size < MAX_HTTP_HOST_LEN)
                {
                    ver_info = dynamic_malloc_malloc(MAX_HTTP_HOST_LEN);
                    if (ver_info != NULL)
                    {
                        memcpy(ver_info,(char*)http_info->buffer,http_info->file_size);
                    }
                }
            }
        }
        memset(http_info,0,sizeof(DYNAMIC_HTTP_T));
        dynamic_free(s_http_str);
        
        if (ver_info != NULL)
        {
            dynamic_http_ver_ask(ver_info);
            dynamic_free(ver_info);
        }
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

void dynamic_http_test(void)
{
    dynamic_http_start(URL_TEST,0);
}


