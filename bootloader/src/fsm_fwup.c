#include "common.h"
#include "fsm_fwup.h"
#include "rl78g14_serial.h"
#include "rl78g14_it.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define DLE  0x10
#define XON  0x11
#define XOFF  0x13
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A
typedef enum {
	XMODEM_CODE_SOH = 0,
	XMODEM_CODE_STX,
	XMODEM_CODE_EOT,
	XMODEM_CODE_ACK,
	XMODEM_CODE_DLE,
	XMODEM_CODE_XON,
	XMODEM_CODE_XOFF,
	XMODEM_CODE_NAK,
	XMODEM_CODE_CAN,
	XMODEM_CODE_CTRLZ,
	XMODEM_CODE_NUM
} e_xmodem_code;

static uint8_t str_xmodem[] = {0x01, 0x02, 0x04, 0x06, 0x10, 0x11, 0x13, 0x15, 0x18, 0x1A};
static uint8_t str_xmodem_ready[] = "RX ready ...";
static uint8_t str_xmodem_done[] = "done\r\n";

/******************************************************************************
 Macro definitions
******************************************************************************/

/******************************************************************************
 EEPROM Production Reset Value
******************************************************************************/

/******************************************************************************
 Global Variable
******************************************************************************/
e_fsm_fwup_state	gFsmFwupState;
uint16_t			gFsmFwupStateTimeout;

void fsm_fwup_create (void) {
	gFsmFwupState = FSM_FWUP_STATE_INIT;
}

void fsm_fwup_handler (void) {
	uint8_t header;
	uint8_t block, block_complement;
	uint8_t packet[128];
	uint8_t chksum;
	
	switch (gFsmFwupState) {
		case FSM_FWUP_STATE_INIT:
			RL78G14_UART2_Send (str_xmodem_ready, sizeof (str_xmodem_ready));
			gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (1000);
			gFsmFwupState = FSM_FWUP_STATE_PRINT_HEADER;
			break;

		case FSM_FWUP_STATE_PRINT_HEADER:
			if (RL78G14_GetTick ( ) == gFsmFwupStateTimeout)
				gFsmFwupState = FSM_FWUP_STATE_IDLE;
			break;
			
		case FSM_FWUP_STATE_IDLE:
			header = 0;
			block = 0;
			block_complement = 0;
			memset (packet, 0, 128);
			RL78G14_UART2_Send (&str_xmodem[XMODEM_CODE_NAK], 1);
			gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (1000);
			gFsmFwupState = FSM_FWUP_STATE_WAIT_PACKET;
			break;

		case FSM_FWUP_STATE_WAIT_PACKET:
			if (RL78G14_GetTick ( ) == gFsmFwupStateTimeout)
				gFsmFwupState = FSM_FWUP_STATE_IDLE;
			else {
				RL78G14_UART2_Receive (&header, 1);
				switch (header) {
					case SOH:
						RL78G14_UART2_Receive (&block, 1);
						RL78G14_UART2_Receive (&block_complement, 1);
						if (block == ~block_complement) {
							RL78G14_UART2_Receive (packet, 128);
							RL78G14_UART2_Receive (&chksum, 1);
							RL78G14_UART2_Send (&str_xmodem[XMODEM_CODE_ACK], 1);
						}
						else
							RL78G14_UART2_Send (&str_xmodem[XMODEM_CODE_NAK], 1);
						break;
						
					case STX:
						RL78G14_UART2_Send (&str_xmodem[XMODEM_CODE_NAK], 1);
						break;
						
					case EOT:
						RL78G14_UART2_Send (&str_xmodem[XMODEM_CODE_ACK], 1);
						gFsmFwupState = FSM_FWUP_STATE_EOT;
						break;
						
					default:
						break;
				}
			}
			break;
			
		case FSM_FWUP_STATE_EOT:
			RL78G14_UART2_Send (str_xmodem_done, sizeof (str_xmodem_done));
			gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (1000);
			gFsmFwupState = FSM_FWUP_STATE_PRINT_DONE;
			break;

		case FSM_FWUP_STATE_PRINT_DONE:
			if (RL78G14_GetTick ( ) == gFsmFwupStateTimeout)
				gFsmFwupState = FSM_FWUP_STATE_COMPLETE;
			break;

		case FSM_FWUP_STATE_COMPLETE:
			break;
	}	// switch
}

e_fsm_fwup_state fsm_fwup_get_state (void) {
	return gFsmFwupState;
}
