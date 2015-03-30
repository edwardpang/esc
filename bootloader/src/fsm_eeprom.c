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
#define SAM_WRITE_SIZE EEP_IDENTITY_SIZE           /* Declare write data size */

/******************************************************************************
 EEPROM Production Reset Value
******************************************************************************/
const eep_type_identity eep_reset_version = {
	'E', 'S', 'C', ' ', ' ', ' ',
	'E', 'S', 'C', '2', ' ', ' ',
	'0', '0', '0', '0', '0', '0', '0', '0',
	0, 1,									// HARDWARD VERSION
	0, 1,									// SOFTWARE VERSION
	0, 1,									// EEPROM VERSION
};

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
					
		            /*== ->  EEL / opened -> ( Change EEL state ) ===============*/
		            /***************************************************************************/
		            /* EEL execution start processing ( state : EEL / opened -> EEL / started )*/
		            /***************************************************************************/
		            do {
		                /* Set parameter of EEL_CMD_STARTUP command   */
		                dtyEelReq.command_enu = EEL_CMD_STARTUP;
		                
		                /* Execute EEL_CMD_STARTUP command */
		                EEL_Execute( &dtyEelReq ); 
		                
		                /* Wait of command processing end */
		                while( dtyEelReq.status_enu == EEL_BUSY )
		                {
		                     EEL_Handler(); /* Check of command end */
		                }
		                    
		                /* EEPROM emulation block inconsistency error */
		                if( dtyEelReq.status_enu == EEL_ERR_POOL_INCONSISTENT )
		                {
		                    /* Set parameter of EEL_CMD_FORMAT command   */
		                    dtyEelReq.command_enu = EEL_CMD_FORMAT;
		                    
		                    /* Execute EEL_CMD_FORMAT command */
		                    EEL_Execute( &dtyEelReq );
		                    
		                    /* Wait of command processing end */
		                    while( dtyEelReq.status_enu == EEL_BUSY )
		                    {
		                        EEL_Handler();  /* Check of command end */
		                    }
		                }
		            /* If EEL_CMD_FORMAT command is successful,       */
		            /* re-execute EEL_CMD_STARTUP command.            */
		            } while( ( dtyEelReq.command_enu == EEL_CMD_FORMAT ) &&
		                     ( dtyEelReq.status_enu  == EEL_OK         )  );
		            
		            /* EEL_CMD_FORMAT command or EEL_CMD_STARTUP command is successful. */
		            /* If command is abnormal end,  execute EEL_CMD_SHUTDOWN command.   */
					if( dtyEelReq.status_enu == EEL_OK ) {
						FsmEepromState = FSM_EEPROM_STATE_EEL_READY;
					}
					else {
						FsmEepromState = FSM_EEPROM_STATE_EEL_SHUTDOWN;
					}
				}
				else {
					FsmEepromState = FSM_EEPROM_STATE_FDL_CLOSE;
				}
			}
			else {
				FsmEepromState = FSM_EEPROM_STATE_END;
			}
			break; // FSM_EEPROM_STATE_INIT
			
		case FSM_EEPROM_STATE_EEL_READY:
            do {
	            /**************************/
	            /* data write processing  */
	            /**************************/
	            /* Set of write data */
				memcpy (dubWriteBuffer, eep_reset_version, sizeof (eep_type_identity));
	            
	            /* Set parameter of EEL_CMD_WRITE command */
	            dtyEelReq.address_pu08   = dubWriteBuffer;
	            dtyEelReq.identifier_u08 = 1; 
	            dtyEelReq.command_enu    = EEL_CMD_WRITE;
	            
	            /* Execute EEL_CMD_WRITE command */
	            EEL_Execute( &dtyEelReq );
	            
	            /* Wait of command processing end */
	            while( dtyEelReq.status_enu == EEL_BUSY )
	            {
	                /* Check of command end */
	                EEL_Handler();
	            }
	            
	             /* Pool full error */
	            if (dtyEelReq.status_enu == EEL_ERR_POOL_FULL)
	            {
	               /* Set parameter of EEL_CMD_REFRESH command */
	                dtyEelReq.command_enu    = EEL_CMD_REFRESH;
	                
	                /* Execute EEL_CMD_REFRESH command */
	                EEL_Execute( &dtyEelReq );
	                
	                /* Wait of command processing end */
	                while( dtyEelReq.status_enu == EEL_BUSY )
	                {
	                    /* Check of command end */
	                    EEL_Handler();
	                }
	            }
	        /* If EEL_CMD_REFRESH command is  successful, */
	        /* re-execute EEL_CMD_WRITE command..         */
	        } while( ( dtyEelReq.command_enu == EEL_CMD_REFRESH ) &&
	                 ( dtyEelReq.status_enu  == EEL_OK          )  );
	        
	        /* EEL_CMD_WRITE command or EEL_CMD_REFRESH command is successful. */
	        /* If command is abnormal end, execute EEL_CMD_SHUTDOWN command.   */
	        if ( dtyEelReq.status_enu == EEL_OK )   
	        {
	            /************************/
	            /* data read processing */
	            /************************/
	            /* Definition for loop variables*/
	            fdl_u16    duh_i;   
	            /* Set parameter of EEL_CMD_READ command */
	            dtyEelReq.address_pu08   = dubReadBuffer;
	            dtyEelReq.identifier_u08 = 1; 
	            dtyEelReq.command_enu    = EEL_CMD_READ;
	            
	            /* Execute EEL_CMD_READ command */
	            EEL_Execute( &dtyEelReq );
	            
	            /* Wait of command processing end */
	            while( dtyEelReq.status_enu == EEL_BUSY )
	            {
	                /* Check of command end */
	                EEL_Handler();
	            }
	            
	            if (dtyEelReq.status_enu == EEL_OK )
	            {
	                /* Compare of data of read and data of write. */
	                for( duh_i = 0 ; duh_i < sizeof(eep_type_identity) ; duh_i++ )
	                {
	                    if( dubWriteBuffer[ duh_i ] != dubReadBuffer[ duh_i ] )
	                    {
	                        /* error handling */
	                        err_flag = 1;
	                        break;
	                    }
	                }
	            }
	        }
	        /*== ->  EEL / started -> ( Change EEL state ) ==========*/
			FsmEepromState = FSM_EEPROM_STATE_EEL_SHUTDOWN;
			break;
		
		case FSM_EEPROM_STATE_EEL_SHUTDOWN:
			/*== ->  EEL / started -> ( Change EEL state ) ==========*/
            /***************************************************************************/
            /* EEL Execution stop processing ( state : EEL / started -> opened )       */
            /***************************************************************************/
            /* Set parameter of EEL_CMD_SHUTDOWN command */
            dtyEelReq.command_enu = EEL_CMD_SHUTDOWN;
            
            /* Execute EEL_CMD_SHUTDOWN */
            EEL_Execute( &dtyEelReq );
            
            /* Wait of command processing end */
            while( dtyEelReq.status_enu == EEL_BUSY )
            {
                /* Check of command end */
                EEL_Handler();
            }
			FsmEepromState = FSM_EEPROM_STATE_EEL_CLOSE;
			break;
			
		case FSM_EEPROM_STATE_EEL_CLOSE:
			EEL_Close ( );
			FsmEepromState = FSM_EEPROM_STATE_FDL_CLOSE;
			break;
			
		case FSM_EEPROM_STATE_FDL_CLOSE:
			FDL_Close ( );
			FsmEepromState = FSM_EEPROM_STATE_END;
			break;
			
		case FSM_EEPROM_STATE_END:
			break;
	}	// switch
}
