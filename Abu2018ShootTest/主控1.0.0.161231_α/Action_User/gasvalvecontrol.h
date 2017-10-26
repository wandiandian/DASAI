#ifndef __GASVALVECONTROL_H
#define __GASVALVECONTROL_H


#include  "can.h"
#define CLAMP_OPEN_IO_ID 1


#define CLAMP_OPEN_BOARD_ID 2


void GasValveControl(uint8_t boardNum , uint8_t valveNum , uint8_t valveState);
void ClampOpen(void);
void ClampClose(void);

#endif
