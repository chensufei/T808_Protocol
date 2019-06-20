#ifndef __DYNAMIC_STRUCT_H__
#define __DYNAMIC_STRUCT_H__


// 定义各种级别的LOG打印接口
#define dynamic_log(...) dynamic_print(DYNAMIC_LOG_NONE, __VA_ARGS__)
#define dynamic_debug(...) dynamic_print(DYNAMIC_LOG_DEBUG, __VA_ARGS__)
#define dynamic_info(...) dynamic_print(DYNAMIC_LOG_INFO, __VA_ARGS__)
#define dynamic_warn(...) dynamic_print(DYNAMIC_LOG_WARNING, __VA_ARGS__)
#define dynamic_error(...) dynamic_print(DYNAMIC_LOG_ERROR, __VA_ARGS__)


typedef unsigned int 		uint32;
typedef unsigned short 		uint16;
typedef unsigned char 		uint8;
typedef signed int 			int32;
typedef signed short 		int16;
typedef signed char 		int8;
typedef uint8 byte;
typedef signed int DCL_HANDLE;

typedef char CHAR;

typedef char                S8;      /* signed 8 bit integer */
typedef char                *PS8;    /* signed 8 bit integer pointer */
typedef unsigned char       U8;      /* unsigned 8 bit integer */
typedef unsigned char       *PU8;    /* unsigned 8 bit integer pointer */

typedef signed short int    S16;     /* signed 16 bit integer */
typedef signed short int    *PS16;   /* signed 16 bit integer pointer */
typedef unsigned short int  U16;     /* unsigned 16 bit integer */
typedef unsigned short int  *PU16;   /* unsigned 16 bit integer pointer */

typedef signed int          S32;     /* signed 32 bit integer */
typedef signed int          *PS32;   /* signed 32 bit integer pointer */
typedef unsigned int        U32;     /* unsigned 32 bit integer */
typedef unsigned int        *PU32;   /* unsigned 32 bit integer pointer */

typedef unsigned long long	 U64;	  /* unsigned 64 bit integer */
typedef long long			 S64;	  /* signed 64 bit integer */



typedef unsigned char       u8;      /* unsigned 8 bit integer */
typedef unsigned short int  u16;     /* unsigned 16 bit integer */
typedef unsigned int        u32;     /* unsigned 32 bit integer */
typedef unsigned long long	u64;	  /* unsigned 64 bit integer */

/* portable character for multichar character set */
typedef char                    kal_char;
/* portable wide character for unicode character set */
typedef unsigned short          kal_wchar;
/* portable 8-bit unsigned integer */
typedef unsigned char           kal_uint8;
/* portable 8-bit signed integer */
typedef signed char             kal_int8;
/* portable 16-bit unsigned integer */
typedef unsigned short int      kal_uint16;
/* portable 16-bit signed integer */
typedef signed short int        kal_int16;
/* portable 32-bit unsigned integer */
typedef unsigned int            kal_uint32;
/* portable 32-bit signed integer */
typedef signed int              kal_int32;
/* portable 64-bit unsigned integer */
typedef unsigned long long   kal_uint64;
/* portable 64-bit signed integer */
typedef signed long long     kal_int64;

#ifndef bool
typedef uint8				bool;
#endif

#ifndef true
#define true		1
#endif

#ifndef false
#define false		0
#endif

#ifndef MMI_BOOL
typedef bool MMI_BOOL;
#endif
#ifndef MMI_TRUE
#define MMI_TRUE	true
#endif
#ifndef MMI_FALSE
#define	MMI_FALSE	false
#endif

#ifndef Boolean
typedef enum { FALSE = 0, TRUE = 1 } Boolean;
#endif

#ifndef NULL
#define NULL 		0
#endif

/* boolean representation */
typedef enum 
{
    /* FALSE value */
    KAL_FALSE,
    /* TRUE value */
    KAL_TRUE
} kal_bool;

#define 	OUTPUT 	1
#define 	INPUT  	0

typedef enum
{
	DYNAMIC_LOG_NONE = 0,
	DYNAMIC_LOG_ERROR,
	DYNAMIC_LOG_WARNING,
	DYNAMIC_LOG_INFO,
	DYNAMIC_LOG_DEBUG,
	DYNAMIC_LOG_END // dummy level for validation
} DYNAMIC_LOG_LVL_E;

typedef enum {
	DYNAMIC_SOC_NONE,
	DYNAMIC_SOC_INITIALIZED,
	DYNAMIC_SOC_CREATED,
	DYNAMIC_SOC_CONNECTING,
	DYNAMIC_SOC_CONNECTED,
	DYNAMIC_SOC_CLOSING
} DYNAMIC_SOC_CON_STATUS_E;

typedef enum {
	DYNAMIC_SOC_HOST_OK,
	DYNAMIC_SOC_HOST_FAILED,
	DYNAMIC_SOC_CONNECT_OK,
	DYNAMIC_SOC_CONNECT_FAILED,
	DYNAMIC_SOC_CLOSE,
	DYNAMIC_SOC_RECV,
	DYNAMIC_SOC_WRITE
} DYNAMIC_SOC_CB_E;


typedef void(*dynamic_soc_callback)(DYNAMIC_SOC_CB_E,kal_int8,void *, void *, int);

#define MAX_IP_LEN 20
typedef struct _soc_connection_struct {
    kal_uint8                   sock_type; // 0:TCP 1:UDP
    kal_uint16					wd_cnt; // 异常检测次数
    kal_int8					socket; // socket id
    DYNAMIC_SOC_CON_STATUS_E 	status; // 当前链接状态
    dynamic_soc_callback		soc_cb; // 回调函数
    kal_char					ip[MAX_IP_LEN]; // 当前连接的IP
    kal_uint16					port; // 端口
    void						*user_data; // 传递的参数
} DYNAMIC_SOC_CONNECTION_T;

typedef struct
{
    kal_uint16 nYear;
    kal_uint8 nMonth;
    kal_uint8 nDay;
    kal_uint8 nHour;
    kal_uint8 nMin;
    kal_uint8 nSec;
    kal_uint8 DayIndex; /* 0=Sunday */
} applib_time_struct;

typedef struct
{
    double lat; // 纬度 度格式   //22.545624
    double lng; // 经度 度格式   //113.921432
    double altitude; // 海拔高度
    applib_time_struct mtime; // UTC时间
    int sat_num; // 卫星数
    float hdop; // 水平精度
    char lat_str[20]; // 字符串度分格式
    char lng_str[20]; // 字符串度分格式
    float milespeed; // 速度 海里
    kal_uint32 kmspeed; // 速度 公里
    float degree; // 角度
    char d_lat; // 北半球，南半球  //dLat:N,dLon:E
    char d_lng; // 东经，西经
    char valid; // 是否定位成功
}dynamic_gps_info_t;

#define DE_GPS_NMEA_MAX_SVVIEW 20
#define MAX_GSA_NUM 4

typedef struct {            
    kal_uint8      tag_hour;
    kal_uint8      tag_minute;
    kal_uint8      tag_second;
    kal_uint8      tag_second2;
    kal_uint16     tag_millisecond;
    kal_uint16     tag_millisecond2;
} uct_gps_nmea_utc_time_struct;

typedef struct {            
    kal_uint16 tag_year;
    kal_uint8 tag_month;
    kal_uint8 tag_day;
} uct_gps_nmea_utc_date_struct;

/*GPGGA -- Global Positioning System Fix Data*/
typedef struct
{            
    double    tag_latitude;               /*Latitude South<0  North>0*/
    double    tag_longitude;              /*Longitude West<0 east>0*/
    float     tag_h_precision;            /*Horizontal Dilution of precision*/
    double    tag_altitude;               /*Antenna Altitude above/below mean-sea-level (geoid)*/
    char      tag_unit_of_altitude;       /*Units of antenna altitude, meters*/
    float     tag_geoidal ;               /*Geoidal separation, the difference between the WGS-84 earth*/
    char      tag_unit_of_geoidal;        /*Units of geoidal separation, meters*/
    float     tag_gps_age;                /*Age of differential GPS data, time in seconds since last SC104*/
    kal_uint16    tag_station_id;       /*Differential reference station ID, 0000-1023*/
    kal_uint8     tag_sat_in_fix;      /*Number of satellites in use*/
    char      tag_north_south;      /*north or south*/
    uct_gps_nmea_utc_time_struct    tag_utc_time;       /*Time (UTC)*/
    char      tag_east_west;        /*east or west*/
    char      tag_quality;          /*GPS Quality Indicator*/
    char      tag_east_westw2;        /*east or west*/
    char      tag_quality2;          /*GPS Quality Indicator*/
} uct_gps_nmea_gga_struct;


/*GPGSA -- GNSS DOP and Active Satellites*/
typedef struct {           
    float   tag_pdop;       /*PDOP in meters*/
    float   tag_hdop;       /*HDOP in meters*/
    float   tag_vdop;       /*VDOP in meters*/
    kal_uint16    tag_sate_id[12]; /*ID of satellites*/
    char      tag_op_mode;      /*Selection mode: A=auto M=manual*/
    char      tag_fix_mode;     /*Mode*/
    char      buf[2];     /*Mode*/
} uct_gps_nmea_gsa_struct;


