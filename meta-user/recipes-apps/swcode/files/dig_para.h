/*
 * dig_para.h
 *
 *  Created on: 2019Äê9ÔÂ22ÈÕ
 *      Author: ´÷¶û
 */

#ifndef DIG_PARA_H_
#define DIG_PARA_H_
#include "common.h"

#define DIG_PARA_LENGTH 14
#define TOTAL_DATA_SEND_NUM 10
#define DIG_DATA_SEND_DATA_SIZE 20

#define PORT_LINK_STATE_LINKED 1
#define PORT_LINK_STATE_UNLINKED 0

#define RIGHT_COMMAND 1
#define WRONG_COMMAND 0

#define SOFTAWARE_SET 0
#define BINDPARA_SET 1

#define ORIGIN_SOFT 0
#define RESTRUCT_SOFT 1

#define PORT_IP_ENABLE_FLAG 0
#define PORT_IP_DISABLE_FLAG 1

#define TOTAL_PKG_COUNT 0
#define WRONG_PKG_COUNT 1
#define RESET_PKG_COUNT 2
#define LOCGIC_SOLIDIFY_VERSION 0x312
#define INVALID_CASCADE_PORT 0xAA


#define MST_COMMUNICATE_SYNC_WORD 0x98

#define MST_COMMUNICATE_DIG_PARA 0x99
#define MST_COMMUNICATE_RESTRUCT_PARA 0xFF
#define MST_COMMUNICATE_BIND_PARA 0xAA
#define MST_COMMUNICATE_INTO_DATA 0x11
#define MST_COMMUNICATE_INJECTION_DATA 0x11
#define MST_COMMUNICATE_DEST_MASTER 0x69 // master control
#define MST_COMMUNICATE_BROADCAST_UTC_TIME 0x33

#define CHECK_RETURN_VALUE(returnValue)  \
if ((returnValue) != RET_SUCCESS) { \
    return; \
}


#pragma pack(1)
typedef union {
    u8 value;
    struct {
        u8 resetStatus : 2;
        u8 timeSyncMasterStatue : 1;
        u8 timeSyncSource : 1;
        u8 ppsValid : 1;
        u8 locatorData : 1;
        u8 ppsExist : 1;
        u8 heartBeat : 1;
    } BitValue;
} SWStatueStrut;

typedef union {
    u8 value;
    struct {
        u8 wrongCommandNum : 3;
        u8 rightCommandNum : 5;
    } BitValue;
} CommandCount;

typedef union {
    u8 value;
    struct {
        u8 plVersion : 4; /* bit[0-3] */
        u8 psVersion : 4; /* bit[4-7] */
    } BitValue;
} VersionFlag;

typedef union {
    u8 value;
    struct {
        u8 restructFlag : 3;  
        u8 bindingParaFlag : 1;
        u8 bindingParaNextStartLoc : 1;
        u8 psNextStartLoc : 1;
        u8 plCurrentStartLoc : 1;
        u8 psCurrentStartLoc : 1;
    } BitValue;
} SoftWareStartFlag;

typedef union {
    u16 value;
    struct {
        u8 linkStatePort0 : 1;
        u8 linkStatePort1 : 1;
        u8 linkStatePort2 : 1;
        u8 linkStatePort3 : 1;
        u8 linkStatePort4 : 1;
        u8 linkStatePort5 : 1;
        u8 linkStatePort6 : 1;
        u8 linkStatePort7 : 1;
        
        u8 linkStatePort8 : 1;
        u8 linkStatePort9 : 1;
        u8 linkStatePort10 : 1;
        u8 linkStatePort11 : 1;
        u8 linkStatePort12 : 1;
        u8 linkStatePort13 : 1;
        u8 linkStatePort14 : 1;
        u8 linkStatePort15 : 1;
    } BitValue;
}PortLinkStruct;

