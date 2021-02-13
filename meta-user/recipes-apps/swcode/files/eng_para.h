/*
 * eng_para.h
 *
 *  Created on: 2019Äê9ÔÂ22ÈÕ
 *      Author: ´÷¶û
 */

#ifndef ENG_PARA_H_
#define ENG_PARA_H_
#include "dig_para.h"
#include "common.h"
#define DATE_IN_RETURN_DATA_SIZE 32
#define ENG_PARA_LENGTH 2048

#define ADDR_TBL_ONEPAGESIZE 32
#define ROUTER_TBL_ONEPAGESIZE 32

#define PORTNUM_DATA_MANAGEMENT 10
#define PORTNUM_SLAVE_DATA_MANAGEMENT 4

#define AXIS_P_CREDIT_COUNT_OFFSET 0x28
#define ENGPARA_FRAME_HEADER_SW_HXC      0x0F41
#define ENGPARA_FRAME_HEADER_SW_SYC_I  0x0F44
#define ENGPARA_FRAME_HEADER_SW_SYC_II 0x0F47

#define SUBSYSTEM_FLAG_MICU01_SW01 0x1A03
#define SUBSYSTEM_FLAG_MICU01_SW02 0x1A04 //reserved

#define SUBSYSTEM_FLAG_MICU02_SW03 0x1A05
#define SUBSYSTEM_FLAG_MICU02_SW04 0x1A06 //reserved

#define SUBSYSTEM_FLAG_MICU03_SW01 0x1A03
#define SUBSYSTEM_FLAG_MICU03_SW02 0x1A04
#define SUBSYSTEM_FLAG_MICU04_SW03 0x1A05
#define SUBSYSTEM_FLAG_MICU04_SW04 0x1A06

#define SUBSYSTEM_FLAG_MICU05_SW01 0x1A03
#define SUBSYSTEM_FLAG_MICU05_SW02 0x1A04
#define SUBSYSTEM_FLAG_MICU06_SW03 0x1A05
#define SUBSYSTEM_FLAG_MICU06_SW04 0x1A06

#define RESEVED_PARA_FILL_DATA_HX 0x1A
#define RESEVED_PARA_FILL_DATA_SYI 0x2A
#define RESEVED_PARA_FILL_DATA_SYII 0x3A
#define RESERVED_PARA_LENGTH_FOR_SYC 31
#define RESERVED_PARA_LENGTH_FOR_HXC 86

#define MAC_NOTMATCH_DROP_FRAME_COUNT 8*4 // RX_IPFC_MAC_DROP_COUNT
#define QUERY_ROUTETBL_DROP_FRAME_COUNT 9*4 // RX_IPFC_RT_DROP_COUNT
#define RX_DATA_FRAME_RIGHT_BYTE_COUNT 10*4 // RX_IPFC_BYTE_COUNT
#define IPFC_BANDWIDTH_OCCUPY 11*4 // RX_IPFC_BYTE_COUNT_1MS
#define RX_ARP_RIGHT_COUNT 12*4 // RX_IPFC_ARP_COUNT
#define TX_DATA_FRAME_RIGHT_BYTE_COUNT 13*4 // TX_IPFC_BYTE_COUNT
#define TX_ARP_RIGHT_COUNT 14*4 // TX_IPFC_ARP_COUNT
#define RX_CREDIT_COUNT 15*4 // TX_IPFC_CREDIT
#define PORT_BANDWIDTH  0x1DB5//(4.25 * 80/100(8b/10b coding loss) / 8 / 60000 * 1024 * 1024 * 1024 Byte)

#define AXIS_PPM_RX_CMD_RIGHT_COUNT_OFFSET 0x58
#define AXIS_PPM_RX_STATUE_RIGHT_COUNT_OFFSET 0x5C
#define AXIS_PPM_RX_DATA_RIGHT_COUNT_OFFSET 0x60
#define AXIS_PPM_RX_DATA_RIGHT_BYTE_COUNT_OFFSET 0x64
#define AXIS_PPM_RX_QUERY_TIMEOUT_COUNT_OFFSET 0x68
#define AXIS_PPM_RX_NO_DEST_ID_COUNT_OFFSET 0x6C

#define AXIS_PPM_TX_CMD_RIGHT_COUNT_OFFSET 0x70
#define AXIS_PPM_TX_STATUE_RIGHT_COUNT_OFFSET 0x74
#define AXIS_PPM_TX_DATA_RIGHT_COUNT_OFFSET 0x78
#define AXIS_PPM_TX_DATA_RIGHT_BYTE_COUNT_OFFSET 0x7C

