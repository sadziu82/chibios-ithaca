#include <ithaca.h>

#if ITHACA_USE_BUTTON || defined(__DOXYGEN__)

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
static uint8_t const BUTTON_DEBOUNCE_DELAY = 25;

/*
 * @brief   ...
 * @details ...
 */
bool buttonInit(button_t *btn, ioportid_t port, uint8_t pin,
                uint16_t press_delay, uint16_t auto_repeat_delay,
                bool idle_low) {
    // hardware io
    btn->io_port = port;
    btn->io_pin = pin;
    // button behaviour
    btn->press_delay = press_delay;
    btn->auto_repeat_delay = auto_repeat_delay;
    btn->idle_low = idle_low;
    // current state
    btn->state = BUTTON_STATE_IDLE;
    btn->key_press = false;
    // 
    btn->debounce_start = 0;
    btn->active_start = 0;
    //
    btn->multipress_count = 0;
    btn->multipress_start = 0;
    btn->multipress_debounce_start = 0;
    btn->multipress_state = BUTTON_STATE_IDLE;
    // 
    if (btn->idle_low) {
        palSetPadMode(btn->io_port, btn->io_pin, PAL_MODE_INPUT_PULLDOWN);
    } else {
        palSetPadMode(btn->io_port, btn->io_pin, PAL_MODE_INPUT_PULLUP);
    }
    btn->pad = palReadPad(btn->io_port, btn->io_pin);
    btn->multipress_pad = palReadPad(btn->io_port, btn->io_pin);
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
void buttonRefresh(button_t *btn, varg_t arg) {
    //
    (void)arg;
    // 
    uint8_t pad = palReadPad(btn->io_port, btn->io_pin);
    // monostable button support
    switch (btn->state) {
        case BUTTON_STATE_IDLE:
            if (pad != btn->pad) {
                btn->state = BUTTON_STATE_DEBOUNCE;
                btn->debounce_start = chTimeNow();
            }
            break;
        case BUTTON_STATE_DEBOUNCE:
            if (pad != btn->pad) {
                if (chTimeNow() - btn->debounce_start > MS2ST(BUTTON_DEBOUNCE_DELAY)) {
                    btn->state = BUTTON_STATE_ACTIVE;
                    btn->active_start = chTimeNow();
                    btn->pad = pad;
                    btn->key_press = true;
                }
            } else {
                btn->state = BUTTON_STATE_IDLE;
                btn->key_press = false;
            }
            break;
        case BUTTON_STATE_ACTIVE:
            if (pad == btn->pad) {
                if (chTimeNow() - btn->active_start > MS2ST(btn->press_delay)) {
                    btn->state = BUTTON_STATE_AUTOREPEAT;
                    btn->active_start = chTimeNow();
                    btn->key_press = true;
                } else {
                    btn->key_press = false;
                }
            } else {
                btn->state = BUTTON_STATE_IDLE;
                btn->key_press = false;
                btn->pad = pad;
            }
            break;
        case BUTTON_STATE_AUTOREPEAT:
            if (pad == btn->pad) {
                if (chTimeNow() - btn->active_start > MS2ST(btn->auto_repeat_delay)) {
                    btn->active_start = chTimeNow();
                    btn->key_press = true;
                } else {
                    btn->key_press = false;
                }
            } else {
                btn->state = BUTTON_STATE_IDLE;
                btn->key_press = false;
                btn->pad = pad;
            }
            break;
    }
    // multipress (bistable button) support
    switch (btn->multipress_state) {
        case BUTTON_STATE_IDLE:
            if (pad != btn->multipress_pad) {
                btn->multipress_state = BUTTON_STATE_DEBOUNCE;
                btn->multipress_debounce_start = chTimeNow();
            }
            break;
        case BUTTON_STATE_DEBOUNCE:
            if (pad != btn->multipress_pad) {
                if (chTimeNow() - btn->debounce_start > MS2ST(BUTTON_DEBOUNCE_DELAY)) {
                    btn->multipress_state = BUTTON_STATE_IDLE;
                    btn->multipress_start = chTimeNow();
                    btn->multipress_count++;
                    btn->multipress_pad = pad;
                }
            } else {
                btn->multipress_state = BUTTON_STATE_IDLE;
            }
            break;
        default:
            break;
    }
    //
    if ((btn->multipress_count > 0) &&
        (chTimeNow() - btn->multipress_start > MS2ST(btn->press_delay))) {
        btn->multipress_active = btn->multipress_count;
        btn->multipress_count = 0;
    } else {
        btn->multipress_active = 0;
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool buttonKeyPress(button_t *btn, varg_t arg) {
    // unused in this function
    (void) arg;
    // return key press state
    return btn->key_press;
}

/*
 * @brief   ...
 * @details ...
 */
bool buttonKeyMultiPress(button_t *btn, varg_t count) {
    // return true only if multipress_active equals count
    if ((uint32_t)count == btn->multipress_active) {
        return true;
    }
    // by default return false
    return false;
}


#endif /* ITHACA_USE_BUTTON */

