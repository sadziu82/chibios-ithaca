#ifndef _ITHACA_H_
#define _ITHACA_H_


#include "ch.h"
#include "hal.h"

#include "ithacaconf.h"

#if ITHACA_USE_LIB || defined(__DOXYGEN__)

#include <misc/console.h>

#include <misc/block.h>
#include <misc/rung.h>
#include <misc/ladder.h>

#include <misc/button.h>
#include <misc/digital_output.h>
#include <misc/hcsr501.h>
#include <misc/keypad.h>
#include <misc/mono_timer.h>
#include <misc/pca9633.h>

#include <misc/font.h>
#include <misc/font_std.h>
#include <misc/lcd_st7735.h>

#include <misc/imu.h>

#include <misc/rfm12b.h>
#include <misc/rc_radio.h>
#include <misc/radio_link.h>

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
extern EXTConfig EXTCFG1;
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_LIB */

#endif /* _ITHACA_H_ */

