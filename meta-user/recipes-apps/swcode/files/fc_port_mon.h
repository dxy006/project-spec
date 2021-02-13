#ifndef FC_PORT_MON_H
#define FC_PORT_MON_H
#include "common.h"
typedef struct {
    u8 portStatus;
    u8 portB2BCredit;
    u8 reserved1;
    u8 reserved2;
} PortStatusStruct;


void PortReset(u8 portNum);
u8 DeletePortFromPortTbl(u32 portDID);
u8 GetPortStatus(u8 portNum);
void DelDidTblFromPortTbl(u32 port_num);
void AddDidTblToPortTbl(u32 port_num);
u8 AddPortToPortTbl(u16 portNum, u32 portDID);
bool IsPortDidExistInSoftWareTbl(u32 portDid, u16 *portNum);


#endif
