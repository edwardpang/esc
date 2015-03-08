#include "common.h"

#include "fsl.h"
#include "fsl_types.h"

void init (void) {
	PIOR0 = 0x00U;
	PIOR1 = 0x00U;
	
	CRC0CTL = 0x00U;
	IAWCTL = 0x00U;
	PMS = 0x00U;
}

void main (void) {
}
