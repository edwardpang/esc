/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "common.h"
#include "rl78g14_cgc.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: RL78G14_CGC_Create
* Description  : This function initializes the clock generator.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_CGC_Create(void)
{
    /* Set fMX */
    CMC = _00_CGC_HISYS_PORT | _00_CGC_SYSOSC_DEFAULT;
    MSTOP = 1U;
    /* Set fMAIN */
    MCM0 = 0U;
    OSMC = _00_CGC_RTC_CLK_NO;
    /* Set fIH */
    HIOSTOP = 0U;
}
