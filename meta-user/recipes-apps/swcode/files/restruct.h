#ifndef RESTRUCT_H
#define RESTRUCT_H
#include "zk_communicate.h"
#define RESTRUCT_DATA_FLASH_DEV "/dev/mtd5"
#define RESTRUCT_DATA_SIZE 20 * 1024 * 1024
#define RESTRUCT_BINDPARA_DATA_SIZE 2 * 1024
#define RESTRUCT_DATA_FISRT_PACKAGE 0XA5
#define RESTRUCT_DATA_MIDDLE_PACKAGE 0XAB
#define RESTRUCT_DATA_LAST_PACKAGE 0XAF
#define RESTRUCT_DATA_SINGAL_PACKAGE 0XAA

#define RESTRUCT_SOFTWARE 0XD1
#define RESTRUCT_BINDING_PARA 0XD2

#define RESTRUCT_DATA_PACKAGE_SIZE 746 // Size of one package restruct data



#define RESTRUCT_STATE_NO_RESTRUCT_PACKAGE 0
#define RESTRUCT_STATE_PKG_RECIVING 0x01
#define RESTRUCT_STATE_PKG_RECIVED_COMP 0x02
#define RESTRUCT_STATE_PKG_RECIVED_NOTCOMP 0x03
#define RESTRUCT_STATE_FLSH_WRITE_SUCESS 0x04
#define RESTRUCT_STATE_FLSH_WRITE_FAILE 0x05
#define RESTRUCT_STATE_RESTRUCT_FINISH 0x06
#define RESTRUCT_STATE_RESTRUCT_FAILED 0x07

void DealRestructData(SpiDataInfoStruct *spiDataInfo);
#endif