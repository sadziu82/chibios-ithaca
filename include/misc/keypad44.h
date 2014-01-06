#ifndef _KEYPAD44_H_
#define _KEYPAD44_H_

#if ITHACA_USE_KEYPAD44 || defined(__DOXYGEN__)

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

/*
 * @brief   ...
 * @details ...
 */
enum {
    KEYPAD44_1 = 0,
    KEYPAD44_2 = 1,
    KEYPAD44_3 = 2,
    KEYPAD44_A = 3,
    KEYPAD44_4 = 4,
    KEYPAD44_5 = 5,
    KEYPAD44_6 = 6,
    KEYPAD44_B = 7,
    KEYPAD44_7 = 8,
    KEYPAD44_8 = 9,
    KEYPAD44_9 = 10,
    KEYPAD44_C = 11,
    KEYPAD44_S = 12,
    KEYPAD44_0 = 13,
    KEYPAD44_H = 14,
    KEYPAD44_D = 15,
};

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    // hardware
    ioportid_t port;
    uint8_t pin;
} keypad44_io_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    // hardware
    keypad44_io_t in[4];
    keypad44_io_t out[4];
    //
    button_t buttons[16];
} keypad44_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
bool keypad44Init(keypad44_t *keypad44, uint16_t press_delay,
                  uint16_t auto_repeat_delay);
bool keypad44Refresh(keypad44_t *keypad, varg_t arg);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_KEYPAD */

#endif /* _KEYPAD44_H_ */

