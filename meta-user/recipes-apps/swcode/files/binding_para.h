#ifndef BINDING_PARA_H
#define BINDING_PARA_H
#include "common.h"

#define ORIGIN_BINDING_PARA_FLASH_DEV "/dev/mtd6"
#define RESTRUCT_BINDING_PARA_FLASH_DEV "/dev/mtd7"
#define BOOT_FLAG_FLASH_DEV "/dev/mtd8"

#define ORIGIN_BINDING_PARA_FLAG 0
#define RESTRUCT_BINDING_PARA_FLAG 1

#define ENGPARA_DEST_ID_MICU01_SW01 0x000F81 
#define ENGPARA_DEST_ID_MICU02_SW01 0x000F83

#define ENGPARA_DEST_ID_MICU03_SW01 0x010F81 
#define ENGPARA_DEST_ID_MICU03_SW02 0x010F82

#define ENGPARA_DEST_ID_MICU04_SW01 0x010F83
#define ENGPARA_DEST_ID_MICU04_SW02 0x010F84

#define ENGPARA_DEST_ID_MICU05_SW01 0x020F81
#define ENGPARA_DEST_ID_MICU05_SW02 0x020F82

#define ENGPARA_DEST_ID_MICU06_SW01 0x020F83
#define ENGPARA_DEST_ID_MICU06_SW02 0x020F84

#define ENGPARA_DEST_ID_DEFAULTE_VALUE 0xAA


#define MST_COMMUNICATE_SW1_DEST_WORD 0xB1 //destination device of switch 1
#define MST_COMMUNICATE_SW2_DEST_WORD 0xB6
#define MST_COMMUNICATE_DEFAULT_DEST_WORD 0xAA

#define BINDING_PARA_BYTE_CONUNT 3*1024


#define ROUTE_IS_VALID 1
#define ROUTE_NOT_VALID 0
#define INVALID_PORT_NUM 0xAAAA
#define INVALID_NEXT_PORT_IP 0xAAAAAAAA


#define GET_PORTNUM_FROM_BINDING_PARA(portIDbaseAddress, indexNum) (*((u8*)(portIDbaseAddress) + (indexNum) * 4))
#define GET_PORTDID_FROM_BINDING_PARA(portIDbaseAddress, indexNum) (*((u8*)(portIDbaseAddress) + 1 + (indexNum) * 4) << 16 | \
    *((portIDbaseAddress) + 2 + (indexNum) * 4) << 8 | *((u8*)(portIDbaseAddress) + 3 + (indexNum) * 4))

#define GET_PORTINFO_BASE_ADDRESS(bindParaAdress) ((u8*)(bindParaAdress) + 2) 
#define GET_PORT_TBL_NUMBER(bindParaAdress) (*((u8*)(bindParaAdress) + 1))


#define GET_MAC_ADDR_BASE_ADDRESS(bindParaAddress) ((u8*)(bindParaAddress) + 258)
#define GET_ROUTER_TBL_NUMBER(bindParaAdress) (*((u8*)(bindParaAdress) + 482)) // 1location  + 1portnum +  4*64porttbl + 224Mac
#define GET_ROUTER_TBL_ADDR(bindParaAdress) ((u8*)(bindParaAdress) + 483) // 1location  + 1portnum +  4*64porttbl + 80Mac + 1tablnum
#define GET_ROUTERNUM_FROM_BINDING_PARA(RouterbaseAddress, indexNum) (*((u8*)(RouterbaseAddress) + (indexNum) * 9))
#define GET_DESTIP_FROM_BINDING_PARA(portIDbaseAddress, indexNum) \
    (*((u8*)(portIDbaseAddress) + 1 + (indexNum) * 9) << 24 | \
    *((u8*)(portIDbaseAddress) + 2 + (indexNum) * 9) << 16 | \
    *((u8*)(portIDbaseAddress) + 3 + (indexNum) * 9) << 8 |  *((u8*)(portIDbaseAddress) + 4 + (indexNum) * 9))
    
#define GET_NEXTIP_FROM_BINDING_PARA(portIDbaseAddress, indexNum) \
    (*((u8*)(portIDbaseAddress) + 5 + (indexNum) * 9) << 24 | \
    *((u8*)(portIDbaseAddress) + 6 + (indexNum) * 9) << 16 |\
    *((u8*)(portIDbaseAddress) + 7 + (indexNum) * 9) << 8 |  *((u8*)(portIDbaseAddress) + 8 + (indexNum) * 9))
    
#define GET_CASCADE_ROUTER_ADDR(bindParaAdress) ((u8*)(bindParaAdress) + 1059)
#define GET_CASCADE_PORT_ADDR(bindParaAdress) ((u8*)(bindParaAdress) + 1086)


#pragma pack(1)

typedef struct {
    u32 digBindSWDestID;
    u32 engBindSWDestID;
} DestIDMapTbl;

typedef struct {
    u32 portDid;
    u16 portNum;
    u16 portIsValid; 
} PortTbl;

typedef struct {
    u8 portNum;
    u8 macAddress[6];
    u8 ipAddress[4];
    u8 localSid[3];
} LocalAddrTbl;

typedef struct {
    u8 portNum;
    u32 destIP;
    u32 nextIP;
    u32 routeIsValied;
} RouterTbl;

typedef struct {
   u8 remotePort;
   u8 netIP[4];
   u8 maskOrJumpIp[4];
} CascadeRouterData;

typedef struct {
    u8 localCascadePort1;
    u8 localCascadePort2;
    u8 remoteCascadePort1;
    u8 remoteCascadePort2;
} CascadePortInfo;



typedef struct {
    u8 bootFlag;
        
} BootFlagInfo;
#pragma pack()

extern u32 g_validPortNum;
extern u32 g_routeTblNum;
extern CascadePortInfo g_cascadePortInfo;
//extern u8 g_swLocation;
extern PortTbl g_portTbl[MAX_PORT_TABLE_NUM];

void DealBindingParaDataFromFlash(void);
u32 GetDIDByIndex(u32 indexNum);
u32 GetPortNumByIndex(u32 indexNum);
u8 GetDigBindSWDestID(void);
u8 GetEngBindSWDestID(void);
u16 GetValidFlag(u32 indexNum);
u32 GetSwLocation(void);
u32 WriteBindingParaToFlash(u32 bindingParaFlag, u32 inputLength, u8 *writeBuffer);
void GetCascadePortSetting(CascadePortInfo *cascadePortInfo);
u32 GetBootFlagFromFlash(BootFlagInfo *bootFlagData);
u32 ReWriteBindingParaToFlash(u8* dataAddr, u32 dataCount);

#endif
