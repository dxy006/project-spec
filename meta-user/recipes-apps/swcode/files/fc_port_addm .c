/*
 * fc_port_addm.c
 *
 *  Created on: 20201102
 *      Author: duxingyu
 */
 #include "fc_port_addm.h"
u32 fc_addm_delete_table(u32 did)
{
#if 0
    Fc_Addm_Dcpr *fe = &g_fcAddmData;
    u32 wrdata;
    //do read if busy
    while((fc_addm_read_reg(fe, FC_ADDM_REG_STATUS) & 0X1) != 0) {
        sleep(QUERY_STATUE_REG_SCHE_INTERVEL);
    }

    ClearTheAddmStatusReg();
    //write delete did
    wrdata = did & 0XFFFFFF;
    fc_addm_write_reg(fe, FC_ADDM_REG_WDID, wrdata);
    //do read if delete is not complete
    while ((!(fc_addm_read_reg(fe, FC_ADDM_REG_STATUS) & mask_delete_complete)) != 0) {
        sleep(QUERY_STATUE_REG_SCHE_INTERVEL);
    }

    ClearTheAddmStatusReg();
#endif
    return RET_SUCCESS;
}

