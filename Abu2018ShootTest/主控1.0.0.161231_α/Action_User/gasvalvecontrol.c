#include  "can.h"
#include "gasvalvecontrol.h"
#include "gpio.h"
#include "usart.h"

#define GAS_VALVE_PRINT_LOG 0U
/**
* @brief  气阀控制
* @param  boardNum：气阀板号
* @param  valveNum：气阀号
* @param  valveState： 气阀状态，0为关，1为开
* @author ACTION
*/
void GasValveControl(uint8_t boardNum , uint8_t valveNum , uint8_t valveState)
{
	uint8_t data = 0x00;
//	uint8_t mbox;
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x0001 ;					 // standard identifier=1
	TxMessage.ExtId = 0x0001 ;				 	 // extended identifier=StdId
	TxMessage.IDE = CAN_Id_Standard;			 // type of identifier for the message is Standard
	TxMessage.RTR = CAN_RTR_Data;			 	 // the type of frame for the message that will be transmitted
	TxMessage.DLC = 1;
	
	data = boardNum<<5|valveNum<<1|valveState;
	
	TxMessage.Data[0] = data;

	OSCANSendCmd(CAN1, &TxMessage);
//	mbox = CAN_Transmit(CAN2, &TxMessage);         
//	while ((CAN_TransmitStatus(CAN2, mbox) != CAN_TxStatus_Ok));
}
//夹子开
void ClampOpen(void)
{
		GasValveControl(CLAMP_OPEN_BOARD_ID , CLAMP_OPEN_IO_ID , 1);
}
//夹子关
void ClampClose(void)
{

		GasValveControl(CLAMP_OPEN_BOARD_ID , CLAMP_OPEN_IO_ID , 0);
}







