#ifndef FC_PORT_PPM
#define FC_PORT_PPM
#include "common.h"

#define IPFC_STATUE_MASK 0x01
#define FC_STATUE_MASK 0x02
#define PORT_STATUE_MASK 0x03

u8 PortEnableControl(u8 portNum, u8 contrlMask, u8 contrlFlag);

#endif
