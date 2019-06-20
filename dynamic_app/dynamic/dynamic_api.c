
#include "dynamic_api.h"


// log
DYNAMIC_API_ENTRY(dynamic_load_api);
DYNAMIC_API_ENTRY(dynamic_print);
DYNAMIC_API_ENTRY(dynamic_log_get_lvl);
DYNAMIC_API_ENTRY(dynamic_log_set_lvl);
DYNAMIC_API_ENTRY(dynamic_log_hex);


// sys
DYNAMIC_API_ENTRY(dynamic_sys_reset);
DYNAMIC_API_ENTRY(dynamic_sys_shutdown);
DYNAMIC_API_ENTRY(dynamic_sys_get_power_on_reason);
DYNAMIC_API_ENTRY(dynamic_app_crc32);
DYNAMIC_API_ENTRY(dynamic_checksum);
DYNAMIC_API_ENTRY(dynamic_app_cntx_get);
DYNAMIC_API_ENTRY(dynamic_sleep_enable);
DYNAMIC_API_ENTRY(dynamic_sleep_disable);
DYNAMIC_API_ENTRY(dynamic_sim_get_valid);
DYNAMIC_API_ENTRY(dynamic_find_short_pos);
DYNAMIC_API_ENTRY(dynamic_ucs2_wcsstr);
DYNAMIC_API_ENTRY(dynamic_ascii2bcd);
DYNAMIC_API_ENTRY(dynamic_hex2ascii);
DYNAMIC_API_ENTRY(dynamic_sys_sleep_task);
DYNAMIC_API_ENTRY(mmi_ucs2strlen);
DYNAMIC_API_ENTRY(mmi_ucs2cpy);
DYNAMIC_API_ENTRY(mmi_ucs2cmp);
DYNAMIC_API_ENTRY(mmi_ucs2ncmp);
DYNAMIC_API_ENTRY(mmi_ucs2ncpy);
DYNAMIC_API_ENTRY(mmi_asc_to_ucs2);



// timer
DYNAMIC_API_ENTRY(dynamic_timer_start);
DYNAMIC_API_ENTRY(dynamic_timer_stop);
DYNAMIC_API_ENTRY(dynamic_timer_is_runing);
DYNAMIC_API_ENTRY(dynamic_time_get_build_date_time);
DYNAMIC_API_ENTRY(dynamic_time_get_systime);
DYNAMIC_API_ENTRY(dynamic_time_set_systime);
DYNAMIC_API_ENTRY(dynamic_kal_create_timer);
DYNAMIC_API_ENTRY(dynamic_kal_set_timer);
DYNAMIC_API_ENTRY(dynamic_kal_cancel_timer);
DYNAMIC_API_ENTRY(dynamic_kal_get_systicks);
DYNAMIC_API_ENTRY(dynamic_kal_milli_secs_to_ticks);
DYNAMIC_API_ENTRY(dynamic_kal_ticks_to_milli_secs);
DYNAMIC_API_ENTRY(dynamic_time_utc_to_rtc);
DYNAMIC_API_ENTRY(dynamic_time_rtc_to_utc_with_default_tz);
DYNAMIC_API_ENTRY(dynamic_time_utc_to_rtc_with_default_tz);
DYNAMIC_API_ENTRY(dynamic_timer_time2sec);
    
// mem
DYNAMIC_API_ENTRY(dynamic_med_alloc_ext_mem);
DYNAMIC_API_ENTRY(dynamic_med_free_ext_mem);
DYNAMIC_API_ENTRY(dynamic_malloc);
DYNAMIC_API_ENTRY(dynamic_free);
DYNAMIC_API_ENTRY(dynamic_mem_left_size);
DYNAMIC_API_ENTRY(dynamic_queue_create);
DYNAMIC_API_ENTRY(dynamic_queue_destroy);
DYNAMIC_API_ENTRY(dynamic_queue_prepend);
DYNAMIC_API_ENTRY(dynamic_queue_is_full);
DYNAMIC_API_ENTRY(dynamic_queue_push);
DYNAMIC_API_ENTRY(dynamic_queue_pop);
DYNAMIC_API_ENTRY(dynamic_queue_peek);

