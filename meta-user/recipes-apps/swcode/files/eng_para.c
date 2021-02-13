/*
 * eng_para.c
 *
 *  Created on: 2019Äê9ÔÂ22ÈÕ
 *      Author: ´÷¶û
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "eng_para.h"
#include "fc_port_mon.h"
#include "binding_para.h"
#include "common.h"
#include "into_data.h"
#include "time_sync.h"
#include "sys_print.h"

volatile u32 g_engCnt = 0;
extern DigDataSendFormat g_digUpdateBuffer;
extern MstTempFlag g_ZKTempFlag;
extern u8 g_swLocation;
extern u32 g_validPortNum;
extern u32 g_softWareVersion;
u8 g_engPrintEnable = 0;


const EngHeaderMapTbl g_engHeaderMapTbl[TOTAL_SW_LOCATION_NUM] = {
    {ENGPARA_FRAME_HEADER_SW_HXC,   SUBSYSTEM_FLAG_MICU01_SW01}, //MICU01-SW01
    {ENGPARA_FRAME_HEADER_SW_HXC,   SUBSYSTEM_FLAG_MICU01_SW02}, //Invalid
    {ENGPARA_FRAME_HEADER_SW_HXC,   SUBSYSTEM_FLAG_MICU02_SW03}, //MICU02-SW03
    {ENGPARA_FRAME_HEADER_SW_HXC,   SUBSYSTEM_FLAG_MICU02_SW04}, //Invalid
    
    {ENGPARA_FRAME_HEADER_SW_SYC_I, SUBSYSTEM_FLAG_MICU03_SW01}, //MICU03_SW01
    {ENGPARA_FRAME_HEADER_SW_SYC_I, SUBSYSTEM_FLAG_MICU03_SW02}, //MICU03_SW02 
    {ENGPARA_FRAME_HEADER_SW_SYC_I, SUBSYSTEM_FLAG_MICU04_SW03}, //MICU04_SW03 
    {ENGPARA_FRAME_HEADER_SW_SYC_I, SUBSYSTEM_FLAG_MICU04_SW04}, //MICU04_SW04 
    
    {ENGPARA_FRAME_HEADER_SW_SYC_II, SUBSYSTEM_FLAG_MICU05_SW01},//MICU05_SW01
    {ENGPARA_FRAME_HEADER_SW_SYC_II, SUBSYSTEM_FLAG_MICU05_SW02},//MICU05_SW02
    {ENGPARA_FRAME_HEADER_SW_SYC_II, SUBSYSTEM_FLAG_MICU06_SW03},//MICU06_SW03
    {ENGPARA_FRAME_HEADER_SW_SYC_II, SUBSYSTEM_FLAG_MICU06_SW04} //MICU06_SW04
};

const u8 g_defaultTimeSyncSource[TOTAL_SW_LOCATION_NUM] = {
    TIME_SYNC_SOURCE_HXC_BD_GPS, TIME_SYNC_SOURCE_HXC_BD_GPS,
    TIME_SYNC_SOURCE_HXC_BD_GPS, TIME_SYNC_SOURCE_HXC_BD_GPS,
    
    TIME_SYNC_SOURCE_SYCI_BD_GPS, TIME_SYNC_SOURCE_SYCI_BD_GPS,
    TIME_SYNC_SOURCE_SYCI_BD_GPS, TIME_SYNC_SOURCE_SYCI_BD_GPS,

    TIME_SYNC_SOURCE_SYCII_BD_GPS, TIME_SYNC_SOURCE_SYCII_BD_GPS,    
    TIME_SYNC_SOURCE_SYCII_BD_GPS, TIME_SYNC_SOURCE_SYCII_BD_GPS
};


const u8 g_reserveDataMapTbl[TOTAL_SW_LOCATION_NUM] = {
    RESEVED_PARA_FILL_DATA_HX,
    RESEVED_PARA_FILL_DATA_HX,
    RESEVED_PARA_FILL_DATA_HX,
    RESEVED_PARA_FILL_DATA_HX,

    RESEVED_PARA_FILL_DATA_SYI,
    RESEVED_PARA_FILL_DATA_SYI,
    RESEVED_PARA_FILL_DATA_SYI,
    RESEVED_PARA_FILL_DATA_SYI,

    RESEVED_PARA_FILL_DATA_SYII,
    RESEVED_PARA_FILL_DATA_SYII,
    RESEVED_PARA_FILL_DATA_SYII,
    RESEVED_PARA_FILL_DATA_SYII
};


static const u32 crc32Tab[256] =
{
    0x00000000,  0x04c11db7,  0x09823b6e,  0x0d4326d9,
    0x130476dc,  0x17c56b6b,  0x1a864db2,  0x1e475005,
    0x2608edb8,  0x22c9f00f,  0x2f8ad6d6,  0x2b4bcb61,
    0x350c9b64,  0x31cd86d3,  0x3c8ea00a,  0x384fbdbd,
    0x4c11db70,  0x48d0c6c7,  0x4593e01e,  0x4152fda9,
    0x5f15adac,  0x5bd4b01b,  0x569796c2,  0x52568b75,
    0x6a1936c8,  0x6ed82b7f,  0x639b0da6,  0x675a1011,
    0x791d4014,  0x7ddc5da3,  0x709f7b7a,  0x745e66cd,
    0x9823b6e0,  0x9ce2ab57,  0x91a18d8e,  0x95609039,
    0x8b27c03c,  0x8fe6dd8b,  0x82a5fb52,  0x8664e6e5,
    0xbe2b5b58,  0xbaea46ef,  0xb7a96036,  0xb3687d81,
    0xad2f2d84,  0xa9ee3033,  0xa4ad16ea,  0xa06c0b5d,
    0xd4326d90,  0xd0f37027,  0xddb056fe,  0xd9714b49,
    0xc7361b4c,  0xc3f706fb,  0xceb42022,  0xca753d95,
    0xf23a8028,  0xf6fb9d9f,  0xfbb8bb46,  0xff79a6f1,
    0xe13ef6f4,  0xe5ffeb43,  0xe8bccd9a,  0xec7dd02d,

    0x34867077,  0x30476dc0,  0x3d044b19,  0x39c556ae,
    0x278206ab,  0x23431b1c,  0x2e003dc5,  0x2ac12072,
    0x128e9dcf,  0x164f8078,  0x1b0ca6a1,  0x1fcdbb16,
    0x018aeb13,  0x054bf6a4,  0x0808d07d,  0x0cc9cdca,
    0x7897ab07,  0x7c56b6b0,  0x71159069,  0x75d48dde,
    0x6b93dddb,  0x6f52c06c,  0x6211e6b5,  0x66d0fb02,
    0x5e9f46bf,  0x5a5e5b08,  0x571d7dd1,  0x53dc6066,
    0x4d9b3063,  0x495a2dd4,  0x44190b0d,  0x40d816ba,
    0xaca5c697,  0xa864db20,  0xa527fdf9,  0xa1e6e04e,
    0xbfa1b04b,  0xbb60adfc,  0xb6238b25,  0xb2e29692,
    0x8aad2b2f,  0x8e6c3698,  0x832f1041,  0x87ee0df6,
    0x99a95df3,  0x9d684044,  0x902b669d,  0x94ea7b2a,
    0xe0b41de7,  0xe4750050,  0xe9362689,  0xedf73b3e,
    0xf3b06b3b,  0xf771768c,  0xfa325055,  0xfef34de2,
    0xc6bcf05f,  0xc27dede8,  0xcf3ecb31,  0xcbffd686,
    0xd5b88683,  0xd1799b34,  0xdc3abded,  0xd8fba05a,

    0x690ce0ee,  0x6dcdfd59,  0x608edb80,  0x644fc637,
    0x7a089632,  0x7ec98b85,  0x738aad5c,  0x774bb0eb,
    0x4f040d56,  0x4bc510e1,  0x46863638,  0x42472b8f,
    0x5c007b8a,  0x58c1663d,  0x558240e4,  0x51435d53,
    0x251d3b9e,  0x21dc2629,  0x2c9f00f0,  0x285e1d47,
    0x36194d42,  0x32d850f5,  0x3f9b762c,  0x3b5a6b9b,
    0x0315d626,  0x07d4cb91,  0x0a97ed48,  0x0e56f0ff,
    0x1011a0fa,  0x14d0bd4d,  0x19939b94,  0x1d528623,
    0xf12f560e,  0xf5ee4bb9,  0xf8ad6d60,  0xfc6c70d7,
    0xe22b20d2,  0xe6ea3d65,  0xeba91bbc,  0xef68060b,
    0xd727bbb6,  0xd3e6a601,  0xdea580d8,  0xda649d6f,
    0xc423cd6a,  0xc0e2d0dd,  0xcda1f604,  0xc960ebb3,
    0xbd3e8d7e,  0xb9ff90c9,  0xb4bcb610,  0xb07daba7,
    0xae3afba2,  0xaafbe615,  0xa7b8c0cc,  0xa379dd7b,
    0x9b3660c6,  0x9ff77d71,  0x92b45ba8,  0x9675461f,
    0x8832161a,  0x8cf30bad,  0x81b02d74,  0x857130c3,

    0x5d8a9099,  0x594b8d2e,  0x5408abf7,  0x50c9b640,
    0x4e8ee645,  0x4a4ffbf2,  0x470cdd2b,  0x43cdc09c,
    0x7b827d21,  0x7f436096,  0x7200464f,  0x76c15bf8,
    0x68860bfd,  0x6c47164a,  0x61043093,  0x65c52d24,
    0x119b4be9,  0x155a565e,  0x18197087,  0x1cd86d30,
    0x029f3d35,  0x065e2082,  0x0b1d065b,  0x0fdc1bec,
    0x3793a651,  0x3352bbe6,  0x3e119d3f,  0x3ad08088,
    0x2497d08d,  0x2056cd3a,  0x2d15ebe3,  0x29d4f654,
    0xc5a92679,  0xc1683bce,  0xcc2b1d17,  0xc8ea00a0,
    0xd6ad50a5,  0xd26c4d12,  0xdf2f6bcb,  0xdbee767c,
    0xe3a1cbc1,  0xe760d676,  0xea23f0af,  0xeee2ed18,
    0xf0a5bd1d,  0xf464a0aa,  0xf9278673,  0xfde69bc4,
    0x89b8fd09,  0x8d79e0be,  0x803ac667,  0x84fbdbd0,
    0x9abc8bd5,  0x9e7d9662,  0x933eb0bb,  0x97ffad0c,
    0xafb010b1,  0xab710d06,  0xa6322bdf,  0xa2f33668,
    0xbcb4666d,  0xb8757bda,  0xb5365d03,  0xb1f740b4,
};

#if 0
void InitPortStaticData(void)
{
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) {
        return;
    }

    CascadePortInfo cascadePortInfo = {0};
    g_digUpdateBuffer.digData.ipStates.value = 0;
    g_ZKTempFlag.portFcEnableFlag = 0xFFFF;
    g_engParaBuffer.IP_credit_sts.value = 0xFFFF;
    g_ZKTempFlag.timeSyncGrade = 0;
    g_ZKTempFlag.timeSyncEnableFlag = 0xFFFF;
    g_ZKTempFlag.timeSyncPeriod = TS_SYNC_PERIOD_DEFAULT_VALUE;
    GetCascadePortSetting(&cascadePortInfo);
    g_ZKTempFlag.timeSyncSource =  g_defaultTimeSyncSource[g_swLocation];

    if (cascadePortInfo.remoteCascadePort1 != INVALID_CASCADE_PORT) {
        g_digUpdateBuffer.digData.ipStates.value &= (~(0x01 << cascadePortInfo.remoteCascadePort1));
        g_ZKTempFlag.portFcEnableFlag &= (~(0x01 << cascadePortInfo.remoteCascadePort1));
    }
    
    if (cascadePortInfo.remoteCascadePort2 != INVALID_CASCADE_PORT) {
        g_digUpdateBuffer.digData.ipStates.value &= (~(0x01 << cascadePortInfo.remoteCascadePort2));
        g_ZKTempFlag.portFcEnableFlag &= (~(0x01 << cascadePortInfo.remoteCascadePort2));
    }
    
    g_digUpdateBuffer.digData.tsStatue.value = 0;
    g_digUpdateBuffer.digData.interConnectState.BitValue.cascadePort2FcEnableStatus = 0;
    g_digUpdateBuffer.digData.interConnectState.BitValue.cascadePort2IpFcEnableStatus = 0;
    g_digUpdateBuffer.digData.interConnectState.BitValue.cascadePort1FcEnableStatus = 0;
    g_digUpdateBuffer.digData.interConnectState.BitValue.cascadePort1IpFcEnableStatus = 0;
    return;
}

void FillStaticDataForEng(void)
{
    u32 i;
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) {
        return;
    }
    g_engParaBuffer.syncWord = 0x352EF853;
    g_engParaBuffer.frame_header = g_engHeaderMapTbl[g_swLocation].framHeader;
    g_engParaBuffer.package_lenth = 0x7F1; // 2033
    g_engParaBuffer.data_type = 0xED1D;

    g_engParaBuffer.sub_system = g_engHeaderMapTbl[g_swLocation].subSystem;
    g_engParaBuffer.soft_version = g_softWareVersion;
    g_engParaBuffer.logic_version = GetLogicVersion();
    memset(&(g_engParaBuffer.digParaReserved), g_reserveDataMapTbl[g_swLocation], 2);
    memset(&(g_engParaBuffer.timeSyncReserved), g_reserveDataMapTbl[g_swLocation], 1);
    
    for (i = 0; i < FC_PORT_NUM_INSTANCES; i++) {  
        memset(&(g_engParaBuffer.port2pDataCnt[i].reserved), g_reserveDataMapTbl[g_swLocation], 2);
    }
    return;
}

void FillFc2PData(eng_para_in_buf *engParaBuffer)
{
    u32 i;
    for (i = 0; i < FC_PORT_NUM_INSTANCES; i++) {
        JUMP_OVER_UNUSED_PORT(i);
        
        /* 2p count */
        engParaBuffer->port2pDataCnt[i].portCredit = (u8)fc_drv_read(FC_PORT_REG_TABLE[i],
            AXIS_P_CREDIT_COUNT_OFFSET);
        engParaBuffer->port2pDataCnt[i].rxCRCErrorCnt = (u8)fc_drv_read(FC_PORT_REG_TABLE[i],
            RX_CRC_ERR_CNT_STAT_OFFSET);
        engParaBuffer->port2pDataCnt[i].rxRreadyCount = (u16)fc_drv_read(FC_PORT_REG_TABLE[i],
            RX_RRDY_CNT_STAT_OFFSET);
        engParaBuffer->port2pDataCnt[i].txRreadyCount = (u16)fc_drv_read(FC_PORT_REG_TABLE[i],
            TX_RRDY_CNT_STAT_OFFSET);
        engParaBuffer->port2pDataCnt[i].rxFrameCount = (u16)fc_drv_read(FC_PORT_REG_TABLE[i],
            RX_FRAME_CNT_STAT_OFFSET);
        engParaBuffer->port2pDataCnt[i].txFrameCount = (u16)fc_drv_read(FC_PORT_REG_TABLE[i],
            TX_FRAME_CNT_STAT_OFFSET);
        engParaBuffer->port2pDataCnt[i].rxWordCount = fc_drv_read(FC_PORT_REG_TABLE[i],
            RX_WORD_CNT_STAT_OFFSET);
        engParaBuffer->port2pDataCnt[i].txWordCount = fc_drv_read(FC_PORT_REG_TABLE[i],
            TX_WORD_CNT_STAT_OFFSET);        
    }    
    return;
}

