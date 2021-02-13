#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long long u64;
typedef signed int INT32;
typedef signed char INT8;

#define RET_SUCCESS 0
#define RET_FAILED 1
#define PORT_IS_VALID 1
#define PORT_NOT_VALID 0

#define TOTAL_SW_LOCATION_NUM 12
#define FC_PORT_NUM_INSTANCES 16
#define MAX_PORT_TABLE_NUM 64
#define MAX_ROUTER_TABLE_NUM 64

#define UPDATE_FLASH_DELAY_TIME 20

#define ALL_PORT_FLAG 0xFF
#define ENABLE_FLAG 0x0F 
#define DISABLE_FLAG 0xF0

#define PORT_RST            0
#define PORT_READY          1
#define PORT_PHY_RDY        2
#define PORT_DELETED        3
#define QEURY_SPI_SCHE_INTERVEL 1
#define DEAL_SPI_DATA_SCHE_INTERVEL 10

#define RESET_WAY_NO_RESET 0
#define RESET_WAY_COMMAND_RESET 1
#define RESET_WAY_ABNORMAL_RESET 2

#define RESET_ENABLE 1

#define ENV_ERRO (-1)
#define GET_CABIND_LOCATION(location) (((location) >> 2) & 0xFF)
#endif
