/*
 * binding_para.c
 *
 *  Created on: 2019Äê8ÔÂ8ÈÕ
 *      Author: ´÷¶û
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <string.h>

#include "flash_apply.h"
#include "sys_print.h"
#include "binding_para.h"

#include "dig_para.h"
u32 g_validPortNum = 0;
u32 g_routeTblNum = 0;
CascadePortInfo g_cascadePortInfo = {0};
static u8 g_swLocation = 0;
PortTbl g_portTbl[MAX_PORT_TABLE_NUM] = {0};
static u32 g_digBindSWDestID = MST_COMMUNICATE_DEFAULT_DEST_WORD;
static u32 g_engBindSWDestID;

LocalAddrTbl g_localAddressTbl[FC_PORT_NUM_INSTANCES] = {0};
RouterTbl g_routerTbl[MAX_ROUTER_TABLE_NUM] = {0};

/*****************************************************************************
* Tabl Name : g_destIdMapTbl
* DestWord For mst, DestId for data manger
*****************************************************************************/
static DestIDMapTbl g_destIdMapTbl[TOTAL_SW_LOCATION_NUM + 1] = {
    {MST_COMMUNICATE_SW1_DEST_WORD, ENGPARA_DEST_ID_MICU01_SW01},
    {MST_COMMUNICATE_SW2_DEST_WORD, ENGPARA_DEST_ID_MICU01_SW01}, //reserved
    {MST_COMMUNICATE_SW1_DEST_WORD, ENGPARA_DEST_ID_MICU02_SW01},
    {MST_COMMUNICATE_SW2_DEST_WORD, ENGPARA_DEST_ID_MICU02_SW01}, //reserved
    {MST_COMMUNICATE_SW1_DEST_WORD, ENGPARA_DEST_ID_MICU03_SW01},
    {MST_COMMUNICATE_SW2_DEST_WORD, ENGPARA_DEST_ID_MICU03_SW02},
    {MST_COMMUNICATE_SW1_DEST_WORD, ENGPARA_DEST_ID_MICU04_SW01},
    {MST_COMMUNICATE_SW2_DEST_WORD, ENGPARA_DEST_ID_MICU04_SW02},
    {MST_COMMUNICATE_SW1_DEST_WORD, ENGPARA_DEST_ID_MICU05_SW01},
    {MST_COMMUNICATE_SW2_DEST_WORD, ENGPARA_DEST_ID_MICU05_SW02},
    {MST_COMMUNICATE_SW1_DEST_WORD, ENGPARA_DEST_ID_MICU06_SW01},
    {MST_COMMUNICATE_SW2_DEST_WORD, ENGPARA_DEST_ID_MICU06_SW02},
    {MST_COMMUNICATE_DEFAULT_DEST_WORD, ENGPARA_DEST_ID_DEFAULTE_VALUE},
};
    
