/*
 * dig_para.c
 *
 *  Created on: 2019ƒÍ9‘¬22»’
 *      Author: ¥˜∂˚
 */

#include <unistd.h>
#include <string.h>
#include "common.h"
#include "sys_print.h"
#include "dig_para.h"
#include "spi_drive.h"
#include "binding_para.h"
#include "zk_communicate.h"

//extern CascadePortInfo g_cascadePortInfo;
DigDataSendFormat g_digUpdateBuffer = {0};

//extern Fc_Port_Dcpr fpd[];
extern MstTempFlag g_ZKTempFlag;
u32 g_softWareVersion = 0;

/**************************************************************
* Function Name : FillPortLinkStatus
* Description : Fill the port link status for dig para
* Input :  digSendBuffer
* Output : none
* Attention : none
**************************************************************/
void FillPortLinkStatus(u8 portNum, u8 portStatus)
{
    if (portStatus == PORT_LINK_STATE_LINKED) {
        g_digUpdateBuffer.digData.portLinkStates.value |= (1 << portNum);
    } else {
        g_digUpdateBuffer.digData.portLinkStates.value &= (~(1 << portNum));
    }
    return;
}

void SetZkCommandCount(u8 commandCount, u8 commandType)
{
    if (commandType == RIGHT_COMMAND) {
        if (commandCount > 0x1F) {
            return;
        }
        g_digUpdateBuffer.digData.commandCount.BitValue.rightCommandNum = commandCount;
    } else {
        if (commandCount > 0x07) {
            return;
        }
        g_digUpdateBuffer.digData.commandCount.BitValue.wrongCommandNum = commandCount;
    }
    return;
}

void SetStartLocation(u8 type, u8 location)
{
    if (type == BINDPARA_SET) {
        g_digUpdateBuffer.digData.softwareStartFlag.BitValue.bindingParaNextStartLoc = location;
    } else if (type == SOFTAWARE_SET) {
        g_digUpdateBuffer.digData.softwareStartFlag.BitValue.psNextStartLoc = location;
    } else {
        SW_DBG(DBG_ERROR, "The start type is wrong. type = %d \n", type);
    }
    return;
}

void SetRestructState(u8 restructState)
{   
    g_digUpdateBuffer.digData.softwareStartFlag.BitValue.restructFlag = restructState & 0x07;
    return;
}

#if 0
u8 GetTimeSyncEnableState(u8 portNum)
{
    u8 enableStatus = ENABLE_FLAG;
    if ((g_ZKTempFlag.timeSyncEnableFlag & (1 << portNum)) == 0) {
        enableStatus = DISABLE_FLAG;
    }
    return enableStatus;  
}

u8 GetPortEnableState(u8 portNum, u8 contrlType)
{
    u8 enableStatus = ENABLE_FLAG;
    if (contrlType == FC_STATUE_MASK) {
        if ((g_ZKTempFlag.portFcEnableFlag & (1 << portNum)) == 0) {
            enableStatus = DISABLE_FLAG;
        }
    } else if (contrlType == IPFC_STATUE_MASK) {
        if ((g_digUpdateBuffer.digData.ipStates.value & (1 << portNum)) == 0) {
            enableStatus = DISABLE_FLAG;
        }
    } else {
        SW_DBG(DBG_ERROR, "The contrl type is wrong! \n");
    }
    return enableStatus;
}


static void SetPortIpState(u8 portNum, u8 ipState)
{
    if (ipState == ENABLE_FLAG) {
        if (portNum == ALL_PORT_FLAG) {
            g_digUpdateBuffer.digData.ipStates.value |= ALL_PORT_FLAG;
        } else {
            g_digUpdateBuffer.digData.ipStates.value |= (1 << portNum);
        }
    } else {
        if (portNum == ALL_PORT_FLAG) {
            g_digUpdateBuffer.digData.ipStates.value &= (~ALL_PORT_FLAG);
        } else {
            g_digUpdateBuffer.digData.ipStates.value &= (~(1 << portNum));
        }
    }
    SW_DBG(DBG_DBG_DATA, "ipstate = 0x%x \n", g_digUpdateBuffer.digData.ipStates.value);
    return;
}

