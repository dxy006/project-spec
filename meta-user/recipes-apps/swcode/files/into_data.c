/*
 *into_data.c
 *
 *  Created on: 2019Äê8ÔÂ8ÈÕ
 *      Author: ´÷¶û
 */
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "into_data.h"
#include "sys_print.h"
#include "binding_para.h"
#include "fc_port_mon.h"
#include "fc_port_ppm.h"
#include "time_sync.h"
#include "dig_para.h"
#include "restruct.h"
#include "eng_para.h"

extern RouterTbl g_routerTbl[];
extern MstTempFlag g_ZKTempFlag;
extern LocalAddrTbl g_localAddressTbl[];
extern const u8 g_reserveDataMapTbl[];

BootFlagInfo g_bootInfo = {0};


static const SwitchLocMap switchLocMap[TOTAL_SW_LOCATION_NUM] = {
    { HOST_01, SW_01 }, // MICU01 Switch01
    { HOST_01, SW_03 }, // reserved
    { HOST_02, SW_01 }, // MICU02 Switch01
    { HOST_02, SW_03 }, // reserved
    
    { HOST_01, SW_01 }, // MICU03 Switch01
    { HOST_01, SW_03 }, // MICU03 Switch02
    { HOST_02, SW_01 }, // MICU04 Switch01
    { HOST_02, SW_03 }, // MICU04 Switch02
    
    { HOST_01, SW_01 }, // MICU05 Switch01
    { HOST_01, SW_03 }, // MICU05 Switch02
    { HOST_02, SW_01 }, // MICU06 Switch01
    { HOST_02, SW_03 }, // MICU06 Switch02
};

static const u8 switchNumMap[TOTAL_SW_LOCATION_NUM] = {
    SW1_SOFTWARE_REBOOT_FLAG, SW2_SOFTWARE_REBOOT_FLAG, // MICU01
    SW1_SOFTWARE_REBOOT_FLAG, SW2_SOFTWARE_REBOOT_FLAG, // MICU02
    SW1_SOFTWARE_REBOOT_FLAG, SW2_SOFTWARE_REBOOT_FLAG, // MICU03
    SW1_SOFTWARE_REBOOT_FLAG, SW2_SOFTWARE_REBOOT_FLAG, // MICU04
    SW1_SOFTWARE_REBOOT_FLAG, SW2_SOFTWARE_REBOOT_FLAG, // MICU05
    SW1_SOFTWARE_REBOOT_FLAG, SW2_SOFTWARE_REBOOT_FLAG, // MICU06
};
    
/* capsule communicate Loc map */
static const CapsuleLocationMap capsuleLocMap[TOTAL_CAPSULE_COMMU_LOC] = {
    {SW_LOCATION_SYCI,  SW_LOCATION_SYCII}, //hxc Map sycI & sycII
    {SW_LOCATION_HXC,  SW_LOCATION_SYCII}, //sycI Map hxc & sycII
    {SW_LOCATION_HXC,  SW_LOCATION_SYCI}, //sycII Map hxc & sycI
};

static u8 LocationCheckForIntoData(u8 *spiData)
{
    u8 informationHost = spiData[1];
    u8 switchNum = spiData[2];
    u32 swLocation = GetSwLocation();
    if (swLocation == TOTAL_SW_LOCATION_NUM) {
        /* Binding para get failed, to ensure that we can communicate with mst, we donot do the check*/
        return RET_SUCCESS;
    }

    if (swLocation > TOTAL_SW_LOCATION_NUM) {
        return RET_FAILED;
    }
    if ((informationHost != switchLocMap[swLocation].hostID) ||
        (switchNum != switchLocMap[swLocation].switchNum)) {
        SW_DBG(DBG_ERROR, "The host or location is wrong  Host = %d, Locaton = %d\n",
            informationHost, switchNum);
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

static u8 DataCheckForPortEnableCtl(u8 *spiData, u8 *enableCtlFlag, u8 *portNum)
{
    /* parameter Check */
    *portNum = spiData[3];
    *enableCtlFlag = spiData[4];
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }    

    if ((*portNum != ALL_PORT_FLAG) && (*portNum >= FC_PORT_NUM_INSTANCES)) {
        SW_DBG(DBG_ERROR, "The portnum is wrong!\n");        
        return RET_FAILED;
    }
/*
    ret = ValidPortNumCheckForIntoData(*portNum);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "The port is not valid !"); 
        return RET_FAILED;
    }       
*/
    /* Deal the data */
    if ((*enableCtlFlag != ENABLE_FLAG) && (*enableCtlFlag != DISABLE_FLAG)) {
        SW_DBG(DBG_ERROR, "The enableCtlFlag is wrong!\n"); 
        return RET_FAILED;
    } 
    
    return RET_SUCCESS;
}

static u8 DealFcEnableData(u8 *spiData)
{
    u8 enableControl = 0;
    u8 portNum = 0;
    u8 ret = DataCheckForPortEnableCtl(spiData, &enableControl, &portNum);
    if ((ret != RET_SUCCESS) || (portNum == ALL_PORT_FLAG)) {
        return RET_FAILED;
    }

    ret = PortEnableControl(portNum, FC_STATUE_MASK, enableControl);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }

    if (enableControl == ENABLE_FLAG) {
        SW_DBG(DBG_STEP, "[Info] Fc Enable. \n");        
    } else {
        SW_DBG(DBG_STEP, "[Info] Fc Disable. \n"); 
    }
    
    //g_bootSavedCfg.fcEnableModifyFlag = CFG_MODYFIED;
    //g_bootSavedCfg.fcEnableFlag = g_ZKTempFlag.portFcEnableFlag;
    
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    return RET_SUCCESS;
}

