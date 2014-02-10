#include <ithaca.h>

#if ITHACA_USE_LIB || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
EXTConfig ext1_cfg;

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
 * @brief   
 */
inline bool ithacaLock(ithaca_lock_t *lock) {
    bool locked = false;
    //
    chSysLock();
    if (lock->flag == false) {
        lock->flag = true;
        locked = true;
    }
    chSysUnlock();
    if (locked == false) {
        consoleDebug("%s lock failed\r\n", lock->name);
    }
    return locked;
}

/*
 * @brief   
 */
inline bool ithacaLockTimeout(ithaca_lock_t *lock, systime_t tm) {
    bool locked = false;
    systime_t timeout = chTimeNow() + MS2ST(tm);
    //
    while (chTimeNow() < timeout) {
        chSysLock();
        if (lock->flag == false) {
            lock->flag = true;
            locked = true;
            chSysUnlock();
            break;
        }
        chSysUnlock();
        chThdYield();
    }
    if (locked == false) {
        consoleDebug("%s lock wait timeout failed\r\n", lock->name);
    }
    return locked;
}

/*
 * @brief   
 */
inline void ithacaUnlock(ithaca_lock_t *lock) {
    //
    chSysLock();
    if (lock->flag == true) {
        lock->flag = false;
    }
    chSysUnlock();
}

/*
 * @brief   
 */
inline bool ithacaLockISR(ithaca_lock_t *lock) {
    bool locked = false;
    //
    chSysLockFromIsr();
    if (lock->flag == false) {
        lock->flag = true;
        locked = true;
    }
    chSysUnlockFromIsr();
    return locked;
}

/*
 * @brief   
 */
inline void ithacaUnlockISR(ithaca_lock_t *lock) {
    //
    chSysLockFromIsr();
    if (lock->flag == true) {
        lock->flag = false;
    }
    chSysUnlockFromIsr();
}

#endif /* ITHACA_USE_LIB */