static void SetPortFcState(u8 portNum, u8 fcState)
{
    if (fcState == ENABLE_FLAG) {
        /* The enable flag, set to 1 is enable */
        g_ZKTempFlag.portFcEnableFlag |= (1 << portNum);
    } else {
        /* The enable flag, set to 0 is disable */
        g_ZKTempFlag.portFcEnableFlag &= (~(1 << portNum));
    }
    return;
}

/**************************************************************************************
Function: SetPortEnableState
contrlMask: select the control type, fc or ipfc.  1 means fc, 2 means ipfc.
contrlFlag: Set the enable and disable statue. 0x0F enable, 0xF0 disable
***************************************************************************************/
u8 SetPortEnableState(u8 portNum, u8 contrlMask, u8 contrlFlag)
{
    if (contrlMask == FC_STATUE_MASK) {
        SetPortFcState(portNum, contrlFlag);
    } else if (contrlMask == IPFC_STATUE_MASK) {
        SetPortIpState(portNum, contrlFlag);
    } else {
        SW_DBG(DBG_ERROR, "The contrl type is wrong! \n");
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}

void SetTimeSyncEnableFlag(u8 portNum, u8 timeSyncState)
{
    if (timeSyncState == ENABLE_FLAG) {
        g_ZKTempFlag.timeSyncEnableFlag |= (1 << portNum);
    } else {
        g_ZKTempFlag.timeSyncEnableFlag &= (~(1 << portNum));
    }   
    return;
}
#endif

void UpdateRestructCount(u32 pkgCount, u8 pkgType)
{
    if (pkgType == WRONG_PKG_COUNT) {
        g_digUpdateBuffer.digData.interConnectState.BitValue.restructWrongPkgCount = (u8)(pkgCount & 0xF);
    } else if (pkgType == RESET_PKG_COUNT) {
        g_digUpdateBuffer.digData.interConnectState.BitValue.restructWrongPkgCount = 0;
    } else {
        ;
    }
    return;
}

void UpdateHeartBeat(void)
{
    g_digUpdateBuffer.digData.switchStatue.BitValue.heartBeat =
        0x01^g_digUpdateBuffer.digData.switchStatue.BitValue.heartBeat;
    return;
}

#if 0
void SetTimeSyncMasterStatus(DigDataFormat *digData)
{
    u32 timeSyncSecOffset;
    u32 msAndNsOffset;
    u32 msOffSet;

    timeSyncSecOffset = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_OFFSET_SECOND); 
    msAndNsOffset = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_OFFSET_MS_AND_NS); 

    msOffSet = ((msAndNsOffset >> 20) & 0xFFF);
    if ((timeSyncSecOffset == 0) && (msOffSet == 0) && (msAndNsOffset != 0)) {
        digData->switchStatue.BitValue.timeSyncMasterStatue = 1;
    } else {
        digData->switchStatue.BitValue.timeSyncMasterStatue = 0;
    }  
    return;
}