static u8 DealTimeSyncEnable(u8 *spiData)
{
    u8 enableControl = 0;
    u8 portNum = 0;
    u8 ret = DataCheckForPortEnableCtl(spiData, &enableControl, &portNum);
    if ((ret != RET_SUCCESS) || (portNum == ALL_PORT_FLAG)) {
        return RET_FAILED;
    }
    TimeSyncSlavePortEnableCtl(portNum, enableControl);
    SetTimeSyncEnableFlag(portNum, enableControl);
    if (enableControl == ENABLE_FLAG) { 
        SW_DBG(DBG_STEP, "[Info] Time sync Enable.  \n"); 
    } else {
        SW_DBG(DBG_STEP, "[Info] Time sync Disable.  \n"); 
    }
    //g_bootSavedCfg.timeSyncEnable = g_ZKTempFlag.timeSyncEnableFlag;
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    return RET_SUCCESS;
}

static u8 DealPortIpEnable(u8 *spiData)
{
    u8 portIndex;
    u8 enableControl = 0;
    u8 portNum = 0;
    u8 ret = DataCheckForPortEnableCtl(spiData, &enableControl, &portNum);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }

    if (portNum == ALL_PORT_FLAG) {
        for (portIndex = 0; portIndex < FC_PORT_NUM_INSTANCES; ++portIndex) {
            //JUMP_OVER_UNUSED_PORT(portIndex);
            ret = PortEnableControl(portIndex, IPFC_STATUE_MASK, enableControl);
            if (ret != RET_SUCCESS) {
                return RET_FAILED;
            }
        }
    } else {
        ret = PortEnableControl(portNum, IPFC_STATUE_MASK, enableControl);
        if (ret != RET_SUCCESS) {
            return RET_FAILED;
        }
    }
    
    if (enableControl == ENABLE_FLAG) {
        SW_DBG(DBG_STEP, "[Info] Port IP Enable.  \n");
    } else {
        SW_DBG(DBG_STEP, "[Info] Port IP Disable.  \n");
    }
    //g_bootSavedCfg.ipEnableModifyFlag = CFG_MODYFIED;
    //g_bootSavedCfg.ipEnable = g_digUpdateBuffer.digData.ipStates.value;
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    return RET_SUCCESS;
}

static u8 DealPortResetCmd(u8 *spiData)
{
    /* parameter Check */
    u8 portIndex;
    u8 portNum = spiData[3];
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }  
    /*
    ret = ValidPortNumCheckForIntoData(portNum);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "The port is unvalid port.");
        return RET_FAILED;
    }  
    */
    if ((portNum >= FC_PORT_NUM_INSTANCES) && (portNum != ALL_PORT_FLAG)) {
        SW_DBG(DBG_ERROR, "The portnum is wrong! \n");
        return RET_FAILED;
    }

    if (portNum == ALL_PORT_FLAG) {
        for (portIndex = 0; portIndex < FC_PORT_NUM_INSTANCES; ++portIndex) { 
            /* reset the port */
            //JUMP_OVER_UNUSED_PORT(portIndex);
            PortReset(portIndex);
        }
    } else {
        PortReset(portNum);
    }
    /* reset the port */
    SW_DBG(DBG_STEP, "Deal the port reset function! \n");
    return RET_SUCCESS;
}

static u8 DeletePortForIntoData(u8 portNum, u32 portDid) 
{
    u32 indexNum, location;
    u32 validPortNum;
    u32 delLoc = INVALIDE_LOCATION;

    validPortNum = g_validPortNum;
    
    for (indexNum = 0; indexNum < g_validPortNum; ++indexNum) {
        if ((GetPortNumByIndex(indexNum) == portNum) && (portDid == GetDIDByIndex(indexNum))) {
            DeletePortFromPortTbl(GetDIDByIndex(indexNum));
            g_portTbl[indexNum].portIsValid = PORT_NOT_VALID;
            validPortNum--; 
            delLoc = indexNum; // Get the delete location
            break;
        }
    }
    
    if (delLoc == INVALIDE_LOCATION) {
        SW_DBG(DBG_ERROR, "The port num or port did is wrong ! \n");
        return RET_FAILED;
    }
    
    for (indexNum = delLoc; indexNum < g_validPortNum; indexNum++) {
        if (g_portTbl[indexNum].portIsValid == PORT_NOT_VALID) {
            for (location = indexNum + 1; location < g_validPortNum; ++location) {
                if (g_portTbl[location].portIsValid == PORT_IS_VALID) {
                    memcpy(&(g_portTbl[indexNum]), &(g_portTbl[location]), sizeof(PortTbl));
                    g_portTbl[indexNum].portIsValid = PORT_IS_VALID;
                    g_portTbl[location].portIsValid = PORT_NOT_VALID;                    
                    break;
                }
            }
        }
    }
    g_portTbl[validPortNum].portNum = INVALID_PORT_NUM;
    g_portTbl[validPortNum].portDid = 0;
    g_validPortNum = validPortNum;
    return RET_SUCCESS;
}

bool IsCascadePort(u8 portNum)
{
    CascadePortInfo cascadePortInfo = {0};
    GetCascadePortSetting(&cascadePortInfo);
    if ((portNum == cascadePortInfo.localCascadePort1) ||
        (portNum == cascadePortInfo.localCascadePort2) || 
        (portNum == cascadePortInfo.remoteCascadePort1) ||
        (portNum == cascadePortInfo.remoteCascadePort2)) {
        return true;
    }
    return false;
}