void FillStatusData(eng_para_in_buf *engParaBuffer)
{
    g_ZKTempFlag.engPackageNum++;
    if (g_ZKTempFlag.engPackageNum == 0) {
        g_ZKTempFlag.engPackageNum = 1; //engPackageNum start from 1
    }
    engParaBuffer->packet_num = g_ZKTempFlag.engPackageNum;  

    memcpy(&(engParaBuffer->digData), &(g_digUpdateBuffer.digData), sizeof(DigDataFormat));
    /* Fill the time sync data for engpara, ignore the data set in dig request */
    FillTimeSyncStatusForEngPara(&(engParaBuffer->digData));
    FillUtcTimeFromLocData(&(engParaBuffer->digData));
    FillCascadePortStatue(&(engParaBuffer->digData));
    engParaBuffer->FPGA_temperature = (u16)fc_drv_read(XPAR_XADC_WIZ_0_BASEADDR, CORE_TEMPRATURE_OFFSET);
    engParaBuffer->VccInt = (u16)fc_drv_read(XPAR_XADC_WIZ_0_BASEADDR, FPGA_VOLTAGE_INT_OFFSET);
    engParaBuffer->VccAux = (u16)fc_drv_read(XPAR_XADC_WIZ_0_BASEADDR, FPGA_VOLTAGE_AUX_OFFSET);
    engParaBuffer->VccBram = (u16)fc_drv_read(XPAR_XADC_WIZ_0_BASEADDR, FPGA_VOLTAGE_BRAM_OFFSET);
    return;
}