void FillTimeSyncStatusForEngPara(DigDataFormat *digData)
{
#ifdef XPAR_TIMESYNC1588V2_0_BASEADDR

    static u32 rxTsFrameCntP0 = 0;
    static u32 rxTsFrameCntP1 = 0;
    u32 rxTsFrameCntTempP0;
    u32 rxTsFrameCntTempP1;
    static u8 ppsDataCount = 0;
    u8 ppsDataCountTemp = 0;
    u32 ppsValidSecAndMilSec = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR, TIME_SYNC_REG_4);

    digData->switchStatue.BitValue.ppsValid = (ppsValidSecAndMilSec >> 16) & 0x01;
    SW_DBG(DBG_DBG_DATA, "pps valid = %d \n", digData->switchStatue.BitValue.ppsValid);
    digData->switchStatue.BitValue.ppsExist = (ppsValidSecAndMilSec >> 17) & 0x01;
    SW_DBG(DBG_DBG_DATA, "pps exist = %d \n", digData->switchStatue.BitValue.ppsExist);
    
    ppsDataCountTemp = (ppsValidSecAndMilSec & 0xFF000000) >> 24;
    if (ppsDataCountTemp == ppsDataCount) {
        digData->switchStatue.BitValue.locatorData = 0;
    } else {
        digData->switchStatue.BitValue.locatorData = 1;
    }
    
    SetTimeSyncMasterStatus(digData);

    #if 1
    rxTsFrameCntTempP0 = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_RX_TS_FRAME_CNT_P0);
    rxTsFrameCntTempP1 = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_RX_TS_FRAME_CNT_P1);
    
    if (rxTsFrameCntTempP0 != rxTsFrameCntP0) {
        digData->tsStatue.BitValue.tsStatuePort0 = 1;
    } else {
        digData->tsStatue.BitValue.tsStatuePort0 = 0;
    }
    
    if (rxTsFrameCntTempP1 != rxTsFrameCntP1) {
        digData->tsStatue.BitValue.tsStatuePort1 = 1;
    } else {
        digData->tsStatue.BitValue.tsStatuePort1 = 0;
    }
    
    rxTsFrameCntP0 = rxTsFrameCntTempP0;
    rxTsFrameCntP1 = rxTsFrameCntTempP1;
    #endif
    ppsDataCount = ppsDataCountTemp;
    SW_DBG(DBG_DBG_DATA, "locatorData  = %d \n", digData->switchStatue.BitValue.locatorData);
    ppsDataCount = ppsDataCountTemp;
#endif
    return;
}


void FillTimeSyncStatus(DigDataFormat *digData)
{
#ifdef XPAR_TIMESYNC1588V2_0_BASEADDR
    static u32 rxTsFrameCntP0 = 0;
    static u32 rxTsFrameCntP1 = 0;
    u32 rxTsFrameCntTempP0;
    u32 rxTsFrameCntTempP1;
    static u8 ppsDataCount = 0;
    u8 ppsDataCountTemp = 0;
    u32 ppsValidSecAndMilSec = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR, TIME_SYNC_REG_4);
    SW_DBG(DBG_DBG_DATA, "ppsValidSecAndMilSec = 0x%x \n", ppsValidSecAndMilSec);
    digData->switchStatue.BitValue.ppsValid = (ppsValidSecAndMilSec >> 16) & 0x01;
    SW_DBG(DBG_DBG_DATA, "pps valid = %d \n", digData->switchStatue.BitValue.ppsValid);
    digData->switchStatue.BitValue.ppsExist = (ppsValidSecAndMilSec >> 17) & 0x01;
    SW_DBG(DBG_DBG_DATA, "pps exist = %d \n", digData->switchStatue.BitValue.ppsExist);
    
    ppsDataCountTemp = ((ppsValidSecAndMilSec >> 24) & 0xFF);
    if (ppsDataCountTemp == ppsDataCount) {
        digData->switchStatue.BitValue.locatorData = 0;
    } else {
        digData->switchStatue.BitValue.locatorData = 1;
    }
    
    SetTimeSyncMasterStatus(digData);

#if 1
    rxTsFrameCntTempP0 = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_RX_TS_FRAME_CNT_P0);
    rxTsFrameCntTempP1 = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_RX_TS_FRAME_CNT_P1);

    if (rxTsFrameCntTempP0 != rxTsFrameCntP0) {
        digData->tsStatue.BitValue.tsStatuePort0 = 1;
    } else {
        digData->tsStatue.BitValue.tsStatuePort0 = 0;
    }
    
    if (rxTsFrameCntTempP1 != rxTsFrameCntP1) {
        digData->tsStatue.BitValue.tsStatuePort1 = 1;
    } else {
        digData->tsStatue.BitValue.tsStatuePort1 = 0;
    }
    
    rxTsFrameCntP0 = rxTsFrameCntTempP0;
    rxTsFrameCntP1 = rxTsFrameCntTempP1;
