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
typedef struct {
    // ...
    char *name;
    bool flag;
} lld_lock_t;

#include <misc/device_id.h>

#include <misc/console.h>

#include <misc/block.h>
#include <misc/rung.h>
#include <misc/ladder.h>

#include <misc/button.h>
#include <misc/digital_output.h>
#include <misc/hcsr501.h>
#include <misc/keypad44.h>
#include <misc/mono_timer.h>
#include <misc/pca9633.h>

#include <misc/font.h>
#include <misc/font_std.h>
#include <misc/lcd_st7735.h>

#include <misc/imu.h>

#include <radio/packet.h>
#include <radio/rfm12b.h>
#include <radio/mesh.h>
#include <radio/rc.h>

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
bool lldLock(lld_lock_t *lock);
bool lldLockWaitTimeout(lld_lock_t *lock, systime_t tm);
void lldUnlock(lld_lock_t *lock);
bool lldLockISR(lld_lock_t *lock);
void lldUnlockISR(lld_lock_t *lock);
//
extern EXTConfig EXTCFG1;
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_LIB */

#endif /* _ITHACA_H_ */

