#include <ithaca.h>

#if ITHACA_USE_KEYPAD44 || defined(__DOXYGEN__)

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
bool keypad44Init(keypad44_t *keypad44, uint16_t press_delay,
                  uint16_t auto_repeat_delay) {
    uint8_t i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            buttonInit(&keypad44->buttons[i + j*4],
                       keypad44->in[j].port, keypad44->in[j].pin,
                       press_delay, auto_repeat_delay, false);
        }
    }
    for (j = 0; j < 4; j++) {
        palSetPadMode(keypad44->in[j].port, keypad44->in[j].pin, PAL_MODE_INPUT_PULLUP);
    }
    for (i = 0; i < 4; i++) {
        palSetPadMode(keypad44->out[i].port, keypad44->out[i].pin, PAL_MODE_OUTPUT_PUSHPULL);
    }
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool keypad44Refresh(keypad44_t *keypad, varg_t arg) {
    uint8_t i, j;
    (void)arg;
    for (i = 0; i < 4; i++) {
        palClearPad(keypad->out[i].port, keypad->out[i].pin);
        for (j = 0; j < 4; j++) {
            buttonRefresh(&keypad->buttons[i + j*4], NULL);
        }
        palSetPad(keypad->out[i].port, keypad->out[i].pin);
    }
    return true;
}


#endif /* ITHACA_USE_KEYPAD44 */

