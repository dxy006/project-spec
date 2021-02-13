/*
 * flash_apply.c
 *
 *  Created on: 2019Äê8ÔÂ8ÈÕ
 *      Author: ´÷¶û
 */
#include <stdio.h>
#include <mtd/mtd-user.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>

#include "common.h"
#include "sys_print.h"
#include "flash_apply.h"

INT32 EreaseFlash(int flashFd, u32 startaddr, u32 eraseSize)
{
    int ret;
    struct erase_info_user userInfo = {0};
    userInfo.start = startaddr;
    userInfo.length = eraseSize;
    ret = ioctl(flashFd, MEMERASE, &userInfo);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "Erease flash failed! ret = %d\r\n", ret);
    }
    return ret;
}


int EreaseBulkFlash(int flashFd, u32 startaddr, u32 bulkNum)
{
    int ret;
    u32 eraseSize = bulkNum * BULK_SIZE;
    struct erase_info_user userInfo = {0};
    userInfo.start = startaddr;
    userInfo.length = eraseSize;
    ret = ioctl(flashFd, MEMERASE, &userInfo);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "Erease flash failed! ret = %d\r\n", ret);
    }
    return ret;
}

INT32 ReadDataFromFlash(INT8 *deviceName, u32 inputLength, u8 *readBuffer)
{
    INT32 flashFd;
    flashFd = open (deviceName, O_RDWR | O_SYNC);

    if (flashFd < 0) {
        SW_DBG (DBG_ERROR, "open flash device falied! \n");
        return RET_FAILED;
    }
    lseek (flashFd, 0, SEEK_SET); //set the read point to the start of file
    u32 dataLength = read(flashFd, readBuffer, inputLength);
    close (flashFd);
    
    if (dataLength != inputLength) {
        SW_DBG (DBG_ERROR, "Read failed. dataLength = 0x%x, dataLength = 0x%x \n", dataLength, inputLength);
        return RET_FAILED;
    }
    return RET_SUCCESS;
}


u32 ReWriteDataToFlash(INT8 *deviceName, u8 *writeBuffer, u32 inputLength)
{
    INT32 flashFd;
    flashFd = open (deviceName, O_RDWR | O_SYNC);
    if (flashFd < 0) {
        SW_DBG (DBG_ERROR, "open flash device falied! \n");
        return RET_FAILED;
    }
    
    int ret = EreaseBulkFlash(flashFd, 0, (inputLength / BULK_SIZE) + 1);
    if (ret < 0) {
        SW_DBG (DBG_ERROR, "Erease flash failed! \n");
        return RET_FAILED;
    }
    
    lseek (flashFd, 0, SEEK_SET); //set the write point to the start of file
    u32 dataLength = write(flashFd, writeBuffer, inputLength);
    close (flashFd);
    
    if (dataLength != inputLength) {
        SW_DBG (DBG_ERROR, "write failed. dataLength = 0x%x, dataLength = 0x%x \n", dataLength, inputLength);
        return RET_FAILED;
    }
    return RET_SUCCESS;    
}