static void ModifyFcIdCfg(u8 portNum, u32 fcID)
{
    u32 index;
    for (index = 0; index < g_validPortNum; index++) {
        if (g_portTbl[index].portNum == portNum) {
            break;
        }
    }
    
    /* Insert a new port */
    if (index == g_validPortNum) {
        ++g_validPortNum;
    }
    
    if (GetPortStatus(portNum) == PORT_READY) {
        /* delete the did use the old did value */
        DelDidTblFromPortTbl(portNum);
        /* updata the did value to new value and add the did to table */
        /* If the did already exist, modify the value. If not, Add new data */
        g_portTbl[index].portDid = fcID;
        g_portTbl[index].portNum = portNum;
        g_portTbl[index].portIsValid = PORT_IS_VALID;
        AddDidTblToPortTbl(portNum);
    } else {
        /* If the port is not ready then there is no did in logic table,
            Modify the did vale directly */
        g_portTbl[index].portNum = portNum;
        g_portTbl[index].portDid = fcID;
        g_portTbl[index].portIsValid = PORT_IS_VALID;
    }
    return;
}


static u8 AddPortFcIdCfg(u8 portNum, u32 fcID)
{
    u16 portNumInTbl = 0;
    if (IsPortDidExistInSoftWareTbl(fcID, &portNumInTbl)) {
        SW_DBG(DBG_ERROR, "[ERROR] The id already exist! ");
        return RET_FAILED;            
    }
    
    if (g_validPortNum >= MAX_PORT_TABLE_NUM) {
        SW_DBG(DBG_ERROR, "Reach the max num of port table !");
        return RET_FAILED;
    }
    
    if (IsCascadePort(portNum)) {
        g_portTbl[g_validPortNum].portIsValid = PORT_IS_VALID;
        g_portTbl[g_validPortNum].portDid = fcID;
        g_portTbl[g_validPortNum].portNum = portNum;   
        ++g_validPortNum;
        if (GetPortStatus(portNum) == PORT_READY) {
            AddDidTblToPortTbl(portNum);
        }
        
        SW_DBG(DBG_DATA, "Add id, portNum = %d, fcID = 0x%x ", portNum, fcID);
    } else {
        ModifyFcIdCfg(portNum, fcID);
    }

    return RET_SUCCESS;
}

static u8 DeletePortFcIdCfg(u8 portNum, u32 fcID)
{
    u8 ret;
    SW_DBG(DBG_DATA, "Delete id, portNum = %d, portID = 0x%x ", portNum, fcID);
    ret = DeletePortForIntoData(portNum, fcID);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }

    return RET_SUCCESS;
}

static u8 DealFcIdCfg(u8 *spiData)
{
    u8 portNum;
    u8 portOperation;
    u32 fcID;
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }
    
    portOperation = spiData[3];
    portNum = spiData[4];
    fcID = ((u32)spiData[5] << 16) | ((u32)spiData[6] << 8) | (spiData[7]);
    if (portNum >= FC_PORT_NUM_INSTANCES) {
        SW_DBG(DBG_ERROR, "The Port num is wrong!");
        return RET_FAILED;
    }
    /*
    ret = ValidPortNumCheckForIntoData(portNum);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "The port is unvalid port.");
        return RET_FAILED;
    }
    */
    if (portOperation == ADD_PORT_CFG) {
        ret = AddPortFcIdCfg(portNum, fcID);
        if (ret != RET_SUCCESS) {
            return RET_FAILED;
        }
    } else if (portOperation == DELETE_PORT_CFG) {
        ret = DeletePortFcIdCfg(portNum, fcID);
        if (ret != RET_SUCCESS) {
            return RET_FAILED;
        }
    } else {
        return RET_FAILED;
    }

    //g_bootSavedCfg.portTblSavedCfg.validPortNum = g_validPortNum;
    //g_bootSavedCfg.portTblSavedCfg.portTblModifyFlag = CFG_MODYFIED;
    //memcpy(g_bootSavedCfg.portTblSavedCfg.portTbl, g_portTbl,
    //    MAX_PORT_TABLE_NUM * sizeof(PortTbl));
    //WriteBootCfgToFlash(&g_bootSavedCfg);    
    return RET_SUCCESS;
}

static bool CheckMacIpIsConsistent(LocalAddrTbl *inAddrTbl,
    LocalAddrTbl *existAddrTbl)
{
    u32 i;
    for (i = 0; i < 4; i++) {
        if (inAddrTbl->ipAddress[i] != existAddrTbl->ipAddress[i]) {
            SW_DBG(DBG_ERROR, "The address is not fit!");
            return false;
        }
    }
    
    for (i = 0; i < 6; i++) {
        if (inAddrTbl->ipAddress[i] != existAddrTbl->ipAddress[i]) {
            SW_DBG(DBG_ERROR, "The mac address is not fit!");
            return false;
        }
        
        if (inAddrTbl->macAddress[i] != existAddrTbl->macAddress[i]) {
            SW_DBG(DBG_ERROR, "The mac address is not fit!");
            return false;
        }
    }

    for (i = 0; i < 3; i++) {
        if (inAddrTbl->localSid[i] != existAddrTbl->localSid[i]) {
            SW_DBG(DBG_ERROR, "The sid address is not fit!");
            return false;     
        }
    }

    return true;
}

