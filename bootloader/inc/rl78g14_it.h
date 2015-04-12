#ifndef IT_H
#define IT_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/* 
    Interval timer control register (ITMC)
*/
/* Interval timer operation enable/disable specification (RINTE) */
#define _0000_IT_OPERATION_DISABLE    (0x0000U)    /* disable interval timer operation */
#define _8000_IT_OPERATION_ENABLE     (0x8000U)    /* enable interval timer operation */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Interval timer compare value (ITMCMP11 - 0) */
#define _05DB_ITMCMP_VALUE            (0x05DBU)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void RL78G14_IT_Create(void);
void RL78G14_IT_Start(void);
void RL78G14_IT_Stop(void);
void RL78G14_IT_Set_PowerOff(void);
void RL78G14_IT_Create_UserInit(void);
uint16_t RL78G14_GetTick (void);
uint16_t RL78G14_GetTickAfterMs (uint16_t interval);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
