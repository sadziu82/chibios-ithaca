#ifndef _ITHACA_H_
#define _ITHACA_H_

#include "ch.h"
#include "hal.h"

#include "ithacaconf.h"

#if ITHACA_USE_LIB || defined(__DOXYGEN__)

/*
 * @brief   ...
 * @details ...
 */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    // ...
    char *name;
    bool flag;
} ithaca_lock_t;

#include <stdarg.h>
#include <misc/chsprintf.h>

#include <misc/device_id.h>

#include <misc/console.h>

#include <misc/block.h>
#include <misc/rung.h>
#include <misc/ladder.h>

#include <misc/button.h>
#include <misc/digital_output.h>
#include <misc/keypad44.h>
#include <misc/mono_timer.h>

#include <misc/pca9633.h>
#include <sensors.h>
#include <misc/imu.h>

#include <radio.h>

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
//
bool ithacaLock(ithaca_lock_t *lock);
bool ithacaLockTimeout(ithaca_lock_t *lock, systime_t tm);
void ithacaUnlock(ithaca_lock_t *lock);
bool ithacaLockISR(ithaca_lock_t *lock);
void ithacaUnlockISR(ithaca_lock_t *lock);
//
char *asprintf(char *buffer, char *fmt, ...);

//
extern EXTConfig ext1_cfg;
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_LIB */

#endif /* _ITHACA_H_ */

