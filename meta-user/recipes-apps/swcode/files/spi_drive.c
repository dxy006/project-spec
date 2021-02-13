/*
 * spi_drive.c
 *
 *  Created on: 2019Äê9ÔÂ22ÈÕ
 *      Author: ´÷¶û
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h> //for ioctl

#include <fcntl.h> //for open
#include "common.h"
#include "sys_print.h"
#include "spi_drive.h"
int g_spiDevFd = -1;

/*****************************************
* Function Nameï¼šWriteCommandToSpi
* 
*
* Current statue ï¼štest pass
*****************************************/
static u32 WriteCommandToSpi(u32 commandType)
{
    int retValue = ioctl (g_spiDevFd, commandType);
    if (retValue < 0) {
        retValue = RET_FAILED;
        SW_DBG (DBG_ERROR, "write data failed! retValue = 0x%x \n", retValue);
    } else {
        retValue = RET_SUCCESS;
    }
    return retValue;
}

#define SPI_RX_FIFO_CTL_OFFSET_ADDR 4*4
#define SPI_RX_FIFO_RST_MASK    0x00000001

/*****************************************
* Function Nameï¼šResetSpiRxBuffer
* 
*
* Current statue ï¼štest not pass
*****************************************/
u32 ResetSpiRxBuffer(void)
{
#if 0
    u32 regValue = 0;
    //ReadSpiReg(SPI_RX_FIFO_CTL_OFFSET_ADDR, &regValue);
    regValue |= SPI_RX_FIFO_RST_MASK;
    //printf("====write value = 0x%x \n", regValue);
    WriteSpiReg(SPI_RX_FIFO_CTL_OFFSET_ADDR, regValue);
    //ReadSpiReg(SPI_RX_FIFO_CTL_OFFSET_ADDR, &regValue);
    //printf("====read value = 0x%x \n", regValue);
    regValue &= (~SPI_RX_FIFO_RST_MASK);
    WriteSpiReg(SPI_RX_FIFO_CTL_OFFSET_ADDR, regValue);
    //ReadSpiReg(SPI_RX_FIFO_CTL_OFFSET_ADDR, &regValue);
    //printf("====read value = 0x%x \n", regValue);
#endif

    return WriteCommandToSpi(CMD_RESET_RX_BUFFER);
}

#define SPI_TX_FIFO_CTL_OFFSET_ADDR   12*4 
#define SPI_TX_FIFO_RST_MASK  0x00000001

/*****************************************
* Function Nameï¼šResetSpiRxBuffer
* 
*
* Current statue ï¼šTx fifo cannot be cleared 
*****************************************/
u32 ResetSpiTxBuffer(void)
{
    u32 regValue = 0;
    ReadSpiReg(SPI_TX_FIFO_CTL_OFFSET_ADDR, &regValue);
    regValue |= SPI_TX_FIFO_RST_MASK;
    WriteSpiReg(SPI_TX_FIFO_CTL_OFFSET_ADDR, regValue);
    usleep(50);
    ReadSpiReg(SPI_TX_FIFO_CTL_OFFSET_ADDR, &regValue);
    printf("read value = 0x%x \n", regValue);
    regValue &= (~SPI_TX_FIFO_RST_MASK);
    WriteSpiReg(SPI_TX_FIFO_CTL_OFFSET_ADDR, regValue);
    ReadSpiReg(SPI_TX_FIFO_CTL_OFFSET_ADDR, &regValue);
    printf("read value = 0x%x \n", regValue);

    #if 0
    return WriteCommandToSpi(CMD_RESET_TX_BUFFER);
    #endif
}

u32 ResetAllBuffer(void)
{
    return WriteCommandToSpi(CMD_RESET_ALL_BUFFER);
}

/*****************************************
* Function OpenSpiDevice
* 
*
* Current statue ï¼štest pass
*****************************************/
u32 OpenSpiDevice(void)
{
    u32 retValue;
    g_spiDevFd = open(SPI_DEVICE_NAME, O_RDWR | O_SYNC);
    if (g_spiDevFd < 0) {
        SW_DBG (DBG_ERROR, "cannot open device file : %s\n", SPI_DEVICE_NAME);
        exit(EXIT_FAILURE);
    } 
    retValue = ResetSpiRxBuffer();
    return retValue;
}

void CloseSpiDevice(void)
{
    close(g_spiDevFd);
    return;
}

