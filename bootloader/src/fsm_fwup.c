#include "common.h"
#include "fsm_fwup.h"

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

void fsm_fwup_create (void) {
	gFsmFwupState = FSM_FWUP_STATE_INIT;
}

void fsm_fwup_handler (void) {
	switch (gFsmFwupState) {

	}	// switch
}

e_fsm_fwup_state fsm_fwup_get_state (void) {
	return gFsmFwupState;
}
