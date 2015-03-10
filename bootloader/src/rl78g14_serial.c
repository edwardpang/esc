/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "common.h"
#include "rl78g14_serial.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_uart1_tx_address;        /* uart1 transmit buffer address */
volatile uint16_t  g_uart1_tx_count;           /* uart1 transmit data number */
volatile uint8_t * gp_uart1_rx_address;        /* uart1 receive buffer address */
volatile uint16_t  g_uart1_rx_count;           /* uart1 receive data number */
volatile uint16_t  g_uart1_rx_length;          /* uart1 receive data length */

/***********************************************************************************************************************
* Function Name: RL78G14_SAU0_Create
* Description  : This function initializes the SAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_SAU0_Create(void)
{
    SAU0EN = 1U;    /* supply SAU0 clock */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS0 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;
    RL78G14_UART1_Create();
}

/***********************************************************************************************************************
* Function Name: RL78G14_UART1_Create
* Description  : This function initializes the UART1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_UART1_Create(void)
{
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
    SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR02 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
    SDR02 = _CE00_UART1_TRANSMIT_DIVISOR;
    NFEN0 |= _04_SAU_RXD1_FILTER_ON;
    SIR03 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;    /* clear error flag */
    SMR03 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL |
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR03 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 |
            _0007_SAU_LENGTH_8;
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
}

/***********************************************************************************************************************
* Function Name: RL78G14_UART1_Start
* Description  : This function starts the UART1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_UART1_Start(void)
{
    STIF1 = 0U;    /* clear INTST1 interrupt flag */
    STMK1 = 0U;    /* enable INTST1 interrupt */
    SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
    SRMK1 = 0U;    /* enable INTSR1 interrupt */
    SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
    SREMK1 = 0U;   /* enable INTSRE1 interrupt */
    SO0 |= _0004_SAU_CH2_DATA_OUTPUT_1;    /* output level normal */
    SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;    /* enable UART1 output */
    SS0 |= _0008_SAU_CH3_START_TRG_ON | _0004_SAU_CH2_START_TRG_ON;    /* enable UART1 receive and transmit */
}

/***********************************************************************************************************************
* Function Name: RL78G14_UART1_Stop
* Description  : This function stops the UART1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_UART1_Stop(void)
{
    ST0 |= _0008_SAU_CH3_STOP_TRG_ON | _0004_SAU_CH2_STOP_TRG_ON;    /* disable UART1 receive and transmit */
    SOE0 &= ~_0004_SAU_CH2_OUTPUT_ENABLE;    /* disable UART1 output */
    STMK1 = 1U;    /* disable INTST1 interrupt */
    STIF1 = 0U;    /* clear INTST1 interrupt flag */
    SRMK1 = 1U;    /* disable INTSR1 interrupt */
    SRIF1 = 0U;    /* clear INTSR1 interrupt flag */
    SREMK1 = 1U;   /* disable INTSRE1 interrupt */
    SREIF1 = 0U;   /* clear INTSRE1 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: RL78G14_UART1_Receive
* Description  : This function receives UART1 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS RL78G14_UART1_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart1_rx_count = 0U;
        g_uart1_rx_length = rx_num;
        gp_uart1_rx_address = rx_buf;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: RL78G14_UART1_Send
* Description  : This function sends UART1 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS RL78G14_UART1_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart1_tx_address = tx_buf;
        g_uart1_tx_count = tx_num;
        STMK1 = 1U;    /* disable INTST1 interrupt */
        TXD1 = *gp_uart1_tx_address;
        gp_uart1_tx_address++;
        g_uart1_tx_count--;
        STMK1 = 0U;    /* enable INTST1 interrupt */
    }

    return (status);
}