/*GPGSV -- GNSS Satellites in View*/
typedef struct {            
    signed short     tag_msg_sum;              /*total number of messages*/
    signed short     tag_msg_index;            /*message number*/
    signed short     tag_sates_in_view;        /*satellites in view*/
    signed short     tag_max_snr;              /*Max snr*/
    signed short     tag_min_snr;              /*Min snr*/
    struct
    {
        kal_uint16  tag_sate_id;              /*satellite id*/
        kal_uint16  tag_elevation;            /*elevation in degrees*/
        kal_uint16  tag_azimuth;              /*azimuth in degrees to true*/
        kal_uint16  tag_snr;                  /*SNR in dB*/
    }tag_rsv[DE_GPS_NMEA_MAX_SVVIEW];
} uct_gps_nmea_gsv_struct;


/*GPRMC -- Recommended Minimum Specific GNSS Data*/
typedef struct
{            
    double  tag_latitude; /*latitude*/
    double  tag_longitude; /*longitude*/
    char    tag_lat_str[20];
    char    tag_lng_str[20];
    float   tag_ground_speed; /*Speed over ground, knots*/
    float   tag_trace_degree; /*Track mode degrees,north is 0*/
    float   tag_magnetic;
    uct_gps_nmea_utc_time_struct    tag_utc_time; /*UTC time*/
    uct_gps_nmea_utc_date_struct    tag_utc_date; /*UTC date*/
    char    tag_status; /*Status, V = Navigation receiver warning*/
    char    tag_north_south; /*N or S*/
    char    tag_east_west; /*E or W*/
    char    tag_magnetic_e_w; /*Magnetic E or W*/
    char    tag_cmode; /*Mode*/
    char    tag_nav_status; /*navigational status*/   
    char    buf[2]; /*Mode*/
    char    tag_time[40];
}uct_gps_nmea_rmc_struct;

/*GPGLL -- Geographic Position - Latitude/Longitude*/
typedef struct
{            
    double tag_latitude;               /*latitude*/ 
    double tag_longitude;              /*longitude*/
    uct_gps_nmea_utc_time_struct tag_utc_time;     /*UTC time*/
    signed char tag_north_south;      /*N or S*/
    signed char tag_east_west;        /*E or W*/
    signed char tag_status;           /*Status A - Data Valid, V - Data Invalid*/
    signed char tag_mode;             /*mode*/
}uct_gps_nmea_gll_struct;


/*GPVTG -- VTG Data*/
typedef struct
{
    float tag_true_heading;   /*Track Degrees*/
    float tag_mag_heading;    /*Magnetic Track Degrees*/
    float tag_hspeed_knot;    /*Speed Knots*/
    float tag_hspeed_km;      /*Speed Kilometers Per Hour*/
    signed char tag_mode;     /*Mode*/
    signed char buf[3];     /*Mode*/
}uct_gps_nmea_vtg_struct;

typedef struct 
{
    uct_gps_nmea_gga_struct tag_gga;
    uct_gps_nmea_gsa_struct tag_gsa[MAX_GSA_NUM];
    uct_gps_nmea_gsv_struct tag_gsv;
    uct_gps_nmea_rmc_struct tag_rmc;
    uct_gps_nmea_gll_struct tag_gll;
    uct_gps_nmea_vtg_struct tag_vtg;
}nmea_data_t;

#define FS_READ_WRITE       0x00000000L
#define FS_READ_ONLY        0x00000100L
#define FS_CREATE           0x00010000L
#define FS_CREATE_ALWAYS    0x00020000L
#define FS_OPEN_SHARED      0x00000200L
#define FS_MOVE_COPY        0x00000001
#define FS_MOVE_KILL        0x00000002
#define FS_MOVE_OVERWRITE   0x00010000
#define FS_FILE_TYPE        0x00000004     
#define FS_DIR_TYPE         0x00000008     
#define FS_RECURSIVE_TYPE   0x00000010

/* FS_Seek Parameter */
typedef enum
{
    FS_FILE_BEGIN,
    FS_FILE_CURRENT,
    FS_FILE_END
} FS_SEEK_POS_ENUM;

#define DYNAMIC_IMEI_MAX_LEN 		15
#define DYNAMIC_ICCID_MAX_LEN 		20
#define DYNAMIC_MAX_PLMN_LEN        6
#define DYNAMIC_IMSI_MAX_LEN 		15

typedef struct _app_cntx_struct
{
    kal_uint8   app_id; // APP ID
    kal_uint8   gsm_state; // 1搜索到运营商  其它:未搜索到运营商
    kal_uint8   gprs_state; // 处于可联网状态  其它:无法联网
    kal_uint8   cbm_activated; // 1网络已激活  0:网络已去活
    kal_uint32  nwk_acct_id; // ACCOUNT ID
    kal_uint8   csq; // CSQ值:0~31  99表示未知
    kal_uint16  lac; // 当前小区
    kal_uint16  ci; // 当前基站号
    kal_int8    iccid[DYNAMIC_ICCID_MAX_LEN + 1]; // ICCID
    kal_int8    imsi[DYNAMIC_IMSI_MAX_LEN + 1]; // IMSI
    kal_int8    imei[DYNAMIC_IMEI_MAX_LEN + 1]; // IMEI
    kal_int8    plmn[DYNAMIC_MAX_PLMN_LEN + 1]; // 联网识别到的PLMN
    kal_int8    sim_plmn[DYNAMIC_MAX_PLMN_LEN + 1]; // SIM卡的PLMN
    kal_uint16  mcc; // 移动国家号
    kal_uint16  mnc; // 移动网络号码
} APP_CNTX_T;


typedef struct {
    kal_uint16      mcc; // 移动国家号
    kal_uint16      mnc; // 移动网络号码
    kal_uint16      lac; // 小区编号
    kal_uint16      ci;  // 基站编号
    kal_int16       rssi; // 接收信号强度
    kal_uint16      arfcn;
    kal_uint8       bsic;
    kal_uint8       rxlev; 

    kal_uint8       bcch;
    kal_int8        c1;
    kal_int8        c2;
    
} lbs_cell_struct;

typedef struct {
    lbs_cell_struct  mainCell; // 主基站信息
    lbs_cell_struct  nearCell[6]; // 邻近6个基站信息
    kal_uint8 timing_advance; 
    kal_uint8 valid_cel_num;
} lbs_struct;

typedef enum {
   DCL_VBAT_ADC_CHANNEL = 0,
   DCL_VISENSE_ADC_CHANNEL,
   DCL_VBATTMP_ADC_CHANNEL,
   DCL_BATTYPE_ADC_CHANNEL,
   DCL_VCHARGER_ADC_CHANNEL,
   DCL_PCBTMP_ADC_CHANNEL,
   DCL_AUX_ADC_CHANNEL,
   DCL_CHR_USB_ADC_CHANNEL,
   DCL_OTG_VBUS_ADC_CHANNEL,
   DCL_RFTMP_ADC_CHANNEL,
   DCL_MAX_ADC_CHANNEL
} DCL_ADC_CHANNEL_TYPE_ENUM;

typedef struct {
   kal_int32 VBAT; // 电池电压
   kal_int32 ICHARGE; // 是否充电中
   kal_int32 BATTMP; // 电池温度
   kal_int32 VCHARGER; // 充电电压
   kal_int32 ISense_Offset;
   //kal_uint8 BATTYPE;
} BATPHYStruct;

typedef enum {
   CHARGER_PLUG_IN = 0,
   CHARGER_PLUG_OUT = 1,
   CHARGER_UNINIT = 0xff
}CHARGER_IN_OUT_STATUS;

typedef enum _call_status_
{
    CALL_RELEASE, // 挂断
    CALL_COMING, // 来电
    CALL_ACCEPT, // 来电接听
    CALL_REQ, // 呼叫请求
    CALL_ALERT, // 呼叫开始响铃
    CALL_CONNECT, // 呼叫被接听
    CALL_MAX
}CALL_STATUS_E;

#define MAX_HTTP_HOST_LEN 512
#define MAX_HTTP_URL_LEN 1024
#define MAX_HTTP_FEAME_SIZE 1024

typedef enum _enum_http_result
{
		enum_http_result_ok = 0, // 正常
		enum_http_result_faile, // 失败
		enum_http_result_exited, // 已有任务正在下载
		enum_http_result_download_ok, // 下载ok
		enum_http_result_url_err, // url异常
		enum_http_result_mem_fail, // 内存不够
		enum_http_result_send_fail, // 发送失败
		enum_http_result_fileCreate_fail // 文件创建失败
}enum_http_result;

