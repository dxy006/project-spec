/*
 * sys_print.c
 *
 *  Created on: 2020��7��29��
 *      Author: ����
 */
#include "sys_print.h"
signed char g_dbgSwitch = SW_DBG_LEVEL;
signed char *g_colorMap[6] = {NO_CLOR, RED, YELLOW, GREEN, LIGHT_GRAY, LIGHT_CYAN};