void FillStatisticsData(eng_para_in_buf *engParaBuffer)
{
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) {
        return;
    }    
    engParaBuffer->recv_zk_right_cmd_num = g_ZKTempFlag.rXRightCommandNum;
    engParaBuffer->recv_zk_error_cmd_num = g_ZKTempFlag.rXErrorCommandNum;
    engParaBuffer->restruct_pkt_num = g_ZKTempFlag.restructPkgNum;
    engParaBuffer->restruct_pkg_wrong_num = g_ZKTempFlag.restructErrorNum;
    if (IS_LOCATION_SYC(g_swLocation) != 0) {
        engParaBuffer->resBindingOrFirstWrongPakge.restructFirstWongPkgSeg = g_ZKTempFlag.restructFirstErrorNum;
        
        engParaBuffer->resBindingParaNum.BitValue.restructBindingParaPkg =
            g_ZKTempFlag.restructBindingParaTotalNum & 0xF;
        engParaBuffer->resBindingParaNum.BitValue.restructBindingParaWrongPkg =
            g_ZKTempFlag.restructBindingParaErrorNum & 0xF;
    } else {
        engParaBuffer->resBindingOrFirstWrongPakge.restructBindingParaNum = g_ZKTempFlag.restructBindingParaTotalNum;
        
        engParaBuffer->resBindingParaNum.resBindingWrongPkgValue = g_ZKTempFlag.restructBindingParaErrorNum;
    }
    
    engParaBuffer->digCollectErrorNum = g_ZKTempFlag.digCollectErrorNum; 
    engParaBuffer->mstDataDiscardNum = g_ZKTempFlag.mstDataDiscardNum; 
    engParaBuffer->reset_num = g_ZKTempFlag.resetNum;
    return;
}

