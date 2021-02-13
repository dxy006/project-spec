/*
 *\name spidrv.h
 *\date 2020-10-06
*/

#ifndef SPI_DRV_H
#define SPI_DRV_H

#define SUCCESS 0
#define IOCTL_FAIL -2
#define SPI_IOC_MAGIC 's'
//#define DBG_OPEN
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


#define TOTAL_DATA_SEND_NUM 10
#define BYTE_NUM 4
#define SPI_RX_FIFO_RDTRIG_MASK    0x00000006   //SPI_RxFIFO_RdTrig_MASK   0x00000002   //reg 4,bit 1, read FIFO
#define SPI_RX_FIFO_RST_MASK    0x00000001   //reg4 bit 0, rx_fifo reset 
#define SPI_TX_FIFO_RST_MASK  0x00000001   //reg12 bit 0, tx_fifo reset

#define SPI_RX_FIFO_CTL_OFFSET_ADDR 4*4 //reg4,receive fifo control reg. bit0:rx_fifo rst,1 valid. bit1:fifo_trig.

#define SPI_RX_DATA_OFFSET_ADDR  5*4 //reg5,receive data reg,bit[7:0] valid

#define SPI_TX_FIFO_CTL_OFFSET_ADDR   12*4 //reg12,transmit fifo control reg. bit0:tx_fifo rst,1 valid. bit1:fifo_trig.
#define SPI_TX_DATA_OFFSET_ADDR  13*4 //reg13,transmit data reg,bit[7:0] valid



typedef unsigned int u32;
typedef unsigned char u8; 

typedef struct SpiDevStruct
{
    dev_t devNo;
    void * baseAddress;
    struct cdev cdev;
}SpiDev;


#endif
