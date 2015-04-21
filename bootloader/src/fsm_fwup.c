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
			break;
			
		case FSM_FWUP_STATE_COMPLETE:
			break;
	}	// switch
}

e_fsm_fwup_state fsm_fwup_get_state (void) {
	return gFsmFwupState;
}
