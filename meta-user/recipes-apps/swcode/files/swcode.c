/*
* Copyright (C) 2013 - 2016  Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in this
* Software without prior written authorization from Xilinx.
*
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "sys_print.h"
#include "spi_drive.h"
#include "binding_para.h"


#include "sw_pthread.h"
extern u32 readBindingParaFromFlash(u32 bindingParaFlag, u32 inputLength, u8 *readBuffer);

void FlashTest()
{
#if 0
    u8 readBuffer[BINDING_PARA_BYTE_CONUNT] = {0};
    u8 writeBuffer[1000] = {0};
    u32 inputLength = 756;
    memset(writeBuffer, 0x1, 756);
    WriteBindingParaToFlash(ORIGIN_BINDING_PARA_FLAG, inputLength, writeBuffer);
    u32 retValue = readBindingParaFromFlash(ORIGIN_BINDING_PARA_FLAG, BINDING_PARA_BYTE_CONUNT, readBuffer);

    for (u32 i = 0; i < BINDING_PARA_BYTE_CONUNT; i++) {
        if (i % 4 == 0) {
            printf("#  %d  #\n", i/4);                        
        }
        printf("%x ", readBuffer[i]);
    }
#endif
    return;
}

void TCS_ClearTxFifo(void)
{
#if 1
    u32 writeDataBuffer[5] = {0x01020383, 0x04050683, 0x07080983, 0x0A0B0C83, 0x0D0E0F83};
    SW_DBG(DBG_DBG, "=======begin ======\n");

    if (IsPLTxFifoEmpty()) {
        SW_DBG(DBG_WARN, "====Before: The tx fifo is empty");
    } 
        
    WriteDataToSpi(writeDataBuffer, 20);
    u32 txFifoDataCount = 0;
    GetTxFifoDataCount(&txFifoDataCount);
    SW_DBG(DBG_ERROR, "=====txFifoDataCount = %d\n", txFifoDataCount);
    ResetSpiTxBuffer();
    GetTxFifoDataCount(&txFifoDataCount);
    SW_DBG(DBG_ERROR, "=====After txFifoDataCount = %d \n", txFifoDataCount);   
    SW_DBG(DBG_DBG, "=======end =======\n");
#endif   
    return;
}

void TCS_ClearRxFifo(void)
{
#if 1
    u32 dataCount = 0;
    SW_DBG(DBG_DBG, "=======begin ======\n");
    GetRxFifoDataCount(&dataCount);
    SW_DBG(DBG_ERROR, "dataCount = %d \n", dataCount);
    if (IsPLRxFifoEmpty()) {
        SW_DBG(DBG_WARN, "The rx fifo is empty");
    } else {
        //for (u32 i = 0; i < dataCount; i++) {
            //ReadDataFromSpi(&returnedData);
            //printf("data[%d] = 0x%x \r\n", i, returnedData);
        //}
    }
    ResetSpiRxBuffer();
    SW_DBG(DBG_DBG, "=======end =======\n");
#endif    
    return;
}


extern u32 g_digBindSWDestID;

int main(int argc, char **argv)
{
    u32 retValue;
    
    printf("=============version 0.7 ============ \n");
    DealBindingParaDataFromFlash();

    //FlashTest();
    OpenSpiDevice();
    ResetAllBuffer();
    
    PthreadCreat();

    while(1) {
        //TCS_ClearRxFifo();
        //SW_DBG(DBG_WARN, "=======alived =======\n");
        sleep(2);
    }
    CloseSpiDevice();
    return 0;
}
