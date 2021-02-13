#include "stdio.h"
#include "sys_print.h"
#include "fc_port_ppm.h"

/***************************************************************************************************
* Function :PortEnableControl
* Description: port enable control for into data.
* Attention:   we must jump over the unused port before use this function
* Paramate: portNum: Input port. 
*           contrlMask:  IPFC_STATUE_MASK or FC_STATUE_MASK or PORT_STATUE_MASK
*           contrlFlag:   DISABLE_FLAG or ENABLE_FLAG
****************************************************************************************************/
u8 PortEnableControl(u8 portNum, u8 contrlMask, u8 contrlFlag)
{
    u8 ret;
    u32 portStatue;
    /* 00 is fc and ipfc all enable, 01 is FC disable, 10 is ipfc disable */
    if ((portNum >= FC_PORT_NUM_INSTANCES) ||
        (contrlMask >= PORT_STATUE_MASK) ||
        (contrlMask == 0)) {
        SW_DBG(DBG_ERROR, "The port num is wrong or the control Mask is wrong");
        return RET_FAILED;
    }
  #if 0
    portStatue = fc_drv_read(FC_PORT_PPM_ADDRESS_TABLE[portNum], PORT_CONTROL_REG_OFFSET_ADDR);
   
    if (contrlFlag == DISABLE_FLAG) {
        /* Set to 1 is disable */
        portStatue |= contrlMask;
        fc_drv_write(portStatue, FC_PORT_PPM_ADDRESS_TABLE[portNum], PORT_CONTROL_REG_OFFSET_ADDR);
        DeleteRelatedTable(portNum, contrlMask);
    } else if (contrlFlag == ENABLE_FLAG){
        /* Set to 0 is enable */
        portStatue &= (~contrlMask);
        fc_drv_write(portStatue, FC_PORT_PPM_ADDRESS_TABLE[portNum], PORT_CONTROL_REG_OFFSET_ADDR);
        /*Set status to ready to update the table value in fc_port_mon*/
        SetPortStatus(portNum, PORT_PHY_RDY);
    } else {
        SW_DBG(DBG_ERROR, "The control flag is wrong!");
        return RET_FAILED;
    }

    ret = SetPortEnableState(portNum, contrlMask, contrlFlag);
    if (ret != XST_SUCCESS) {
        return RET_FAILED;
    }
    #endif
    return RET_SUCCESS;
}

