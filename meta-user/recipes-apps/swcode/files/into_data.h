#ifndef INTO_DATA_H
#define INTO_DATA_H
#include "common.h"
#include "zk_communicate.h"

/***************** Macros (Inline Functions) Definitions *********************/
#define HOST_01 0x01
#define HOST_02 0x02
#define SW_01 0x01
#define SW_03 0x02
#define SW1_SOFTWARE_REBOOT_FLAG 0x95
#define SW2_SOFTWARE_REBOOT_FLAG 0x96

#define TOTAL_CAPSULE_COMMU_LOC 3
#define SW_LOCATION_HXC 0
#define SW_LOCATION_SYCI 1
#define SW_LOCATION_SYCII 2
#define INVALIDE_LOCATION 0xFFFFFFFF

#define IS_CASCADE_PORT 1
#define NOT_CASCADE_PORT 0

#define ADD_PORT_CFG 0x0F
#define DELETE_PORT_CFG 0xF0

#define LOCAL_CABIN 1
#define REMOTE_CABIN 2

#define ORIGIN_BOOT_IMAGE 0x01
#define RESTRUCT_BOOT_IMAGE 0x02

#define TIME_SYNC_PERIOD_1MS 1
#define TIME_SYNC_PERIOD_10MS 10
#define TIME_SYNC_PERIOD_100MS 100
#define TIME_SYNC_PERIOD_1000MS 1000

#define BOOT_STATE_ORIGINAL    0xFF
#define BOOT_STATE_RESTRUCTING 0x01
#define BOOT_STATE_RESTRUCTED  0x02

#define TOTAL_FUNCTION_NUM 14

#define PORT_FC_ENABLE 0x70
#define PORT_TIME_SYNC_ENABLE 0x71
#define PORT_IP_ENABLE 0x72
#define PORT_RESET 0x73
#define PORT_FC_ID_CFG 0x74
#define PORT_MAC_IP_CFG 0x75
#define PORT_IP_BANDWIDTH_CFG 0x76
#define TIME_SYNC_PERIOD 0x77
#define TIME_SYNC_PPS 0x78
#define CAPSULE_COMMUNICATE_ENABLE 0x79
#define ROUTER_TABLE 0x7A
#define SET_SOFTWARE_REBOOT_MODE 0x12
#define SW1_SOFTWARE_REBOOT_FLAG 0x95
#define SW2_SOFTWARE_REBOOT_FLAG 0x96

#define SET_BINDING_PARA_REBOOT_MODE 0x7B
#define REBOOT_CMD 0x10
#define TIME_SYNC_SOURCE 0x7C

#define PORT_FC_ENABLE_CMD_LENGTH 5
#define PORT_TIME_SYNC_ENABLE_CMD_LENGTH 5
#define PORT_IP_ENABLE_CMD_LENGTH 5
#define PORT_RESET_CMD_LENGTH 4
#define PORT_FC_ID_CFG_CMD_LENGTH 8
#define PORT_MAC_IP_CFG_CMD_LENGTH 18
#define PORT_IP_BANDWIDTH_CFG_CMD_LENGTH 5
#define TIME_SYNC_PERIOD_CMD_LENGTH 5
#define TIME_SYNC_PPS_CMD_LENGTH 5
#define CAPSULE_COMMUNICATE_ENABLE_CMD_LENGTH 6
#define ROUTER_TABLE_CMD_LENGTH 14
#define SET_SOFTWARE_REBOOT_MODE_CMD_LENGTH 4

#define SET_BINDING_PARA_REBOOT_MODE_CMD_LENGTH 4
#define REBOOT_CMD_LENGTH 3
#define TIME_SYNC_SOURCE_LENGTH 4

#define INTO_DATA_DATASIZE 0x20
#define FLASH_RESTRUCT_IMAGE_BOOT_TIME 1 

#define MAX_BAND_WIDTH 0x14
#pragma pack(1)
typedef struct {
    u8 hostID;
    u8 switchNum;
} SwitchLocMap;

typedef struct {
    u8 remoteLoc1;
    u8 remoteLoc2;
} CapsuleLocationMap;

typedef struct {
    u8 functionFlag;
    u8 (*dealFun)(u8*);
    u8 dataSize;
} IntoDataFunMatchTbl;

#if 0
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
#endif
#pragma pack()

/************************** Function Prototypes ******************************/
void DealIntoData(SpiDataInfoStruct *spiDataInfo);
void StoreTheIntoDataToRingBuf(SpiDataInfoStruct *spiDataInfo);
#endif