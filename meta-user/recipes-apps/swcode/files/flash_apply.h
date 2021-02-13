#ifndef FLASH_APPLY_H
#define FLASH_APPLY_H
#include "common.h"
#define BULK_SIZE 0x80000 //512k

typedef struct {
    u8 bootTime;        // The boot time of software if the sofware can not start at the first time.
    u8 bootStateFlag;   // 
    u8 useReservedData; /* Check if use the reserved data or not. If reset the software,
                             The running software use the reserved data. If reboot, do not need use the reserved data. */
    u8 realBootMode;  /* Set the real boot mode */ 
    int resetTime;     // The reset time of the software. Init vale is 0. 
    u32 commandResetTime; // not really commandreset time, just use as a flag to distinguish command reset and abnormal reset
    u8 commandResetFlag; 
    u8 reserved[3];
} BootFlagStruct;

int EreaseBulkFlash(int flashFd, u32 startaddr, u32 bulkNum);
u32 ReWriteDataToFlash(INT8 *deviceName, u8 *writeBuffer, u32 inputLength);
INT32 ReadDataFromFlash(INT8 *deviceName, u32 inputLength, u8 *readBuffer);

#endif