// uart
DYNAMIC_API_ENTRY(dynamic_uart_debug_port_callback_reg);
DYNAMIC_API_ENTRY(dynamic_uart_req_read);
DYNAMIC_API_ENTRY(dynamic_uart_put_bytes);
DYNAMIC_API_ENTRY(dynamic_uart_set_baudrate);
DYNAMIC_API_ENTRY(dynamic_uart_close);
DYNAMIC_API_ENTRY(dynamic_uart_register);
DYNAMIC_API_ENTRY(dynamic_uart_open);
DYNAMIC_API_ENTRY(dynamic_uart_configure);
DYNAMIC_API_ENTRY(dynamic_uart_turn_on_power);



// soc
DYNAMIC_API_ENTRY(dynamic_gprs_get_apn_dns);
DYNAMIC_API_ENTRY(dynamic_gprs_set_apn_dns);
DYNAMIC_API_ENTRY(dynamic_soc_send);
DYNAMIC_API_ENTRY(dynamic_soc_close);
DYNAMIC_API_ENTRY(dynamic_soc_connect);
DYNAMIC_API_ENTRY(dynamic_soc_find_connection);
// http
DYNAMIC_API_ENTRY(dynamic_http_get_file);
DYNAMIC_API_ENTRY(dynamic_http_get_info);

// fs
DYNAMIC_API_ENTRY(dynamic_fs_open);
DYNAMIC_API_ENTRY(dynamic_fs_read);
DYNAMIC_API_ENTRY(dynamic_fs_write);
DYNAMIC_API_ENTRY(dynamic_fs_seek);
DYNAMIC_API_ENTRY(dynamic_fs_get_file_position);
DYNAMIC_API_ENTRY(dynamic_fs_truncate);
DYNAMIC_API_ENTRY(dynamic_fs_flush);
DYNAMIC_API_ENTRY(dynamic_fs_close);
DYNAMIC_API_ENTRY(dynamic_fs_get_size);
DYNAMIC_API_ENTRY(dynamic_fs_delete);
DYNAMIC_API_ENTRY(dynamic_fs_check);
DYNAMIC_API_ENTRY(dynamic_fs_rename);
DYNAMIC_API_ENTRY(dynamic_fs_create_dir);
DYNAMIC_API_ENTRY(dynamic_fs_delete_dir);
DYNAMIC_API_ENTRY(dynamic_fs_check_dir);
DYNAMIC_API_ENTRY(dynamic_fs_get_free_space);
DYNAMIC_API_ENTRY(dynamic_fs_get_total_space);
DYNAMIC_API_ENTRY(dynamic_fs_format);

// adc
DYNAMIC_API_ENTRY(dynamic_adc_get_channel_voltage);
DYNAMIC_API_ENTRY(dynamic_adc_get_data);
DYNAMIC_API_ENTRY(dynamic_adc_get_chr_status);
DYNAMIC_API_ENTRY(dynamic_adc_is_charge_full);
DYNAMIC_API_ENTRY(dynamic_adc_vbat_percentage);


// telephony
DYNAMIC_API_ENTRY(dynamic_telephony_call_status);
DYNAMIC_API_ENTRY(dynamic_telephony_call_cb_reg);
DYNAMIC_API_ENTRY(dynamic_telephony_sms_recv_reg);
DYNAMIC_API_ENTRY(dynamic_telephony_dial);
DYNAMIC_API_ENTRY(dynamic_telephony_reject);
DYNAMIC_API_ENTRY(dynamic_telephony_accept);
DYNAMIC_API_ENTRY(dynamic_telephony_sms_send);
DYNAMIC_API_ENTRY(dynamic_telephony_sms_del_all);


// I2C
DYNAMIC_API_ENTRY(dynamic_i2c_configure);
DYNAMIC_API_ENTRY(dynamic_i2c_receive);
DYNAMIC_API_ENTRY(dynamic_i2c_send);


//LBS
DYNAMIC_API_ENTRY(dynamic_lbs_get_info);
DYNAMIC_API_ENTRY(dynamic_lbs_req);
    