typedef struct _dynamic_http_
{
    DYNAMIC_SOC_CONNECTION_T *soc_con;
    kal_int8 url[MAX_HTTP_URL_LEN]; // 保存下载的url
    kal_int8 host[MAX_HTTP_HOST_LEN]; // 保存url中的域名
    kal_uint16 port; // url中的端口
    kal_uint32 file_size; // 文件总大小
    kal_uint32 cur_len; // 当前下载的长度
    kal_uint8 *buffer; // 把下载的数据保存到此地址中
    kal_uint32 buffer_size; // tag_buffer 的大小
    kal_bool result; // 返回的结果
    void *user; // 用户信息
    void *cb; // 回掉函数
}DYNAMIC_HTTP_T;

typedef enum {
      uart_port1=0,
      uart_port2,
      uart_port3,
      uart_max_port,      
      uart_port_null = 99    /* a uart port for those who uses physical port */
} UART_PORT;

typedef struct
{
    kal_uint8 unused;
} *kal_timerid;

typedef struct
{
    kal_bool result;   /* Result of the operation: MMI_TRUE: success; MMI_FALSE: failed */
    int action;        /* Action to indicate the operation type(read/save/Delete...), refer to srv_sms_action_enum */
    int cause;         /* Cause failed, only valid result be MMI_FALSE, refer to srv_sms_cause_enum */
    void *action_data; /* Callback data, refer to  srv_sms_xxx_cb_struct, ex. read callback data struct: srv_sms_read_msg_cb_struct*/
    void *user_data;   /* User data */
}DYNAMIC_SMS_SEND_CB_T;

typedef void(*FuncPtr)(void);
typedef int (*msgbased_at_cb)(char* data, kal_uint32 len, void* user);
typedef void (*CALL_CB) (CALL_STATUS_E call_stat,kal_uint8 *num); // 电话状态回调，只有来电时号码参数才有效
typedef void (*SMS_CB) (char *num,char* data,kal_uint32 len); 
typedef void (*SMS_SEND_CB) (DYNAMIC_SMS_SEND_CB_T* send_result);
typedef void (*kal_timer_func_ptr)(void *param_ptr);
typedef void (*SOCKET_CB)(DYNAMIC_SOC_CB_E type,kal_int8 socket,void *user_data, void *data, int len);
typedef void (*DYNAMIC_GPS_CB) (kal_uint8* data,kal_uint16 len); 

typedef enum _gps_adide_type
{
    ADIED_NONE, // 关闭辅助定位功能
    ADIED_EPO, // EPO辅助定位
    ADIED_AGPS, // AGPS辅助定位
    ADIED_EPO_AGPS, // EPO和AGPS辅助定位
    ADIED_MAX
}GPS_ADIED_TYPE_E;


/* Enum of all supported charsets*/
typedef enum
{
    CHSET_BASE,
    MMI_CHSET_BASE = CHSET_BASE,
    CHSET_ASCII,
    MMI_CHSET_ASCII = CHSET_ASCII, // ASCII
    CHSET_ARABIC_ISO, 
    MMI_CHSET_ARABIC_ISO = CHSET_ARABIC_ISO, // ISO 8859-6 (Arabic)
    CHSET_ARABIC_WIN,
    MMI_CHSET_ARABIC_WIN = CHSET_ARABIC_WIN, // Windows-1256 (Arabic)
    CHSET_PERSIAN_CP1097,
    MMI_CHSET_PERSIAN_CP1097 = CHSET_PERSIAN_CP1097, // CP1097  (Persian) 
    CHSET_PERSIAN_CP1098,
    MMI_CHSET_PERSIAN_CP1098 = CHSET_PERSIAN_CP1098, // CP1098  (Persian)
    CHSET_BALTIC_ISO,
    MMI_CHSET_BALTIC_ISO = CHSET_BALTIC_ISO, // ISO-8859-4 (Baltic language group)
    CHSET_BALTIC_WIN,
    MMI_CHSET_BALTIC_WIN = CHSET_BALTIC_WIN, // Windows-1257 (Baltic language group)
    CHSET_CEURO_ISO,
    MMI_CHSET_CEURO_ISO = CHSET_CEURO_ISO, // ISO-8859-2 (Central european language group)
    CHSET_CEURO_WIN,
    MMI_CHSET_CEURO_WIN = CHSET_CEURO_WIN, // Windows-1250 (Central european language group)
    CHSET_CYRILLIC_ISO,
    MMI_CHSET_CYRILLIC_ISO = CHSET_CYRILLIC_ISO, // ISO-8859-5 (Cyrillic script based languages)
    CHSET_CYRILLIC_WIN,
    MMI_CHSET_CYRILLIC_WIN = CHSET_CYRILLIC_WIN, // Windows-1251 (Cyrillic script)
    CHSET_GREEK_ISO,
    MMI_CHSET_GREEK_ISO = CHSET_GREEK_ISO, // ISO-8859-7 (Greek)
    CHSET_GREEK_WIN,
    MMI_CHSET_GREEK_WIN = CHSET_GREEK_WIN, // Windows-1253 (Greek)
    CHSET_HEBREW_ISO,
    MMI_CHSET_HEBREW_ISO = CHSET_HEBREW_ISO, // ISO-8859-8 (Hebrew)
    CHSET_HEBREW_WIN,
    MMI_CHSET_HEBREW_WIN = CHSET_HEBREW_WIN, // Windows-1255 (Hebrew)
    CHSET_LATIN_ISO,
    MMI_CHSET_LATIN_ISO = CHSET_LATIN_ISO, // ISO-8859-4 (North european language group)
    CHSET_NORDIC_ISO,
    MMI_CHSET_NORDIC_ISO = CHSET_NORDIC_ISO, // ISO-8859-10 (Nordic language group)
    CHSET_SEURO_ISO,
    MMI_CHSET_SEURO_ISO = CHSET_SEURO_ISO,  // ISO-8859-3 (South european language group)
    CHSET_TURKISH_ISO,
    MMI_CHSET_TURKISH_ISO = CHSET_TURKISH_ISO, // ISO-8859-9 (Turkish)
    CHSET_TURKISH_WIN,
    MMI_CHSET_TURKISH_WIN = CHSET_TURKISH_WIN, // Windows-1254 (Turkish)
    CHSET_WESTERN_ISO,
    MMI_CHSET_WESTERN_ISO = CHSET_WESTERN_ISO, // ISO-8859-1 (Latin script based languages)
    CHSET_ARMENIAN_ISO,
    MMI_CHSET_ARMENIAN_ISO  = CHSET_ARMENIAN_ISO, // ISO-10585 ARMSCII (Armenian)
	CHSET_THAI_WIN,
    MMI_CHSET_THAI_WIN = CHSET_THAI_WIN, // Windows-874 (Thai)
    CHSET_VIETNAMESE_WIN,
    MMI_CHSET_VIETNAMESE_WIN = CHSET_VIETNAMESE_WIN,  // Windows-1258 (Vietnamese)
    CHSET_KOI8_R,
    MMI_CHSET_KOI8_R = CHSET_KOI8_R,  // KOI8-russian 
    CHSET_TIS_620,
    MMI_CHSET_TIS_620 = CHSET_TIS_620,  // Thai Industrial Standard 620
    CHSET_WESTERN_WIN,
    MMI_CHSET_WESTERN_WIN = CHSET_WESTERN_WIN, // Windows-1252 (Latin script based languages)
    
    /* Add new (8-bit) encodings above this line */
    
    CHSET_BIG5,
    MMI_CHSET_BIG5 = CHSET_BIG5,  // Big5 (Traditional chinese)
    CHSET_GB2312,
    MMI_CHSET_GB2312 = CHSET_GB2312, // GB2312 (Simplified chinese)
    CHSET_HKSCS,
    MMI_CHSET_HKSCS = CHSET_HKSCS,  // HKSCS 2004 (Hong Kong chinese)
    CHSET_SJIS,
    MMI_CHSET_SJIS = CHSET_SJIS,  // SJIS (Japanese)
    CHSET_GB18030,
    MMI_CHSET_GB18030 = CHSET_GB18030, // GB18030 (Simplified chinese-extended)
    CHSET_UTF7,
    MMI_CHSET_UTF7 = CHSET_UTF7,  // UTF-7 
    CHSET_EUCKR,
    MMI_CHSET_EUCKR = CHSET_EUCKR, // Extended Unix Code - korean

    /* Place all CJK encodings above this one */

    CHSET_UTF16LE,
    MMI_CHSET_UTF16LE = CHSET_UTF16LE, // UTF-16LE
    CHSET_UTF16BE,
    MMI_CHSET_UTF16BE = CHSET_UTF16BE, // UTF-16BE
    CHSET_UTF8,
    MMI_CHSET_UTF8 = CHSET_UTF8,  // UTF-8
    CHSET_UCS2,
    MMI_CHSET_UCS2 = CHSET_UCS2,  // UCS2
    CHSET_TOTAL,
    MMI_CHSET_TOTAL = CHSET_TOTAL 
} mmi_chset_enum, chset_enum;


