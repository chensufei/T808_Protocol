#ifndef __DYNAMIC_API_H__
#define __DYNAMIC_API_H__


#include "dynamic_struct.h"



#define DYNAMIC_DEFINE_API(ret_type,name,...) \
	typedef ret_type(*name##_ptr)(##__VA_ARGS__); \
	extern name##_ptr name
	
#define DYNAMIC_API_ENTRY(name) name##_ptr name = NULL

#define DYNAMIC_LOAD_API_EX(x,y) \
	y = (##y##_ptr)dynamic_load_api(#x); \
	if (!y) \
		dynamic_error("[API] can't load API: %s", #x);

#define DYNAMIC_LOAD_API(x) \
	DYNAMIC_LOAD_API_EX(x,x)



int dynamic_load_apis(void *load_api_ptr);


// ------------- API Function Types
// log
DYNAMIC_DEFINE_API(void *, dynamic_load_api, char *str);
DYNAMIC_DEFINE_API(void,  dynamic_print,DYNAMIC_LOG_LVL_E lvl, char *fmt, ...);
DYNAMIC_DEFINE_API(DYNAMIC_LOG_LVL_E, dynamic_log_get_lvl, void);
DYNAMIC_DEFINE_API(kal_bool, dynamic_log_set_lvl, DYNAMIC_LOG_LVL_E lvl);
DYNAMIC_DEFINE_API(void,dynamic_log_hex,kal_uint8 *str,kal_uint16 len);

// sys
DYNAMIC_DEFINE_API(void,dynamic_sys_reset,void);
DYNAMIC_DEFINE_API(void,dynamic_sys_shutdown,kal_uint32 time);
DYNAMIC_DEFINE_API(kal_uint32,dynamic_sys_get_power_on_reason,void);
DYNAMIC_DEFINE_API(kal_uint32,dynamic_app_crc32,const char *data, int len);
DYNAMIC_DEFINE_API(kal_uint16,dynamic_checksum,kal_uint8 *dptr, kal_uint16 len);
DYNAMIC_DEFINE_API(APP_CNTX_T *,dynamic_app_cntx_get,void);
DYNAMIC_DEFINE_API(void,dynamic_sleep_enable,void);
DYNAMIC_DEFINE_API(void,dynamic_sleep_disable,void);
DYNAMIC_DEFINE_API(kal_uint8,dynamic_sim_get_valid,void);
DYNAMIC_DEFINE_API(kal_int8*,dynamic_find_short_pos,kal_int8 *sptr,kal_uint16 findshort,kal_uint32 maxlen);
DYNAMIC_DEFINE_API(kal_uint16 *,dynamic_ucs2_wcsstr,kal_uint16 *string,kal_uint16 *str_char_set);
DYNAMIC_DEFINE_API(int,dynamic_ascii2bcd,unsigned char *dptr, unsigned char *sptr, int len);
DYNAMIC_DEFINE_API(int,dynamic_hex2ascii,unsigned char *dptr, unsigned char *sptr, int len);
DYNAMIC_DEFINE_API(void,dynamic_sys_sleep_task,kal_uint32 x);
DYNAMIC_DEFINE_API(S32,mmi_ucs2strlen,const CHAR *arrOut);
DYNAMIC_DEFINE_API(CHAR *,mmi_ucs2cpy,CHAR *strDestination, const CHAR *strSource);
DYNAMIC_DEFINE_API(S32,mmi_ucs2cmp,const CHAR *string1, const CHAR *string2);
DYNAMIC_DEFINE_API(S32,mmi_ucs2ncmp,const CHAR *string1, const CHAR *string2, U32 size);
DYNAMIC_DEFINE_API(CHAR *,mmi_ucs2ncpy,CHAR *strDestination, const CHAR *strSource, U32 size);
DYNAMIC_DEFINE_API(U16,mmi_asc_to_ucs2,CHAR *pOutBuffer, CHAR *pInBuffer);


// timer
DYNAMIC_DEFINE_API(kal_bool, dynamic_timer_start, kal_uint32 timerid,kal_uint32 delay, void* funcPtr,void *user,kal_bool repeat);
DYNAMIC_DEFINE_API(kal_bool, dynamic_timer_stop, kal_uint32 timerid);
DYNAMIC_DEFINE_API(kal_bool,dynamic_timer_is_runing,kal_uint32 timerid);
DYNAMIC_DEFINE_API(char*, dynamic_time_get_build_date_time,void);
DYNAMIC_DEFINE_API(void, dynamic_time_get_systime,applib_time_struct *nowTime);
DYNAMIC_DEFINE_API(void, dynamic_time_set_systime,applib_time_struct *nowTime);
DYNAMIC_DEFINE_API(kal_timerid,dynamic_kal_create_timer,kal_char* timer_name_ptr);
DYNAMIC_DEFINE_API(void,dynamic_kal_set_timer,kal_timerid timer_id, kal_timer_func_ptr handler_func_ptr, void* handler_param_ptr, \
                   kal_uint32 delay, kal_uint32 reschedule_time);
DYNAMIC_DEFINE_API(void,dynamic_kal_cancel_timer,kal_timerid timer_id);
DYNAMIC_DEFINE_API(kal_uint32,dynamic_kal_get_systicks,void);
DYNAMIC_DEFINE_API(kal_uint32,dynamic_kal_milli_secs_to_ticks,kal_uint32 milli_secs);
DYNAMIC_DEFINE_API(kal_uint32,dynamic_kal_ticks_to_milli_secs,kal_uint32 ticks);
DYNAMIC_DEFINE_API(void,dynamic_time_utc_to_rtc,float tz, applib_time_struct *utc, applib_time_struct *result);
DYNAMIC_DEFINE_API(void,dynamic_time_rtc_to_utc_with_default_tz,applib_time_struct *rtc, applib_time_struct *result);
DYNAMIC_DEFINE_API(void,dynamic_time_utc_to_rtc_with_default_tz,applib_time_struct *utc, applib_time_struct *result);
DYNAMIC_DEFINE_API(kal_uint32,dynamic_timer_time2sec,applib_time_struct *datetime);

// mem
DYNAMIC_DEFINE_API(void *,dynamic_med_alloc_ext_mem,kal_uint32 size);
DYNAMIC_DEFINE_API(void,dynamic_med_free_ext_mem,void *mem_ptr);
DYNAMIC_DEFINE_API(void *,dynamic_malloc,kal_uint32 size);
DYNAMIC_DEFINE_API(void,dynamic_free,void *mem_ptr);
DYNAMIC_DEFINE_API(kal_uint32,dynamic_mem_left_size,void);
DYNAMIC_DEFINE_API(DYNAMIC_QUEUE_T *,dynamic_queue_create,kal_uint16 limit);
DYNAMIC_DEFINE_API(void,dynamic_queue_destroy,DYNAMIC_QUEUE_T *q);
DYNAMIC_DEFINE_API(int,dynamic_queue_prepend,DYNAMIC_QUEUE_T *q, void *data);
DYNAMIC_DEFINE_API(int,dynamic_queue_is_full,DYNAMIC_QUEUE_T *q);
DYNAMIC_DEFINE_API(int,dynamic_queue_push,DYNAMIC_QUEUE_T *q, void *data);
DYNAMIC_DEFINE_API(void *,dynamic_queue_pop,DYNAMIC_QUEUE_T *q);
DYNAMIC_DEFINE_API(void *,dynamic_queue_peek,DYNAMIC_QUEUE_T *q);

// uart
DYNAMIC_DEFINE_API(void,dynamic_uart_debug_port_callback_reg,void*fun);
DYNAMIC_DEFINE_API(kal_uint16,dynamic_uart_req_read,kal_uint16 port, char *buf, kal_uint16 len);
DYNAMIC_DEFINE_API(kal_uint16,dynamic_uart_put_bytes,kal_uint16 port, char *data, kal_uint32 len);
DYNAMIC_DEFINE_API(void,dynamic_uart_set_baudrate,kal_uint16 port, kal_uint32 baudrate);
DYNAMIC_DEFINE_API(void,dynamic_uart_close,kal_uint16 port);
DYNAMIC_DEFINE_API(kal_bool,dynamic_uart_register,kal_uint16 port, dynamic_uart_cb callback_uart);
DYNAMIC_DEFINE_API(void *,dynamic_uart_open,kal_uint16 port, kal_uint32 baudrate,dynamic_uart_cb callback_uart);
DYNAMIC_DEFINE_API(void,dynamic_uart_configure,kal_uint16 port, UARTDCBStruct *config);  
DYNAMIC_DEFINE_API(void,dynamic_uart_turn_on_power,UART_PORT port, kal_bool enable);

    

// soc
DYNAMIC_DEFINE_API(dynamic_apn_set_sturct * ,dynamic_gprs_get_apn_dns,void);
DYNAMIC_DEFINE_API(int,dynamic_gprs_set_apn_dns,dynamic_apn_set_sturct *info);
DYNAMIC_DEFINE_API(kal_int32,dynamic_soc_send,kal_int8, char *, kal_uint32);
DYNAMIC_DEFINE_API(kal_int8,dynamic_soc_close,kal_int8);
DYNAMIC_DEFINE_API(kal_int8,dynamic_soc_connect,kal_uint8 type,kal_int8 *server, kal_uint16 port,void *soc_cb,void *user_data);
DYNAMIC_DEFINE_API(DYNAMIC_SOC_CONNECTION_T *,dynamic_soc_find_connection,kal_int8);

// http
DYNAMIC_DEFINE_API(enum_http_result,dynamic_http_get_file,char *url,char *buffer,kal_uint32 buffer_size,void *cb,void *user,kal_uint32 cur_len,kal_uint32 total);
DYNAMIC_DEFINE_API(DYNAMIC_HTTP_T * ,dynamic_http_get_info,void);

// fs
DYNAMIC_DEFINE_API(int,dynamic_fs_open,char* FileName, kal_uint32 flag);
DYNAMIC_DEFINE_API(int,dynamic_fs_read,int fileHandle, kal_uint8 *readBuffer, kal_uint32 numberOfBytesToRead, kal_uint32 *numberOfBytesRead);
DYNAMIC_DEFINE_API(int,dynamic_fs_write,int fileHandle, kal_uint8 *writeBuffer, kal_uint32 numberOfBytesToWrite, kal_uint32 *numberOfBytesWritten);
DYNAMIC_DEFINE_API(int,dynamic_fs_seek,int fileHandle, int offset, kal_uint32 whence);
DYNAMIC_DEFINE_API(int,dynamic_fs_get_file_position,int fileHandle);
DYNAMIC_DEFINE_API(int,dynamic_fs_truncate,int fileHandle);
DYNAMIC_DEFINE_API(void,dynamic_fs_flush,int fileHandle);
DYNAMIC_DEFINE_API(void,dynamic_fs_close,int fileHandle);
DYNAMIC_DEFINE_API(int,dynamic_fs_get_size,char *lpFileName);
DYNAMIC_DEFINE_API(int,dynamic_fs_delete,char *lpFileName);
DYNAMIC_DEFINE_API(int,dynamic_fs_check,char *lpFileName);
DYNAMIC_DEFINE_API(int,dynamic_fs_rename,char *lpFileName, char *newLpFileName);
DYNAMIC_DEFINE_API(int,dynamic_fs_create_dir,char *lpDirName);
DYNAMIC_DEFINE_API(int,dynamic_fs_delete_dir,char *lpDirName);
DYNAMIC_DEFINE_API(int,dynamic_fs_check_dir,char *lpDirName);
DYNAMIC_DEFINE_API(kal_int64,dynamic_fs_get_free_space,void);
DYNAMIC_DEFINE_API(kal_int64,dynamic_fs_get_total_space,void);
DYNAMIC_DEFINE_API(int,dynamic_fs_format,void);

// adc
DYNAMIC_DEFINE_API(kal_bool,dynamic_adc_get_channel_voltage,DCL_ADC_CHANNEL_TYPE_ENUM ch, kal_uint32 *voltage);
DYNAMIC_DEFINE_API(void,dynamic_adc_get_data,BATPHYStruct *BATPHYS);
DYNAMIC_DEFINE_API(CHARGER_IN_OUT_STATUS,dynamic_adc_get_chr_status,void);
DYNAMIC_DEFINE_API(kal_bool,dynamic_adc_is_charge_full,void);
DYNAMIC_DEFINE_API(kal_uint8,dynamic_adc_vbat_percentage,kal_uint32 vbat);


// telephony
DYNAMIC_DEFINE_API(kal_uint8,dynamic_telephony_call_status,void);
DYNAMIC_DEFINE_API(void,dynamic_telephony_call_cb_reg,CALL_CB cb);
DYNAMIC_DEFINE_API(void,dynamic_telephony_sms_recv_reg,SMS_CB cb);
DYNAMIC_DEFINE_API(kal_bool,dynamic_telephony_dial,char *number);
DYNAMIC_DEFINE_API(kal_bool,dynamic_telephony_reject,void);
DYNAMIC_DEFINE_API(kal_bool,dynamic_telephony_accept,void);
DYNAMIC_DEFINE_API(kal_bool,dynamic_telephony_sms_send,char *number,char *content,SMS_SEND_CB callback);
DYNAMIC_DEFINE_API(kal_bool,dynamic_telephony_sms_del_all,void);



// I2C
DYNAMIC_DEFINE_API(void,dynamic_i2c_configure,kal_uint32 slave_addr, kal_uint32 speed);
DYNAMIC_DEFINE_API(kal_bool,dynamic_i2c_receive,kal_uint8 ucDeviceAddr, kal_uint8 ucBufferIndex, kal_uint8* pucData, kal_uint32 unDataLength);
DYNAMIC_DEFINE_API(kal_bool,dynamic_i2c_send,kal_uint8 ucDeviceAddr, kal_uint8 ucBufferIndex, kal_uint8* pucData, kal_uint32 unDataLength);


// LBS
DYNAMIC_DEFINE_API(lbs_struct *,dynamic_lbs_get_info,void);
DYNAMIC_DEFINE_API(void,dynamic_lbs_req,void* str);


// GPIO,EINT
DYNAMIC_DEFINE_API(void,GPIO_ModeSetup,kal_uint16 pin, kal_uint16 conf_dada);
DYNAMIC_DEFINE_API(void,GPIO_InitIO,char direction, kal_uint16 port);
DYNAMIC_DEFINE_API(char,GPIO_ReadIO,kal_uint16 port);
DYNAMIC_DEFINE_API(void,GPIO_WriteIO,char data, kal_uint16 port);
DYNAMIC_DEFINE_API(void,GPIO_PullenSetup,kal_uint16 pin, kal_bool enable);
DYNAMIC_DEFINE_API(void,GPIO_PullSelSetup,kal_uint16 pin, kal_bool pull_up);
DYNAMIC_DEFINE_API(kal_uint32,EINT_Set_Sensitivity,kal_uint8 eintno, kal_bool sens);
DYNAMIC_DEFINE_API(void,EINT_Set_Polarity,kal_uint8 eintno, kal_bool ACT_Polarity);
DYNAMIC_DEFINE_API(kal_int32,EINT_SW_Debounce_Modify,kal_uint8 eintno, kal_uint8 debounce_time);
DYNAMIC_DEFINE_API(void,EINT_Registration,kal_uint8 eintno, kal_bool Dbounce_En, kal_bool ACT_Polarity, \
                       void (reg_hisr)(void), kal_bool auto_umask);
DYNAMIC_DEFINE_API(void,EINT_UnMask,kal_uint8 eintno);
DYNAMIC_DEFINE_API(void,EINT_Mask,kal_uint8 eintno);
DYNAMIC_DEFINE_API(void,EINT_Set_HW_Debounce,kal_uint8 eintno, kal_uint32 ms);


// MSG AT
DYNAMIC_DEFINE_API(int,dynamic_msgbased_at_send,char* cmd, kal_uint32 len, msgbased_at_cb cb, void* user);
DYNAMIC_DEFINE_API(void,dynamic_msgbased_at_reg_cb,msgbased_at_cb fun);

// audio
DYNAMIC_DEFINE_API(void,dynamic_audio_set_audio_mode,audio_mode_enum mode);
DYNAMIC_DEFINE_API(void,dynamic_audio_set_volume,volume_type_enum type, kal_uint8 volume);
DYNAMIC_DEFINE_API(void,dynamic_record_cb_reg,void* cb);
DYNAMIC_DEFINE_API(void,dynamic_record_recv_data,kal_uint8 *data,kal_uint32 len);
DYNAMIC_DEFINE_API(void,dynamic_record_start,void);
DYNAMIC_DEFINE_API(void,dynamic_record_stop,void);


// GPS
DYNAMIC_DEFINE_API(void,dynamic_gps_recv_cb_reg,DYNAMIC_GPS_CB cb);
DYNAMIC_DEFINE_API(void,dynamic_gps_pow_crl,kal_uint8 onoff);
DYNAMIC_DEFINE_API(kal_uint8,dynamic_gps_get_state,void);
DYNAMIC_DEFINE_API(kal_uint16,dynamic_gps_uart_write,kal_uint8 *buffer, kal_uint16 length);
DYNAMIC_DEFINE_API(kal_uint8,dynamic_gps_time_aid,applib_time_struct cur_time);
DYNAMIC_DEFINE_API(kal_uint8,dynamic_gps_refloc_aid,double lat,double lng,double alt);
DYNAMIC_DEFINE_API(void,dynamic_gps_epo_req,void);
DYNAMIC_DEFINE_API(kal_bool,dynamic_epo_is_valid,void);
DYNAMIC_DEFINE_API(kal_bool,dynamic_gps_epo_aiding,void);


#endif
