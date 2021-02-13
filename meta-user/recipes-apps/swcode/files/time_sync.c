/*
 * time_sync.c
 *  Created on: 20201024
 *  Author: DuXingyu
 */

#include "time_sync.h"

/*************************************************
* Function : SetTimeSyncPerid
* Input : timeSyncPerid  time sync period
*
*************************************************/
void SetTimeSyncPerid(u16 timeSyncPerid)
{
#if 0
    u32 registerData = fc_drv_read(TIMESYNC_BASEADDR, TIME_SYNC_PERIOD_SET_OFFSET_ADDR);
    /* reset the time sync perid value */
    registerData &= 0xFFFFF000;
    registerData |= (timeSyncPerid & 0xFFF);
    fc_drv_write(registerData, TIMESYNC_BASEADDR, TIME_SYNC_PERIOD_SET_OFFSET_ADDR);
#endif
    return;
}

/*************************************************
* Function : TimeSyncSlavePortEnableCtl
* Input : 
* Attention : Set to 0 is enable, set to 1 is disable
*************************************************/
void TimeSyncSlavePortEnableCtl(u8 portNum, u8 contrlFlag)
{
    u32 dataTemp;
    /*
    if (contrlFlag == DISABLE_FLAG) {
        dataTemp = fc_drv_read(TIMESYNC_BASEADDR, TIME_SYNC_ENABLE_OFFSET_ADDR);
        dataTemp &= (~(1 << portNum));
        fc_drv_write(dataTemp, TIMESYNC_BASEADDR, TIME_SYNC_ENABLE_OFFSET_ADDR);
    } else {
        dataTemp = fc_drv_read(TIMESYNC_BASEADDR, TIME_SYNC_ENABLE_OFFSET_ADDR);
        dataTemp |= (1 << portNum);
        fc_drv_write(dataTemp, TIMESYNC_BASEADDR, TIME_SYNC_ENABLE_OFFSET_ADDR);
    }
    */
    return;
}

u8 GetTimeSyncEnableState(u8 portNum)
{
    u8 enableStatus = ENABLE_FLAG;
    /*
    if ((g_ZKTempFlag.timeSyncEnableFlag & (1 << portNum)) == 0) {
        enableStatus = DISABLE_FLAG;
    }
    */
    return enableStatus;  
}

void SetTimeSyncEnableFlag(u8 portNum, u8 timeSyncState)
{
    /*
    if (timeSyncState == ENABLE_FLAG) {
        g_ZKTempFlag.timeSyncEnableFlag |= (1 << portNum);
    } else {
        g_ZKTempFlag.timeSyncEnableFlag &= (~(1 << portNum));
    }   
    */
    return;
}