void FillFcAe1553Data(eng_para_in_buf *engParaBuffer)
{
    u32 index;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        JUMP_OVER_UNUSED_PORT(index);
        engParaBuffer->FC_AE_number[index].tx_data_byte_right = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_TX_DATA_RIGHT_BYTE_COUNT_OFFSET);
        engParaBuffer->FC_AE_number[index].tx_data_frame_right = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_TX_DATA_RIGHT_COUNT_OFFSET);
        engParaBuffer->FC_AE_number[index].tx_sts_frame_right = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_TX_STATUE_RIGHT_COUNT_OFFSET);
        engParaBuffer->FC_AE_number[index].tx_cmd_frame_rigth = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_TX_CMD_RIGHT_COUNT_OFFSET); 
        engParaBuffer->FC_AE_number[index].rx_no_did_lost = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_RX_NO_DEST_ID_COUNT_OFFSET); 
        engParaBuffer->FC_AE_number[index].rx_addr_timeout_lost = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_RX_QUERY_TIMEOUT_COUNT_OFFSET);
        engParaBuffer->FC_AE_number[index].rx_data_byte_right = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_RX_DATA_RIGHT_BYTE_COUNT_OFFSET);
        engParaBuffer->FC_AE_number[index].rx_data_frame_right = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_RX_DATA_RIGHT_COUNT_OFFSET);
        engParaBuffer->FC_AE_number[index].rx_sts_frame_right = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_RX_STATUE_RIGHT_COUNT_OFFSET);
        engParaBuffer->FC_AE_number[index].rx_cmd_frame_rigth = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            AXIS_PPM_RX_CMD_RIGHT_COUNT_OFFSET);
    }
    return;
}

void FillFcIpData(eng_para_in_buf *engParaBuffer)
{
    u32 index;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        JUMP_OVER_UNUSED_PORT(index);
        engParaBuffer->FC_IP_number[index].rx_credit = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            RX_CREDIT_COUNT);
        engParaBuffer->FC_IP_number[index].rx_ARP_right = (u8)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            RX_ARP_RIGHT_COUNT);
        engParaBuffer->FC_IP_number[index].tx_ARP_rigth = (u8)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            TX_ARP_RIGHT_COUNT);
        engParaBuffer->FC_IP_number[index].rx_data_byte_right = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            RX_DATA_FRAME_RIGHT_BYTE_COUNT);
        engParaBuffer->FC_IP_number[index].tx_data_byte_right = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            TX_DATA_FRAME_RIGHT_BYTE_COUNT);
        engParaBuffer->FC_IP_number[index].rx_route_timeout_lost = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            QUERY_ROUTETBL_DROP_FRAME_COUNT);
        engParaBuffer->FC_IP_number[index].rx_MAC_error_lost = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            MAC_NOTMATCH_DROP_FRAME_COUNT);
    }
    return;
}

void FillFcTsData(eng_para_in_buf *engParaBuffer)
{
#if 1
    engParaBuffer->TS_number[0].rx_rigth = (u16)fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_RX_TS_FRAME_CNT_P0);
    engParaBuffer->TS_number[0].tx_right = (u16)fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_TX_TS_FRAME_CNT_P0);
    
    engParaBuffer->TS_number[1].rx_rigth = (u16)fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_RX_TS_FRAME_CNT_P1);
    engParaBuffer->TS_number[1].tx_right = (u16)fc_drv_read(XPAR_TIMESYNC1588V2_0_BASEADDR,
        TIME_SYNC_TX_TS_FRAME_CNT_P1);
#endif
    return;
}

void FillIpBandWidthData(eng_para_in_buf *engParaBuffer)
{
    u32 index;
    u32 ipBandwidth;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        JUMP_OVER_UNUSED_PORT(index);
        ipBandwidth = (u16)fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[index],
            IPFC_BANDWIDTH_OCCUPY);
        engParaBuffer->IPBW[index] = ipBandwidth * 100 / PORT_BANDWIDTH / 5;
//      engParaBuffer->IPBW[index] = g_ZKTempFlag.ipBandWidth[index];
    }
    return;
}

void FillTimeSynStatusData(eng_para_in_buf *engParaBuffer)
{
    engParaBuffer->ts_enable.value = g_ZKTempFlag.timeSyncEnableFlag;
    engParaBuffer->timeGrade = g_ZKTempFlag.timeSyncGrade;
    engParaBuffer->timeSyncPerid = g_ZKTempFlag.timeSyncPeriod;
    engParaBuffer->timeSyncSource = g_ZKTempFlag.timeSyncSource;
    engParaBuffer->ppsPeriod = fc_drv_read(TIMESYNC_BASEADDR,
        TIME_SYNC_PERID_CNT_OUT); 
    engParaBuffer->ppsPulseWidth = fc_drv_read(TIMESYNC_BASEADDR,
        TIME_SYNC_WIDTH_CNT_OUT);
    engParaBuffer->timeSyncSecOffset = fc_drv_read(TIMESYNC_BASEADDR,
        TIME_SYNC_OFFSET_SECOND); 
    engParaBuffer->msAndNsOffsest.value = fc_drv_read(TIMESYNC_BASEADDR,
        TIME_SYNC_OFFSET_MS_AND_NS); 
    return;
}

