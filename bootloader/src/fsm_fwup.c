#include "common.h"
#include "fsm_fwup.h"
#include "rl78g14_serial.h"
#include "rl78g14_it.h"

#include <string.h>

static uint8_t str_at[] = "AT";
static uint8_t str_at_ok[] = "OK";
static uint8_t str_type_remote_control[] = "AT+TYPE1";
static uint8_t str_type_remote_control_ok[] = "OK+Set:1";
static uint8_t str_type_remote_address[] = "AT+RADD?";
static uint8_t str_type_remote_not_connected[] = "OK+RADD:000000000000";

static uint8_t str_type_ep_hello[] = "EP+HELLO";
static uint8_t str_type_ep_fwup_start[] = "EP+FWUP+START";
static uint8_t str_type_ok_fwup_start[] = "OK+FWUP+START";
static uint8_t str_type_ep_fwup_total_block_num[] = "EP+FWUP+TBN:";
static uint8_t str_type_ep_fwup_block[] = "EP+FWUP+BLK:";
static uint8_t str_type_ok_fwup_block[] = "OK+FWUP+BLK:000";

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

void delay (uint8_t t) {
	uint16_t dummy = (t << 8);

	while (dummy--);
}

void fsm_fwup_create (void) {
	gFsmFwupState = FSM_FWUP_STATE_INIT;
}

void fsm_fwup_handler (void) {
	uint8_t rxbuf[200];
	
	memset (rxbuf, 0, 200);
	switch (gFsmFwupState) {
		case FSM_FWUP_STATE_INIT:
			RL78G14_UART2_Send (str_at, sizeof (str_at)-1);
			RL78G14_UART2_Receive (&rxbuf, sizeof (str_at_ok)-1);

			gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (100);
			while (RL78G14_GetTick ( ) != gFsmFwupStateTimeout);

			if (strstr (rxbuf, str_at_ok) != NULL) {
				RL78G14_UART2_Send (str_type_remote_control, sizeof (str_type_remote_control)-1);
				RL78G14_UART2_Receive (&rxbuf, sizeof (str_type_remote_control_ok)-1);
				gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (100);
				while (RL78G14_GetTick ( ) != gFsmFwupStateTimeout);
				if (strstr (rxbuf, str_type_remote_control_ok) != NULL)
					gFsmFwupState = FSM_FWUP_STATE_IDLE;
			}
			break;

		case FSM_FWUP_STATE_IDLE:
			RL78G14_UART2_Send (str_type_remote_address, sizeof (str_type_remote_address)-1);
			RL78G14_UART2_Receive (&rxbuf, sizeof (str_type_remote_not_connected)-1);
			gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (200);
			while (RL78G14_GetTick ( ) != gFsmFwupStateTimeout);
			if (strlen (rxbuf) == 0)
				gFsmFwupState = FSM_FWUP_STATE_DEVICE_CONNECTED;
			break;

		case FSM_FWUP_STATE_DEVICE_CONNECTED:
			RL78G14_UART2_Receive (&rxbuf, sizeof (str_type_ep_fwup_start)-1);
			gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (1000);
			while (RL78G14_GetTick ( ) != gFsmFwupStateTimeout);
			if (strstr (rxbuf, str_type_ep_fwup_start) != NULL) {
				RL78G14_UART2_Send (str_type_ok_fwup_start, sizeof (str_type_ok_fwup_start)-1);
				gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (1000);
				while (RL78G14_GetTick ( ) != gFsmFwupStateTimeout);
				gFsmFwupState = FSM_FWUP_STATE_FWUP_STARTED;
			}
			break;

		case FSM_FWUP_STATE_ANDROID_ECHO:
			break;

		case FSM_FWUP_STATE_FWUP_STARTED:
			RL78G14_UART2_Receive (&rxbuf, sizeof (str_type_ep_fwup_block)-1+3);
			gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (5000);
			while (RL78G14_GetTick ( ) != gFsmFwupStateTimeout);
			if (strstr (rxbuf, str_type_ep_fwup_block) != NULL) {
				int i = sizeof (str_type_ok_fwup_block) - 1;
				str_type_ok_fwup_block[i-1] = rxbuf[i-1];
				str_type_ok_fwup_block[i-2] = rxbuf[i-2];
				str_type_ok_fwup_block[i-3] = rxbuf[i-3];
				RL78G14_UART2_Send (str_type_ok_fwup_block, sizeof (str_type_ok_fwup_block)-1);
				gFsmFwupStateTimeout = RL78G14_GetTickAfterMs (1000);
				while (RL78G14_GetTick ( ) != gFsmFwupStateTimeout);
				//gFsmFwupState = FSM_FWUP_STATE_FWUP_READY;
			}
			break;
			
		case FSM_FWUP_STATE_COMPLETE:
			break;
	}	// switch
}

e_fsm_fwup_state fsm_fwup_get_state (void) {
	return gFsmFwupState;
}
