#ifndef __FDL_DESCRIPTOR_H_INCLUDED
#define __FDL_DESCRIPTOR_H_INCLUDED

#include "fdl_types.h"

/* specify the CPU frequency in [Hz]                                                              */
#define   FDL_SYSTEM_FREQUENCY      32000000

/* programming voltage mode                                                                       */
/* #define   FDL_WIDE_VOLTAGE_MODE  */

/* specify the size of the FDL pool expressed in number of blocks                                 */
#define   FDL_POOL_BLOCKS               0

/* specify the size of the EEL pool inside the FDL pool expressed in blocks (min. value is 2)     */
#define   EEL_POOL_BLOCKS               4

/* external reference to descriptor */
extern    __far const fdl_descriptor_t  fdl_descriptor_str;

#endif