void FillAddressTblData(eng_para_in_buf *engParaBuffer)
{
    u32 i;
    static u16 pageNum = 0;
    u16 totalPageNum = MAX_PORT_TABLE_NUM / ADDR_TBL_ONEPAGESIZE;
    
    engParaBuffer->addr_NO.currentPageNum = pageNum;
    engParaBuffer->addr_NO.TotalPageNum = totalPageNum;
    for (i = 0; i < ADDR_TBL_ONEPAGESIZE; i++) {
       engParaBuffer->addr[i].port_num = g_portTbl[pageNum * ADDR_TBL_ONEPAGESIZE + i].portNum;
       memcpy(&(engParaBuffer->addr[i].port_did), &(g_portTbl[pageNum * ADDR_TBL_ONEPAGESIZE + i].portDid), 3);      
    }
    pageNum++;
    if (pageNum >= totalPageNum) {
        pageNum = 0;
    }
    return;
}

void FillRouterTblData(eng_para_in_buf *engParaBuffer)
{
    u32 i;
    static u16 pageNum = 0;
    u16 totalPageNum = MAX_ROUTER_TABLE_NUM / ROUTER_TBL_ONEPAGESIZE;
    engParaBuffer->route_NO.currentPageNum = pageNum;
    engParaBuffer->route_NO.TotalPageNum = totalPageNum;   
   
    for (i = 0; i < ROUTER_TBL_ONEPAGESIZE; i++) {
        engParaBuffer->route[i].destIp = g_routerTbl[pageNum * ADDR_TBL_ONEPAGESIZE + i].destIP;
        engParaBuffer->route[i].nextIp = g_routerTbl[pageNum * ADDR_TBL_ONEPAGESIZE + i].nextIP;
        engParaBuffer->route[i].destPort = (u16)g_routerTbl[pageNum * ADDR_TBL_ONEPAGESIZE + i].portNum;
    }
    pageNum++;
    if (pageNum >= totalPageNum) {
        pageNum = 0;
    }
    return;
}

void FillArpTblData(eng_para_in_buf *engParaBuffer)
{
    u32 i;
    u32 ipAddress;
    for (i = 0; i < FC_PORT_NUM_INSTANCES; i++) {
        engParaBuffer->ARP[i].portNum = (u16)g_localAddressTbl[i].portNum;
        memcpy(engParaBuffer->ARP[i].macAddr, g_localAddressTbl[i].macAddress, 6);
        ipAddress = (u32)(g_localAddressTbl[i].ipAddress[0] << 24) |
            (u32)(g_localAddressTbl[i].ipAddress[1] << 16) |
            (u32)(g_localAddressTbl[i].ipAddress[2] << 8) | 
            (u32)(g_localAddressTbl[i].ipAddress[3]);
        engParaBuffer->ARP[i].ipAddr = ipAddress;
    }    
    return;
}

void FillPortStatusData(eng_para_in_buf *engParaBuffer)
{
    u32 ipCreditStatus;
    u8 portNum;
    u8 creditValue;
    engParaBuffer->portFcEnable.value = g_ZKTempFlag.portFcEnableFlag;

    for (portNum = 0; portNum < FC_PORT_NUM_INSTANCES; ++portNum) {
        JUMP_OVER_UNUSED_PORT(portNum);
        creditValue = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[portNum], RX_CREDIT_COUNT);
        if (creditValue == 0) {
            ipCreditStatus &= (~(1 << portNum));
        } else {
            ipCreditStatus |= (1 << portNum);
        }
    }
    
    engParaBuffer->IP_credit_sts.value = (u16)ipCreditStatus;    
    return;
}

void FillLocatorData(eng_para_in_buf *engParaBuffer)
{
    u32 index ;
    u8 baseOffSetAddr = TIME_SYNC_OFFSET_LOCATOR_DATA_BASE;
    u32 reservedData = 0;  
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) {   
        return;
    }
    if (IS_LOCATION_SYC(g_swLocation) != 0) {
        for (index = 0; index < 24; index++) {
            engParaBuffer->locatorData[index] = fc_drv_read(TIMESYNC_BASEADDR,
                baseOffSetAddr);
            baseOffSetAddr = baseOffSetAddr + 4;
        }
    } else {
        for (index = 0; index < 11; index++) {
            engParaBuffer->locatorData[index] = fc_drv_read(TIMESYNC_BASEADDR,
                baseOffSetAddr);
            baseOffSetAddr = baseOffSetAddr + 4;
        }
        
        reservedData = ((u32)(g_reserveDataMapTbl[g_swLocation]) << 16) |
            ((u32)(g_reserveDataMapTbl[g_swLocation]) << 8) | 
            g_reserveDataMapTbl[g_swLocation];
        engParaBuffer->locatorData[10] = (engParaBuffer->locatorData[10] << 24) | reservedData;
    }
    return;
}


void FillEngParaData(eng_para_in_buf *engParaBuffer)
{
    /* The max num of segPackageNum is 0x3FFF */
    /* The paramater below is for engpara header ,some header data is in FillStaticDataForEng */
    u32 segPackageNum = g_ZKTempFlag.engPackageNum % 0x4000;

    engParaBuffer->package_seg_control = 0xC000 | segPackageNum; //segPackageNum start from 0
    memcpy(engParaBuffer->time, g_ZKTempFlag.utcTime, 6); 
    /* The paramater above is for engpara header */

    FillStatusData(engParaBuffer);
    FillStatisticsData(engParaBuffer);
    FillFc2PData(engParaBuffer);
    FillFcAe1553Data(engParaBuffer);
    FillFcIpData(engParaBuffer);
    FillIpBandWidthData(engParaBuffer);
    FillTimeSynStatusData(engParaBuffer);
    FillAddressTblData(engParaBuffer);
    FillRouterTblData(engParaBuffer);
    FillFcTsData(engParaBuffer);
    FillArpTblData(engParaBuffer);
    FillPortStatusData(engParaBuffer);
    FillLocatorData(engParaBuffer);
    RestoreTheIntoDataToEngPara(engParaBuffer->datain_return);
    /* reserved data is filled in ConvertEngParaFromBufToSend */
    return;
}

