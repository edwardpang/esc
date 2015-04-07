#include "common.h"

#include "rl78g14_cgc.h"
#include "rl78g14_serial.h"

#include "fsm_eeprom.h"

void HwInit (void);
void SwInit (void);

void main (void) {
	HwInit ( );
	SwInit ( );
	while (1)
		fsm_eeprom_handler ( );
}

void HwInit (void) {
	DI ( );
	
	PIOR0 = 0x00U;
	PIOR1 = 0x00U;
	
	RL78G14_CGC_Create ( );
	RL78G14_SAU0_Create	( );
	
	CRC0CTL = 0x00U;
	IAWCTL = 0x00U;
	PMS = 0x00U;
	
	EI ( );
}

void SwInit (void) {
	fsm_eeprom_create ( );
}
