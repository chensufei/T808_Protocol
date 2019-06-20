/********************************************************************************
**
** 文件名:     dynamic_sensor.c
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



#define DYNAMIC_ACCELECATION_SPORT_THRESHOLD 25 // 判断运动阈值
#define DYNAMIC_ACCELECATION_CHECK_CNT 20 // 检测震动次数
#define DYNAMIC_CHECK_SPORT_CNT 5 // 判断为运动的次数
#define DYNAMIC_CHECK_STATIC_CNT 3 // 小于次数判断为没有动
//#define DYNAMIC_CHECK_STATIC_TIME 120 // x/2 s没有动判断为静止

#define DYNAMIC_MIN_VIB_ALM_THRESHOLD 1 // 震动报警阈值
#define DYNAMIC_DEFAULT_VIB_ALM_THRESHOLD 1 // 震动报警阈值
#define DYNAMIC_MAX_VIB_ALM_THRESHOLD 6 // 震动报警阈值

static kal_uint16 s_check_cnt = 0;
static kal_uint32 s_accelecation_dis[DYNAMIC_ACCELECATION_CHECK_CNT];
static kal_uint8 s_vib_state_3min = 0;
static char s_sensor_buffer[10];

#define MS_SLAVE_ADDR       0x98//0xD8 //0x4C << 1 
#define DYNAMIC_I2C_SPEED 350

#define MS_REG_XOUT 0x00

//Register
#define MS_REG_INTEN 0x06
#define MS_REG_MODE 0x07
#define MS_REG_SAMPR 0x08
#define MS_REG_TAPEN 0x09

#define MS_REG_TAPX 0x0a
#define MS_REG_TAPY 0x0b
#define MS_REG_TAPZ 0x0c

#define MS_REG_CHIPID 0x18
#define MS_REG_PCODE 0x3B


//Operate state
#define MS_STANDBY_MODE 0x03
#define MS_WAKE_MODE 0x01
#define MS_SNIFF_MODE 0x02 

kal_uint8 s_vibsensity_cnt[DYNAMIC_MAX_VIBSENSITY+1]={5,4,2};
kal_uint16 s_vibsensity_dis[DYNAMIC_MAX_VIBSENSITY+1]={300,25,15};
kal_uint32 s_sensor_chipid = 0;


#define dynamic_i2c_read(a,b,c) dynamic_i2c_receive(MS_SLAVE_ADDR,a,b,c)
kal_bool dynamic_i2c_write_byte(kal_uint8 ucBufferIndex, kal_uint8 pucData)
{
	return dynamic_i2c_send(MS_SLAVE_ADDR,ucBufferIndex,&pucData,1);
}

/*******************************************************************
** 函数名:     dynamic_i2c_read_xyz
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_i2c_read_xyz(kal_int16 *x,kal_int16 *y,kal_int16 *z)
{	 
    dynamic_i2c_read(0x0d,(kal_uint8*)s_sensor_buffer,6);
	*x=(kal_int16)(s_sensor_buffer[0] + (s_sensor_buffer[1] << 8));
	*y= (kal_int16)(s_sensor_buffer[2] + (s_sensor_buffer[3] << 8));
	*z=(kal_int16)(s_sensor_buffer[4] + (s_sensor_buffer[5] << 8));
}
/*******************************************************************
** 函数名:     dynamic_sensor_read_xyz
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_read_xyz(kal_int16 *x,kal_int16 *y,kal_int16 *z)
{	 
    dynamic_i2c_read_xyz(x,y,z);
}

/*******************************************************************
** 函数名:     dynamic_sensor_vib_3min_clr
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_vib_3min_clr(void)
{
    s_vib_state_3min = 0;
}

/*******************************************************************
** 函数名:     dynamic_sensor_vib_in_3min
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
kal_uint8 dynamic_sensor_vib_in_3min(void)
{
    return s_vib_state_3min;
}

/*******************************************************************
** 函数名:     dynamic_sensor_value_count
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_value_count(void)
{
    kal_uint16 i;
    kal_uint16 valid_cnt = 0;
    kal_uint8 vibsensity = DYNAMIC_DEFAULT_VIBSENSITY;
#ifdef __XY_SUPPORT__
    XY_INFO_T * xy_info = xy_get_info();
    
    vibsensity = xy_info->vibsensity;
#endif
    for(i=0;i<DYNAMIC_ACCELECATION_CHECK_CNT;i++)
    {
        if (s_accelecation_dis[i] >= s_vibsensity_dis[vibsensity])
        {
            valid_cnt++;
        }
    }

    if (valid_cnt <= DYNAMIC_CHECK_STATIC_CNT)
    {

    }
    else
    {
        if (valid_cnt >= s_vibsensity_cnt[vibsensity])
        {
            s_vib_state_3min = 1;
            dynamic_timer_start(enum_timer_vib_delay_timer,180*1000,(void*)dynamic_sensor_vib_3min_clr,NULL,FALSE);
#ifdef __XY_SUPPORT__
            if (xy_info->sport_state == 0)
            {
                xy_info->sport_state = 1;
                dynamic_debug("震动检测判断为运动状态");
                xy_info_save();

                if (xy_info->mode != XY_TRACK_MODE1)
                {
                    dynamic_timer_start(enum_timer_track_task_timer,1000,(void*)xy_track_dataup_task,NULL,FALSE);
                }
            }
            if (xy_info->anti_thrief && xy_info->vib_alm == 0)
            {
                xy_info->vib_alm = 1;
                xy_alm_vib_alm();
            }            
            xy_track_wakeup();
#endif
        }
    }
}

/*******************************************************************
** 函数名:     dynamic_sensor_clr_vib
** 函数描述:   
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_clr_vib(void)
{        
    memset(s_accelecation_dis,0,sizeof(s_accelecation_dis));
}

/*******************************************************************
** 函数名:     dynamic_sensor_count_for_accon
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_count_for_accon(void)
{    
    memset(s_accelecation_dis,0,sizeof(s_accelecation_dis));
}

/*******************************************************************
** 函数名:     dynamic_sensor_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_sensor_task(void*str)
{
    kal_int16 x,y,z;
    kal_uint32 accelecation = 0;
    kal_uint32 xyz = 0;
    kal_uint32 accelecation_dis = 0;
    
    static kal_uint32 accelecation_buf = 0xffff;
    static kal_uint16 err_cnt = 0;
    static kal_int16 x_buf,y_buf,z_buf;
    static kal_uint16 log_cnt = 0;
    
    dynamic_sensor_read_xyz(&x,&y,&z);
    
    if (++log_cnt >= 200)
    {
        log_cnt = 0;
        dynamic_debug("x:%d,y:%d,z:%d",x,y,z);
    }
    xyz = x*x+y*y+z*z;
    
    accelecation = (kal_uint32)sqrt(xyz);
    if (accelecation != 0)
    {
        kal_uint16 temp;

        err_cnt = 0;
        temp = 1024;
        accelecation = accelecation*980/temp;
        //dynamic_debug("accelecation:%d",accelecation);
        #if 0
        accelecation_dis = abs(accelecation-980); // 加速度
        #else
        if (accelecation_buf != 0xffff)
        {
            accelecation_dis = abs(accelecation-accelecation_buf); // 加速度变化，扩大100倍          
        }
        accelecation_buf = accelecation;
        #endif
        
        if (s_check_cnt < DYNAMIC_ACCELECATION_CHECK_CNT)
        {
            s_accelecation_dis[s_check_cnt++] = accelecation_dis;
        }
        else
        {
            memcpy(&s_accelecation_dis[0],&s_accelecation_dis[1],(DYNAMIC_ACCELECATION_CHECK_CNT-1)*sizeof(kal_uint32));
            s_accelecation_dis[DYNAMIC_ACCELECATION_CHECK_CNT-1] = accelecation_dis;
        }

        if (s_check_cnt >= DYNAMIC_ACCELECATION_CHECK_CNT)
        {
            dynamic_sensor_value_count();
        }
        //dynamic_debug("xyz:%d,%d,%d",x,y,z);  
#ifdef __XY_SUPPORT__
        {
            XY_INFO_T * xy_info = xy_get_info();
            if (accelecation_dis >= s_vibsensity_dis[xy_info->vibsensity])
            {
                dynamic_debug("accelecation_dis:%d",accelecation_dis);
            }
        }
#endif
    }
    else
    {
        err_cnt++;
        dynamic_debug("GSENSOR数据读取错误");
    }

    if (x == x_buf && y == y_buf && z == z_buf)
    {
        err_cnt++;
        dynamic_debug("GSENSOR数据读取不变");
    }
    x_buf = x;
    y_buf = y;
    z_buf = z;

    if (++err_cnt >= 100)
    {
        err_cnt = 0;
        dynamic_sensor_init();
        //dynamic_start_reset(RESET_TYPE_FOR_CMD);
    }
}

/*******************************************************************
** 函数名:     dynamic_sensor_get_id
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
kal_uint32 dynamic_sensor_get_id(void)
{
    return s_sensor_chipid;
}

/*******************************************************************
** 函数名:     dynamic_sensor_cnfig
** 函数描述: 
** 参数: 
** 返回:       
********************************************************************/
void dynamic_sensor_cnfig(void)
{
    //C901_INFO_T * c901_info = c901_get_info();
    
    dynamic_i2c_configure(MS_SLAVE_ADDR, DYNAMIC_I2C_SPEED); 
    dynamic_i2c_write_byte(MS_REG_MODE, MS_STANDBY_MODE); //standby mode
    
    //get chipid
    dynamic_i2c_read(MS_REG_CHIPID,(kal_uint8*)s_sensor_buffer,1);
    dynamic_debug("chip id = %d",s_sensor_buffer[0]);
    s_sensor_chipid = s_sensor_buffer[0];
    //get Product code
    dynamic_i2c_read(MS_REG_PCODE,(kal_uint8*)s_sensor_buffer,1);
    dynamic_debug("Product code = %d",s_sensor_buffer[0]);
    if(0x10==(s_sensor_buffer[0]&0xF1))
    {
        dynamic_debug("MC3413");
    }
    dynamic_i2c_write_byte(MS_REG_MODE, MS_STANDBY_MODE); //standby mode

    dynamic_i2c_write_byte(MS_REG_SAMPR, 0x0a);
    dynamic_i2c_write_byte(0x20,0x25);
    /* Interrupt Configuration */
    //dynamic_i2c_write_byte(MS_REG_INTEN,0x3f); // 中断使能
    //dynamic_i2c_write_byte(MS_REG_TAPEN, 0xff); // Registers 0xA, 0xB, 0xC point to tap threshold settings
    
    /* Mode Configuration Structure */	 
    //dynamic_i2c_write_byte(MS_REG_MODE,0x81); //wake up mode 0x81中断低电平  0x01高电平
    dynamic_i2c_write_byte(MS_REG_MODE, MS_WAKE_MODE);
}

/*******************************************************************
** 函数名:     dynamic_sensor_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void dynamic_sensor_init(void)
{    
    dynamic_sensor_cnfig();
    dynamic_timer_start(enum_timer_sensor_task_timer,500,(void*)dynamic_sensor_task,NULL,TRUE);    
}