void FillPort2PNumber(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        *((*engParaForSend)++) = (u32)(engParaInBuf->port2pDataCnt[index].portCredit) << 24 |
           (u32)(engParaInBuf->port2pDataCnt[index].rxCRCErrorCnt) << 16 |
           (u32)(engParaInBuf->port2pDataCnt[index].reserved);
        
        *((*engParaForSend)++) = (u32)(engParaInBuf->port2pDataCnt[index].rxRreadyCount) << 16 |
            (u32)(engParaInBuf->port2pDataCnt[index].txRreadyCount);
        
        *((*engParaForSend)++) = (u32)(engParaInBuf->port2pDataCnt[index].rxFrameCount) << 16 |
            (u32)(engParaInBuf->port2pDataCnt[index].txFrameCount);
        
        *((*engParaForSend)++) = (u32)(engParaInBuf->port2pDataCnt[index].rxWordCount);

        *((*engParaForSend)++) = (u32)(engParaInBuf->port2pDataCnt[index].txWordCount);
    }
    return;
}

void FillFcIpNumber(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        *((*engParaForSend)++) = (u32)(engParaInBuf->FC_IP_number[index].rx_credit) << 16 
            | (u32)(engParaInBuf->FC_IP_number[index].rx_ARP_right) << 8 
            | engParaInBuf->FC_IP_number[index].tx_ARP_rigth;
        *((*engParaForSend)++) = engParaInBuf->FC_IP_number[index].rx_data_byte_right;
        *((*engParaForSend)++) = engParaInBuf->FC_IP_number[index].tx_data_byte_right;
        *((*engParaForSend)++) = engParaInBuf->FC_IP_number[index].rx_route_timeout_lost
            | engParaInBuf->FC_IP_number[index].rx_MAC_error_lost;
    }
    return;
}

void FillFcAENumber(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        *((*engParaForSend)++) = (u32)(engParaInBuf->FC_AE_number[index].rx_cmd_frame_rigth) << 16
            | engParaInBuf->FC_AE_number[index].tx_cmd_frame_rigth;

        *((*engParaForSend)++) = (u32)(engParaInBuf->FC_AE_number[index].rx_sts_frame_right) << 16
            | engParaInBuf->FC_AE_number[index].tx_sts_frame_right;

        *((*engParaForSend)++) = (u32)(engParaInBuf->FC_AE_number[index].rx_data_frame_right) << 16
            | engParaInBuf->FC_AE_number[index].tx_data_frame_right;

        *((*engParaForSend)++) = engParaInBuf->FC_AE_number[index].rx_data_byte_right;
        
        *((*engParaForSend)++) = engParaInBuf->FC_AE_number[index].tx_data_byte_right;

        *((*engParaForSend)++) = (u32)(engParaInBuf->FC_AE_number[index].rx_no_did_lost) << 16
            | engParaInBuf->FC_AE_number[index].rx_addr_timeout_lost;
    }
    return;
}

void FillTsNumber(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        *((*engParaForSend)++) = engParaInBuf->TS_number[index].rx_rigth;
        *((*engParaForSend)++) = engParaInBuf->TS_number[index].tx_right;
    }
    return;
}

void FillIpBw(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    for (index = 0; index < 4; ++index) {
        *((*engParaForSend)++) = (u32)(engParaInBuf->IPBW[index*4]) << 24 
            | (u32)(engParaInBuf->IPBW[index*4 + 1]) << 16
            | (u32)(engParaInBuf->IPBW[index*4 + 2]) << 8
            | engParaInBuf->IPBW[index*4 + 3];
    }
    return;
}

void FillAddrTblInfo(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    *((*engParaForSend)++) = engParaInBuf->addr_NO.TotalPageNum << 16 |
        engParaInBuf->addr_NO.currentPageNum;
    
    // Attention: addr[0]port_num is added out of here.
    for (index = 0; index < ADDR_TBL_ONEPAGESIZE; ++index) {
        *((*engParaForSend)++) = (u32)(engParaInBuf->addr[index].port_num) << 24 |
            (u32)(engParaInBuf->addr[index].port_did[2]) << 16 |
            (u32)(engParaInBuf->addr[index].port_did[1]) << 8 |
            (u32)(engParaInBuf->addr[index].port_did[0]);
    }
    return;
}


void FillRouterInfo(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    *((*engParaForSend)++) = engParaInBuf->route_NO.TotalPageNum << 16 |
        engParaInBuf->route_NO.currentPageNum;

    for (index = 0; index < ROUTER_TBL_ONEPAGESIZE ; index = index + 2) {
        *((*engParaForSend)++) = ((u32)(engParaInBuf->route[index].destPort) << 16) |
            GET_HIGH16(engParaInBuf->route[index].destIp);
        *((*engParaForSend)++) = GET_LOW16((u32)(engParaInBuf->route[index].destIp)) << 16 |
            GET_HIGH16(engParaInBuf->route[index].nextIp);
        *((*engParaForSend)++) = GET_LOW16(engParaInBuf->route[index].nextIp) << 16 |
            (u32)(engParaInBuf->route[index + 1].destPort);
        *((*engParaForSend)++) = engParaInBuf->route[index + 1].destIp;
        *((*engParaForSend)++) = engParaInBuf->route[index + 1].nextIp;
    }
    return;
}

void FillArpInfo(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    for (index = 0; index < FC_PORT_NUM_INSTANCES; ++index) {
        *((*engParaForSend)++) =  ((u32)(engParaInBuf->ARP[index].portNum) << 16) |
            ((u32)(engParaInBuf->ARP[index].macAddr[0] << 8)) |
            ((u32)(engParaInBuf->ARP[index].macAddr[1]));
        
        *((*engParaForSend)++) = ((u32)(engParaInBuf->ARP[index].macAddr[2])) << 24 |
            ((u32)(engParaInBuf->ARP[index].macAddr[3])) << 16 |
            ((u32)(engParaInBuf->ARP[index].macAddr[4])) << 8 |
            ((u32)(engParaInBuf->ARP[index].macAddr[5]));
        *((*engParaForSend)++) = engParaInBuf->ARP[index].ipAddr;
    }
    return;
}

void FillDatain(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    for(index = 0; index < DATE_IN_RETURN_DATA_SIZE; index = index + 4) {
        *((*engParaForSend)++) = (u32)(engParaInBuf->datain_return[index]) << 24 |
            (u32)(engParaInBuf->datain_return[index + 1]) << 16 |
            (u32)(engParaInBuf->datain_return[index + 2]) << 8 |
            (u32)(engParaInBuf->datain_return[index + 3]);
    }
    return;
}
    