#endif
    ppsDataCount = ppsDataCountTemp;
    SW_DBG(DBG_DBG_DATA, "tsStatue = %d \n", digData->tsStatue.value);
    SW_DBG(DBG_DBG_DATA, "locatorData = %d \n", digData->switchStatue.BitValue.locatorData);

#endif
    return;
}

void FillUtcTimeFromLocData(DigDataFormat *digData)
{
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u8 month;
    u8 year;
    u32 ppsValidSecAndMilSec;
    u32 yearToMin = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR, TIME_SYNC_REG_3);
    #if 0
    year = yearToMin >> 20;
    SW_DBG(DBG_DBG_DATA, "year = %d \n", year);

    
    month = (yearToMin >> 16) & 0xF;
    SW_DBG(DBG_DBG_DATA, "month = %d \n", month);
    #endif
    day = (yearToMin >> 11) & 0x1F;
    
    digData->utcTime.BitValue.utcTimeOfDay = day;
    SW_DBG(DBG_DBG_DATA, "day = %d \n", day);
    
    hour = (yearToMin >> 6) & 0x1F; //5bit
    digData->utcTime.BitValue.utcTimeOfHour = hour;
    SW_DBG(DBG_DBG_DATA, "hour = %d \n", hour);
    
    minute = yearToMin & 0x3F; //6bit
    digData->utcTime.BitValue.utcTimeOfMinute = minute;
    SW_DBG(DBG_DBG_DATA, "minute = %d \n", minute);
        
    ppsValidSecAndMilSec = fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR, TIME_SYNC_REG_4);
    second = (ppsValidSecAndMilSec >> 10) & 0x3F; //6bit
    digData->utcTime.BitValue.utcTimeOfSecond = second;
    SW_DBG(DBG_DBG_DATA, "second = %d \n", second);
    return;
}

void GetRealCascadePortNum(u8 *outCascadePort1, u8 *outCascadePort2)
{
    if (g_cascadePortInfo.remoteCascadePort1 != INVALID_CASCADE_PORT) {
        *outCascadePort1 = g_cascadePortInfo.remoteCascadePort1;
    } else {
        *outCascadePort1 = g_cascadePortInfo.localCascadePort1;
    }
    
    if (g_cascadePortInfo.remoteCascadePort2 != INVALID_CASCADE_PORT) {
        *outCascadePort2 = g_cascadePortInfo.remoteCascadePort2;
    } else {
        *outCascadePort2 = g_cascadePortInfo.localCascadePort2;
    }  
    return;
}

void FillCascadePortStatue(DigDataFormat *digData)
{
    u8 cascadePort1 = 0;
    u8 cascadePort2 = 0;
  
    GetRealCascadePortNum(&cascadePort1, &cascadePort2);
    if (cascadePort1 < FC_PORT_NUM_INSTANCES) {
        digData->interConnectState.BitValue.cascadePort1FcEnableStatus =
            (g_ZKTempFlag.portFcEnableFlag >> cascadePort1) & 0x01;
        digData->interConnectState.BitValue.cascadePort1IpFcEnableStatus = 
            (g_digUpdateBuffer.digData.ipStates.value >> cascadePort1) & 0x01;
    } else {
        SW_DBG(DBG_ERROR, "cascadePort1 is wrong = %d \n", cascadePort1);
        return;
    }
    
    if (cascadePort2 < FC_PORT_NUM_INSTANCES) {
        digData->interConnectState.BitValue.cascadePort2FcEnableStatus = 
            (g_ZKTempFlag.portFcEnableFlag >> cascadePort2) & 0x01;
        digData->interConnectState.BitValue.cascadePort2IpFcEnableStatus = 
            (g_digUpdateBuffer.digData.ipStates.value >> cascadePort2) & 0x01;
    } else {
        SW_DBG(DBG_ERROR, "cascadePort2 is wrong = %d \n", cascadePort2);
        return;
    }
    return;
}
#endif

