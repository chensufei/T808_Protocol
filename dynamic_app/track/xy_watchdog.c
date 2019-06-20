/********************************************************************************
**
** 文件名:     xy_watchdog.c
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

static kal_bool s_watchdog_status = KAL_FALSE;


/*******************************************************************
** 函数名:     xy_watchdog_task
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_watchdog_task(void*str)
{
    s_watchdog_status = !s_watchdog_status;
#ifdef __XY_SUPPORT__
    GPIO_WriteIO(s_watchdog_status,XY_WATCHDOG_PORT);
#endif
}

/*******************************************************************
** 函数名:     xy_watchdog_init
** 函数描述:   
** 参数:       
** 返回:       
********************************************************************/
void xy_watchdog_init(void)
{
#ifdef __XY_SUPPORT__
    // 设置为输出模式
    GPIO_ModeSetup(XY_WATCHDOG_PORT,0);   
    GPIO_InitIO(OUTPUT,XY_WATCHDOG_PORT);
    GPIO_WriteIO(s_watchdog_status,XY_WATCHDOG_PORT);
#endif

    dynamic_timer_start(enum_timer_watchdog_task_timer,500,(void*)xy_watchdog_task,NULL,TRUE);
}


