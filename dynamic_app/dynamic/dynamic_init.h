#ifndef __DYNAMIC_INIT_H__
#define __DYNAMIC_INIT_H__


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define __XY_SUPPORT__
//#define __XY_MILEAGE_SUPPORT__

#include "dynamic_struct.h"
#include "dynamic_common.h"
#include "dynamic_file.h"
#include "dynamic_reset.h"
#include "dynamic_msg_at.h"
#include "dynamic_telephony.h"
#include "dynamic_timer.h"
#include "dynamic_sms.h"
#include "dynamic_gpio.h"
#include "dynamic_gps.h"
#include "dynamic_sensor.h"
#include "dynamic_upgrade.h"


#ifdef __XY_SUPPORT__
#include "xy_main.h"
#include "xy_watchdog.h"
#include "xy_alm.h"
#include "xy_soc.h"
#include "xy_t808_protocol.h"
#include "xy_track.h"
#include "xy_telephony.h"
#include "xy_led.h"
#include "xy_sms.h"
#ifdef __XY_MILEAGE_SUPPORT__
#include "xy_mileage.h"
#endif

#endif

#include "dynamic_api.h"

#define MAX_IMAGE_SIZE (200 * 1024)

#define DYNAMIC_SOFT_VER "TEST_V1.0"

#define DYNAMIC_SOFT_VER_FOR_PROTOCOL 0x0101 // 高8位硬件版本，低8位软件版本

#endif

