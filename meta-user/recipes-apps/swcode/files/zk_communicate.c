#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "common.h"
#include "spi_drive.h"
#include "zk_communicate.h"
#include "dig_para.h"
#include "sys_print.h"
#include "binding_para.h"
#include "into_data.h"
#include "restruct.h"

MstTempFlag g_ZKTempFlag;
static INT32 g_spiFilePointer;
static INT32 g_intoDataRetPointer;


static const SpiDataMaxLengthMapTbl g_lengthMapTbl[LENGTH_MAP_TBL] = {
    {MST_COMMUNICATE_RESTRUCT_PARA, RESTRUCT_DATA_MAX_SIZE},
    {MST_COMMUNICATE_INTO_DATA, INTO_DATA_MAX_SIZE},
    {MST_COMMUNICATE_BROADCAST_UTC_TIME, UTC_TIME_MAX_SIZE}
};

static void ReadAndFillSpiData(u8 dataType)
{   
    u16 i;
    u8 dataLengthL = 0;
    u8 dataLengthH = 0;
    u16 dataLength;
    u8 spiDataTemp[SPI_DATA_SIZE] = {0};
    u16 dataNumTemp = 0;
    /* Add data type to buffer 1Byte*/
    spiDataTemp[dataNumTemp++] = dataType;
    u8 ret = ReadDataFromSpi(&dataLengthH);
    SW_DBG(DBG_DBG_DATA, "spiDataTemp 0x%x \n", dataLengthH);
    CHECK_RETURN_VALUE(ret);
    ret = ReadDataFromSpi(&dataLengthL);
    SW_DBG(DBG_DBG_DATA, "spiDataTemp 0x%x \n", dataLengthL);
    CHECK_RETURN_VALUE(ret);
    dataLength = (((u16)dataLengthH << 8) | dataLengthL);
    /* Add data length to buffer. (to fit the get process, the low byte is before the high.)  2Bytes*/
    spiDataTemp[dataNumTemp++] = dataLengthL;
    spiDataTemp[dataNumTemp++] = dataLengthH;
    for (i = 0; i < LENGTH_MAP_TBL; i++) {
        if ((g_lengthMapTbl[i].dataType == dataType) &&
            (g_lengthMapTbl[i].maxLength < dataLength)) {
            g_ZKTempFlag.mstDataDiscardNum++;       
            return;
        }
    }
    
    for (i = 0; i < dataLength; i++) {
        ret = ReadDataFromSpi(&(spiDataTemp[dataNumTemp++]));
        CHECK_RETURN_VALUE(ret);
    }

    /* Add crc to buffer 1Byte */
    ret = ReadDataFromSpi(&(spiDataTemp[dataNumTemp++]));
    CHECK_RETURN_VALUE(ret);
#if 1
    u32 dataNumWrite = write(g_spiFilePointer, spiDataTemp, dataNumTemp);
    if (dataNumWrite < dataNumTemp) {
        SW_DBG(DBG_WARN, "Info: The fifo write failed, dataNumWrite = %d, dataNumToWrite = %d",
            dataNumWrite, dataNumTemp);
    }
#endif
    return;
}


/* Get the command And deal the dig data */
static void GetCmdTypeAndDealTheData(u8 cmdType)
{
    switch (cmdType) {
        case MST_COMMUNICATE_DIG_PARA :
            DealTheDigParaData();
            break;
        case MST_COMMUNICATE_RESTRUCT_PARA :
        case MST_COMMUNICATE_INTO_DATA :
        case MST_COMMUNICATE_BROADCAST_UTC_TIME : //The restruct, into data, broadcast utc use same branch
            ReadAndFillSpiData(cmdType);
            break;
        default:
            SW_DBG(DBG_ERROR, "The command word is wrong!, cmdType = 0x%x \n", cmdType);
            break;
    }
    return;
}

