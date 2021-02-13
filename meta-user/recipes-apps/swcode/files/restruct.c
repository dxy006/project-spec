/*
 * restruct.c
 *
 *  Created on: 20201025
 *  Author: duxingyu
 */
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "sys_print.h"
#include "zk_communicate.h"
#include "dig_para.h"
#include "flash_apply.h"
#include "binding_para.h"
#include "restruct.h"

extern MstTempFlag g_ZKTempFlag;

static u8 g_restructData[RESTRUCT_DATA_SIZE] = {0};
static u8 g_restructDataFromFlash[RESTRUCT_DATA_SIZE] = {0};
static u8 g_restructBindParaData[RESTRUCT_BINDPARA_DATA_SIZE] = {0};
static u32 CheckTheRestructCrc(u32 ByteCount, u8 *dataBuf, u8 crcIn, u8 *crcOut)
{
    u32 i;
    for (i = 0; i < ByteCount; i++) {
        (*crcOut) ^= dataBuf[i];
    }

    if (crcIn != *crcOut) {
        SW_DBG(DBG_ERROR, "The crc is wrong!, crcIn = 0x%x, crcOut = 0x%x \n", crcIn, *crcOut);
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

static u32 ReCheckTheBootImag(INT8* deviceName,u8 dataCrc, u32 ByteCount)
{
    u8 crcOut = 0;
    (void)ReadDataFromFlash(deviceName, ByteCount, g_restructDataFromFlash);
    u32 ret = CheckTheRestructCrc(ByteCount, g_restructDataFromFlash, dataCrc, &crcOut);
    memset(g_restructDataFromFlash, 0, RESTRUCT_DATA_SIZE);
    return ret;
}

static u32 CheckThePackageFlag(u8 packageFlag)
{
    if ((packageFlag != RESTRUCT_DATA_FISRT_PACKAGE) && 
        (packageFlag != RESTRUCT_DATA_MIDDLE_PACKAGE) &&
        (packageFlag != RESTRUCT_DATA_LAST_PACKAGE) &&
        (packageFlag != RESTRUCT_DATA_SINGAL_PACKAGE)) {
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

static u32 CheckTheSpiData(u8 *dataBaseAddress, SpiDataInfoStruct *spiDataInfo)
{
    u32 ret;
    u8 crcTemp = CalculateCrc(spiDataInfo);
    u8 packageFlag = *(dataBaseAddress + 4);
    if (*(dataBaseAddress + 5) == RESTRUCT_SOFTWARE) {
        if ((packageFlag == RESTRUCT_DATA_FISRT_PACKAGE) ||
            (packageFlag == RESTRUCT_DATA_SINGAL_PACKAGE)) {
            g_ZKTempFlag.restructPkgNum = 0;
            g_ZKTempFlag.restructErrorNum = 0;
            g_ZKTempFlag.restructPkgRightNum = 0;
            g_ZKTempFlag.restructFirstErrorNum = 0;
            UpdateRestructCount(0, RESET_PKG_COUNT);
        }

        g_ZKTempFlag.restructPkgNum++;
        UpdateRestructCount(g_ZKTempFlag.restructPkgNum, TOTAL_PKG_COUNT);

        ret = CheckThePackageFlag(packageFlag);
        if ((crcTemp != spiDataInfo->crc) || (ret != 0)) {
            g_ZKTempFlag.restructErrorNum++;
            UpdateRestructCount(g_ZKTempFlag.restructErrorNum, WRONG_PKG_COUNT);
            if ((g_ZKTempFlag.restructErrorNum != 0) && (g_ZKTempFlag.restructFirstErrorNum == 0)) {
                g_ZKTempFlag.restructFirstErrorNum = g_ZKTempFlag.restructPkgNum;
            }
            SW_DBG(DBG_ERROR, "The crc or the package flag is wrong! package flag = %d, temp = 0x%x, crcIn = 0x%x \n",
                packageFlag, crcTemp, spiDataInfo->crc);
            return RET_FAILED;
        } else {
            g_ZKTempFlag.restructPkgRightNum++;
            return RET_SUCCESS;
        }
    } else if (*(dataBaseAddress + 5) == RESTRUCT_BINDING_PARA) {
        if ((packageFlag == RESTRUCT_DATA_FISRT_PACKAGE) || 
            (packageFlag == RESTRUCT_DATA_SINGAL_PACKAGE)) {
            g_ZKTempFlag.restructBindingParaTotalNum = 0;
            g_ZKTempFlag.restructFirstErrorNum = 0;
            UpdateRestructCount(0, RESET_PKG_COUNT);
        }
        g_ZKTempFlag.restructBindingParaTotalNum++;
        UpdateRestructCount(g_ZKTempFlag.restructBindingParaTotalNum, TOTAL_PKG_COUNT);
        ret = CheckThePackageFlag(packageFlag);
        if ((crcTemp != spiDataInfo->crc) || (ret != 0)) {
            g_ZKTempFlag.restructBindingParaErrorNum++;
            if ((g_ZKTempFlag.restructBindingParaErrorNum != 0) &&
                (g_ZKTempFlag.restructFirstErrorNum == 0)) {
                g_ZKTempFlag.restructFirstErrorNum = g_ZKTempFlag.restructBindingParaErrorNum;
            }
            
            UpdateRestructCount(g_ZKTempFlag.restructBindingParaErrorNum, WRONG_PKG_COUNT);
            SW_DBG(DBG_ERROR, "The crc is wrong! temp = 0x%x, crcIn = 0x%x \n", crcTemp, spiDataInfo->crc);
            return RET_FAILED; 
        }
    } else {
        SW_DBG(DBG_ERROR, "The restruct type is wong \n");
        return RET_FAILED; 
    }
    return RET_SUCCESS;
}


static void UpdateRestructDataToFlash(const u32 totalDataNum, const u8 crcOut)
{
    u32 ret;
    u32 flashWriteTime;
    
    for (flashWriteTime = 0; flashWriteTime < 3; ++flashWriteTime) {
        ret = ReWriteDataToFlash(RESTRUCT_DATA_FLASH_DEV, g_restructBindParaData, totalDataNum);
        sleep(UPDATE_FLASH_DELAY_TIME);

        if(ret != 0) {
            g_ZKTempFlag.restructState = RESTRUCT_STATE_FLSH_WRITE_FAILE;
            SetRestructState(RESTRUCT_STATE_FLSH_WRITE_FAILE);
            SW_DBG(DBG_WARN, "Rewrite flash failed! \n");
            continue;
        } else {
            ret = ReCheckTheBootImag(RESTRUCT_DATA_FLASH_DEV, crcOut, totalDataNum);
            if (ret != 0) {
                SW_DBG(DBG_WARN, "The data in flash is wrong! \n");
                continue;
            }

            g_ZKTempFlag.restructState = RESTRUCT_STATE_FLSH_WRITE_SUCESS;
            SetRestructState(RESTRUCT_STATE_FLSH_WRITE_SUCESS);
            SW_DBG(DBG_STEP, "R S!");
            break;
        }
    }
    return;
}


static void UpdateBindParaDataToFlash(const u32 totalDataNum, const u8 crcOut)
{
    u32 ret;
    u32 flashWriteTime;
    for (flashWriteTime = 0; flashWriteTime < 3; ++flashWriteTime) {
        ret = ReWriteDataToFlash(RESTRUCT_BINDING_PARA_FLASH_DEV, g_restructBindParaData, totalDataNum);
        if(ret != 0) {
            continue;
        } else {
            SW_DBG(DBG_STEP, "Write Binding para flash success! \n");
            ret = ReCheckTheBootImag(RESTRUCT_BINDING_PARA_FLASH_DEV, crcOut, totalDataNum);
            if (ret != 0) {
                SW_DBG(DBG_ERROR, "The Binding para data in flash is wrong! \n");
                SetRestructState(RESTRUCT_STATE_FLSH_WRITE_FAILE);
                continue;
            }
            SetRestructState(RESTRUCT_STATE_FLSH_WRITE_SUCESS);
            SW_DBG(DBG_STEP, "Restruct Binding Para data success! \n");
            break;
        }
    }
    return;
}

void DealRestructSoftWareData(u8 *dataBaseAddress)
{
    u32 ret;
    u8 crcIn = 0;
    u8 crcOut = 0;
    u32 dataNumTemp;
    u32 totalDataNum;
    static u32 totalPakageNum = 0;
    totalPakageNum++;

    u32 packageSerial = ((u32)(*dataBaseAddress) << 24) | ((u32)(*(dataBaseAddress + 1)) << 16)
        | (*(dataBaseAddress + 2) << 8) | (*(dataBaseAddress + 3));
    SW_DBG(DBG_DATA, "Pnum %d ", packageSerial);
        
    if((*(dataBaseAddress + 4) == RESTRUCT_DATA_LAST_PACKAGE) ||
        (*(dataBaseAddress + 4) == RESTRUCT_DATA_SINGAL_PACKAGE)) {
        g_ZKTempFlag.restructState = RESTRUCT_STATE_PKG_RECIVED_COMP;
        SetRestructState(RESTRUCT_STATE_PKG_RECIVED_COMP);
        dataNumTemp = ((*(dataBaseAddress + 6) << 8) | *(dataBaseAddress + 7)) - 1;
        crcIn = *(dataBaseAddress + 8 + dataNumTemp); //The last data
    } else if ((*(dataBaseAddress + 4) == RESTRUCT_DATA_FISRT_PACKAGE) || 
        (*(dataBaseAddress + 4) == RESTRUCT_DATA_MIDDLE_PACKAGE)) {
        g_ZKTempFlag.restructState = RESTRUCT_STATE_PKG_RECIVING;
        SetRestructState(RESTRUCT_STATE_PKG_RECIVING);
        dataNumTemp = *(dataBaseAddress + 6) << 8 | *(dataBaseAddress + 7);
    } else {
        SW_DBG(DBG_ERROR, "The package flag is wrong ! \n");
        g_ZKTempFlag.mstDataDiscardNum++;
    }
    
    if ((*(dataBaseAddress + 4) == RESTRUCT_DATA_FISRT_PACKAGE) || 
        (*(dataBaseAddress + 4) == RESTRUCT_DATA_SINGAL_PACKAGE)) {
        /* package num start from 1 */
        totalPakageNum = 1;
    }
    
    if (packageSerial * RESTRUCT_DATA_PACKAGE_SIZE >= RESTRUCT_DATA_SIZE) {
        SW_DBG(DBG_ERROR, "The package serial is wrong ! \n");
        return;
    }
    
    memcpy(&(g_restructData[packageSerial * RESTRUCT_DATA_PACKAGE_SIZE]), dataBaseAddress + 8, dataNumTemp);

    if ((*(dataBaseAddress + 4) == RESTRUCT_DATA_LAST_PACKAGE) || 
        (*(dataBaseAddress + 4) == RESTRUCT_DATA_SINGAL_PACKAGE)) {
        if (packageSerial != totalPakageNum - 1) {
            SW_DBG(DBG_ERROR, "The restruct package num is wrong! total num = %d \n", totalPakageNum);
            g_ZKTempFlag.restructState = RESTRUCT_STATE_PKG_RECIVED_NOTCOMP;
            SetRestructState(RESTRUCT_STATE_PKG_RECIVED_NOTCOMP);
            totalPakageNum = 0;
            return;
        }
        totalPakageNum = 0;

        totalDataNum = packageSerial * RESTRUCT_DATA_PACKAGE_SIZE + dataNumTemp;
        ret = CheckTheRestructCrc(totalDataNum, g_restructData, crcIn, &crcOut);
        if (ret != 0) {
            g_ZKTempFlag.restructState = RESTRUCT_STATE_PKG_RECIVED_NOTCOMP;
            SetRestructState(RESTRUCT_STATE_PKG_RECIVED_NOTCOMP);
            return;
        } else {
            g_ZKTempFlag.restructState = RESTRUCT_STATE_PKG_RECIVED_COMP;
            SetRestructState(RESTRUCT_STATE_PKG_RECIVED_COMP);
        }
        sleep(UPDATE_FLASH_DELAY_TIME);
        UpdateRestructDataToFlash(totalDataNum, crcOut);
        memset(g_restructData, 0, RESTRUCT_DATA_SIZE);
    }
    return;
}


void DealRestructBindingParaData(u8 *dataBaseAddress)
{
    u32 ret;
    u8 crcIn = 0;
    u8 crcOut = 0;
    u32 dataNumTemp;
    u32 totalDataNum;
    BootFlagStruct bootFlag = {0};
    static u32 bindParaTotalPakageNum = 0;
    bindParaTotalPakageNum++;

    u32 packageSerial = ((u32)(*dataBaseAddress) << 24) | ((u32)(*(dataBaseAddress + 1)) << 16)
        | (*(dataBaseAddress + 2) << 8) | (*(dataBaseAddress + 3));

    if ((*(dataBaseAddress + 4) == RESTRUCT_DATA_LAST_PACKAGE) ||
        (*(dataBaseAddress + 4) == RESTRUCT_DATA_SINGAL_PACKAGE)) {
        dataNumTemp = ((*(dataBaseAddress + 6) << 8) | *(dataBaseAddress + 7)) - 1;
    } else {
        dataNumTemp = *(dataBaseAddress + 6) << 8 | *(dataBaseAddress + 7);
    }

    if (*(dataBaseAddress + 4) == RESTRUCT_DATA_FISRT_PACKAGE ||
        (*(dataBaseAddress + 4) == RESTRUCT_DATA_SINGAL_PACKAGE)) {
        /* package num start from 1 */
        bindParaTotalPakageNum = 1;
        SetRestructState(RESTRUCT_STATE_PKG_RECIVING);
    }
    
    if (packageSerial * RESTRUCT_DATA_PACKAGE_SIZE >= RESTRUCT_BINDPARA_DATA_SIZE) {
        SW_DBG(DBG_ERROR, "The package serial is wrong ! \n");
        return;
    }

    memcpy(&(g_restructBindParaData[packageSerial * RESTRUCT_DATA_PACKAGE_SIZE]), dataBaseAddress + 8, dataNumTemp);

    if ((*(dataBaseAddress + 4) == RESTRUCT_DATA_LAST_PACKAGE) ||
        (*(dataBaseAddress + 4) == RESTRUCT_DATA_SINGAL_PACKAGE)) {
        if (packageSerial != bindParaTotalPakageNum - 1) {
            SW_DBG(DBG_ERROR, "The restruct package num is wrong! total num = %d \n", bindParaTotalPakageNum);
            SetRestructState(RESTRUCT_STATE_PKG_RECIVED_NOTCOMP);
            bindParaTotalPakageNum = 0;
            return;
        }
        bindParaTotalPakageNum = 0;
        crcIn = *(dataBaseAddress + 8 + dataNumTemp);
        totalDataNum = packageSerial * RESTRUCT_DATA_PACKAGE_SIZE + dataNumTemp;

        ret = CheckTheRestructCrc(totalDataNum, g_restructBindParaData, crcIn, &crcOut);
        if (ret != 0) {
            g_ZKTempFlag.restructBindingParaErrorNum++;
            SetRestructState(RESTRUCT_STATE_PKG_RECIVED_NOTCOMP);
            SW_DBG(DBG_ERROR, "The crc is wrong! crcIn = 0x%x \n", crcIn);           
            return;
        }

        SetRestructState(RESTRUCT_STATE_PKG_RECIVED_COMP);
        //sleep(UPDATE_FLASH_DELAY_TIME);
        UpdateBindParaDataToFlash(totalDataNum, crcOut);
        //sleep(UPDATE_FLASH_DELAY_TIME);
        /* If modify the binding para, the reserved data is covered */
        //GetBootFlagFromFlash(&bootFlag);
        //sleep(UPDATE_FLASH_DELAY_TIME);
        //bootFlag.useReservedData = false;
        //WriteBootFlagToFlash(&bootFlag);
        //sleep(UPDATE_FLASH_DELAY_TIME);
        /* clear the saved config */
        //EreaseBootCfgFromFlash();
        //sleep(UPDATE_FLASH_DELAY_TIME);
        memset(g_restructBindParaData, 0, RESTRUCT_BINDPARA_DATA_SIZE);
    }

    return;
}

void DealRestructData(SpiDataInfoStruct *spiDataInfo)
{
    u8 *dataBaseAddress = spiDataInfo->spiData;
    u32 ret = CheckTheSpiData(dataBaseAddress, spiDataInfo);
    if ((ret != 0) && (GetDigBindSWDestID() != MST_COMMUNICATE_DEFAULT_DEST_WORD)) {
        return;
    }

    SW_DBG(DBG_STEP, "Deal R S");
    if (*(dataBaseAddress + 5) == RESTRUCT_BINDING_PARA) {
        DealRestructBindingParaData(dataBaseAddress);
    } else if (*(dataBaseAddress + 5) == RESTRUCT_SOFTWARE) {
        DealRestructSoftWareData(dataBaseAddress);
    } else {
        SW_DBG(DBG_ERROR, "The restruct type is wrong! type = %d \n", *(dataBaseAddress + 5));  
    }
    return;
}
