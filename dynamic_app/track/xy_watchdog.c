/********************************************************************************
**
** �ļ���:     xy_watchdog.c
** ��Ȩ����:   
** �ļ�����:   
**
*********************************************************************************
**             �޸���ʷ��¼
**===============================================================================
**| ����       | ����   |  �޸ļ�¼
**===============================================================================
**|  |  | 
*********************************************************************************/

#include "dynamic_init.h"

static kal_bool s_watchdog_status = KAL_FALSE;


/*******************************************************************
** ������:     xy_watchdog_task
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_watchdog_task(void*str)
{
    s_watchdog_status = !s_watchdog_status;
#ifdef __XY_SUPPORT__
    GPIO_WriteIO(s_watchdog_status,XY_WATCHDOG_PORT);
#endif
}

/*******************************************************************
** ������:     xy_watchdog_init
** ��������:   
** ����:       
** ����:       
********************************************************************/
void xy_watchdog_init(void)
{
#ifdef __XY_SUPPORT__
    // ����Ϊ���ģʽ
    GPIO_ModeSetup(XY_WATCHDOG_PORT,0);   
    GPIO_InitIO(OUTPUT,XY_WATCHDOG_PORT);
    GPIO_WriteIO(s_watchdog_status,XY_WATCHDOG_PORT);
#endif

    dynamic_timer_start(enum_timer_watchdog_task_timer,500,(void*)xy_watchdog_task,NULL,TRUE);
}