static void DealTheSWData(void)
{
    u8 receivedData = 0;
    u8 ret = ReadDataFromSpi(&receivedData);
    u8 digBindSWDestID = GetDigBindSWDestID();
    CHECK_RETURN_VALUE(ret);
    /* Check the next byte which is represent the device */
    if ((receivedData == digBindSWDestID) ||
        (digBindSWDestID == MST_COMMUNICATE_DEFAULT_DEST_WORD)) {
        /* If the binding para get failed,  donot check the destID 
        to ensure that the zkcommucate can work well */
        if (digBindSWDestID == MST_COMMUNICATE_DEFAULT_DEST_WORD) {
            digBindSWDestID = receivedData;
        }
        SW_DBG(DBG_DBG_DATA, "Recived Data 0x%x \n", receivedData);
        /* Get and deal the cmd command */
        ret = ReadDataFromSpi(&receivedData);
        CHECK_RETURN_VALUE(ret);
        GetCmdTypeAndDealTheData(receivedData);
    } else {
        g_ZKTempFlag.mstDataDiscardNum++;
        SW_DBG(DBG_ERROR, "The spi data Dest State 0x%x \n", receivedData);
    }
    return;
}


static void GetAndCheckTheCommandWord(void)
{
    u8 receivedData = 0;
    (void)ReadDataFromSpi(&receivedData);
    /* Check the sysnc header fist */
    if(receivedData == MST_COMMUNICATE_SYNC_WORD) {
        SW_DBG(DBG_DBG_DATA, "Recived Data 0x%x \n", receivedData);
        DealTheSWData();
    } else {
        if (receivedData != INVALID_DATA) {
            SW_DBG(DBG_DBG_DATA, "The spi data 0x%x \n", receivedData);
        }
    }
    return;
}

void *QuerySpiData(void * arg)
{
    g_spiFilePointer = open(NAME_SPI_DATA_FIFO, O_RDWR);
    g_intoDataRetPointer = open(NAME_INTODAYA_RET_FIFO, O_RDWR | O_NONBLOCK);
    while(1) {
        /*If open the file failed, open the file twice*/
        if (g_spiFilePointer == ENV_ERRO) {
            (void)open(NAME_SPI_DATA_FIFO, O_RDWR);
        }
        
        if (g_intoDataRetPointer == ENV_ERRO) {
            (void)open(NAME_INTODAYA_RET_FIFO, O_RDWR);
        }
        
        /* Check if the fifo is empty */
        while (IsPLRxFifoEmpty()) {
            usleep(QEURY_SPI_SCHE_INTERVEL);
        }
        GetAndCheckTheCommandWord();
    }
    return RET_SUCCESS;
}

static void DealTheWrongData(SpiDataInfoStruct *spiDataInfo)
{
    /* There is no flag for utc time. so if utc is wrong do nothing */
    if (spiDataInfo->dataType == MST_COMMUNICATE_INTO_DATA) { 
        g_ZKTempFlag.rXErrorCommandNum++;
        g_ZKTempFlag.rXWrongCmdForDig++;
        if (g_ZKTempFlag.rXWrongCmdForDig > 0x07) {
            g_ZKTempFlag.rXWrongCmdForDig = 0;
        }
        SetZkCommandCount(g_ZKTempFlag.rXWrongCmdForDig, WRONG_COMMAND);
        StoreTheIntoDataToRingBuf(spiDataInfo);
    } else if (spiDataInfo->dataType == MST_COMMUNICATE_BROADCAST_UTC_TIME) {
        g_ZKTempFlag.mstDataDiscardNum++;
    } else {
        SW_DBG(DBG_ERROR, "wrong data type \n");
    }
    return;
}

u8 CalculateCrc(SpiDataInfoStruct *spiDataInfo)
{
    u32 i;
    u32 digBindSWDestID = GetDigBindSWDestID();
    u8 crcTemp = (u8)MST_COMMUNICATE_SYNC_WORD ^ digBindSWDestID;
    crcTemp ^= spiDataInfo->dataType;
    crcTemp ^= ((spiDataInfo->dataSize >> 8) & 0x00FF);
    crcTemp ^= (spiDataInfo->dataSize & 0x00FF);
    for(i = 0; i < spiDataInfo->dataSize; i++) {
        crcTemp ^= spiDataInfo->spiData[i];
    }
    return crcTemp;
}

