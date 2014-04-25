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

/*
 * @brief   ...
 * @details ...
 */
char *asprintf(char *buffer, char *fmt, ...) {
    //
    va_list args;
    //
    if (buffer != NULL) {
        chHeapFree(buffer);
        buffer = NULL;
    }
    // calculate string legth
    va_start(args, fmt);
    uint8_t len = chvsprintf(NULL, fmt, args);
    va_end(args);
    //
    buffer = (char *)chHeapAlloc(NULL, len + 1);
    //
    va_start(args, fmt);
    chvsprintf(buffer, fmt, args);
    va_end(args);
    buffer[len] = '\0';
    return buffer;
}

/*
 * @brief   
 */
uint32_t ithacaCRC32(uint8_t message[], uint8_t len) {
    uint32_t crc = 0;
    uint8_t idx;
    //
    for (idx = 0; idx < len; idx++) {
        crc += (message[idx] << (idx % 24));
    }
    //
    return crc;
}

#endif /* ITHACA_USE_LIB */


