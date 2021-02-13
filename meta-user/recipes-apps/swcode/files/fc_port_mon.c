/*
 * fc_port_mon.c
 *
 *  Created on: 20201102
 *      Author: duxingyu
*/
#include "common.h"
#include "fc_port_mon.h"
#include "sys_print.h"
#include "binding_para.h"
static PortStatusStruct g_portStatus[FC_PORT_NUM_INSTANCES] = {0};
extern u32 g_validPortNum;

void PortReset(u8 portNum)
{
#if 0
    u32 writeValue;
    if (portNum < (FC_PORT_NUM_INSTANCES / 2)) {
        writeValue = PORT_RESET_VALUE << (portNum * 4);
        fc_drv_write(writeValue, XPAR_FC_RESET_ALL_0_BASEADDR, FC_RESET_OFFESET0);
    } else {
        writeValue = PORT_RESET_VALUE << ((portNum - FC_PORT_NUM_INSTANCES / 2) * 4);
        fc_drv_write(writeValue, XPAR_FC_RESET_ALL_0_BASEADDR, FC_RESET_OFFESET1);
    }
#endif
    return;
}

u8 DeletePortFromPortTbl(u32 portDID)
{
#if 0
    if (fc_addm_delete_table(portDID) != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "Delete table failed. Id = 0x%x ", portDID);
        return RET_FAILED;
    }
#endif
    return RET_SUCCESS;
}

bool IsPortDidExistInSoftWareTbl(u32 portDid, u16 *portNum)
{
    u32 index;
    u16 portNumTemp;
    u32 maxIndexNum;
    
    maxIndexNum = g_validPortNum;
    
    for (index = 0; index < maxIndexNum; ++index) {
        if (portDid == GetDIDByIndex(index)) {
            portNumTemp = GetPortNumByIndex(index);
            SW_DBG(DBG_WARN, "PortDid exist. The port num is %d ", portNumTemp);
            *portNum = portNumTemp;
            return true;
        }
    }
    *portNum = 0;
    return false;
}

u8 AddPortToPortTbl(u16 portNum, u32 portDID)
{
    u16 portNumTemp = 0;
    #if 0
    u8 portDidExist = IsPortDidExistInLogicTbl(portDID, &portNumTemp);
    if (portDidExist != TRUE) {
        if(fc_addm_write_unicast(portDID, 0, portNum) != RET_SUCCESS) {
            SW_DBG(DBG_ERROR, "Write logic table failed. Id = 0x%x ", portDID);
            return RET_FAILED;
        }
    } else {
        if (portNum != portNumTemp) {
             SW_DBG(DBG_WARN, "The Did alreay exist in port %d ", portNumTemp);
             return RET_FAILED;
        }
    }
    #endif
    return RET_SUCCESS;
}

void AddDidTblToPortTbl(u32 port_num)
{
    u32 indexNum;
    u32 validPortNum;
    validPortNum = g_validPortNum;

    for (indexNum = 0; indexNum < validPortNum; indexNum++) {
        if (GetPortNumByIndex(indexNum) == port_num) {
            AddPortToPortTbl((u16)port_num, GetDIDByIndex(indexNum));
        }
    }
    return;
}

void DelDidTblFromPortTbl(u32 port_num)
{
    u32 indexNum;
    u32 validPortNum;
    validPortNum = g_validPortNum;
    
    for (indexNum = 0; indexNum < validPortNum; indexNum++) {
        if (GetPortNumByIndex(indexNum) == port_num) {
            DeletePortFromPortTbl(GetDIDByIndex(indexNum));
        }
    }
    return;
}

u8 GetPortStatus(u8 portNum)
{
    return g_portStatus[portNum].portStatus;
}