/**************************************************************
* Function Name : GetAndFillDigData
* Description : Add the next package of data to the record
*               Header is not included, but crc is.
* Input :  digSendBuffer
* Output : none
* Attention : none
**************************************************************/
static void GetAndFillDigData(DigDataSendFormat *digSendBuffer)
{
    u8 i;
    u8 sum = 0;
    u32 crcDataSize = DIG_DATA_SEND_DATA_SIZE - 1;
    memcpy(digSendBuffer, &g_digUpdateBuffer, sizeof(DigDataSendFormat));
    //FillTimeSyncStatus(&(digSendBuffer->digData));
    //FillUtcTimeFromLocData(&(digSendBuffer->digData));
    //FillCascadePortStatue(&(digSendBuffer->digData));
    for (i = 0; i < crcDataSize; ++i) {
        sum = sum ^ (*((u8*)digSendBuffer + i)); 
    }

    digSendBuffer->checksum = sum;
    SW_DBG(DBG_STEP, "D \n");
}

u8 g_digTest = 1;
void TestFillDataToSpi(u32 *dataForSendFormat)
{
    u8 index = 0;  
    if (g_digTest != 0) {
        dataForSendFormat[index++] = 0x98b19983;
        dataForSendFormat[index++] = 0x000E0083;
        dataForSendFormat[index++] = 0x01020383;
        dataForSendFormat[index++] = 0x04050683;
        dataForSendFormat[index++] = 0x07080983;
        dataForSendFormat[index++] = 0x0A0B0C83;
        dataForSendFormat[index++] = 0x0D670082;
    }
    return;
}

static void FillSpiDataByPara(DigDataSendFormat *digSendBuffer)
{
    u8 i;
    u8 index = 0;
    u32 dataForSendFormat[TOTAL_DATA_SEND_NUM] = {0};

    dataForSendFormat[index++] = (u32)(digSendBuffer->syncWord) << 24 | (u32)(digSendBuffer->deviceId) << 16  
        | (u32)(digSendBuffer->cmdWord)  << 8 | 0x83;
    
    dataForSendFormat[index++] = (u32)(digSendBuffer->length[0]) << 24 | (u32)(digSendBuffer->length[1]) << 16  
        | (u32)(digSendBuffer->digData.switchStatue.value) << 8 | 0x83;
    
    dataForSendFormat[index++] = (u32)(digSendBuffer->digData.commandCount.value) << 24 
        | (u32)(digSendBuffer->digData.versionTbl.value) << 16
        | (u32)(digSendBuffer->digData.softwareStartFlag.value) << 8 | 0x83;
    
    dataForSendFormat[index++] = (u32)(digSendBuffer->digData.portLinkStates.value) << 16 |
        (u32)(digSendBuffer->digData.ipStates.value & 0xFF00) | 0x83;
    
    dataForSendFormat[index++] = (u32)((digSendBuffer->digData.ipStates.value) & 0xFF) << 24 |
        (u32)(digSendBuffer->digData.tsStatue.value) << 8 | 0x83;
    
    dataForSendFormat[index++] = (u32)(digSendBuffer->digData.interConnectState.value) << 24 |
        (u32)(digSendBuffer->digData.utcTime.value) & 0xFFFF00 | 0x83;

    dataForSendFormat[index++] = (u32)(digSendBuffer->digData.utcTime.value & 0xFF) << 24 
        | (u32)(digSendBuffer->checksum) << 16 | 0x82;
    TestFillDataToSpi(dataForSendFormat);
    u32 retValue = WriteDataToSpi(dataForSendFormat, index * sizeof (u32));
    if (retValue == RET_FAILED) {
        SW_DBG(DBG_ERROR, "Write spi data failed! \n");
    }
    SW_DBG(DBG_DBG_DATA, "Sum: 0x%x \n", digSendBuffer->checksum);
    return;
}