u32 calculateCrc32(u32 *buffer, u32 dataSize)
{
    int i, j;
    u32 crcTemp = 0;
    u8 *dataConvertTemp; 
    u32 tempSize = dataSize >> 2; //get the 4bytes number
    if(buffer == 0) {
        return 0;
    }
    for (i = 0; i < tempSize; i++)
    {
        dataConvertTemp = (u8*)buffer;
        for (j =0; j < 4; j++) {
            crcTemp = (crcTemp << 8)^(crc32Tab[(dataConvertTemp[3-j] ^ (crcTemp >> 24)) & 0xFF]);
        }

        buffer++;
    }
    return crcTemp;
}


void FillFrameHeader(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
     *((*engParaForSend)++) = engParaInBuf->syncWord;
     *((*engParaForSend)++) = (u32)(engParaInBuf->frame_header) << 16 | engParaInBuf->package_seg_control;
     *((*engParaForSend)++) = (u32)(engParaInBuf->package_lenth) << 16 | engParaInBuf->data_type;
     *((*engParaForSend)++) = (u32)(engParaInBuf->sub_system) << 16 |
        (u32)(engParaInBuf->time[0]) << 8 | engParaInBuf->time[1];
     *((*engParaForSend)++) = (u32)(engParaInBuf->time[2]) << 24 | (u32)(engParaInBuf->time[3]) << 16 |
        (u32)(engParaInBuf->time[4]) << 8 | engParaInBuf->time[5];
    return;
}

void FillStatusInfo(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    /*The paramete below is for dig para */
    *((*engParaForSend)++) = engParaInBuf->packet_num;
    *((*engParaForSend)++) = (u32)(engParaInBuf->digData.switchStatue.value << 24) |
        (u32)(engParaInBuf->digData.commandCount.value << 16) | 
        (u32)(engParaInBuf->digData.versionTbl.value) << 8 | engParaInBuf->digData.softwareStartFlag.value;
    *((*engParaForSend)++) = (u32)(engParaInBuf->digData.portLinkStates.value) << 16 | 
        (u32)(engParaInBuf->digData.ipStates.value);
    *((*engParaForSend)++) = (u32)(engParaInBuf->digData.tsStatue.value) << 16 |
        (u32)(engParaInBuf->digData.interConnectState.value) << 8 | engParaInBuf->digData.utcTime.value >> 16;
    *((*engParaForSend)++) = (u32)(engParaInBuf->digData.utcTime.value << 16)  | engParaInBuf->digParaReserved;
    /*The paramete above is for dig para */
    *((*engParaForSend)++) = engParaInBuf->soft_version;
    *((*engParaForSend)++) = engParaInBuf->logic_version;
    *((*engParaForSend)++) = ((u32)(engParaInBuf->FPGA_temperature) << 16) | (engParaInBuf->VccInt);
    *((*engParaForSend)++) = ((u32)(engParaInBuf->VccAux) << 16) | engParaInBuf->VccBram;
    return;
}

void FillCmdCountData(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    *((*engParaForSend)++) = ((u32)(engParaInBuf->recv_zk_right_cmd_num) << 24) |
        ((u32)engParaInBuf->recv_zk_error_cmd_num << 16) | (engParaInBuf->restruct_pkt_num);
    *((*engParaForSend)++) = ((u32)(engParaInBuf->restruct_pkg_wrong_num) << 16) | 
        (u32)(engParaInBuf->resBindingOrFirstWrongPakge.restructBindingParaNum);
    *((*engParaForSend)++) = ((u32)(engParaInBuf->resBindingParaNum.resBindingWrongPkgValue) << 24) | 
        (u32)(engParaInBuf->digCollectErrorNum << 16) | ((u32)(engParaInBuf->mstDataDiscardNum) << 8) |
        engParaInBuf->reset_num;
    return;
}

void FillTimeSyncInfo(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    *((*engParaForSend)++)  = ((u32)(engParaInBuf->ts_enable.value) << 16) |
        (u32)(engParaInBuf->timeSyncSource) << 8  | engParaInBuf->timeSyncReserved;
    *((*engParaForSend)++) = (u32)(engParaInBuf->timeGrade << 16) |
        engParaInBuf->timeSyncPerid;
    *((*engParaForSend)++) = engParaInBuf->ppsPeriod;
    *((*engParaForSend)++) = engParaInBuf->ppsPulseWidth;
    *((*engParaForSend)++) = engParaInBuf->timeSyncSecOffset;
    *((*engParaForSend)++) = engParaInBuf->msAndNsOffsest.value;
    return;
}

void FillPortCommunicateStatus(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    *((*engParaForSend)++) = ((u32)engParaInBuf->portFcEnable.value << 16) |
        engParaInBuf->IP_credit_sts.value;
    return;
}

void FillLocationInfo(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    u32 index;
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) { 
        return;
    }
    if (IS_LOCATION_SYC(g_swLocation) != 0) {
        /*SYC 96 BYTES*/
        for (index = 0; index < SYC_LOCDATA_4BYTES_COUNT; index++) {
            *((*engParaForSend)++) = engParaInBuf->locatorData[index];
        }
    } else {
        /*HXC 41 BYTES + 3BYTES Filled. 44 total*/
        for (index = 0; index < HXC_LOCDATA_4BYTES_COUNT; index++) {
            *((*engParaForSend)++) = engParaInBuf->locatorData[index];
        }
    }
    return;
}

