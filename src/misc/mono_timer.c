#include <ithaca.h>

#if ITHACA_USE_MONO_TIMER || defined(__DOXYGEN__)

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
 */
bool monoTimerInit(mono_timer_t *timer) {
    //
    timer->end = 0;
    // 
    return true;
}

/*
 * @brief   ...
 */
bool monoTimerRestart(mono_timer_t *timer, varg_t period) {
    //
    timer->end = chTimeNow() + S2ST((uint32_t)period);
    //
    return true;
}

/*
 * @brief   ...
 */
bool monoTimerActive(mono_timer_t *timer, varg_t arg) {
    //
    (void)arg;
    //
    if (chTimeNow() < timer->end) {
        return true;
    } else {
        return false;
    }
}

#endif /* ITHACA_USE_MONO_TIMER */