typedef enum {
    VOL_TYPE_CTN  = 0 ,   /* MMI can apply to associate volume; call tone attribute */
    VOL_TYPE_KEY      ,   /* MMI can apply to associate volume; keypad tone attribute */
    VOL_TYPE_MIC      ,   /* microphone attribute */
    VOL_TYPE_GMI      ,   /* MMI can apply to associate volume; melody, imelody, midi attribute */
    VOL_TYPE_SPH      ,   /* MMI can apply to associate volume; speech sound attribute */
    VOL_TYPE_SID	  ,   /* side tone attribute */
    VOL_TYPE_MEDIA    ,   /* MMI can apply to associate volume; As MP3, Wave,... attribute */
    MAX_VOL_TYPE           
}volume_type_enum;

typedef enum {
    AUD_MODE_NORMAL   =0 , /* Normal Mode */
    AUD_MODE_HEADSET     , /* HeadSet (Earphone) Mode */
    AUD_MODE_LOUDSPK     , /* Loudspeaker Mode */
    MAX_AUD_MODE_NUM     
}audio_mode_enum;



#define     UART_BAUD_AUTO          0
#define     UART_BAUD_75            75
#define     UART_BAUD_150           150
#define     UART_BAUD_300           300
#define     UART_BAUD_600           600
#define     UART_BAUD_1200          1200
#define     UART_BAUD_2400          2400
#define     UART_BAUD_4800          4800
#define     UART_BAUD_7200          7200
#define     UART_BAUD_9600          9600
#define     UART_BAUD_14400         14400
#define     UART_BAUD_19200         19200
#define     UART_BAUD_28800         28800
#define     UART_BAUD_33900         33900
#define     UART_BAUD_38400         38400
#define     UART_BAUD_57600         57600
#define     UART_BAUD_115200        115200
#define     UART_BAUD_230400        230400
#define     UART_BAUD_460800        460800
#define     UART_BAUD_921600        921600
#define     UART_BAUD_1500000       1500000

typedef enum {
      DYNAMIC_UART1=0,
      DYNAMIC_UART2,
      DYNAMIC_UART3,
      DYNAMIC_UART_IRDA,
      DYNAMIC_UART_USB,
      DYNAMIC_UART_USB2,
      DYNAMIC_UART_USB3,
      DYNAMIC_UART_MAX,      
      DYNAMIC_UART_NULL = 99    /* a uart port for those who uses physical port */
} UART_PORT_E;

typedef enum{
	PWRKEYPWRON = 0, /* power on reason is powerkey */
	CHRPWRON	= 1,      /* power on reason is charger in */
	RTCPWRON = 2,	    /* power on reason is rtc alarm */
	CHRPWROFF = 3,     /* reserved */
	WDTRESET = 4, 	   /* reserved */
	ABNRESET = 5,       /* power on reason is abnormal reboot */
	USBPWRON = 6,     /* power on reason is usb cable in */
	USBPWRON_WDT = 7, /* power on reason is usb cable in and abnormal reboot*/
	PRECHRPWRON = 8,   /* power on reason is precharger power on*/
	UNKNOWN_PWRON = 0xF9	/* power on reason is unknown*/
}PW_CTRL_POWER_ON_REASON;

typedef enum
{
    DYNAMIC_AT_ERROR = -1,
    DYNAMIC_AT_OK = 0,
    DYNAMIC_AT_RET = 1 // 不需再返回数据
}DYNAMIC_AT_RSP_E;

typedef enum
{
	CUSTOM_WRONG_MODE,
	CUSTOM_SET_OR_EXECUTE_MODE,
	CUSTOM_READ_MODE,
	CUSTOM_TEST_MODE,
	CUSTOM_ACTIVE_MODE
} custom_cmd_mode_enum;

typedef enum {
      len_5=5,
      len_6,
      len_7,
      len_8
} UART_bitsPerCharacter;

typedef enum {
      sb_1=1,
      sb_2,
      sb_1_5
} UART_stopBits;

typedef enum {
      pa_none=0,
      pa_odd,
      pa_even,
      pa_mark,
      pa_space
} UART_parity;

typedef enum {
      fc_none=1,
      fc_hw,
      fc_sw
} UART_flowCtrlMode;

typedef struct
{
    kal_uint32               baud; 
    UART_bitsPerCharacter    dataBits;
    UART_stopBits            stopBits;
    UART_parity              parity;
    UART_flowCtrlMode        flowControl;
    kal_uint8                xonChar;
    kal_uint8                xoffChar;
    kal_bool                 DSRCheck;
} UARTDCBStruct;

typedef enum {
    EVENT_UART_READY_TO_READ = 0,
    EVENT_UART_READY_TO_WRITE,
    EVENT_UART_FE_IND,
    EVENT_UART_RI_IND,
    EVENT_UART_DCD_IND,
    EVENT_UART_DTR_IND
}UART_EVENTTYPE;


typedef void (*dynamic_uart_cb)(kal_uint16 port, UART_EVENTTYPE msg);
typedef struct dynamic_queue_item{
	char *	data;
	struct dynamic_queue_item *next;
} DYNAMIC_QUEUE_ITEM_T;

typedef struct {
	DYNAMIC_QUEUE_ITEM_T*	head;
	DYNAMIC_QUEUE_ITEM_T*	tail;
	kal_uint16		limit;
	kal_uint16		size;
} DYNAMIC_QUEUE_T;
typedef void (*mdi_ext_callback) (kal_int32 result, void* user_data);


typedef enum
{
   MIN_TONE_ID = 0,                         
   TONE_DTMF_0 = MIN_TONE_ID,            	/* 0 */
   TONE_DTMF_1,            					/* 1 */
   TONE_DTMF_2,            					/* 2 */
   TONE_DTMF_3,            					/* 3 */
   TONE_DTMF_4,            					/* 4 */
   TONE_DTMF_5,            					/* 5 */
   TONE_DTMF_6,            					/* 6 */
   TONE_DTMF_7,            					/* 7 */
   TONE_DTMF_8,            					/* 8 */
   TONE_DTMF_9,								/* 9 */
   TONE_KEY_STAR,          					/* 10 */
   TONE_KEY_HASH,          					/* 11 */
   TONE_KEY_NORMAL,							/* 12 */
   TONE_KEY_VOL_UP = TONE_KEY_NORMAL,     /* 12 */   
   TONE_KEY_VOL_DOWN = TONE_KEY_NORMAL,  /* 12 */   
   TONE_KEY_UP = TONE_KEY_NORMAL,         /* 12 */   
   TONE_KEY_DOWN = TONE_KEY_NORMAL,      /* 12 */   
   TONE_KEY_LEFT = TONE_KEY_NORMAL,       /* 12 */   
   TONE_KEY_RIGHT = TONE_KEY_NORMAL,     /* 12 */    
   TONE_KEY_MENU = TONE_KEY_NORMAL,      /* 12 */    
   TONE_KEY_FUNCTION = TONE_KEY_NORMAL,  /* 12 */    
   TONE_KEY_SK_LEFT = TONE_KEY_NORMAL,   /* 12 */    
   TONE_KEY_SK_RIGHT = TONE_KEY_NORMAL,  /* 12 */    
   TONE_KEY_SEND = TONE_KEY_NORMAL,      /* 12 */    
   TONE_KEY_END = TONE_KEY_NORMAL,       /* 12 */    
   TONE_KEY_POWER = TONE_KEY_NORMAL,     /* 12 */   

   TONE_KEY_CLICK,						/* 13 */

   TONE_CALL_CONNECT,					/* 14 */
   TONE_MT_SMS_IN_CALL,					/* 15 */
   TONE_CALL_TIME_REMINDER,				/* 16 */

   TONE_DIAL_CALL_GSM,    				/* 17 */
   TONE_DIAL_CALL_PCS,     				/* 18 */
   TONE_BUSY_CALL_GSM,    				/* 19 */
   TONE_BUSY_CALL_PCS,    				/* 20 */
   TONE_CONGESTION_GSM,  				/* 21 */ 
   TONE_CONGESTION_PCS,   				/* 22 */
   TONE_RADIO_PATH,       				/* 23 */
   TONE_RADIO_NA,         				/* 24 */
   TONE_ERROR_INFO,        				/* 25 */
   TONE_NUM_UNOBTAIN,     				/* 26 */
   TONE_AUTH_ERROR,       				/* 27 */
   TONE_CALL_WAITING,      				/* 28 */
   TONE_RINGING_CALL_GSM, 				/* 29 */
   TONE_RINGING_CALL_PCS, 				/* 30 */
   TONE_POSITIVE_ACK,      				/* 31 */
   TONE_NEGATIVE_ACK,     				/* 32 */   
													
   TONE_DIAL_CALL_GSM_STK,				/* 33 */
   TONE_DIAL_CALL_PCS_STK,				/* 34 */
   TONE_BUSY_CALL_GSM_STK,				/* 35 */
   TONE_BUSY_CALL_PCS_STK,				/* 36 */
   TONE_CONGESTION_GSM_STK,				/* 37 */
   TONE_CONGESTION_PCS_STK,				/* 38 */
   TONE_RADIO_PATH_STK,   				/* 39 */
   TONE_RADIO_NA_STK,     				/* 40 */
   TONE_ERROR_INFO_STK,   			    /* 41 */
   TONE_NUM_UNOBTAIN_STK, 				/* 42 */
   TONE_AUTH_ERROR_STK,   				/* 43 */
   TONE_CALL_WAITING_STK, 				/* 44 */
   TONE_RINGING_CALL_GSM_STK,			/* 45 */
   TONE_RINGING_CALL_PCS_STK,			/* 46 */
   TONE_POSITIVE_ACK_STK, 				/* 47 */
   TONE_NEGATIVE_ACK_STK, 				/* 48 */
			
			
   TONE_WARNING1,						/* 49 */
   TONE_WARNING2,						/* 50 */
   TONE_ERROR1,							/* 51 */
   TONE_ERROR2,							/* 52 */
   TONE_GENERAL_BEEP,					/* 53 */
   TONE_BATTERY_LOW,					/* 54 */
   TONE_BATTERY_WARNING,				/* 55 */
   TONE_AUX_IND,						/* 56 */
   TONE_DATA_CALL,						/* 57 */
#ifdef __OP01_FWPBW__
   TONE_CONTINUE_SOUND,		    		/* 58 */
#endif
   NUM_RESOURCE_TONES,

   MAX_TONE_ID = 80
}audio_id_enum;