static void SendDigParaBySpi(DigDataSendFormat *digSendBuffer)
{
    FillSpiDataByPara(digSendBuffer);
    return;
}

/**************************************************************
* Function Name : InitDigPara
* Description : digital paramenter init, 
*               add the header and tailor and
*               add the first package of data to the record
* Input :     digSendBuffer
* Output :    none
* Attention : none
**************************************************************/
void InitDigParaAndClearRXFifo(void)
{
    DigDataSendFormat *digSendBuffer = &g_digUpdateBuffer;
    if (digSendBuffer == 0) {
        SW_DBG(DBG_ERROR, "The send buffer is null! \n");
        return;
    }
    
    u32 logicVersion = 0;//GetLogicVersion();
    (void)ResetSpiRxBuffer();

    digSendBuffer->syncWord = MST_COMMUNICATE_SYNC_WORD;
    digSendBuffer->deviceId = MST_COMMUNICATE_DEST_MASTER;
    digSendBuffer->cmdWord = MST_COMMUNICATE_DIG_PARA;
    digSendBuffer->length[0] = 0;
    digSendBuffer->length[1] = DIG_PARA_LENGTH;
    digSendBuffer->digData.versionTbl.BitValue.psVersion = g_softWareVersion & 0x0F;
    digSendBuffer->digData.versionTbl.BitValue.plVersion = logicVersion & 0x0F;
    //digSendBuffer->digData.softwareStartFlag.BitValue.psCurrentStartLoc = g_ZKTempFlag.bootStateInfo.softwareState;
    if (logicVersion != LOCGIC_SOLIDIFY_VERSION) {
        digSendBuffer->digData.softwareStartFlag.BitValue.plCurrentStartLoc = 1;
    } else {
        digSendBuffer->digData.softwareStartFlag.BitValue.plCurrentStartLoc = 0;
    }
    
    //digSendBuffer->digData.softwareStartFlag.BitValue.bindingParaFlag = g_ZKTempFlag.bootStateInfo.bindingParaState;
    GetAndFillDigData(digSendBuffer);
    return;
}

/**************************************************************
* Function Name : DealTheDigParaData
* Description : 1„ÄÅClear the rx fifo 2„ÄÅwrite the fifo 
*               3„ÄÅFill next package of data 
* Input :  g_digSendBuffer
* Output : none
* Attention : none
**************************************************************/
void DealTheDigParaData(void)
{
    u32 ret;
    u8 dataLenthL = 0;
    u8 dataLenthH = 0;
    u8 crcTemp = 0;
    DigDataSendFormat digSendBuffer = {0};
    u8 crcReal;
    u8 digBindSWDestID = GetDigBindSWDestID();
    GetAndFillDigData(&digSendBuffer);
    SendDigParaBySpi(&digSendBuffer);
    
    ret = ReadDataFromSpi(&dataLenthH);
    CHECK_RETURN_VALUE(ret);
    ret = ReadDataFromSpi(&dataLenthL);
    CHECK_RETURN_VALUE(ret);
    ret = ReadDataFromSpi(&crcTemp);
    CHECK_RETURN_VALUE(ret);
    crcReal = MST_COMMUNICATE_SYNC_WORD ^ digBindSWDestID ^ MST_COMMUNICATE_DIG_PARA ^ 0 ^ 0;
    
    if ((dataLenthL != 0) || (dataLenthH != 0) 
        || (crcTemp != crcReal)) {
        
        g_ZKTempFlag.digCollectErrorNum++;
        SW_DBG(DBG_ERROR, "The command is wrong! dataLenth = %d, crc = 0x%x, crcreal = 0x%x \n",
            (u16)((u16)dataLenthH << 8 | dataLenthL), crcTemp, crcReal);
    }
    UpdateHeartBeat();
    return;
}
