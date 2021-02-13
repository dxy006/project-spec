#include <pthread.h>
#include "common.h"
#include "sw_pthread.h"
#include "zk_communicate.h"
#include "sys_print.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "serial_cmd.h"

u32 PthreadCreat(void)
{
    pthread_t tidp[MAX_THREAD_NUM] = {0};
    u32 pthreadNum = 0;
    INT32 retValue;
    retValue = InitFiFo();
    
    retValue = pthread_create(&(tidp[pthreadNum++]), NULL, QuerySpiData, NULL);
    if (retValue == ENV_ERRO) {
        SW_DBG(DBG_ERROR, "QuerySpiData thread create error!\n");
        return RET_FAILED;
    }
    
    if (retValue == RET_SUCCESS) {     
        retValue = pthread_create(&(tidp[pthreadNum++]), NULL, GetAndDealSpiData, NULL);
        if (retValue == ENV_ERRO) {
            SW_DBG(DBG_ERROR, "GetAndDealSpiData thread create error!\n");
            return RET_FAILED;
        }
    }
    
    retValue = pthread_create(&(tidp[pthreadNum++]), NULL, DBG_DealSerialCmd, NULL);
    if (retValue == ENV_ERRO) {
        SW_DBG(DBG_ERROR, "DBG_DealSerialCmd thread create error!\n");
        return RET_FAILED;
    }

    return RET_SUCCESS;
}