static u8 DealMacIpCfg(u8 *spiData)
{
    u8 operationCode = spiData[3];
    u8 portNum = spiData[4];
    LocalAddrTbl localAddress = {0};
    localAddress.portNum = portNum;
    memcpy(localAddress.macAddress, &(spiData[5]), 6);
    memcpy(localAddress.ipAddress, &(spiData[11]), 4);
    memcpy(localAddress.localSid, &(spiData[15]), 3);
    
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != 0) {
        return RET_FAILED;
    }
    
    if (portNum >= FC_PORT_NUM_INSTANCES) {
        SW_DBG(DBG_ERROR, "The portnum is wrong. portNum = %d ", portNum);
        return RET_FAILED;
    }
    /*
    ret = ValidPortNumCheckForIntoData(portNum);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "The port is unvalid port.");
        return RET_FAILED;
    }   
    */
    if (operationCode == ADD_PORT_CFG) {
        //ModifyLocalAddr(&localAddress);
        SW_DBG(DBG_STEP, "Modify Mac ip config.");
    } else if (operationCode == DELETE_PORT_CFG) {
        
        ret = CheckMacIpIsConsistent(&localAddress,
            &(g_localAddressTbl[localAddress.portNum]));
        if (ret == false) {
            return RET_FAILED;
        }
        memset(localAddress.macAddress, 0, 6);
        memset(localAddress.ipAddress, 0, 4);
        memset(localAddress.localSid, 0, 3);
        //ModifyLocalAddr(&localAddress);
        SW_DBG(DBG_STEP, "Delete mac ip config");
    } else {
        SW_DBG(DBG_ERROR, "The operation code is wrong. operationCode = %d", operationCode);
        return RET_FAILED;
    }

    //xmk_enter_kernel();
    //g_bootSavedCfg.macIpSavedCfg.macIpModifyFlag = CFG_MODYFIED;
    //memcpy(g_bootSavedCfg.macIpSavedCfg.localAddrTbl, &g_localAddressTbl,
    //    FC_PORT_NUM_INSTANCES * sizeof(LocalAddrTbl));
    //xmk_leave_kernel();
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    /* Not complate */
    return 0;
}


static u8 DealPortIpBandWidth(u8* spiData)
{
    u8 portNum = spiData[3];
    u8 bandWidth = spiData[4];
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }
    
    if (portNum >= FC_PORT_NUM_INSTANCES) {
        SW_DBG(DBG_ERROR, "The portnum is wrong. portNum = %d ", portNum);
        return RET_FAILED;
    }
    /*
    ret = ValidPortNumCheckForIntoData(portNum);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "The port is unvalid port.");
        return RET_FAILED;
    }
    */
    if (bandWidth > MAX_BAND_WIDTH) {
        SW_DBG(DBG_ERROR, "The bandwidth is wrong. bandWidth = %d%% ", bandWidth*5);
        return RET_FAILED;
    }
    //SetIpBandWidth(portNum, bandWidth);
    
    //xmk_enter_kernel();
    //g_ZKTempFlag.ipBandWidth[portNum] = bandWidth;
    //g_bootSavedCfg.ipBandWidthSavedCfg.ipBandWidthModifyFlag = CFG_MODYFIED;
    //memcpy(g_bootSavedCfg.ipBandWidthSavedCfg.ipBandWidthTbl, &(g_ZKTempFlag.ipBandWidth),
    //    FC_PORT_NUM_INSTANCES);
    //xmk_leave_kernel();
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    SW_DBG(DBG_STEP, "Set the bandwidth to %d%", 5 * bandWidth);
    return RET_SUCCESS;
}

static u8 DealTimeSyncPeriod(u8 *spiData)
{
    u32 index;
    u16 timeSyncPeriod = (((u16)spiData[3]) << 8 | spiData[4]);
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }

    if ((timeSyncPeriod < TIME_SYNC_PERIOD_1MS) ||
        (timeSyncPeriod > TIME_SYNC_PERIOD_1000MS)) {
        SW_DBG(DBG_ERROR, "The Time sync perid is wrong! timeSyncPeriod= %d", timeSyncPeriod);
        return RET_FAILED;
    }
    
    //SetTimeSyncPerid(timeSyncPeriod);
    SW_DBG(DBG_STEP, "Set the timeSyncPeriod to %d", timeSyncPeriod);

    //xmk_enter_kernel();
    //g_ZKTempFlag.timeSyncPeriod = timeSyncPeriod;
    //g_bootSavedCfg.timeSyncPerid = timeSyncPeriod;
    //xmk_leave_kernel();
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    return RET_SUCCESS;
}

void InsertRouterItem(u8 portNum, u32 destIP, u32 nextIP)
{
    u32 readData;
    u32 writeData;
    #if 0
    writeData = (destIP & 0xFFFF) << 16;
    writeData |= portNum;
    do {
        readData = fc_drv_read(ROUTE_REG_BASE_ADDR, ROUTE_STATUS_REG_OFFSET_ADDR);
    } while ((readData & 0x1) != 0);
    
    fc_drv_write(0xFFFFFFFF, ROUTE_REG_BASE_ADDR, ROUTE_STATUS_REG_OFFSET_ADDR);
    fc_drv_write(writeData, ROUTE_REG_BASE_ADDR, ROUTE_INSERT_IP_REG_OFFSET_ADDR);
    fc_drv_write(nextIP, ROUTE_REG_BASE_ADDR, ROUTE_INSERT_NEXT_JUMP_IP_REG_OFFSET_ADDR);
    do {
        readData = fc_drv_read(ROUTE_REG_BASE_ADDR, ROUTE_STATUS_REG_OFFSET_ADDR);
    } while ((readData & 0x10) != 0x10); //bit 4
    fc_drv_write(0xFFFFFFFF, ROUTE_REG_BASE_ADDR, ROUTE_STATUS_REG_OFFSET_ADDR);
    #endif
    return;
}