void FillReservedData(eng_para_in_buf *engParaInBuf, u32 **engParaForSend)
{
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) { 
        return;
    }
    
    if (IS_LOCATION_SYC(g_swLocation) != 0) {
        /*SYC 68 BYTES*/
        memset((*engParaForSend), g_reserveDataMapTbl[g_swLocation], SYC_RESERVED_BYTES);
        (*engParaForSend) += (SYC_RESERVED_BYTES / 4);
    } else {
        /*HXC 123 BYTES reserved 3 byte filled in locator data. 120 bytes need Filled*/
        memset((*engParaForSend), g_reserveDataMapTbl[g_swLocation], HXC_RESERVED_BYTES);
        (*engParaForSend) += (HXC_RESERVED_BYTES / 4);
    }
    return;
}
void ConvertEngParaFromBufToSend(eng_para_in_buf *engParaInBuf, u32 *engParaForSend)
{
    SW_DBG(DBG_STEP, "Enter of ConvertEngParaFromBufToSend");
    u32 *engParaPtrTemp = engParaForSend;
    FillFrameHeader(engParaInBuf, &engParaForSend);
    FillStatusInfo(engParaInBuf, &engParaForSend);
    FillCmdCountData(engParaInBuf, &engParaForSend);
    
    FillPort2PNumber(engParaInBuf, &engParaForSend);
    FillFcAENumber(engParaInBuf, &engParaForSend);
    FillFcIpNumber(engParaInBuf, &engParaForSend);
    FillTsNumber(engParaInBuf, &engParaForSend);
    FillIpBw(engParaInBuf, &engParaForSend);
    
    FillTimeSyncInfo(engParaInBuf, &engParaForSend);
    FillAddrTblInfo(engParaInBuf, &engParaForSend);
    FillRouterInfo(engParaInBuf, &engParaForSend);
    FillArpInfo(engParaInBuf, &engParaForSend);
    FillPortCommunicateStatus(engParaInBuf, &engParaForSend);
    FillLocationInfo(engParaInBuf, &engParaForSend);
    FillReservedData(engParaInBuf, &engParaForSend);
    FillDatain(engParaInBuf, &engParaForSend);
    /* sync word is not included in CRC */
    sleep(ENG_PARA_WAIT_SCHE_INTERVEL);
    
    /* syncWord(4B), is not included in crc */
    engParaInBuf->crc = calculateCrc32((engParaPtrTemp + 1), 2040);
    sleep(ENG_PARA_WAIT_SCHE_INTERVEL);
    *(engParaForSend++) = engParaInBuf->crc;
    *(engParaForSend++) = 0; //CRC of FC
    *(engParaForSend++) = 0xBC957575;

    return;
}

void *UpdataEngPara(void * arg)
{
    u32 his_eng_cnt = 0;
    u32 cur_cnt;
    GetAndSendEngPara();
    while(1) {
        xmk_enter_kernel();
        cur_cnt = g_engCnt - his_eng_cnt;
        his_eng_cnt = g_engCnt;
        xmk_leave_kernel();

        while (cur_cnt != 0) {
            GetAndSendEngPara();
            cur_cnt--;        
            sleep(UPDATE_ENG_PARA_SCHE_INTERVEL);
        }
        sleep(UPDATE_ENG_PARA_SCHE_INTERVEL);
    }
    return 0;
}

void PrintEngParaData(u32 *engParaData)
{
#if 1
    int i;
    u32 *dataTemp = (u32*)engParaData;
    if (g_engPrintEnable == 1) {
        xil_printf("data size = %d \r\n", ENG_PARA_LENGTH + 20);
        for (i = 0; i < 517; i++) {
            xil_printf("%08x  ", *dataTemp);
            if (i % 4 == 0) {
                xil_printf("\r\n");
            }
            dataTemp++;
        }
        xil_printf("\r\n");
    }
#endif
    return;
}

u32 GetDataManagPortId(u8 *dataManagePortId)
{
    CascadePortInfo cascadePortInfo = {0};
    if (g_swLocation >= TOTAL_SW_LOCATION_NUM) { 
        return XST_FAILURE;
    }

    if (IS_SWITCH2(g_swLocation)) {
        GetCascadePortSetting(&cascadePortInfo);

        if ((cascadePortInfo.localCascadePort1 >= FC_PORT_NUM_INSTANCES) ||
            (cascadePortInfo.localCascadePort2 >= FC_PORT_NUM_INSTANCES)) {
            SW_DBG(DBG_ERROR, "The cascade port is wrong.");
            return XST_FAILURE;
        }
    
        if ((fpd[(cascadePortInfo.localCascadePort1)]).port_status == PORT_READY) {
            *dataManagePortId = cascadePortInfo.localCascadePort1;
        } else if ((fpd[(cascadePortInfo.localCascadePort2)]).port_status == PORT_READY) {
            *dataManagePortId = cascadePortInfo.localCascadePort2;
        } else {
            SW_DBG(DBG_WARN, "The cascade port is not linked.");
            return XST_FAILURE;
        }
    } else {
        if((fpd[PORTNUM_DATA_MANAGEMENT]).port_status == PORT_READY) {
            *dataManagePortId = PORTNUM_DATA_MANAGEMENT;
        } else if ((fpd[PORTNUM_SLAVE_DATA_MANAGEMENT]).port_status == PORT_READY){
            *dataManagePortId = PORTNUM_SLAVE_DATA_MANAGEMENT;
        } else {
            SW_DBG(DBG_WARN, "The dataMangement port is not linked.");
            return XST_FAILURE;
        }
    }
    return XST_SUCCESS;
}

void GetAndSendEngPara(void)
{
#if 1
    u8 dataManagmentPortId;
    u32 ret;
    /* 12 byte Filled for PS, And 8 byte filled for tailor */
    u32 engParaExt[518] = {0}; // 517 * 4 bytes is needed
    eng_para_in_buf *engParaBufPtr = &g_engParaBuffer;

    ret = GetDataManagPortId(&dataManagmentPortId);
    if (ret != XST_SUCCESS) {
        SW_DBG(DBG_WARN, "The manage port is not link");
        return;
    }

    FillEngParaData(engParaBufPtr);
    sleep(ENG_PARA_WAIT_SCHE_INTERVEL);
    ConvertEngParaFromBufToSend(engParaBufPtr, &(engParaExt[3]));
    /* Protection for eng para */
    engParaExt[516] = 0xBC957575;
    sleep(ENG_PARA_WAIT_SCHE_INTERVEL);
    (void)Axis_tx_one(dataManagmentPortId, (u8*)(&(engParaExt[0])), ENG_PARA_LENGTH  + 20);
    sleep(ENG_PARA_WAIT_SCHE_INTERVEL);
    PrintEngParaData(engParaExt);  
    SW_DBG(DBG_STEP, "E");
#endif 
    return;
}

#endif