// GPIO,EINT
DYNAMIC_API_ENTRY(GPIO_ModeSetup);
DYNAMIC_API_ENTRY(GPIO_InitIO);
DYNAMIC_API_ENTRY(GPIO_ReadIO);
DYNAMIC_API_ENTRY(GPIO_WriteIO);
DYNAMIC_API_ENTRY(GPIO_PullenSetup);
DYNAMIC_API_ENTRY(GPIO_PullSelSetup);
DYNAMIC_API_ENTRY(EINT_Set_Sensitivity);
DYNAMIC_API_ENTRY(EINT_Set_Polarity);
DYNAMIC_API_ENTRY(EINT_SW_Debounce_Modify);
DYNAMIC_API_ENTRY(EINT_Registration);
DYNAMIC_API_ENTRY(EINT_UnMask);
DYNAMIC_API_ENTRY(EINT_Mask);
DYNAMIC_API_ENTRY(EINT_Set_HW_Debounce);

// MSG AT
DYNAMIC_API_ENTRY(dynamic_msgbased_at_send);
DYNAMIC_API_ENTRY(dynamic_msgbased_at_reg_cb);

// audio
DYNAMIC_API_ENTRY(dynamic_audio_set_audio_mode);
DYNAMIC_API_ENTRY(dynamic_audio_set_volume);
DYNAMIC_API_ENTRY(dynamic_record_cb_reg);
DYNAMIC_API_ENTRY(dynamic_record_recv_data);
DYNAMIC_API_ENTRY(dynamic_record_start);
DYNAMIC_API_ENTRY(dynamic_record_stop);

// GPS
DYNAMIC_API_ENTRY(dynamic_gps_recv_cb_reg);
DYNAMIC_API_ENTRY(dynamic_gps_pow_crl);
DYNAMIC_API_ENTRY(dynamic_gps_get_state);
DYNAMIC_API_ENTRY(dynamic_gps_uart_write);
DYNAMIC_API_ENTRY(dynamic_gps_time_aid);
DYNAMIC_API_ENTRY(dynamic_gps_refloc_aid);
DYNAMIC_API_ENTRY(dynamic_gps_epo_req);
DYNAMIC_API_ENTRY(dynamic_epo_is_valid);
DYNAMIC_API_ENTRY(dynamic_gps_epo_aiding);
DYNAMIC_API_ENTRY(dynamic_gps_get_gps_info);
DYNAMIC_API_ENTRY(dynamic_gps_get_info_src);