void DeleteRouterItem(u32 deleteDestIP)
{
    u32 readData;
    u32 writeData;
    #if 0
    writeData = (deleteDestIP & 0xFFFF) << 16;
    do {
        readData = fc_drv_read(ROUTE_REG_BASE_ADDR,ROUTE_STATUS_REG_OFFSET_ADDR);
        sleep(QUERY_STATUE_REG_SCHE_INTERVEL);
    } while ((readData & 0x01) != 0);
    fc_drv_write(0xFFFFFFFF, ROUTE_REG_BASE_ADDR, ROUTE_STATUS_REG_OFFSET_ADDR);
    fc_drv_write(writeData, ROUTE_REG_BASE_ADDR, ROUTE_DELETE_IP_REG_OFFSET_ADDR);
    do {
        readData = fc_drv_read(ROUTE_REG_BASE_ADDR, ROUTE_STATUS_REG_OFFSET_ADDR);
        sleep(QUERY_STATUE_REG_SCHE_INTERVEL);
    } while ((readData & 0x20) != 0x20); //bit 5
    fc_drv_write(0xFFFFFFFF,ROUTE_REG_BASE_ADDR, ROUTE_STATUS_REG_OFFSET_ADDR);
    #endif
    return;
}

static u8 AddRouterTblCfg(u32 destNetIP, u32 nextHopIP, u8 nextHopPortNum)
{
    u32 index;
    for (index = 0; index < g_routeTblNum; index++) {
        if (g_routerTbl[index].destIP == destNetIP) {
            SW_DBG(DBG_ERROR, "The netIp already exist.");
            return RET_FAILED;
        }
    }
    
    if (g_routeTblNum >= MAX_ROUTER_TABLE_NUM) {
        SW_DBG(DBG_ERROR, "Reach the max num of router table !");
        return RET_FAILED;
    }  
    
    g_routerTbl[g_routeTblNum].routeIsValied = ROUTE_IS_VALID;
    g_routerTbl[g_routeTblNum].nextIP = nextHopIP;
    g_routerTbl[g_routeTblNum].destIP = destNetIP;
    g_routerTbl[g_routeTblNum].portNum = nextHopPortNum;   
    g_routeTblNum++;
    
    if (GetPortStatus(nextHopPortNum) == PORT_READY) {
        InsertRouterItem(nextHopPortNum, destNetIP, nextHopIP);  
    }
    return RET_SUCCESS;
}

static u8 DeleteRouterTblCfg(u32 destNetIP, u32 nextHopIP, u8 nextHopPortNum)
{
    u32 indexNum, location;
    u32 validRouterNum;
    u32 delLoc = INVALIDE_LOCATION;
    validRouterNum = g_routeTblNum;
    
    for (indexNum = 0; indexNum < g_routeTblNum; ++indexNum) {
        if ((g_routerTbl[indexNum].portNum == nextHopPortNum)  &&
            (g_routerTbl[indexNum].destIP == destNetIP) &&
            (g_routerTbl[indexNum].nextIP == nextHopIP)) {
            DeleteRouterItem(destNetIP);
            g_routerTbl[indexNum].routeIsValied = ROUTE_NOT_VALID;
            validRouterNum--;
            delLoc = indexNum; // Get the delete location
            break;
        }
    }
    
    if (delLoc == INVALIDE_LOCATION) {
        SW_DBG(DBG_ERROR, "The port num or port destIp, Nextip is wrong !");
        return RET_FAILED;
    }
    
    for (indexNum = delLoc; indexNum < g_routeTblNum; ++indexNum) {
        if (g_routerTbl[indexNum].routeIsValied == ROUTE_NOT_VALID) {
            for (location = indexNum + 1; location < g_routeTblNum; ++location) {
                if (g_routerTbl[location].routeIsValied == ROUTE_IS_VALID) {
                    memcpy(&(g_routerTbl[indexNum]), &(g_routerTbl[location]), sizeof(RouterTbl));
                    g_routerTbl[indexNum].routeIsValied = ROUTE_IS_VALID;
                    g_routerTbl[location].routeIsValied = ROUTE_NOT_VALID;                    
                    break;
                }
            }
        }
    }
    g_routerTbl[validRouterNum].destIP = INVALID_NEXT_PORT_IP;
    g_routerTbl[validRouterNum].nextIP = 0;
    g_routerTbl[validRouterNum].portNum = 0;
    g_routeTblNum = validRouterNum;
    return RET_SUCCESS;
}


static u8 DealIpRouterTbl(u8 *spiData)
{
    u8 operationCode = spiData[3];
    u8 cabin = spiData[4];
    u32 destNetIP = {0};
    u32 nextHopIP = 0;
    u8 nextHopPortNum = spiData[13];
    SW_DBG(DBG_DATA, "cabin = 0x%x", cabin);

    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }
    
    destNetIP = (u32)(spiData[5]) << 24 | (u32)(spiData[6]) << 16 |
        (u32)(spiData[7]) << 8 | (u32)(spiData[8]);
    nextHopIP = (u32)(spiData[9]) << 24 | (u32)(spiData[10]) << 16 |
        (u32)(spiData[11] << 8) | spiData[12];

    SW_DBG(DBG_DATA, "destNetID = 0x%x, nextHopID = 0x%x", destNetIP, nextHopIP);

    if (nextHopPortNum >= FC_PORT_NUM_INSTANCES) {
        SW_DBG(DBG_ERROR, "The portnum is wrong. nextHopPortNum = %d ", nextHopPortNum);
        return RET_FAILED;
    }
