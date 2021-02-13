#ifndef SERIAL_CMD_H
#define SERIAL_CMD_H
#include "common.h"
typedef struct {
    u8 (*DealFunction)(void*);
    u8 CmdLength;
    INT8 *CmdString;
} DBGFuncStruct;
#endif

void *DBG_DealSerialCmd(void *arg);

