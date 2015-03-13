#include "common.h"
#include "fsm_eeprom.h"

/* Header files for FDL/EEL */
#include "fdl.h"                    /* FDL library header file               */
#include "fdl_types.h"              /* FDL types definition header file      */
#include "fdl_descriptor.h"         /* FDL descriptor header file            */
#include "eel.h"                    /* EEL library header file               */
#include "eel_types.h"              /* EEL types definition header file      */
#include "eel_descriptor.h"         /* EEL descriptor header file            */
#include "eel_user_types.h"         /* EEL user types definition header file */

/******************************************************************************
 Macro definitions
******************************************************************************/
#define SAM_WRITE_SIZE 10           /* Declare write data size */

/******************************************************************************
 Global Variable
******************************************************************************/
e_fsm_eeprom_state	FsmEepromState;

/* Definition of variables */
eel_request_t   dtyEelReq;
eel_u08         dubWriteBuffer[ SAM_WRITE_SIZE ] = {0,1,2,3,4,5,6,7,8,9};
eel_u08         dubReadBuffer[ SAM_WRITE_SIZE ];
fdl_status_t    fdlStatus = 0; 
eel_u08         err_flag = 0;

void fsm_eeprom_create (void) {
	FsmEepromState = FSM_EEPROM_STATE_INIT;
}

void fsm_eeprom_handler (void) {
	switch (FsmEepromState) {
		case FSM_EEPROM_STATE_INIT:
			/* Initialize EEL requester */
			dtyEelReq.address_pu08   = 0;
			dtyEelReq.identifier_u08 = 0;
			dtyEelReq.command_enu    = 0;
			dtyEelReq.status_enu     = 0;

			/*== EEL / uninitialized =*/
			/***************************************************************************/
			/* FDL initialization processing                                           */
			/***************************************************************************/
			/* Initialize FDL parameters */
			fdlStatus = FDL_Init( &fdl_descriptor_str );
		    
		    if( fdlStatus == FDL_OK )
		    {
		        /*== EEL / uninitialized =*/
		        /***************************************************************************/
		        /* FDL preparation processing                                              */
		        /***************************************************************************/
		        FDL_Open();
		       
		        /*== -> EEL / uninitialized -> ( Change EEL state ) ========*/
		        /*******************************************************************************/
		        /* EEL initialization processing( state : EEL / uninitialized -> EEL / closed )*/
		        /*******************************************************************************/
		        dtyEelReq.status_enu = EEL_Init();
		        
		        if( dtyEelReq.status_enu == EEL_OK )
		        {
		            /*== EEL / closed -> ( Change EEL state ) ===============*/
		            /***************************************************************************/
		            /* EEL preparation processing ( state : EEL / closed -> EEL / opened )     */
		            /***************************************************************************/
		            EEL_Open();
				}
			}
			break; // FSM_EEPROM_STATE_INIT
	}	// switch
}
