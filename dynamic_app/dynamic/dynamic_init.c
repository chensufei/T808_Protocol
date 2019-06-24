
#include <locale.h>
#include "dynamic_init.h"

extern unsigned char Image$$RO$$Base;
extern unsigned char Image$$RO$$Limit;
extern unsigned char Image$$RW$$Base;
extern unsigned char Image$$RW$$Limit;

uint32 MSG_ID_DYNAMIC_BASE = 0;


int dynamic_image_size(void) {
	int ro_size = (&Image$$RO$$Limit - &Image$$RO$$Base);
	int rw_size = (&Image$$RW$$Limit - &Image$$RW$$Base);
	
	return ro_size + rw_size;
}

/*******************************************************************
** 函数名:     dynamic_main
** 函数描述:   入口函数
** 参数:       
** 返回:       
********************************************************************/
int dynamic_main(void *load_api_ptr, unsigned int msg_id_base)
{
	setlocale(LC_ALL, "C");
	
	MSG_ID_DYNAMIC_BASE = msg_id_base;

	if (!dynamic_load_apis(load_api_ptr))
	{
        dynamic_log("\r\n-----------dynamic app-------------\r\n");
	    dynamic_log("soft ver:%s\r\n",DYNAMIC_SOFT_VER);

	    dynamic_log("build date time:%s\r\n",dynamic_time_get_build_date_time());
	    dynamic_log("total space:%d\r\n",dynamic_fs_get_total_space());
	    dynamic_log("free space:%d\r\n",dynamic_fs_get_free_space());

        if (dynamic_image_size() > MAX_IMAGE_SIZE)
        {
        	dynamic_error("[SYS]image size overflow, max 200Kb!!! %d", dynamic_image_size());
        	return 0;
        }
        dynamic_log_set_lvl(DYNAMIC_LOG_DEBUG);
        dynamic_reset_init();
        dynamic_telephony_init();
	    dynamic_msg_at_init();
	    dynamic_gps_init();
	    //dynamic_sensor_init();
#ifdef __XY_SUPPORT__
	    xy_main_init();
#endif
        dynamic_telephony_sms_del_all();

	    return 1;
	}
	
	return 0;
}
