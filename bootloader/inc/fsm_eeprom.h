#ifndef _TASK_EEPROM_H_
#define _TASK_EEPROM_H_

typedef enum {
	FSM_EEPROM_STATE_INIT = 0,

	FSM_EEPROM_STATE_NUM
} e_fsm_eeprom_state;

void fsm_eeprom_create (void);
void fsm_eeprom_handler (void);

#endif // _TASK_EEPROM_H_
