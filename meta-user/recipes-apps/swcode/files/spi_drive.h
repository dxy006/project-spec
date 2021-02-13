#ifndef SPI_DRIVE_H
#define SPI_DRIVE_H
#include "common.h"
#define SPI_DEVICE_NAME "/dev/spi_dev"
#define SPI_IOC_MAGIC 's'

typedef struct RegOperStruct
{
    unsigned int regOffsetAddr;
    unsigned int regValue;
} RegOper;//reg operation structure, defined in user space as well

#define CMD_WRITE_REG _IOW(SPI_IOC_MAGIC, 1, RegOper)
#define CMD_READ_REG _IOWR(SPI_IOC_MAGIC, 2, RegOper)
#define CMD_RESET_RX_BUFFER _IO(SPI_IOC_MAGIC, 3)
#define CMD_RESET_TX_BUFFER _IO(SPI_IOC_MAGIC, 4)
#define CMD_RESET_ALL_BUFFER _IO(SPI_IOC_MAGIC, 5)
#define SPI_READ_WAIT_SCHE_INTERVEL 1000

#define SPI_FIFO_EMPTY_FLAG 1

#define SPI_RXFIFO_STATE_OFFSET_ADDR 6*4 //reg6,receive fifo state reg. bit0:rx_fifo empty. bit1:rx_fifo full.
#define SPI_RXFIFO_DATACNT_OFFSET_ADDR 7*4 //reg7,receive fifo valid data count,bit[11:0] valid

#define SPI_TXFIFO_STATE_OFFSET_ADDR 14*4 //reg14,transmit fifo state reg. bit0:tx_fifo empty. bit1:tx_fifo full.
#define SPI_TXFIFO_DATACNT_OFFSET_ADDR 15*4 //reg15,transmit fifo valid data count,bit[11:0] valid

u32 OpenSpiDevice(void);
void CloseSpiDevice(void);
u32 ReadSpiReg(u32 offsetAddr, u32* regValue);
u32 WriteSpiReg(u32 offsetAddr, u32 writeData);
u32 ResetSpiRxBuffer(void);
u32 ResetSpiTxBuffer(void);
u32 ResetAllBuffer(void);
u32 WriteDataToSpi(u32 *writeDataBuffer, u32 writeDataLenth);
u32 ReadDataFromSpi(u8 *returnedData);

bool IsPLRxFifoEmpty(void);
bool IsPLTxFifoEmpty(void);
u32 GetTxFifoDataCount(u32 *txFifoDataCount);
u32 GetRxFifoDataCount(u32 *rxFifoDataCount);


#endif
