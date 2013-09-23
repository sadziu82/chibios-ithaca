#include <ithaca.h>

#if ITHACA_USE_KEYPAD || defined(__DOXYGEN__)

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
bool keypadRefresh(keypad_t *keypad, varg_t arg) {
    uint8_t i, j;
    // FIXME move to init
    // for (i = 0; i < 4; i++) {
    //     palSetPad(keypad->in[i].port, keypad->in[i].pin);
    // }
    for (i = 0; i < 4; i++) {
        palClearPad(keypad->out[i].port, keypad->out[i].pout);
        for (j = 0; j < 4; j++) {
            keypad->pad[i << 2 + j] = palReadpad(keypad->in[j].port, keypad->in[j].pin);
        }
        palSetPad(keypad->out[i].port, keypad->out[i].pout);
    }
}


#endif /* ITHACA_USE_KEYPAD */