/*
    ret = ValidPortNumCheckForIntoData(nextHopPortNum);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "The port is unvalid port.");
        return RET_FAILED;
    }    
*/
    if ((cabin != LOCAL_CABIN) && (cabin != REMOTE_CABIN)) {
        SW_DBG(DBG_ERROR, "The cabin num is wrong! cabin = %d", cabin);
        return RET_FAILED;
    }
    
    if (operationCode == ADD_PORT_CFG) {
        ret = AddRouterTblCfg(destNetIP, nextHopIP, nextHopPortNum);
        if (ret != RET_SUCCESS) {
            SW_DBG(DBG_ERROR, "Add router item failed !");
            return RET_FAILED;
        }        
        SW_DBG(DBG_STEP, "Add port cfg");
    } else if (operationCode == DELETE_PORT_CFG) {
        ret = DeleteRouterTblCfg(destNetIP, nextHopIP, nextHopPortNum);
        if (ret != RET_SUCCESS) {
            SW_DBG(DBG_ERROR, "delete router item failed !");
            return RET_FAILED;
        }
        SW_DBG(DBG_STEP, "Delete router tbl cfg");
    } else {
        SW_DBG(DBG_ERROR, "The operation code is wrong. operationCode = %d", operationCode);
        return RET_FAILED;
    }

    //xmk_enter_kernel();
    //g_bootSavedCfg.routerTblSavedCfg.routerTblModifyFlag = CFG_MODYFIED;
    //g_bootSavedCfg.routerTblSavedCfg.validRouterNum = g_routeTblNum;
    //memcpy(g_bootSavedCfg.routerTblSavedCfg.routerTbl, &g_routerTbl,
    //    MAX_PORT_TABLE_NUM *sizeof(RouterTbl));
    //xmk_leave_kernel();
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    /* Not complate */
    return RET_SUCCESS;
}

static u8 DealCapsuleCommunicateEnable(u8 *spiData) 
{
    u8 inputCabin = spiData[3] - 1;
    u8 routeSelect = spiData[4];
    u8 enableContol = spiData[5];
    u8 portNumber = 0xFF; //invalide port
    u8 statueMask = 0;
    CascadePortInfo cascadePortInfo = {0};
    /* current switch location */
    if (GetSwLocation() >= TOTAL_SW_LOCATION_NUM) { 
        return RET_FAILED;
    }
    
    u8 cabinLocation = GET_CABIND_LOCATION(GetSwLocation());
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }
    GetCascadePortSetting(&cascadePortInfo);
    if (capsuleLocMap[cabinLocation].remoteLoc1 == inputCabin) {
        portNumber = cascadePortInfo.remoteCascadePort1;
    }
    
    if (capsuleLocMap[cabinLocation].remoteLoc2 == inputCabin) {
        portNumber = cascadePortInfo.remoteCascadePort2;
    }
    SW_DBG(DBG_DATA, "portNum = 0x%x ", portNumber);
    /*
    ret = ValidPortNumCheckForIntoData(portNumber);
    if (ret != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "The port is unvalid port. portNum = %d", portNumber);
        return RET_FAILED;
    }  
    */
    if (routeSelect == 1) { //FC
        statueMask = FC_STATUE_MASK;
    } else if (routeSelect == 2) { //IPFC
        statueMask = IPFC_STATUE_MASK;
    } else {
        return RET_FAILED;
    }
    
    if (enableContol == ENABLE_FLAG) {
        SW_DBG(DBG_STEP, "Enable flag \n");
    } else if (enableContol == DISABLE_FLAG) {
        SW_DBG(DBG_STEP, "Disable flag \n");
    } else {
        return RET_FAILED;
    }
 
    ret = PortEnableControl(portNumber, statueMask, enableContol);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }  
    //g_bootSavedCfg.fcEnableFlag = g_ZKTempFlag.portFcEnableFlag;
    //g_bootSavedCfg.ipEnable = g_digUpdateBuffer.digData.ipStates.value;
    //g_bootSavedCfg.fcEnableModifyFlag = CFG_MODYFIED;
    //g_bootSavedCfg.ipEnableModifyFlag = CFG_MODYFIED;
    //xmk_leave_kernel();
    //WriteBootCfgToFlash(&g_bootSavedCfg);
    /* Not complate */
    return RET_SUCCESS;
}

static u8 SetTheSoftwareRebootMode(u8 bootMode)
{
    int status = -1;
    BootFlagInfo bootFlag = {0};
    (void)GetBootFlagFromFlash(&bootFlag);
    
    if (bootMode == ORIGIN_BOOT_IMAGE) {
        //bootFlag.bootTime = 0;
        //bootFlag.bootStateFlag = BOOT_STATE_ORIGINAL;
        SetStartLocation(SOFTAWARE_SET, ORIGIN_SOFT);
        SW_DBG(DBG_STEP, "set mode origin");
    } else if (bootMode == RESTRUCT_BOOT_IMAGE) {
        /* If the restruct data is not complete can not set the reboot mode */
        if ((g_ZKTempFlag.restructState != RESTRUCT_STATE_FLSH_WRITE_SUCESS) 
            && (g_ZKTempFlag.restructState != RESTRUCT_STATE_NO_RESTRUCT_PACKAGE)) {
            g_ZKTempFlag.mstDataDiscardNum++;
            return RET_FAILED;
        }
        
        //bootFlag.bootTime = FLASH_RESTRUCT_IMAGE_BOOT_TIME;
        //bootFlag.bootStateFlag = BOOT_STATE_RESTRUCTING;
        SetStartLocation(SOFTAWARE_SET, RESTRUCT_SOFT);
        SW_DBG(DBG_STEP, "set mode restr");
    } else {
        SW_DBG(DBG_ERROR, "The boot mode is wrong! bootmode = 0x%x", bootMode);
        return RET_FAILED;
    }
    //sleep(UPDATE_FLASH_DELAY_TIME);    
    //status = WriteBootFlagToFlash(&bootFlag);
    //if(status != RET_SUCCESS) {
    //    SW_DBG(DBG_ERROR, "write flag_restruct error");
    //    return RET_FAILED;
    //}        
    return RET_SUCCESS;
}

