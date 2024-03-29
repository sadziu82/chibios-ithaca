#include <ithaca.h>

#if ITHACA_USE_RUNG || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool rungExecute(const rung_t *rung) {
    //
    uint8_t i;
    block_func_t func;
    // 
    i = 0;
    //
    while (rung->blocks[i].block != NULL) {
        func = rung->blocks[i].func;
        if ((*func)(rung->blocks[i].block, rung->blocks[i].arg) == false) {
            return false;
        }
        i++;
    }
    return true;
}

#endif /* ITHACA_USE_RUNG */

