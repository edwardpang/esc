/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt INTIT r_it_interrupt
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "common.h"
#include "rl78g14_it.h"
/* Start user code for include. Do not edit comment generated here */
#define TIME_1MS		(1)					// 1
#define TICK_PERIOD		(100 * TIME_1MS)	// 100ms
#define OVERFLOW_TICK	(10000)				// 10000 * 100ms = 1000000ms = 1000s
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint16_t	gIntervalTimerTick;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: RL78G14_IT_Create
* Description  : This function initializes the IT module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_IT_Create(void)
{
    RTCEN = 1U;    /* supply IT clock */
    ITMC = _0000_IT_OPERATION_DISABLE;    /* disable IT operation */
    ITMK = 1U;    /* disable INTIT interrupt */
    ITIF = 0U;    /* clear INTIT interrupt flag */
    /* Set INTIT low priority */
    ITPR1 = 1U;
    ITPR0 = 1U;
    ITMC = _05DB_ITMCMP_VALUE;

    RL78G14_IT_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: RL78G14_IT_Start
* Description  : This function starts IT module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_IT_Start(void)
{
    ITIF = 0U;    /* clear INTIT interrupt flag */
    ITMK = 0U;    /* enable INTIT interrupt */
    ITMC |= _8000_IT_OPERATION_ENABLE;    /* enable IT operation */
}

/***********************************************************************************************************************
* Function Name: RL78G14_IT_Stop
* Description  : This function stops IT module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_IT_Stop(void)
{
    ITMK = 1U;    /* disable INTIT interrupt */
    ITIF = 0U;    /* clear INTIT interrupt flag */
    ITMC &= (uint16_t)~_8000_IT_OPERATION_ENABLE;    /* disable IT operation */
}

/***********************************************************************************************************************
* Function Name: RL78G14_IT_Set_PowerOff
* Description  : This function stops the clock supplied for IT.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_IT_Set_PowerOff(void)
{
    RTCEN = 0U;    /* stop IT clock */
}

/***********************************************************************************************************************
* Function Name: RL78G14_IT_Create_UserInit
* Description  : This function adds user code after initializing IT module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void RL78G14_IT_Create_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
	gIntervalTimerTick = 0;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_it_interrupt
* Description  : This function is INTIT interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_it_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	gIntervalTimerTick ++;
	if (gIntervalTimerTick == OVERFLOW_TICK)
		gIntervalTimerTick = 0;
		
	/* Clear IT interrupt flag */
    ITIF = 0;
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
uint16_t RL78G14_GetTick (void) {
	return gIntervalTimerTick;
}

uint16_t RL78G14_GetTickAfterMs (uint16_t interval) {
	uint16_t target = gIntervalTimerTick + (interval/TICK_PERIOD);
	if (target < OVERFLOW_TICK)
		return target;
	else
		return target - OVERFLOW_TICK;
}
/* End user code. Do not edit comment generated here */
