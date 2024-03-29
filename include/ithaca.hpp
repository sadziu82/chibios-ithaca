#ifndef _ITHACA_HPP_
#define _ITHACA_HPP_

#include "ch.h"
#include "hal.h"

#include "ithacaconf.h"

#if ITHACA_USE_LIB || defined(__DOXYGEN__)

//
#include <ithaca.h>

// C++ STL
#include <list>

//
#include <misc/input_event.hpp>
#include <misc/input_event_queue.hpp>

//
#include <lcd.hpp>
#include <lcd/st7735.hpp>

//
#include <font.hpp>
#include <font/font_std.hpp>

//
#include <widget.hpp>
#include <widget/main_window.hpp>
#include <widget/frame.hpp>
#include <widget/label.hpp>
#include <widget/widget_push_button.hpp>
#include <widget/top.hpp>
#include <widget/widget_stick.hpp>
#include <widget/ping_pong_player.hpp>
#include <widget/ping_pong_ball.hpp>
//
#include <misc/digital_input.hpp>
#include <misc/digital_push_button.hpp>

//
#include <qobject.hpp>

//
#include <radio.hpp>

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

#endif /* ITHACA_USE_LIB */

#endif /* _ITHACA_HPP_ */