typedef enum
{
    SRV_PROF_TONE_NONE,                 /* None */
    SRV_PROF_TONE_ERROR,                /* Error tone */
    SRV_PROF_TONE_CONNECT,              /* Connect tone */
    SRV_PROF_TONE_CAMP_ON,              /* Camp on tone */
    SRV_PROF_TONE_WARNING,              /* Warning tone */
    SRV_PROF_TONE_INCOMING_CALL,        /* Incoming call tone */
    SRV_PROF_TONE_INCOMING_CALL_CARD2,  /* Incoming call tone - card2 */
    SRV_PROF_TONE_INCOMING_CALL_CARD3,  /* Incoming call tone - card3 */
    SRV_PROF_TONE_INCOMING_CALL_CARD4,  /* Incoming call tone - card4 */
    SRV_PROF_TONE_VIDEO_CALL,           /* Video call tone */
    SRV_PROF_TONE_VIDEO_CALL_CARD2,     /* Video call tone - card2 */

                                        /*11*/
    SRV_PROF_TONE_VIDEO_CALL_CARD3,     /* Video call tone - card3 */
    SRV_PROF_TONE_VIDEO_CALL_CARD4,     /* Video call tone - card4 */
    SRV_PROF_TONE_SMS,                  /* SMS tone */    
    SRV_PROF_TONE_SMS_CARD2,            /* SMS tone - card2 */
    SRV_PROF_TONE_SMS_CARD3,            /* SMS tone - card3 */
    SRV_PROF_TONE_SMS_CARD4,            /* SMS tone - card4 */
    SRV_PROF_TONE_MMS,                  /* MMS tone */
    SRV_PROF_TONE_MMS_CARD2,            /* MMS tone - card2 */
    SRV_PROF_TONE_MMS_CARD3,            /* MMS tone - card3 */
    SRV_PROF_TONE_MMS_CARD4,            /* MMS tone - card4 */

                                        /*21*/
    SRV_PROF_TONE_EMAIL,                /* Email tone */
    SRV_PROF_TONE_EMAIL_CARD2,          /* Email tone - card2 */
    SRV_PROF_TONE_EMAIL_CARD3,          /* Email tone - card3 */
    SRV_PROF_TONE_EMAIL_CARD4,          /* Email tone - card4 */
    SRV_PROF_TONE_VOICE,                /* Voice tone */
    SRV_PROF_TONE_VOICE_CARD2,          /* Voice tone - card2 */
    SRV_PROF_TONE_VOICE_CARD3,          /* Voice tone - card3 */
    SRV_PROF_TONE_VOICE_CARD4,          /* Voice tone - card4 */
    SRV_PROF_TONE_ALARM,                /* Alarm tone */
    SRV_PROF_TONE_POWER_ON,             /* Power on tone */

                                        /*31*/
    SRV_PROF_TONE_POWER_OFF,            /* Power off tone */
    SRV_PROF_TONE_COVER_OPEN,           /* Cover open tone */
    SRV_PROF_TONE_COVER_CLOSE,          /* Cover close tone */
    SRV_PROF_TONE_SUCCESS,              /* Success tone */
    SRV_PROF_TONE_SAVE,                 /* Save tone */
    SRV_PROF_TONE_EMPTY_LIST,           /* Empty tone */
    SRV_PROF_TONE_GENERAL_TONE,         /* General tone */
    SRV_PROF_TONE_AUX,                  /* AUX tone */
    SRV_PROF_TONE_BATTERY_LOW,          /* Battery low tone */
    SRV_PROF_TONE_BATTERY_WARNING,      /* Battery warning tone */

                                        /*41*/
    SRV_PROF_TONE_CALL_REMINDER,        /* Call reminder tone */
    SRV_PROF_TONE_CCBS,                 /* CCBS tone */
    SRV_PROF_TONE_CONGESTION,           /* Congestion tone */
    SRV_PROF_TONE_AUTH_FAIL,            /* Authentication fail tone */
    SRV_PROF_TONE_NUM_UNOBTAIN,         /* Number un-obtained tone */
    SRV_PROF_TONE_CALL_DROP,            /* Call drop tone */
    SRV_PROF_TONE_SMS_INCALL,           /* SMS in call tone */
    SRV_PROF_TONE_WARNING_INCALL,       /* Warning in call tone */
    SRV_PROF_TONE_ERROR_INCALL,         /* Error in call tone */
    SRV_PROF_TONE_CONNECT_INCALL,       /* Connect in call tone */

                                        /*51*/
    SRV_PROF_TONE_SUCCESS_INCALL,       /* Success in call tone */
    SRV_PROF_TONE_IMPS_CONTACT_ONLINE,          /* IMPS tone - contact online */
    SRV_PROF_TONE_IMPS_NEW_MESSAGE,             /* IMPS tone - new message */
    SRV_PROF_TONE_IMPS_CONTACT_INVITATION,      /* IMPS tone - contact invitation */
    SRV_PROF_TONE_IMPS_CHATROOM_NOTIFICATION,   /* IMPS tone - chat room notification */
    SRV_PROF_TONE_FILE_ARRIVAL,         /* File arrival tone */
    SRV_PROF_TONE_SENT,                 /* Sent tone */
    SRV_PROF_TONE_DELETED,              /* Deleted tone */
    SRV_PROF_TONE_PROBLEM,              /* Problem tone */
    SRV_PROF_TONE_CONFIRM,              /* Confirm tone */

                                        /*61*/
    SRV_PROF_TONE_EXPLICITLY_SAVE,      /* Explicitly save tone */
    SRV_PROF_TONE_NOT_AVAILABLE,        /* Not available tone */
    SRV_PROF_TONE_ANS_PHONE,            /* Answer phone tone */
    SRV_PROF_TONE_WAP_PUSH,             /* WAP push tone */
    SRV_PROF_TONE_REMIND,               /* Remind tone */
    SRV_PROF_TONE_KEYPAD,               /* Key pad tone */
    SRV_PROF_TONE_EM,                   /* Engineering mode tone */
    SRV_PROF_TONE_FM,                   /* Factory mode tone */
    SRV_PROF_TONE_PHONEBOOK,            /* Phonebook tone */
    SRV_PROF_TONE_POC,                  /* PoC tone */

                                        /*71*/
    SRV_PROF_TONE_VOIP,                 /* Voip tone */
    SRV_PROF_TONE_SAT,                  /* SAT tone */
    SRV_PROF_TONE_GSM_BUSY,             /* GSM busy */
    SRV_PROF_TONE_WAITING_CALL,         /* Waiting call */
    SRV_PROF_TONE_ALARM_IN_CALL,        /* Alarm in call */
    SRV_PROF_TONE_DIALER,               /* Dialer tone */
    SRV_PROF_TONE_INBAND_PLAY,               // Tone type for inband tone

    NONE_TONE = SRV_PROF_TONE_NONE,
    ERROR_TONE = SRV_PROF_TONE_ERROR,
    CONNECT_TONE = SRV_PROF_TONE_CONNECT,
    CAMP_ON_TONE = SRV_PROF_TONE_CAMP_ON,
    WARNING_TONE = SRV_PROF_TONE_WARNING,
    INCOMING_CALL_TONE = SRV_PROF_TONE_INCOMING_CALL,
    ALARM_TONE = SRV_PROF_TONE_ALARM,
    POWER_ON_TONE = SRV_PROF_TONE_POWER_ON,
    POWER_OFF_TONE = SRV_PROF_TONE_POWER_OFF,
    COVER_OPEN_TONE = SRV_PROF_TONE_COVER_OPEN,
    COVER_CLOSE_TONE = SRV_PROF_TONE_COVER_CLOSE,
    MESSAGE_TONE = SRV_PROF_TONE_SMS,
    KEYPAD_PLAY_TONE = SRV_PROF_TONE_KEYPAD,
    SUCCESS_TONE = SRV_PROF_TONE_SUCCESS,
    SAVE_TONE = SRV_PROF_TONE_SAVE,
    EMPTY_LIST_TONE = SRV_PROF_TONE_EMPTY_LIST,
    GENERAL_TONE = SRV_PROF_TONE_GENERAL_TONE,
    SMS_IN_CALL_TONE = SRV_PROF_TONE_SMS_INCALL,
    AUX_TONE = SRV_PROF_TONE_AUX,
    WARNING_TONE_IN_CALL = SRV_PROF_TONE_WARNING_INCALL,
    ERROR_TONE_IN_CALL = SRV_PROF_TONE_ERROR_INCALL,
    CONNECT_TONE_IN_CALL = SRV_PROF_TONE_CONNECT_INCALL,
    SUCCESS_TONE_IN_CALL = SRV_PROF_TONE_SUCCESS_INCALL,
    BATTERY_LOW_TONE = SRV_PROF_TONE_BATTERY_LOW,
    BATTERY_WARNING_TONE = SRV_PROF_TONE_BATTERY_WARNING,
    CALL_REMINDER_TONE = SRV_PROF_TONE_CALL_REMINDER,
    CCBS_TONE = SRV_PROF_TONE_CCBS,
    CONGESTION_TONE = SRV_PROF_TONE_CONGESTION,
    AUTH_FAIL_TONE = SRV_PROF_TONE_AUTH_FAIL,
    NUM_UNOBTAIN_TONE = SRV_PROF_TONE_NUM_UNOBTAIN,
    CALL_DROP_TONE = SRV_PROF_TONE_CALL_DROP,
    IMPS_CONTACT_ONLINE_TONE = SRV_PROF_TONE_IMPS_CONTACT_ONLINE,
    IMPS_NEW_MESSAGE_TONE = SRV_PROF_TONE_IMPS_NEW_MESSAGE,
    IMPS_CONTACT_INVITATION_TONE = SRV_PROF_TONE_IMPS_CONTACT_INVITATION,
    IMPS_CHATROOM_NOTIFICATION_TONE = SRV_PROF_TONE_IMPS_CHATROOM_NOTIFICATION,
    CARD2_INCOMING_CALL_TONE = SRV_PROF_TONE_INCOMING_CALL_CARD2,
    CARD2_MESSAGE_TONE = SRV_PROF_TONE_SMS_CARD2,
    FILE_ARRIVAL_TONE = SRV_PROF_TONE_FILE_ARRIVAL,

    /*for op11*/
    SENT_TONE = SRV_PROF_TONE_SENT,
    DELETED_TONE = SRV_PROF_TONE_DELETED,
    PROBLEM_TONE = SRV_PROF_TONE_PROBLEM,
    NETWORK_PROBLEM_TONE = SRV_PROF_TONE_PROBLEM,
    CONFIRM_TONE = SRV_PROF_TONE_CONFIRM,
    EXPLICITLY_SAVE_TONE = SRV_PROF_TONE_EXPLICITLY_SAVE,
    NOT_AVAILABLE_TONE = SRV_PROF_TONE_NOT_AVAILABLE,        /*Oops tone*/
    ANS_PHONE_TONE = SRV_PROF_TONE_ANS_PHONE,
    SMS_TONE = SRV_PROF_TONE_SMS,
    MMS_TONE = SRV_PROF_TONE_MMS,
    EMAIL_TONE = SRV_PROF_TONE_EMAIL,
    VOICE_TONE = SRV_PROF_TONE_VOICE,
    WAP_PUSH_TONE = SRV_PROF_TONE_WAP_PUSH,
    REMIND_TONE = SRV_PROF_TONE_REMIND,



    SRV_PROF_TONE_END_OF_ENUM           /* End of enum */
}srv_prof_tone_enum;









