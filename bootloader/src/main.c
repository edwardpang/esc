#include "common.h"

#include "rl78g14_cgc.h"
#include "rl78g14_serial.h"

#include "fsm_eeprom.h"

#include "resource_string.c"

bit g_bootloader_done;

void HwInit (void);
void SwInit (void);

void main (void) {
	
	HwInit ( );
	SwInit ( );
	
	g_bootloader_done = 0;
	while (!g_bootloader_done) {
		fsm_eeprom_handler ( );
	}
}

void HwInit (void) {
	DI ( );
	
	PIOR0 = 0x00U;
	PIOR1 = 0x00U;
	
	RL78G14_CGC_Create ( );
	RL78G14_SAU1_Create	( );
	
	CRC0CTL = 0x00U;
	IAWCTL = 0x00U;
	PMS = 0x00U;
	
	EI ( );
}

void SwInit (void) {
	
	RL78G14_UART2_Start ( );
	RL78G14_UART2_Send (str_header, sizeof (str_header));

	fsm_eeprom_create ( );
}