/*****************************************************************************
* Function Name : readBindingParaFromFlash
* Description : Read the binding para from flash
* Input :       bindingParaFlag : RESTRUCT_BINDING_PARA_FLAG or ORIGIN_BINDING_PARA_FLAG
*****************************************************************************/
static u32 readBindingParaFromFlash(u32 bindingParaFlag, u32 inputLength, u8 *readBuffer)
{
    INT32 flashFd;
    if (bindingParaFlag == RESTRUCT_BINDING_PARA_FLAG) {
        flashFd = open (RESTRUCT_BINDING_PARA_FLASH_DEV, O_RDWR | O_SYNC);
    } else {
        flashFd = open (ORIGIN_BINDING_PARA_FLASH_DEV, O_RDWR | O_SYNC);
    }

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
u32 WriteBindingParaToFlash(u32 bindingParaFlag, u32 inputLength, u8 *writeBuffer)
{
    int flashFd;
    if (bindingParaFlag == RESTRUCT_BINDING_PARA_FLAG) {
        flashFd = open (RESTRUCT_BINDING_PARA_FLASH_DEV, O_RDWR | O_SYNC);
    } else {
        flashFd = open (ORIGIN_BINDING_PARA_FLASH_DEV, O_RDWR | O_SYNC);
    }

    if (flashFd < 0) {
        SW_DBG (DBG_ERROR, "open flash device falied! \n");
        return RET_FAILED;
    }
    
    int ret = EreaseBulkFlash(flashFd, 0, 1);
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

u32 ReWriteBindingParaToFlash(u8* dataAddr, u32 dataCount)
{
    u32 ret = ReWriteDataToFlash(RESTRUCT_BINDING_PARA_FLASH_DEV, dataAddr, dataCount);
    if (ret != RET_SUCCESS) {
        SW_DBG (DBG_ERROR, "Write flash failed \n");
    }
    return ret;
}

/*****************************************************************************
* Function Name : GetBindingPortDid
* Description : Read the binding the portDID from flash for bindpara
* Input :    portDidTbl   
* Output :   none
* Attention : We can use the struct like this 
* u8 portNum;  u8 portDid[3], and use memcpy to deal the data. But it consumu memmory and time.
*****************************************************************************/
void GetBindingPortDid(u8 *readBuffer, PortTbl *portTbl)
{
    u32 index;
    u32 portTblNum = GET_PORT_TBL_NUMBER(readBuffer);
    g_validPortNum = portTblNum;

    if (portTblNum > MAX_PORT_TABLE_NUM) {
        SW_DBG (DBG_ERROR, "The port tbl num is wrong! porttblNum = %d  \n", portTblNum);
        return;
    }

    SW_DBG (DBG_DATA, "portTblNum is %d \n", portTblNum);
    /* Get local did num */
    for (index = 0; index < portTblNum; index++) {
        portTbl[index].portNum = (u32)GET_PORTNUM_FROM_BINDING_PARA(GET_PORTINFO_BASE_ADDRESS(readBuffer), index);
        if (portTbl[index].portNum >= FC_PORT_NUM_INSTANCES) {
            SW_DBG (DBG_ERROR, "The portNum is wrong! portnum = 0x%x  \n", portTbl[index].portNum);
            continue;
        }
        
        portTbl[index].portDid = (u32)GET_PORTDID_FROM_BINDING_PARA(GET_PORTINFO_BASE_ADDRESS(readBuffer), index);
        portTbl[index].portIsValid = PORT_IS_VALID;
        SW_DBG (DBG_DBG_DATA, "=====port_did[%d] = 0x%x =====  \n", index, portTbl[index].portDid);
    }
    
    for (index = portTblNum; index < MAX_PORT_TABLE_NUM; ++index) {
        portTbl[index].portNum = INVALID_PORT_NUM;
        portTbl[index].portDid = 0;
        portTbl[index].portIsValid = 0;
    }
    return;
}

void GetBindingLocalAddress(u8 *readBuffer, LocalAddrTbl *localAddressTbl)
{
    u32 index, i;
    u8* localBaseAddress = GET_MAC_ADDR_BASE_ADDRESS(readBuffer);
    memcpy (localAddressTbl, localBaseAddress, FC_PORT_NUM_INSTANCES * sizeof (LocalAddrTbl));
    return;
}

static void GetBindingRouterTbl(u8 *readBuffer, RouterTbl *routeTbl)
{
    u32 index;  
    u8* routerTblBassAddr;
    g_routeTblNum = GET_ROUTER_TBL_NUMBER(readBuffer);   
    if (g_routeTblNum >= MAX_ROUTER_TABLE_NUM) {
        SW_DBG (DBG_ERROR, "The router tbl num is wrong! routerTblNum = 0x%x", g_routeTblNum);
        g_routeTblNum = 0;
        return;
    }
    SW_DBG (DBG_DATA, "router table num is 0x%x", g_routeTblNum);

    /* Get local did num */
    for (index = 0; index < g_routeTblNum; index++) {
        routerTblBassAddr = GET_ROUTER_TBL_ADDR(readBuffer);
        routeTbl[index].portNum = (u32)GET_ROUTERNUM_FROM_BINDING_PARA(routerTblBassAddr, index);
        if (routeTbl[index].portNum >= FC_PORT_NUM_INSTANCES) {
            SW_DBG (DBG_ERROR, "The portNum is wrong! portnum = 0x%x", routeTbl[index].portNum);
            continue;
        }
        routeTbl[index].destIP = GET_DESTIP_FROM_BINDING_PARA(routerTblBassAddr, index);
        routeTbl[index].nextIP = GET_NEXTIP_FROM_BINDING_PARA(routerTblBassAddr, index);
        routeTbl[index].routeIsValied = ROUTE_IS_VALID;
    }

    for (index = g_routeTblNum; index < MAX_ROUTER_TABLE_NUM; ++index) {
        routeTbl[index].destIP = INVALID_NEXT_PORT_IP;
        routeTbl[index].routeIsValied = ROUTE_NOT_VALID;
    }
    return;
}

static void GetAndSetCascadeRouterSetting(u8 *readBuffer)
{
    u32 index;
    u32 localNetIp = 0;
    u32 netMask = 0;
    u32 remote1NetIpH16 = 0;
    u32 remote2NetIpH16 = 0;
    u32 remote1JumpIp = 0;
    u32 remote2JumpIp = 0;
    u8 nonLocal1Portnum, nonLocal2Portnum;
    CascadeRouterData cascadeData[3];
    memcpy (cascadeData, GET_CASCADE_ROUTER_ADDR(readBuffer), sizeof(CascadeRouterData) * 3);

    for (index = 0; index < 4; index++) {
        localNetIp = (u32)(localNetIp << 8) | (u32)(cascadeData[0].netIP[index]);
        netMask = (u32)(netMask << 8) | (u32)(cascadeData[0].maskOrJumpIp[index]);
        remote1NetIpH16 = (u32)(remote1NetIpH16 << 8) | (u32)(cascadeData[1].netIP[index]);
        remote1JumpIp = (u32)(remote1JumpIp << 8) | (u32)(cascadeData[1].maskOrJumpIp[index]);
        remote2NetIpH16 = (u32)(remote2NetIpH16 << 8) | (u32)(cascadeData[2].netIP[index]);
        remote2JumpIp = (u32)(remote2JumpIp << 8) | (u32)(cascadeData[2].maskOrJumpIp[index]);
    }
    nonLocal1Portnum = cascadeData[1].remotePort;
    nonLocal2Portnum = cascadeData[2].remotePort;


    //SetLocalRouterNetIp(localNetIp, netMask);
    //SetRemoteIRouterIp(nonLocal1Portnum, remote1NetIpH16, remote1JumpIp);
    //SetRemoteIIRouterIp(nonLocal2Portnum, remote2NetIpH16, remote2JumpIp);
    return;
}

static void GetCascadePortFromFlash(u8 *readBuffer)
{
    memcpy (&g_cascadePortInfo, GET_CASCADE_PORT_ADDR(readBuffer), sizeof(CascadePortInfo));
    return;
}

void GetCascadePortSetting(CascadePortInfo *cascadePortInfo)
{
    memcpy (cascadePortInfo, &g_cascadePortInfo, sizeof(CascadePortInfo));
    return;
}

u32 GetBootFlagFromFlash(BootFlagInfo *bootFlagData)
{
    INT32 flashFd = open (BOOT_FLAG_FLASH_DEV, O_RDWR | O_SYNC);
    if (flashFd < 0) {
        SW_DBG (DBG_ERROR, "open flash device falied! \n");
        return RET_FAILED;
    }
    lseek (flashFd, 0, SEEK_SET); //set the read point to the start of file
    u32 dataLength = read(flashFd, bootFlagData, sizeof(BootFlagInfo));
    close (flashFd);
    
    if (dataLength != sizeof(BootFlagInfo)) {
        SW_DBG (DBG_ERROR, "Read failed. dataLength = 0x%x, dataLength = %ld \n", dataLength, sizeof(BootFlagInfo));
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

/*****************************************************************************
* Function Name : DealBindingParaDataFromFlash
* Description : Deal the binding parameta from flash
* Input :     none
* Output :    none
* Attention : 
*****************************************************************************/
void DealBindingParaDataFromFlash(void)
{
    u8 readBuffer[BINDING_PARA_BYTE_CONUNT] = {0};
    BootFlagInfo bootFlagData = {0};
    u32 bindingParaFlag;
    u32 retValue;
    /* Get the boot mode and select is load origin para or restruct para */
    retValue = GetBootFlagFromFlash(&bootFlagData);
    SW_DBG (DBG_DATA, "bootFlag = 0x%x", bootFlagData.bootFlag);
    if (retValue != RET_SUCCESS) {
        bootFlagData.bootFlag = ORIGIN_BINDING_PARA_FLAG;
    }
    
    if (bootFlagData.bootFlag == RESTRUCT_BINDING_PARA_FLAG) {
        SW_DBG (DBG_DBG, "The binding para is restruct mode.");
        SetStartLocation (BINDPARA_SET, RESTRUCT_SOFT);
        //g_ZKTempFlag.bootStateInfo.bindingParaState = 1;
        bindingParaFlag = RESTRUCT_BINDING_PARA_FLAG;
    } else {
        SetStartLocation(BINDPARA_SET, ORIGIN_SOFT);
        SW_DBG (DBG_DBG, "The binding para is origin mode.");
        //g_ZKTempFlag.bootStateInfo.bindingParaState = 0;
        bindingParaFlag = ORIGIN_BINDING_PARA_FLAG;
    }

    retValue = readBindingParaFromFlash(bindingParaFlag, BINDING_PARA_BYTE_CONUNT, readBuffer);
    if (retValue != RET_SUCCESS) {
        readBuffer[0] = TOTAL_SW_LOCATION_NUM;
    }
    for (u32 i = 0; i < BINDING_PARA_BYTE_CONUNT; i++) {
        if (i % 4 == 0) {
            SW_DBG(DBG_DBG_DATA, "%d \n", i/4);                        
        }
        SW_DBG(DBG_DBG_DATA, "%x ", readBuffer[i]);
    }
    g_swLocation = readBuffer[0];
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) {
        g_digBindSWDestID = g_destIdMapTbl[TOTAL_SW_LOCATION_NUM].digBindSWDestID;
        g_swLocation = TOTAL_SW_LOCATION_NUM;
        SW_DBG (DBG_ERROR, "The location is wrong = %d", g_swLocation);
        return;
    }
    g_digBindSWDestID = g_destIdMapTbl[g_swLocation].digBindSWDestID;
    g_engBindSWDestID = g_destIdMapTbl[g_swLocation].engBindSWDestID;    

    SW_DBG (DBG_DBG, "Switch location = %d", g_swLocation);
    GetBindingPortDid ((u8*)readBuffer, g_portTbl);
    GetBindingLocalAddress ((u8*)readBuffer, g_localAddressTbl);
    GetBindingRouterTbl ((u8*)readBuffer, g_routerTbl);
    GetCascadePortFromFlash ((u8*)readBuffer);
    GetAndSetCascadeRouterSetting ((u8*)readBuffer);

    return;
}

u32 GetDIDByIndex(u32 indexNum)
{
    return g_portTbl[indexNum].portDid;
}

u32 GetPortNumByIndex(u32 indexNum)
{
    return (u32)(g_portTbl[indexNum].portNum);
}

u8 GetDigBindSWDestID(void)
{
    return (u8)g_digBindSWDestID;
}

u8 GetEngBindSWDestID(void)
{
    return (u8)g_engBindSWDestID;
}

u16 GetValidFlag(u32 indexNum)
{
    return g_portTbl[indexNum].portIsValid;
}

u32 GetSwLocation(void)
{
    return g_swLocation;
}