#define TIME_SYNC_REG_0 0     
#define TIME_SYNC_REG_3 3*4   
#define TIME_SYNC_REG_4 4*4
#define TIME_SYNC_REG_5 5*4
#define PPS_AVILIED_REG_6 6*4
#define LOCATION_FORWARD_PACKGE_NUM 16*4

#define TIME_SYNC_REG_BASE_ADDR 0x44C20000
#define TIME_SYNC_VERSION  0
#define TIME_SYNC_PARA_SET 1*4
#define TIME_SYNC_PORT_DID 2*4

#define TIME_SYNC_SID 3*4
#define TIME_SYNC_STATE 4*4

#define SYC_LOCDATA_4BYTES_COUNT 24
#define HXC_LOCDATA_4BYTES_COUNT 11
#define SYC_RESERVED_BYTES 68
#define HXC_RESERVED_BYTES 120

#define TIME_SYNC_SOURCE_HXC_BD_GPS 0
#define TIME_SYNC_SOURCE_SYCI_BD_GPS 1
#define TIME_SYNC_SOURCE_SYCII_BD_GPS 2
#define TIME_SYNC_SOURCE_GNSS 3

#pragma pack(1)

typedef struct EngHeaderMapTbl
{
    u16 framHeader;
    u16 subSystem;
} EngHeaderMapTbl;


typedef union {
    u8 value;
    struct 
    {
        unsigned locationTXSuccuess : 1;
        unsigned reserved1 : 1;
        unsigned reserved2 : 1;
        unsigned reserved3 : 1;
        unsigned reserved4 : 1;
        unsigned reserved5 : 1;
        unsigned reserved6 : 1;
        unsigned reserved7 : 1;
    } value_b;
} TimeSyncStatue;

typedef union  {
    u16 value;
    struct
    {
        unsigned port0  :1;//0-disable, 1-enable
        unsigned port1  :1;
        unsigned port2  :1;
        unsigned port3  :1;
        unsigned port4  :1;
        unsigned port5  :1;
        unsigned port6  :1;
        unsigned port7  :1;
        unsigned port8  :1;
        unsigned port9  :1;
        unsigned port10  :1;
        unsigned port11  :1;
        unsigned port12  :1;
        unsigned port13  :1;
        unsigned port14  :1;
        unsigned port15  :1;
    } BitValue;
}PortFcEnableStruct;

typedef union
{
    u16 value;
    struct
    {
        unsigned port0  :1;//0-no credit, 1-has credit
        unsigned port1  :1;
        unsigned port2  :1;
        unsigned port3  :1;
        unsigned port4  :1;
        unsigned port5  :1;
        unsigned port6  :1;
        unsigned port7  :1;
        unsigned port8  :1;
        unsigned port9  :1;
        unsigned port10  :1;
        unsigned port11  :1;
        unsigned port12  :1;
        unsigned port13  :1;
        unsigned port14  :1;
        unsigned port15  :1;
    } BitValue;
} IpCreditStatus;

typedef struct {
    u32 txWordCount;
    u32 rxWordCount;
    u16 txFrameCount;
    u16 rxFrameCount;
    u16 txRreadyCount;
    u16 rxRreadyCount;
    u16 reserved;
    u8 rxCRCErrorCnt;
    u8 portCredit;
} Fc2PDataCountStruct;

typedef union {
    u32 value;
     struct {
        u32 nsOffset : 20;
        u32 msOffset : 12;
     } BitValue;
} TimeSyncMsAndNsOffset;

typedef union {
    u8 resBindingWrongPkgValue;
    struct {
        u8 restructBindingParaPkg : 4; // For sync
        u8 restructBindingParaWrongPkg : 4; // For sync
    } BitValue;
} ResBindingParaCount;

typedef union {
    u16 restructBindingParaNum;
    u16 restructFirstWongPkgSeg;
} ResBindOrFistPackage;

typedef struct {
    u16 currentPageNum;
    u16 TotalPageNum;
} AddrNum;

typedef struct {
    u8   port_num;
    u8   port_did[3];
} AddrFormat;

typedef struct {
    u16 currentPageNum;
    u16 TotalPageNum;    
} RouterNumFormat;

typedef struct {
    u32  nextIp;    
    u32  destIp;
    u16 destPort;
} RouterFormat;

typedef struct {
    u32  ipAddr;
    u8    macAddr[6];    
    u16    portNum;
} ArpFormat;