static u32 SetTheBindParaRebootMode(u8 bindingParaMode)
{
    u32 ret;
    //u32 ret = SpiReWriteFlash(RESTRUCT_BINDING_PARA_MODE_ADDR, 1, &bindingParaMode);
    //SW_DBG(DBG_DATA, "bindingParaMode = %d", bindingParaMode);
    //if (ret != RET_SUCCESS) {
    //    SW_DBG(DBG_ERROR, "write binding para flag_restruct error");
    //}
    return ret;
}

static u8 DealTheRebootMode(u8 *spiData)
{
    u8 ret;
    u8 informationHost = spiData[1];
    u8 bootFlag = spiData[2];
    u8 bootMode = spiData[3];
    u8 swLocation = GetSwLocation();
    if (swLocation == TOTAL_SW_LOCATION_NUM) {
        /* The binding para get failed */
        ret = SetTheSoftwareRebootMode(bootMode);
    } else {
        if ((bootFlag == switchNumMap[swLocation]) &&
            (informationHost == switchLocMap[swLocation].hostID)) {
            ret = SetTheSoftwareRebootMode(bootMode);
        } else {
            SW_DBG(DBG_ERROR, "The bootFlag is wrong!");
            ret = RET_FAILED;
        }
    }
    
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }
    SW_DBG(DBG_STEP, "Set the reoot mode succeed!");
    return RET_SUCCESS;
}

static u8 DealTheBindParaRebootMode(u8 *spiData)
{
    u8 bootMode = spiData[3];

    if ((bootMode != ORIGIN_BOOT_IMAGE) 
        && (bootMode != RESTRUCT_BOOT_IMAGE)) {
        return RET_FAILED;
    }
        
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }

    ret = SetTheBindParaRebootMode(bootMode);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }

    if (bootMode == ORIGIN_BOOT_IMAGE) {
        SetStartLocation(BINDPARA_SET, ORIGIN_SOFT);
    } else if (bootMode == RESTRUCT_BOOT_IMAGE) {
        SetStartLocation(BINDPARA_SET, RESTRUCT_SOFT);        
    } else {
        return RET_FAILED;
    }
    /* clear the saved config */
    //(void)EreaseBootCfgFromFlash();
    SW_DBG(DBG_STEP, "Set the binding para reoot mode succeed!");
    return RET_SUCCESS;
}

static u8 DealTheRebootCmd(u8 *spiData)
{
    u8 informationHost = spiData[1];
    u8 functionUnit = spiData[2];
    if (GetSwLocation() > TOTAL_SW_LOCATION_NUM) { 
        return RET_FAILED;
    }
    /* If binding para get failed, donot do the check */
    if (GetSwLocation() != TOTAL_SW_LOCATION_NUM) {
        if ((informationHost != switchLocMap[GetSwLocation()].hostID) ||
            (functionUnit != switchNumMap[GetSwLocation()])) {
            SW_DBG(DBG_ERROR, "The host number is wrong ! hostID = 0x%x", informationHost);
            return RET_FAILED;
        }
    } 
    
    //g_digUpdateBuffer.digData.switchStatue.BitValue.resetStatus = RESET_WAY_COMMAND_RESET;
    (void)GetBootFlagFromFlash(&g_bootInfo);
    #if 0
    xmk_enter_kernel ();
    g_bootInfo.commandResetFlag = RESET_WAY_COMMAND_RESET;
    g_bootInfo.commandResetTime++;
    xmk_leave_kernel();
    (void)WriteBootFlagToFlash(&g_bootFlag);
    #endif
    sleep(UPDATE_FLASH_DELAY_TIME);
    g_ZKTempFlag.resetFlag = RESET_ENABLE;
    return RET_SUCCESS;
}

static u8 TimeSyncClockSourceSelect(u8 *spiData)
{
    u8 sourceSelect = spiData[3];
    u8 ret = LocationCheckForIntoData(spiData);
    if (ret != RET_SUCCESS) {
        return RET_FAILED;
    }    
    
    if ((sourceSelect == 0) || (sourceSelect > 4)) {
        SW_DBG(DBG_ERROR, "The Source is wrong! sourceSelect= 0x%x", sourceSelect);
        return RET_FAILED;
    }
    
    /* sourceSelect is start from 1, g_ZKTempFlag.timeSyncSource is start from 0 */
    g_ZKTempFlag.timeSyncSource = sourceSelect - 1;
    //TimeSyncSourceSet(g_ZKTempFlag.timeSyncSource);
    if (g_ZKTempFlag.timeSyncSource == TIME_SYNC_SOURCE_GNSS) {
        //g_digUpdateBuffer.digData.switchStatue.BitValue.timeSyncSource = 1; //1 means GNSS
    } else {
        //g_digUpdateBuffer.digData.switchStatue.BitValue.timeSyncSource = 0; //1 means GNSS
    }
    #if 0
    xmk_enter_kernel ();
    g_bootSavedCfg.timeSyncSource = g_ZKTempFlag.timeSyncSource;
    xmk_leave_kernel();
    WriteBootCfgToFlash(&g_bootSavedCfg);
    #endif
    return RET_SUCCESS;
}