typedef union 
{
    u16 value;
    struct {
        u8 ipStatePort0 : 1;
        u8 ipStatePort1 : 1;
        u8 ipStatePort2 : 1;
        u8 ipStatePort3 : 1;
        u8 ipStatePort4 : 1;
        u8 ipStatePort5 : 1;
        u8 ipStatePort6 : 1;
        u8 ipStatePort7 : 1;
        
        u8 ipStatePort8 : 1;
        u8 ipStatePort9 : 1;
        u8 ipStatePort10 : 1;
        u8 ipStatePort11 : 1;
        u8 ipStatePort12 : 1;
        u8 ipStatePort13 : 1;
        u8 ipStatePort14 : 1;
        u8 ipStatePort15 : 1;        
    } BitValue;
} IpStateOfport;

typedef union 
{
    u16 value;
    struct {
        u8 tsStatuePort0 : 1;
        u8 tsStatuePort1 : 1;
        u8 tsStatuePort2 : 1;
        u8 tsStatuePort3 : 1;
        u8 tsStatuePort4 : 1;
        u8 tsStatuePort5 : 1;
        u8 tsStatuePort6 : 1;
        u8 tsStatuePort7 : 1;
        
        u8 tsStatuePort8 : 1;
        u8 tsStatuePort9 : 1;
        u8 tsStatuePort10 : 1;
        u8 tsStatuePort11 : 1;
        u8 tsStatuePort12 : 1;
        u8 tsStatuePort13 : 1;
        u8 tsStatuePort14 : 1;
        u8 tsStatuePort15 : 1;        
    } BitValue;
} TsSyncStatue;

typedef union 
{
    u32 value;
    struct {
        u32 utcTimeOfSecond : 6;
        u32 utcTimeOfMinute : 6;
        u32 utcTimeOfHour : 5;
        u32 utcTimeOfDay : 7;
        u32 reservedData : 8;
    } BitValue;
} UtcTimeStrut;

typedef union 
{
    u8 value;
    struct {
        u8 restructWrongPkgCount : 4;
        u8 cascadePort2FcEnableStatus: 1;
        u8 cascadePort2IpFcEnableStatus : 1;
        u8 cascadePort1FcEnableStatus : 1;
        u8 cascadePort1IpFcEnableStatus : 1;
    } BitValue;
} InterConnect;

typedef struct {
    SWStatueStrut switchStatue; // 1byte
    CommandCount commandCount;  // 1byte
    VersionFlag versionTbl;     // 1byte
    SoftWareStartFlag softwareStartFlag; // 1byte
    PortLinkStruct portLinkStates; // 2byte
    IpStateOfport ipStates; // 2byte
    TsSyncStatue tsStatue;  
    InterConnect interConnectState;
    UtcTimeStrut utcTime; // 4 bytes, 3byte valide
} DigDataFormat;


typedef struct {
    u8 syncWord;    //0x98  
    u8 deviceId;    //¨¦¨¨¡À0xb1
    u8 cmdWord;     // 0x99
    u8 length[2];  //0x0E
    DigDataFormat digData;
    u8 checksum;
} DigDataSendFormat;


#pragma pack()

void DealTheDigParaData(void);
void InitDigParaAndClearRXFifo(void);
//void UpdataDigData(void * arg);
void InitDigDataSem(void);
void FillPortLinkStatus(u8 portNum, u8 portStatus);
void FillUtcTimeFromLocData(DigDataFormat *digData);
void SetZkCommandCount(u8 commandCount, u8 commandType);
void SetStartLocation(u8 type, u8 location);
void SetRestructState(u8 restructState);
void UpdateRestructCount(u32 pkgCount, u8 pkgType);

void FillTimeSyncStatus(DigDataFormat *digData);
void FillTimeSyncStatusForEngPara(DigDataFormat *digData);
u8 SetPortEnableState(u8 portNum, u8 contrlMask, u8 contrlFlag);
u8 GetPortEnableState(u8 portNum, u8 contrlType);
u8 GetTimeSyncEnableState(u8 portNum);
void SetTimeSyncEnableFlag(u8 portNum, u8 timeSyncState);
void FillCascadePortStatue(DigDataFormat *digData);
void GetRealCascadePortNum(u8 *outCascadePort1, u8 *outCascadePort2);

#endif /* DIG_PARA_H_ */