typedef struct {
    u16  rx_addr_timeout_lost;
    u16  rx_no_did_lost;
    u32  tx_data_byte_right;
    u32  rx_data_byte_right;
    u16  tx_data_frame_right;
    u16  rx_data_frame_right;
    u16  tx_sts_frame_right;
    u16  rx_sts_frame_right;
    u16  tx_cmd_frame_rigth;
    u16  rx_cmd_frame_rigth;
} FcAeNum;

typedef struct {
    u16 rx_MAC_error_lost;
    u16 rx_route_timeout_lost;
    u32 tx_data_byte_right;
    u32 rx_data_byte_right;
    u8 tx_ARP_rigth;
    u8 rx_ARP_right;
    u16 rx_credit;
} FcIpNum;

typedef struct {
    u32 tx_right;
    u32  rx_rigth;
} TsNum;

typedef union {
	u16 value;	
	struct 
	{	
		unsigned port0  :1;//0-disable, 1-enable
		unsigned port1  :1;
		unsigned port2  :1;
		unsigned port3  :1;
		unsigned port4  :1;
		unsigned port5  :1;
		unsigned port6  :1;
		unsigned port7  :1;
		unsigned port8  :1;
		unsigned port9  :1;
		unsigned port10  :1;
		unsigned port11  :1;
		unsigned port12  :1;
		unsigned port13  :1;
		unsigned port14  :1;
		unsigned port15  :1; 
	} value_b;
} PortTsStatue;

typedef struct {
    u32 syncWord; // 0x352EF853
        
    u16 frame_header;//01ji:0x0F41
    u16 package_seg_control;

    u16 package_lenth; //2033, 0x7f1
    u16 data_type;//0xED1D

    u16 sub_system;  //sw1-0x1A03,sw3-0x1A05
    u8 time[6];

    u32 packet_num;        //24+

    DigDataFormat digData; // 14+
    u16 digParaReserved; 
    
    u32 soft_version;
    u32 logic_version;

    u16  FPGA_temperature;
    u16  VccInt;

    u16  VccAux;
    u16  VccBram; 

    u8     recv_zk_right_cmd_num ;
    u8     recv_zk_error_cmd_num ;
    
    u16  restruct_pkt_num; // For software restruct
    u16  restruct_pkg_wrong_num; // For software restruct
    
    ResBindOrFistPackage  resBindingOrFirstWrongPakge; // HXC and SYC share the paramater.                                                   //HXC used as restruct binding para pacakge count. SYC used as first wrong package num
    ResBindingParaCount resBindingParaNum; // HXC and SYC share the paramater. HXC used as resBindingWrongPkgValue
    
    u8 digCollectErrorNum; 
    u8 mstDataDiscardNum; 
    u8 reset_num;                    //34+

    Fc2PDataCountStruct  port2pDataCnt[16]; // 11*16   176+
    FcAeNum FC_AE_number[16];   // 28*16   448+
    FcIpNum FC_IP_number[16];  // 16*16   256+
    TsNum TS_number[16];   // 10*16   160+

    //1100
    u8 IPBW[16];                     // 16+

    PortTsStatue ts_enable; 
    u8 timeSyncSource;
    u8 timeSyncReserved;
    u16 timeGrade;
    u16 timeSyncPerid;
    u32 ppsPeriod;
    u32 ppsPulseWidth;
    u32 timeSyncSecOffset;
    TimeSyncMsAndNsOffset msAndNsOffsest;
    
    AddrNum  addr_NO;         // 7+
    AddrFormat  addr[32];           // 4*32 128+
    RouterNumFormat  route_NO;       // 1+
    RouterFormat        route[32];   // 9*32 288+
    ArpFormat   ARP[16];            // 11*16 176+
    //1728
    PortFcEnableStruct portFcEnable;
    IpCreditStatus IP_credit_sts;
    u8 clc_level;
    u32 locatorData[24];               // 41 for hxc, 96 for syc
    u8 reserved[123];                  // 123 for hxc, 68 for syc

    u8 datain_return[DATE_IN_RETURN_DATA_SIZE];             // 32+
    u32  crc;                         // 4+
    //2048
} eng_para_in_buf; // 2048 + 1


#pragma pack()

void *UpdataEngPara(void * arg);
void DealAndSendEngPara(u8 port_num, eng_para_in_buf *para);
void FillEngParaData(eng_para_in_buf *engParaBuffer);
void InitEngParaSem(void);
void FillStaticDataForEng(void);
void GetAndSendEngPara(void);
void InitPortStaticData(void);
void InitAllGlobalVariable(void);

#endif /* ENG_PARA_H_ */