/*****************************************
* Function Nameï¼šReadSpiReg
* 
*
* Current statue ï¼štest pass
*****************************************/
u32 ReadSpiReg(u32 offsetAddr, u32* regValue)
{
    int retValue;
    RegOper regOperValue = {0};
    regOperValue.regOffsetAddr = offsetAddr;
    retValue = ioctl (g_spiDevFd, CMD_READ_REG, &regOperValue);
    if (retValue < 0) {
        *regValue = 0;
        SW_DBG (DBG_ERROR, "ioctl failed! retValue = %d \n", retValue);
        retValue = RET_FAILED;
        
    } else {
        *regValue = regOperValue.regValue;
        retValue = RET_SUCCESS;
    }
    return retValue;
}

/*****************************************
* Function Nameï¼šWriteSpiReg
* 
*
* Current statue ï¼štest pass
*****************************************/
u32 WriteSpiReg(u32 offsetAddr, u32 writeData)
{
    RegOper regOperValue = {.regOffsetAddr = offsetAddr, .regValue = writeData};
    int retValue = ioctl(g_spiDevFd, CMD_WRITE_REG, &regOperValue);
    if (retValue < 0) {
        retValue = RET_FAILED;
        SW_DBG (DBG_ERROR, "ioctl failed! retValue = 0x%x \n", retValue);
    } else {
        retValue = RET_SUCCESS;
    }
    return retValue;
}

/*****************************************
* Function WriteDataToSpi
* 
*
* Current statue ï¼štest pass
*****************************************/
u32 WriteDataToSpi(u32 *writeDataBuffer, u32 writeDataLenth)
{
    int retValue;
    retValue = write(g_spiDevFd, writeDataBuffer, writeDataLenth);
    if (retValue < 0) {
        SW_DBG (DBG_ERROR, "write data failed! retValue = 0x%x \n", retValue);
        retValue = RET_FAILED;
    } else {
        retValue = RET_SUCCESS;
    }
    return retValue;
}

/*****************************************
* Function Nameï¼šIsPLRxFifoEmpty
* 
*
* Current statue ï¼štest pass
*****************************************/
bool IsPLRxFifoEmpty(void)
{
    u32 regValue = 0;
    (void)ReadSpiReg(SPI_RXFIFO_STATE_OFFSET_ADDR, &regValue);
    if (regValue == SPI_FIFO_EMPTY_FLAG) {
        return true;
    }
    return false;
}

/*****************************************
* Function Nameï¼šIsPLTxFifoEmpty
* 
*
* Current statue ï¼štest pass
*****************************************/
bool IsPLTxFifoEmpty(void) 
{
    u32 regValue = 0;
    u32 ret = ReadSpiReg(SPI_TXFIFO_STATE_OFFSET_ADDR, &regValue);
    printf("regvalue = 0x%x \r\n", regValue);
    if (regValue == SPI_FIFO_EMPTY_FLAG) {
        return true;
    }
    return false;
}

/*****************************************
* Function : GetTxFifoDataCount
* 
*
* Current statue ï¼štest pass
*****************************************/
u32 GetTxFifoDataCount(u32 *txFifoDataCount)
{   
    return ReadSpiReg(SPI_TXFIFO_DATACNT_OFFSET_ADDR, txFifoDataCount);
}

/*****************************************
* Function : GetRxFifoDataCount
* 
*
* Current statue ï¼štest pass
*****************************************/
u32 GetRxFifoDataCount(u32 *rxFifoDataCount)
{
    return ReadSpiReg(SPI_RXFIFO_DATACNT_OFFSET_ADDR, rxFifoDataCount);
}

/*****************************************
* Function : ReadDataFromSpi
* 
*
* Current statue ï¼štest pass
*****************************************/
u32 ReadDataFromSpi(u8 *returnedData)
{
    u32 i = 0;
    static u32 receivedData = 0;
    static u8 bytesNum = 0;
    static u8 indexNum = 1;
    if (bytesNum == 0) {
        for (i = 1; i < 1000; i++) {
            bytesNum = read(g_spiDevFd, &receivedData, sizeof(u32));
            indexNum = 1;
            if (bytesNum != 0) {
                SW_DBG (DBG_DATA, "receivedData = 0x%x! \n", receivedData);
                break;
            }
            
            if (i % 10 == 0) { 
                usleep(SPI_READ_WAIT_SCHE_INTERVEL);
            }
        }
    }
    
    if (i == 1000) {
        SW_DBG (DBG_WARN, "There is no data! \n");
        return RET_FAILED;
    } 
    
    *returnedData = (receivedData >> (4 - indexNum) * 8) & 0xFF;
    bytesNum--;
    indexNum++;
    return RET_SUCCESS;
}