/* SMS Action Type */
typedef enum
{
    SRV_SMS_ACTION_NONE,                /* None */
    SRV_SMS_ACTION_SEND,                /* Send SMS */
    SRV_SMS_ACTION_READ,                /* Read SMS */
    SRV_SMS_ACTION_DELETE,              /* Delete SMS */
    SRV_SMS_ACTION_SAVE,                /* Save SMS */
    SRV_SMS_ACTION_UPDATE,              /* Update SMS */
    SRV_SMS_ACTION_COPY,                /* Copy SMS */
    SRV_SMS_ACTION_MOVE,                /* Move SMS */
    SRV_SMS_ACTION_QUERY,               /* Query SMS */
    SRV_SMS_ACTION_CHANGE_STATUS,       /* Change a SMS's status */
    SRV_SMS_ACTION_COPY_TO_ARCHIVE,     /* Copy a Inbox/Sent SMS to Archive folder */
    SRV_SMS_ACTION_MOVE_TO_ARCHIVE,     /* Move a Inbox/Sent SMS to Archive folder */
    SRV_SMS_ACTION_COPY_FROM_ARCHIVE,   /* Move an archive SMS to original folder(Inbox/Sent) */
    SRV_SMS_ACTION_MOVE_FROM_ARCHIVE,   /* Move an archive SMS to original folder(Inbox/Sent) */
    SRV_SMS_ACTION_GET_CONTENT,         /* Get list displayed content of SMS */
    SRV_SMS_ACTION_GET_SIM_NUM,         /* Get the number of SIM Card SMS */
    SRV_SMS_ACTION_GET_REPLY_PATH,      /* Get the reply path of a SMS */
    SRV_SMS_ACTION_CREATE_LIST,         /* Creat Box List */
    SRV_SMS_ACTION_BACKUP,              //for sms backup
    SRV_SMS_ACTION_RESTORE,             //for sms backup

    SRV_SMS_ACTION_CB_GET_MODE,         /* Get Cell Broadcast's setting (Receive Mode/Channels/Languages) information from L4 */
    SRV_SMS_ACTION_CB_SET_MODE,         /* Set Cell Broadcast's setting information to L4  */
    SRV_SMS_ACTION_CB_GET_RECEIVE_MODE, /* Get Cell Broadcast's receive mode */
    SRV_SMS_ACTION_CB_SUBSCRIBE,        /* Subscribe Cell broadcast service */
    SRV_SMS_ACTION_CB_GET_CHNL,         /* Get Cell Broadcast's channel information */
    SRV_SMS_ACTION_CB_ADD_CHNL,         /* Add a new Cell Broadcast channel entry */
    SRV_SMS_ACTION_CB_DELETE_CHNL,      /* Delete a Cell Broadcast channel entry */
    SRV_SMS_ACTION_CB_UPDATE_CHNL,      /* Update a Cell Broadcast channel entry */
    SRV_SMS_ACTION_CB_GET_LANG,         /* Get Cell Broadcast's lanaguage information */
    SRV_SMS_ACTION_CB_SELECT_LANG,      /* Subscibe a Cell Broadcast channel */

    SRV_SMS_BTMAPC_OP_NONE,
    SRV_SMS_BTMAPC_OP_CONN, 
    SRV_SMS_BTMAPC_OP_DISCONN,
    SRV_SMS_BTMAPC_OP_CHANGE_STATUS,
    SRV_SMS_BTMAPC_OP_CHANGE_FOLDER,
    SRV_SMS_BTMAPC_OP_DELETE_MSG, 
    SRV_SMS_BTMAPC_OP_SEND_MSG,
    SRV_SMS_BTMAPC_OP_READ_MSG,
    SRV_SMS_BTMAPC_OP_GET_MSG,
    SRV_SMS_ACTION_TOTAL    
} srv_sms_action_enum;

