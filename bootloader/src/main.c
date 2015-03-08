#include "common.h"

#include "rl78g14_cgc.h"
#include "rl78g14_serial.h"

#include "fsl.h"
#include "fsl_types.h"

void init (void) {
	PIOR0 = 0x00U;
	PIOR1 = 0x00U;
	
	/* CGC */
	/* Set fMX */
	CMC = _00_CGC_HISYS_PORT | _00_CGC_SYSOSC_DEFAULT;
	MSTOP = 1U;
	/* Set fMAIN */
	MCM0 = 0U;
	OSMC = _00_CGC_RTC_CLK_NO;
	/* Set fIH */
	HIOSTOP = 0U;

	/* SAU0 */
	SAU0EN = 1U;    /* supply SAU0 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS0 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;
	
	/* UART1 */
	ST0 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART1 receive and transmit */
	STMK1 = 1U;    /* disable INTST1 interrupt */
	STIF1 = 0U;    /* clear INTST1 interrupt flag */
	SRMK1 = 1U;    /* disable INTSR1 interrupt */
	SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
	SREMK1 = 1U;   /* disable INTSRE1 interrupt */
	SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
	/* Set INTST1 low priority */
	STPR11 = 1U;
	STPR01 = 1U;
	/* Set INTSR1 low priority */
	SRPR11 = 1U;
	SRPR01 = 1U;
	SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR02 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SDR02 = _CE00_UART1_TRANSMIT_DIVISOR;
	NFEN0 |= _04_SAU_RXD1_FILTER_ON;
	SIR03 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
	SMR03 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR03 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SDR03 = _CE00_UART1_RECEIVE_DIVISOR;
	SO0 |= _0004_SAU_CH2_DATA_OUTPUT_1;
	SOL0 |= _0000_SAU_CHANNEL2_NORMAL;    /* output level normal */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART1 output */
	/* Set RxD1 pin */
	PMC0 &= 0xFDU;
	PM0 |= 0x02U;
	/* Set TxD1 pin */
	PMC0 &= 0xFEU;
	P0 |= 0x01U;
	PM0 &= 0xFEU;
	
	CRC0CTL = 0x00U;
	IAWCTL = 0x00U;
	PMS = 0x00U;
}

void main (void) {
}