static const IntoDataFunMatchTbl funMatchTbl[TOTAL_FUNCTION_NUM] = {
    { PORT_FC_ENABLE, DealFcEnableData, PORT_FC_ENABLE_CMD_LENGTH}, //OK
    { PORT_TIME_SYNC_ENABLE, DealTimeSyncEnable, PORT_TIME_SYNC_ENABLE_CMD_LENGTH}, //OK
    { PORT_IP_ENABLE, DealPortIpEnable, PORT_IP_ENABLE_CMD_LENGTH}, //OK
    { PORT_RESET, DealPortResetCmd, PORT_RESET_CMD_LENGTH}, 
    { PORT_FC_ID_CFG, DealFcIdCfg, PORT_FC_ID_CFG_CMD_LENGTH},  //OK
    { PORT_MAC_IP_CFG, DealMacIpCfg, PORT_MAC_IP_CFG_CMD_LENGTH}, //OK 
    { PORT_IP_BANDWIDTH_CFG, DealPortIpBandWidth, PORT_IP_BANDWIDTH_CFG_CMD_LENGTH}, //OK
    { TIME_SYNC_PERIOD, DealTimeSyncPeriod, TIME_SYNC_PERIOD_CMD_LENGTH}, //OK
    { CAPSULE_COMMUNICATE_ENABLE, DealCapsuleCommunicateEnable, CAPSULE_COMMUNICATE_ENABLE_CMD_LENGTH}, //OK
    { ROUTER_TABLE, DealIpRouterTbl, ROUTER_TABLE_CMD_LENGTH}, //OK
    { SET_SOFTWARE_REBOOT_MODE, DealTheRebootMode, SET_SOFTWARE_REBOOT_MODE_CMD_LENGTH}, //OK
    { SET_BINDING_PARA_REBOOT_MODE, DealTheBindParaRebootMode, SET_BINDING_PARA_REBOOT_MODE_CMD_LENGTH}, //OK
    { REBOOT_CMD, DealTheRebootCmd, REBOOT_CMD_LENGTH}, //OK
    { TIME_SYNC_SOURCE, TimeSyncClockSourceSelect, TIME_SYNC_SOURCE_LENGTH},
};
    

void StoreTheIntoDataToRingBuf(SpiDataInfoStruct *spiDataInfo)
{
    u32 i;
    u8 dataTemp[INTO_DATA_DATASIZE] = {0};
    u8 dataNumTemp = 0;
    u32 dataNumWrite;
    u8 swLocation = GetSwLocation();
    if (swLocation >= TOTAL_SW_LOCATION_NUM) {
        return;
    }
    dataTemp[dataNumTemp++] = MST_COMMUNICATE_SYNC_WORD;
    dataTemp[dataNumTemp++] = GetDigBindSWDestID();
    dataTemp[dataNumTemp++] = spiDataInfo->dataType;
    dataTemp[dataNumTemp++] = spiDataInfo->dataSize >> 8; // Get the high 8 bit
    dataTemp[dataNumTemp++] = spiDataInfo->dataSize & 0xFF; // Get the low 8 bit
    if (dataNumTemp + spiDataInfo->dataSize < INTO_DATA_DATASIZE) {
        for(i = 0; i < spiDataInfo->dataSize; i++) {
            dataTemp[dataNumTemp++] = spiDataInfo->spiData[i];
        }
        dataTemp[dataNumTemp++] = spiDataInfo->crc;
    }
    
    if (dataNumTemp < INTO_DATA_DATASIZE) {
        memset(&(dataTemp[dataNumTemp]), g_reserveDataMapTbl[swLocation], (INTO_DATA_DATASIZE - dataNumTemp));
    }
    INT32 intoDataRetPointer = GetIntoDataRetPointer();
    dataNumWrite = write(intoDataRetPointer, dataTemp, INTO_DATA_DATASIZE);
    if (dataNumWrite < dataNumTemp) {
        SW_DBG(DBG_WARN, "The fifo is Full, dataNumWrite = %d, dataNumToWrite = %d",
            dataNumWrite, dataNumTemp);
    }
    return;
}

void RestoreTheIntoDataToEngPara(u8* restoreData)
{
    u8 swLocation = GetSwLocation();
    if (swLocation >= TOTAL_SW_LOCATION_NUM) {
        return;
    }
    
    INT32 intoDataRetPointer = GetIntoDataRetPointer();
    if (read(intoDataRetPointer, restoreData, INTO_DATA_DATASIZE) == ENV_ERRO) {
        memset(restoreData, g_reserveDataMapTbl[swLocation], INTO_DATA_DATASIZE);
    }
    return;
}

void DealIntoData(SpiDataInfoStruct *spiDataInfo)
{
    int i;
    u8 ret = 1;
    u8 tempFuncFlag = spiDataInfo->spiData[0];
    u8 *spiData = spiDataInfo->spiData;
    
    StoreTheIntoDataToRingBuf(spiDataInfo);
    for (i = 0; i < (sizeof(funMatchTbl) / sizeof(IntoDataFunMatchTbl)); i++) {
        if (tempFuncFlag == funMatchTbl[i].functionFlag) {
            if (spiDataInfo->dataSize == funMatchTbl[i].dataSize) {
                ret = funMatchTbl[i].dealFun(spiData);
            } else {
                SW_DBG(DBG_ERROR, "The command length is wrong!");
            }
            break;
        }
    }

    if (ret != 0) {
        g_ZKTempFlag.rXErrorCommandNum++;
        g_ZKTempFlag.rXWrongCmdForDig++;
        if (g_ZKTempFlag.rXWrongCmdForDig > 0x07) {
            g_ZKTempFlag.rXWrongCmdForDig = 1;
        }
        SetZkCommandCount(g_ZKTempFlag.rXWrongCmdForDig, WRONG_COMMAND);
    } else {
        g_ZKTempFlag.rXRightCommandNum++;
        g_ZKTempFlag.rXRightCmdForDig++;
        if (g_ZKTempFlag.rXRightCmdForDig > 0x1F) {
            g_ZKTempFlag.rXRightCmdForDig = 0;
        }
        SetZkCommandCount(g_ZKTempFlag.rXRightCmdForDig, RIGHT_COMMAND);
    }
    return;
}