/* SMS Cause Tpye */
typedef enum
{
	  
	  SRV_SMS_CAUSE_CLOUD_ERROR = (-1),     

    SRV_SMS_CAUSE_CLOUD_NO_MEMORY     = (-4),
    SRV_SMS_CAUSE_CLOUD_FS_ERROR        = (-5),
    SRV_SMS_CAUSE_CLOUD_NWK_ERROR       = (-6),
    SRV_SMS_CAUSE_CLOUD_BUSY            = (-8),
    SRV_SMS_CAUSE_CLOUD_LOGOUT          = (-9),
    SRV_SMS_CAUSE_CLOUD_NO_NEED_SYNC    = (-10),
    SRV_SMS_CAUSE_CLOUD_ACCT_ERROR      = (-11),
    SRV_SMS_CAUSE_CLOUD_NO_STORAGE      = (-12),
    SRV_SMS_CAUSE_CLOUD_DISK_FULL       = (-13),
    SRV_SMS_BTMAPC_OP_SUCC          = (-100), 
    SRV_SMS_BTMAPC_OP_FAIL          = (-200),
    SRV_SMS_CAUSE_NO_ERROR          = 0,

    /* Reference TS 27.005 Clause 3.2.5, For AT Command and MMI */
    SRV_SMS_CAUSE_ME_FAILURE        = 300,      /* ME failure */
    SRV_SMS_CAUSE_OP_NOT_ALLOWED    = 302,      /* Operation not allowed */
    SRV_SMS_CAUSE_OP_NOT_SUPPORTED  = 303,      /* Operation not supported */
    SRV_SMS_CAUSE_INVALID_PDU_PARA  = 304,      /* Invalid PDU mode parameter */
    SRV_SMS_CAUSE_INVALID_TEXT_PARA = 305,      /* Invalid text mode parameter */
    SRV_SMS_CAUSE_SIM_NOT_INSERTED  = 310,      /* (U)SIM not inserted */
    SRV_SMS_CAUSE_SIM_FAILURE       = 313,      /* (U)SIM failure */
    SRV_SMS_CAUSE_MEMORY_FAILURE    = 320,      /* Memory failure */
    SRV_SMS_CAUSE_INVALID_MEM_INDEX = 321,      /* Invalid memory index */
    SRV_SMS_CAUSE_MEM_FULL          = 322,      /* Memory full */
    SRV_SMS_CAUSE_SCA_UNKNOWN       = 330,      /* SMSC address unknown */

    /* MMI Defined Error Codes */
    SRV_SMS_CAUSE_NOT_READY         = 341,      /* SMS servicer not ready */
    SRV_SMS_CAUSE_SEND_BUSY,                    /* SMS servicer busy for sending SMS */
    SRV_SMS_CAUSE_SEND_ABORT,                   /* Send abort */
    SRV_SMS_CAUSE_MEM_INSUFFICIENT,             /* Memory insufficient */
    SRV_SMS_CAUSE_DATA_INVALID,                 /* SMS content invalid*/
    SRV_SMS_CAUSE_DATA_EXCEED,                  /* SMS content exceed*/
    SRV_SMS_CAUSE_NUMBER_EMPTY,                 /* SMS number empty*/
    SRV_SMS_CAUSE_NUMBER_INVALID,               /* SMS number invalid */
    SRV_SMS_CAUSE_SC_EMPTY,                     /* SMS service center is empty*/
    SRV_SMS_CAUSE_SC_INVALID,                   /* SMS service center invalid*/
    SRV_SMS_CAUSE_FDL_FAIL,                     /* FDN check failed */
    SRV_SMS_CAUSE_AT_CONFLICT,                  /* AT commond conflict */
    SRV_SMS_CAUSE_OP_CONFLICT,                  /* Operation conflict */
    SRV_SMS_CAUSE_EMAIL_NO_SPACE,               /* No space to handle email SMS */
    SRV_SMS_CAUSE_FS_ERROR,                     /* File system error */
    SRV_SMS_CAUSE_IN_USB_MODE,                  /* USB mode error */
    SRV_SMS_CAUSE_ARCH_FILE_NOT_EXIST,          /* Archive file not exist */
    SRV_SMS_CAUSE_ABORT,                        /* Abort an action */
    SRV_SMS_CAUSE_TCARD_NOT_AVAILABLE,          /* FOR tcard sms only */

    SRV_SMS_CAUSE_CB_NOT_READY,                 /* Cell Broadcast service not ready */
    SRV_SMS_CAUSE_CB_CHNL_DUPLICATED,           /* Duplicated channel */
    SRV_SMS_CAUSE_CB_CHNL_NOT_EXISTED,          /* Channel not exist */
    SRV_SMS_CAUSE_CB_CHNL_FULL,                 /* Channel full */
    SRV_SMS_CAUSE_CB_LANG_NOT_EXISTED,          /* Language not exist */

    SRV_SMS_CAUSE_UNKNOWN_ERROR     = 500,       /* Unknown error */
    SRV_SMS_CAUSE_ERROR_FDN_ENABLED     = 2601  /* FDN fail */
} srv_sms_cause_enum;


/* SMS Operation Callback Data Structure */
typedef struct
{
    MMI_BOOL result;                /* Result of the operation: MMI_TRUE: success; MMI_FALSE: failed */
    srv_sms_action_enum action;     /* Action to indicate the operation type(read/save/Delete...), refer to srv_sms_action_enum */
    srv_sms_cause_enum cause;       /* Cause failed, only valid result be MMI_FALSE, refer to srv_sms_cause_enum */
    void *action_data;              /* Callback data, refer to  srv_sms_xxx_cb_struct, ex. read callback data struct: srv_sms_read_msg_cb_struct*/
    void *user_data;                /* User data */
} srv_sms_callback_struct;

/* SMS DCS Tpye, Character sets of the Data Coding Scheme (DCS) (Reference SPEC 3GPP TS 23.038, Clause 4). */
typedef enum
{
    SRV_SMS_DCS_7BIT        = 0x00, /* GSM 7 bit default alphabet */
    SRV_SMS_DCS_8BIT        = 0x04, /* 8 bit data */
    SRV_SMS_DCS_UCS2        = 0x08, /* UCS2 (16bit) */
    SRV_SMS_DCS_RESERVED    = 0x0c  /* Reserved */
} srv_sms_dcs_enum;

typedef void(*SrvSmsCallbackFunc)(srv_sms_callback_struct* callback_data);

typedef enum
{
    DYNAMIC_APN_NAME_START,
    DYNAMIC_APN_NAME_1,
    DYNAMIC_APN_NAME_2,
    DYNAMIC_APN_NAME_3,
    DYNAMIC_APN_NAME_4,
    DYNAMIC_APN_NAME_5,
    DYNAMIC_APN_NAME_6,
    DYNAMIC_APN_NAME_7,
    DYNAMIC_APN_NAME_8,
    DYNAMIC_APN_NAME_9,
    DYNAMIC_APN_NAME_10,
    DYNAMIC_APN_NAME_11,
    
        
    DYNAMIC_APN_NAME_END
} dynamic_apn_name_enum;

#define MAX_GPRS_USER_NAME_LEN 32
#define MAX_GPRS_PASSWORD_LEN  32
#define MAX_GPRS_APN_LEN       100
#define MAX_GPRS_IP_ADDR       4 /* IP address length */

typedef struct
{   
    dynamic_apn_name_enum tag_apn_index;
    kal_uint32 tag_numeric; // 国家码的整型数字
    kal_uint8 tag_apn[MAX_GPRS_APN_LEN]; //APN asc码
    kal_uint8 tag_user_name[MAX_GPRS_USER_NAME_LEN];//用户名(可选)
    kal_uint8 tag_password[MAX_GPRS_PASSWORD_LEN];//密码(可选)
    kal_uint8 tag_dns1[MAX_GPRS_IP_ADDR]; //十进制
    kal_uint8 tag_dns2[MAX_GPRS_IP_ADDR]; //十进制
}dynamic_apn_set_sturct;

typedef enum
{
/*
 * The key code sequency cannot be changed.
 */
    KEY_0 = 0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_LSK, //10
    KEY_RSK, //11
    KEY_CSK, //12
    KEY_ENTER = KEY_CSK, //12
    KEY_WAP = KEY_ENTER, //12
    KEY_IP = KEY_ENTER,  //12
    KEY_UP_ARROW, //13
    KEY_DOWN_ARROW, //14
    KEY_LEFT_ARROW, //15
    KEY_RIGHT_ARROW, //16
    KEY_SEND, //17
    KEY_END, //18
    KEY_CLEAR, //19
    KEY_STAR, //20
    KEY_POUND, //21
    KEY_VOL_UP, //22
    KEY_VOL_DOWN, //23
    KEY_CAMERA, //24
    KEY_QUICK_ACS = KEY_CAMERA, //24
    KEY_EXTRA_1, //25
    KEY_EXTRA_2, //26
    KEY_PLAY_STOP, //27
    KEY_FWD, //28
    KEY_BACK, //29
    KEY_POWER, //30
    KEY_EXTRA_A, //31
    KEY_EXTRA_B, //32
    KEY_SEND1, //33
    KEY_SEND2, //34
    KEY_HOME, //35
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_MESSAGE,
    MAX_KEYS,
    KEY_A = MAX_KEYS,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,
    KEY_SPACE,
    KEY_TAB,
    KEY_DEL,
    KEY_ALT,
    KEY_CTRL,
    KEY_WIN,
    KEY_SHIFT,
    KEY_QUESTION,
    KEY_PERIOD,
    KEY_COMMA,
    KEY_EXCLAMATION,
    KEY_APOSTROPHE,
    KEY_AT,
    KEY_BACKSPACE,
    KEY_QWERTY_ENTER,
    KEY_FN,
    KEY_SYMBOL,
    KEY_NUM_LOCK,
    KEY_QWERTY_MENU,
    KEY_OK,
    KEY_DOLLAR,
    MAX_QWERTY_KEYS,

    KEY_INVALID = 0xFE
} mmi_keypads_enum;

typedef enum
{
    KEY_FULL_PRESS_DOWN,
    KEY_EVENT_DOWN = KEY_FULL_PRESS_DOWN,
    KEY_EVENT_UP,
    KEY_LONG_PRESS,
    KEY_REPEAT,
    KEY_HALF_PRESS_DOWN,
    KEY_HALF_PRESS_UP,
    MAX_KEY_TYPE
} mmi_key_types_enum;


