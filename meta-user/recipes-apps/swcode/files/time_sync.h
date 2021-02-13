#ifndef TIME_SYNC_H
#define TIME_SYNC_H
#include "common.h"

u8 GetTimeSyncEnableState(u8 portNum);
void SetTimeSyncPerid(u16 timeSyncPerid);

void TimeSyncSlavePortEnableCtl(u8 portNum, u8 contrlFlag);
u8 GetTimeSyncEnableState(u8 portNum);
void SetTimeSyncEnableFlag(u8 portNum, u8 timeSyncState);


#endif