static void dynamic_load_general_apis(void)
{
	// log
	DYNAMIC_LOAD_API(dynamic_print);
	DYNAMIC_LOAD_API(dynamic_log_get_lvl);
	DYNAMIC_LOAD_API(dynamic_log_set_lvl);
	DYNAMIC_LOAD_API(dynamic_log_hex);
	

    // sys
	DYNAMIC_LOAD_API(dynamic_sys_reset);
    DYNAMIC_LOAD_API(dynamic_sys_shutdown);
    DYNAMIC_LOAD_API(dynamic_sys_get_power_on_reason);
    DYNAMIC_LOAD_API(dynamic_app_crc32);
    DYNAMIC_LOAD_API(dynamic_checksum);
    DYNAMIC_LOAD_API(dynamic_app_cntx_get);
    DYNAMIC_LOAD_API(dynamic_sleep_enable);
    DYNAMIC_LOAD_API(dynamic_sleep_disable);
    DYNAMIC_LOAD_API(dynamic_sim_get_valid);
    DYNAMIC_LOAD_API(dynamic_find_short_pos);
    DYNAMIC_LOAD_API(dynamic_ucs2_wcsstr);
    DYNAMIC_LOAD_API(dynamic_ascii2bcd);
    DYNAMIC_LOAD_API(dynamic_hex2ascii);    
    DYNAMIC_LOAD_API(dynamic_sys_sleep_task);
    DYNAMIC_LOAD_API(mmi_ucs2strlen);
    DYNAMIC_LOAD_API(mmi_ucs2cpy);
    DYNAMIC_LOAD_API(mmi_ucs2cmp);
    DYNAMIC_LOAD_API(mmi_ucs2ncmp);
    DYNAMIC_LOAD_API(mmi_ucs2ncpy);
    DYNAMIC_LOAD_API(mmi_asc_to_ucs2);

    // timer
    DYNAMIC_LOAD_API(dynamic_timer_start);
    DYNAMIC_LOAD_API(dynamic_timer_stop);
    DYNAMIC_LOAD_API(dynamic_timer_is_runing);
    DYNAMIC_LOAD_API(dynamic_time_get_build_date_time);
    DYNAMIC_LOAD_API(dynamic_time_get_systime);
    DYNAMIC_LOAD_API(dynamic_time_set_systime);
    DYNAMIC_LOAD_API(dynamic_kal_create_timer);
    DYNAMIC_LOAD_API(dynamic_kal_set_timer);
    DYNAMIC_LOAD_API(dynamic_kal_cancel_timer);
    DYNAMIC_LOAD_API(dynamic_kal_get_systicks);
    DYNAMIC_LOAD_API(dynamic_kal_milli_secs_to_ticks);
    DYNAMIC_LOAD_API(dynamic_kal_ticks_to_milli_secs);
    DYNAMIC_LOAD_API(dynamic_time_utc_to_rtc);
    DYNAMIC_LOAD_API(dynamic_time_rtc_to_utc_with_default_tz);
    DYNAMIC_LOAD_API(dynamic_time_utc_to_rtc_with_default_tz);    
    DYNAMIC_LOAD_API(dynamic_timer_time2sec);    
    
    // mem
    DYNAMIC_LOAD_API(dynamic_med_alloc_ext_mem);
    DYNAMIC_LOAD_API(dynamic_med_free_ext_mem);
    DYNAMIC_LOAD_API(dynamic_malloc);
    DYNAMIC_LOAD_API(dynamic_free);
    DYNAMIC_LOAD_API(dynamic_mem_left_size);
    DYNAMIC_LOAD_API(dynamic_queue_create);
    DYNAMIC_LOAD_API(dynamic_queue_destroy);
    DYNAMIC_LOAD_API(dynamic_queue_prepend);
    DYNAMIC_LOAD_API(dynamic_queue_is_full);
    DYNAMIC_LOAD_API(dynamic_queue_push);
    DYNAMIC_LOAD_API(dynamic_queue_pop);
    DYNAMIC_LOAD_API(dynamic_queue_peek);


    // uart
    DYNAMIC_LOAD_API(dynamic_uart_debug_port_callback_reg);
    DYNAMIC_LOAD_API(dynamic_uart_req_read);
    DYNAMIC_LOAD_API(dynamic_uart_put_bytes);
    DYNAMIC_LOAD_API(dynamic_uart_set_baudrate);
    DYNAMIC_LOAD_API(dynamic_uart_close);
    DYNAMIC_LOAD_API(dynamic_uart_register);
    DYNAMIC_LOAD_API(dynamic_uart_open);
    DYNAMIC_LOAD_API(dynamic_uart_configure);
    DYNAMIC_LOAD_API(dynamic_uart_turn_on_power);



	// soc
	DYNAMIC_LOAD_API(dynamic_gprs_get_apn_dns);
	DYNAMIC_LOAD_API(dynamic_gprs_set_apn_dns);
	DYNAMIC_LOAD_API(dynamic_soc_send);
	DYNAMIC_LOAD_API(dynamic_soc_close);
	DYNAMIC_LOAD_API(dynamic_soc_connect);
	DYNAMIC_LOAD_API(dynamic_soc_find_connection);

    // http
    DYNAMIC_LOAD_API(dynamic_http_get_file);
    DYNAMIC_LOAD_API(dynamic_http_get_info);
	
	// fs
	DYNAMIC_LOAD_API(dynamic_fs_open);
	DYNAMIC_LOAD_API(dynamic_fs_read);
	DYNAMIC_LOAD_API(dynamic_fs_write);
	DYNAMIC_LOAD_API(dynamic_fs_seek);
	DYNAMIC_LOAD_API(dynamic_fs_get_file_position);
	DYNAMIC_LOAD_API(dynamic_fs_truncate);
	DYNAMIC_LOAD_API(dynamic_fs_flush);
	DYNAMIC_LOAD_API(dynamic_fs_close);
	DYNAMIC_LOAD_API(dynamic_fs_get_size);
	DYNAMIC_LOAD_API(dynamic_fs_delete);
	DYNAMIC_LOAD_API(dynamic_fs_check);
	DYNAMIC_LOAD_API(dynamic_fs_rename);
	DYNAMIC_LOAD_API(dynamic_fs_create_dir);
	DYNAMIC_LOAD_API(dynamic_fs_delete_dir);
	DYNAMIC_LOAD_API(dynamic_fs_check_dir);
	DYNAMIC_LOAD_API(dynamic_fs_get_free_space);
	DYNAMIC_LOAD_API(dynamic_fs_get_total_space);
	DYNAMIC_LOAD_API(dynamic_fs_format);

    // adc
    DYNAMIC_LOAD_API(dynamic_adc_get_channel_voltage);
    DYNAMIC_LOAD_API(dynamic_adc_get_data);
    DYNAMIC_LOAD_API(dynamic_adc_get_chr_status);
    DYNAMIC_LOAD_API(dynamic_adc_is_charge_full);
    DYNAMIC_LOAD_API(dynamic_adc_vbat_percentage);

    // telephony
    DYNAMIC_LOAD_API(dynamic_telephony_call_status);
    DYNAMIC_LOAD_API(dynamic_telephony_call_cb_reg);
    DYNAMIC_LOAD_API(dynamic_telephony_sms_recv_reg);
    DYNAMIC_LOAD_API(dynamic_telephony_dial);
    DYNAMIC_LOAD_API(dynamic_telephony_reject);
    DYNAMIC_LOAD_API(dynamic_telephony_accept);
    DYNAMIC_LOAD_API(dynamic_telephony_sms_send);
    DYNAMIC_LOAD_API(dynamic_telephony_sms_del_all);


    // I2C
    DYNAMIC_LOAD_API(dynamic_i2c_configure);
    DYNAMIC_LOAD_API(dynamic_i2c_receive);
    DYNAMIC_LOAD_API(dynamic_i2c_send);


    //LBS
    DYNAMIC_LOAD_API(dynamic_lbs_get_info);
    DYNAMIC_LOAD_API(dynamic_lbs_req);

    // GPIO,EINT
    DYNAMIC_LOAD_API(GPIO_ModeSetup);
    DYNAMIC_LOAD_API(GPIO_InitIO);
    DYNAMIC_LOAD_API(GPIO_ReadIO);
    DYNAMIC_LOAD_API(GPIO_WriteIO);
    DYNAMIC_LOAD_API(GPIO_PullenSetup);
    DYNAMIC_LOAD_API(GPIO_PullSelSetup);
    DYNAMIC_LOAD_API(EINT_Set_Sensitivity);
    DYNAMIC_LOAD_API(EINT_Set_Polarity);
    DYNAMIC_LOAD_API(EINT_SW_Debounce_Modify);
    DYNAMIC_LOAD_API(EINT_Registration);
    DYNAMIC_LOAD_API(EINT_UnMask);
    DYNAMIC_LOAD_API(EINT_Mask);
    DYNAMIC_LOAD_API(EINT_Set_HW_Debounce);

    // MSG AT
    DYNAMIC_LOAD_API(dynamic_msgbased_at_send);
    DYNAMIC_LOAD_API(dynamic_msgbased_at_reg_cb);

    
    // audio
    DYNAMIC_LOAD_API(dynamic_audio_set_audio_mode);
    DYNAMIC_LOAD_API(dynamic_audio_set_volume);
    DYNAMIC_LOAD_API(dynamic_record_cb_reg);
    DYNAMIC_LOAD_API(dynamic_record_recv_data);
    DYNAMIC_LOAD_API(dynamic_record_start);
    DYNAMIC_LOAD_API(dynamic_record_stop);

    // GPS
    DYNAMIC_LOAD_API(dynamic_gps_recv_cb_reg);
    DYNAMIC_LOAD_API(dynamic_gps_pow_crl);
    DYNAMIC_LOAD_API(dynamic_gps_get_state);
    DYNAMIC_LOAD_API(dynamic_gps_uart_write);
    DYNAMIC_LOAD_API(dynamic_gps_time_aid);
    DYNAMIC_LOAD_API(dynamic_gps_refloc_aid);
    DYNAMIC_LOAD_API(dynamic_gps_epo_req);    
    DYNAMIC_LOAD_API(dynamic_epo_is_valid);    
    DYNAMIC_LOAD_API(dynamic_gps_epo_aiding);
    DYNAMIC_LOAD_API(dynamic_gps_get_gps_info);
    DYNAMIC_LOAD_API(dynamic_gps_get_info_src);
    
}

int dynamic_load_apis(void *load_api_ptr)
{
	if (load_api_ptr) {
        dynamic_load_api = (dynamic_load_api_ptr)load_api_ptr;
        if (dynamic_load_api) {
			dynamic_load_general_apis();

            return 0;
        }
	}
	
	return -1;
}