/* Define the audio play style. */
typedef enum {
    DEVICE_AUDIO_PLAY_CRESCENDO  = 0 ,  /* Play sound for crescendo. */
    DEVICE_AUDIO_PLAY_INFINITE       ,  /* Play sound for infinite. */
    DEVICE_AUDIO_PLAY_ONCE           ,  /* Play sound for once. */
    DEVICE_AUDIO_PLAY_DESCENDO         /* Play sound for descendo. */
}audio_play_style_enum;

/* Define the audio device type */
typedef enum {
    AUDIO_DEVICE_SPEAKER = 0  ,     /* Tone, Keypad sound to play on audio device. */
    AUDIO_DEVICE_MICROPHONE   ,     /* Microphone sound to send on audio device. */
    AUDIO_DEVICE_BUZZER       ,     /* Ring tone sound to play on audio device. */
    AUDIO_DEVICE_GMI          ,     /* MIDI/Melody sound to play on audio device */
    AUDIO_DEVICE_SPEAKER2     ,     /* earphone, carkit */
    AUDIO_DEVICE_LOUDSPEAKER  ,     /* loudspeaker for free sound */
    AUDIO_DEVICE_SPEAKER_BOTH ,
    MAX_AUDIO_DEVICE_NUM      
}audio_type_enum;

typedef enum {
   MEDIA_FORMAT_GSM_FR,
   MEDIA_FORMAT_GSM_HR,
   MEDIA_FORMAT_GSM_EFR,
   MEDIA_FORMAT_AMR,
   MEDIA_FORMAT_AMR_WB,
   MEDIA_FORMAT_DAF,
   MEDIA_FORMAT_AAC,
   MEDIA_FORMAT_PCM_8K,
   MEDIA_FORMAT_PCM_16K,
   MEDIA_FORMAT_G711_ALAW,
   MEDIA_FORMAT_G711_ULAW,
   MEDIA_FORMAT_DVI_ADPCM,
   MEDIA_FORMAT_VRD,
   MEDIA_FORMAT_WAV,
   MEDIA_FORMAT_WAV_ALAW,
   MEDIA_FORMAT_WAV_ULAW,
   MEDIA_FORMAT_WAV_DVI_ADPCM,
   MEDIA_FORMAT_SMF,
   MEDIA_FORMAT_IMELODY,
   MEDIA_FORMAT_SMF_SND,
   MEDIA_FORMAT_MMF,
   MEDIA_FORMAT_AU,
   MEDIA_FORMAT_AIFF,
   MEDIA_FORMAT_VRSI,
   MEDIA_FORMAT_WMA,
   MEDIA_FORMAT_M4A,
   MEDIA_FORMAT_WAV_DVI_ADPCM_16K,
   MEDIA_FORMAT_VOIPEVL,
   MEDIA_FORMAT_AAC_PLUS,
   MEDIA_FORMAT_AAC_PLUS_V2,
   MEDIA_FORMAT_BSAC,
   MEDIA_FORMAT_MUSICAM = 32,
   MEDIA_FORMAT_AWB_PLUS,
   MEDIA_FORMAT_AWB_PLUS_EXTEND,
   MEDIA_FORMAT_WAV_16K,
   MEDIA_FORMAT_MP4_AAC,
   MEDIA_FORMAT_MP4_AMR,
   MEDIA_FORMAT_MP4_AMR_WB,
   MEDIA_FORMAT_MP4_BSAC,
   MEDIA_FORMAT_DRA,
   MEDIA_FORMAT_COOK,
   MEDIA_FORMAT_APE,
   MEDIA_FORMAT_PCM,
   MEDIA_FORMAT_JTS,
   MEDIA_FORMAT_VORBIS,
   MEDIA_FORMAT_FLAC,
   MEDIA_FORMAT_A2DP,
   MEDIA_FORMAT_MIXER = 100,
   MEDIA_FORMAT_UNKNOWN
} Media_Format;

/* this shall be sync with aud_defs.h, backward compatible */
typedef enum
{
    MED_TYPE_NONE           = -1,
    MED_TYPE_GSM_FR         = MEDIA_FORMAT_GSM_FR,        /* 0 */
    MED_TYPE_GSM_HR         = MEDIA_FORMAT_GSM_HR,        /* 1 */
    MED_TYPE_GSM_EFR        = MEDIA_FORMAT_GSM_EFR,       /* 2 */
    MED_TYPE_AMR            = MEDIA_FORMAT_AMR,           /* 3 */
    MED_TYPE_AMR_WB         = MEDIA_FORMAT_AMR_WB,        /* 4 */
    MED_TYPE_DAF            = MEDIA_FORMAT_DAF,           /* 5 */
    MED_TYPE_AAC            = MEDIA_FORMAT_AAC,           /* 6 */
    MED_TYPE_PCM_8K         = MEDIA_FORMAT_PCM_8K,        /* 7 */
    MED_TYPE_PCM_16K        = MEDIA_FORMAT_PCM_16K,       /* 8 */
    MED_TYPE_G711_ALAW      = MEDIA_FORMAT_G711_ALAW,     /* 9 */
    MED_TYPE_G711_ULAW      = MEDIA_FORMAT_G711_ULAW,     /* 10 */
    MED_TYPE_DVI_ADPCM      = MEDIA_FORMAT_DVI_ADPCM,     /* 11 */
    MED_TYPE_VR             = MEDIA_FORMAT_VRD,           /* 12 */
    MED_TYPE_WAV            = MEDIA_FORMAT_WAV,           /* 13 */
    MED_TYPE_WAV_ALAW       = MEDIA_FORMAT_WAV_ALAW,      /* 14 */
    MED_TYPE_WAV_ULAW       = MEDIA_FORMAT_WAV_ULAW,      /* 15 */
    MED_TYPE_WAV_DVI_ADPCM  = MEDIA_FORMAT_WAV_DVI_ADPCM, /* 16 */
    MED_TYPE_SMF            = MEDIA_FORMAT_SMF,           /* 17 */
    MED_TYPE_IMELODY        = MEDIA_FORMAT_IMELODY,       /* 18 */
    MED_TYPE_SMF_SND        = MEDIA_FORMAT_SMF_SND,       /* 19 */
    MED_TYPE_MMF            = MEDIA_FORMAT_MMF,           /* 20 */
    MED_TYPE_AU             = MEDIA_FORMAT_AU,            /* 21 */
    MED_TYPE_AIFF           = MEDIA_FORMAT_AIFF,          /* 22 */
    MED_TYPE_VRSI           = MEDIA_FORMAT_VRSI,          /* 23 */
    MED_TYPE_WMA            = MEDIA_FORMAT_WMA,           /* 24 */
    MED_TYPE_M4A            = MEDIA_FORMAT_M4A,           /* 25 */
    MED_TYPE_BSAC           = MEDIA_FORMAT_BSAC,          /* 31 */
    MED_TYPE_MUSICAM        = MEDIA_FORMAT_MUSICAM,       /* 32 */
    MED_TYPE_APE            = MEDIA_FORMAT_APE,           /* 42 */
    MED_TYPE_TONE           = MEDIA_FORMAT_JTS,           /* 44 : JTone */
    MED_TYPE_VORBIS         = MEDIA_FORMAT_VORBIS,
    MED_TYPE_FLAC           = MEDIA_FORMAT_FLAC,
    MED_TYPE_KURO           = MEDIA_FORMAT_UNKNOWN,       /* 45: The types that are not re-mappable to Media_Format should start from here */
    MED_TYPE_KURO_AAC,      /* 46 */
    MED_TYPE_RMVB,          /* 47 */
    MED_TYPE_RM,            /* 48 */
    MED_TYPE_RA,            /* 49 */
    MED_TYPE_RV,            /* 50 */
    MED_TYPE_MIDI,          /* 51 : JMidi */
    MED_TYPE_3GP,           /* 52 */
    MED_TYPE_MP4,           /* 53 */
    MED_TYPE_JPG,           /* 54 */
    MED_TYPE_GIF,           /* 55 */
    MED_TYPE_MJPG,          /* 56 */
    MED_TYPE_PNG,           /* 57 */
    MED_TYPE_AVI,           /* 58 */
    MED_TYPE_FLV,           /* 59 */
    MED_TYPE_F4V,           /* 60 */
    MED_TYPE_MOV,           /* 61 */
    MED_TYPE_MKV,           /* 62 */
    MED_TYPE_F4A,           /* 63 */
    MED_TYPE_MOT,
    MED_TYPE_YTS,
    MED_TYPE_KMV,
    MED_NO_OF_TYPE
}med_type_enum;

typedef kal_uint32 PWM_CHANNEL;

typedef enum 
{
	pwmclk_1MHZ=0,
	pwmclk_2MHZ,
	pwmclk_4MHZ,
	pwmclk_8MHZ 
}PWMClock_DIV;

typedef enum 
{
	pwmclk_13M=0,
	pwmclk_32k
}PWMClock_SEL;

typedef enum 
{
	PWM1=0,
	PWM2,
	PWM3,
	PWM4
}PWM_TYPE;

#endif
