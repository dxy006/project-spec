#ifndef ZK_COMMUNICATE_H
#define ZK_COMMUNICATE_H

#define INVALID_DATA 0xFF
#define SPI_DATA_SIZE 1024

#define LENGTH_MAP_TBL 3
#define RESTRUCT_DATA_MAX_SIZE 756
#define INTO_DATA_MAX_SIZE     26
#define UTC_TIME_MAX_SIZE      6
#define NAME_SPI_DATA_FIFO "SpiFifo"
#define NAME_INTODAYA_RET_FIFO "IntoRetFifo"

void *DealSpiData(void * arg);
#pragma pack(1)

typedef struct {
	unsigned bindingParaState :1; //0:origin; 1:restruct
	unsigned softwareState    :1; //0:origin; 1:restruct
    unsigned reserved1 : 1;
    unsigned reserved2 : 1;
    unsigned reserved3 : 1;
    unsigned reserved4 : 1;
    unsigned reserved5 : 1;
    unsigned reserved6 : 1;
} BootState; // Restore the current systerm run state.

typedef struct {
    u32 dataType;
    u32 maxLength;
} SpiDataMaxLengthMapTbl;


typedef struct {
    u32 engPackageNum;
    
    u8 rXRightCommandNum;
    u8 rXErrorCommandNum;
    u8 rXRightCmdForDig;
    u8 rXWrongCmdForDig;    
    u16 restructPkgNum;
    
    u16 restructPkgRightNum;
    u16 restructFirstErrorNum;
    
    u16 restructErrorNum;
    u8 restructBindingParaErrorNum;
    u8 restructBindingParaTotalNum;
    u8 digCollectErrorNum;
    
    u8 mstDataDiscardNum;
    u8 resetNum;
    u8 restructState;
    u8 utcTime[6];
    BootState bootStateInfo;
    u8 resetFlag;
    u16 portFcEnableFlag;
    u16 timeSyncEnableFlag;
    u16 timeSyncPeriod;
    u8 ipBandWidth[FC_PORT_NUM_INSTANCES];
    u8 timeSyncGrade;
    u8 timeSyncSource;
} MstTempFlag;

typedef struct {
    u16 dataSize; //CRC is not included
    u8 dataType;
    u8 spiData[SPI_DATA_SIZE];
    u8 crc;
} SpiDataInfoStruct;

#pragma pack()

void *QuerySpiData(void * arg);
void *GetAndDealSpiData(void * arg);
INT32 InitFiFo(void);
INT32 GetIntoDataRetPointer(void);
u8 CalculateCrc(SpiDataInfoStruct *spiDataInfo);

#endif