void DealUtcTimeData(SpiDataInfoStruct *spiDataInfo)
{
#if 1
    int i;
    u8 *dataBaseAddress = spiDataInfo->spiData;
    if ((spiDataInfo->dataSize != UTC_TIME_MAX_SIZE) || (dataBaseAddress == 0)) {
        SW_DBG(DBG_ERROR, "The UTC time is wrong! \n");
        g_ZKTempFlag.mstDataDiscardNum++;
        return;
    }

    for (i = 0; i < spiDataInfo->dataSize; i++) {
        g_ZKTempFlag.utcTime[i] = *dataBaseAddress;
        dataBaseAddress++;
    }
    
    for (i = 0; i < 6; i++) {
        SW_DBG(DBG_DBG_DATA, "utctime is %d \n", g_ZKTempFlag.utcTime[i]);
    }
#endif
    return;
}


void DealTheSpiData(u8 dataType, SpiDataInfoStruct *spiDataInfo)
{
    switch (dataType) {
        case MST_COMMUNICATE_RESTRUCT_PARA :   
            printf("222 \n");
            DealRestructData(spiDataInfo);
            break;
        case MST_COMMUNICATE_INTO_DATA :
            printf("111 \n");
            DealIntoData(spiDataInfo);
            break;
        case MST_COMMUNICATE_BROADCAST_UTC_TIME :           
            DealUtcTimeData(spiDataInfo);               
            break;
        default :
            break;
    }
    return;
}

void *GetAndDealSpiData(void * arg)
{
    SpiDataInfoStruct spiDataInfo = {0};
    u8 crc;
    while(1) {
        memset(&spiDataInfo, 0, sizeof(SpiDataInfoStruct));
    
        /* Get the datatype */
        (void)read(g_spiFilePointer, &(spiDataInfo.dataType), 1);

        /* Get data size */
        (void)read(g_spiFilePointer, (u8*)(&(spiDataInfo.dataSize)), 2);
        /* Fill the data */
        (void)read(g_spiFilePointer, spiDataInfo.spiData, (u32)(spiDataInfo.dataSize));

        /* Get the crc */
        (void)read(g_spiFilePointer, &(spiDataInfo.crc), 1);

        /* restruct data calculate crc in his own data deal function */
        if (spiDataInfo.dataType != MST_COMMUNICATE_RESTRUCT_PARA) {      
            /* Calculate the Crc */
            crc = CalculateCrc(&spiDataInfo);
            if (crc != spiDataInfo.crc) {
                SW_DBG(DBG_ERROR, "The crc is wrong! crcIn = 0x%x, crcCal = 0x%x \n",
                    spiDataInfo.crc, crc);
                DealTheWrongData(&spiDataInfo);
                continue;
            }
        }
        
        DealTheSpiData(spiDataInfo.dataType, &spiDataInfo);
        usleep(DEAL_SPI_DATA_SCHE_INTERVEL);
    }
    return RET_SUCCESS;
}

INT32 InitFiFo(void)
{
    INT32 retValue;
    /*Set the umask, because you do not know who will read or write the fifo*/
    umask(0);
    /*If the file already exist, delete it firstly*/
    unlink(NAME_SPI_DATA_FIFO); // For spi data, recive the data from spi and deal it in another process
    unlink(NAME_INTODAYA_RET_FIFO); // For into data return
    /*Permits the file's owner to read it. |   Permits the file's owner to write to it.
    Permits the file's group to read it. | Permits the file's group to write to it. 
    Permits the other group to read it.  Permits the other group to write to it. (S_IWOTH) */
    retValue = mkfifo(NAME_SPI_DATA_FIFO,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (retValue != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "fifo creat for spi data fifo failed, retValue = %ld \n", retValue);
    }
    
    retValue = mkfifo(NAME_INTODAYA_RET_FIFO,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (retValue != RET_SUCCESS) {
        SW_DBG(DBG_ERROR, "fifo creat for spi data fifo failed, retValue = %ld \n", retValue);
    }
    return retValue;
}

INT32 GetIntoDataRetPointer(void)
{
    return g_intoDataRetPointer;
}