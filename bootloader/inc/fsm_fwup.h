#ifndef _FSM_FWUP_H_
#define _FSM_FWUP_H_

typedef enum {
	FSM_FWUP_STATE_INIT = 0,
	FSM_FWUP_STATE_IDLE,
	FSM_FWUP_STATE_PRINT_HEADER,
	FSM_FWUP_STATE_WAIT_PACKET,
	FSM_FWUP_STATE_EOT,
	FSM_FWUP_STATE_PRINT_DONE,
	FSM_FWUP_STATE_COMPLETE,

	FSM_FWUP_STATE_NUM
} e_fsm_fwup_state;

void fsm_fwup_create (void);
void fsm_fwup_handler (void);
e_fsm_fwup_state fsm_fwup_get_state (void);

#endif // _FSM_FWUP_H_