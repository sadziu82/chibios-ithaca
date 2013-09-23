#ifndef _BUTTON_H_
#define _BUTTON_H_

#if ITHACA_USE_BUTTON || defined(__DOXYGEN__)

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
typedef enum {
    BUTTON_STATE_IDLE = 0,
    BUTTON_STATE_DEBOUNCE,
    BUTTON_STATE_ACTIVE,
    BUTTON_STATE_AUTOREPEAT,
} button_state_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    // hardware
    ioportid_t io_port;
    uint8_t io_pin;
    // 
    uint16_t press_delay;
    uint16_t auto_repeat_delay;
    bool idle_low;
    // 
    button_state_t state;
    systime_t debounce_start;                       // start of button debounce
    systime_t active_start;                         // start of button press
    uint8_t pad;
    //
    bool key_press;
    // 
    systime_t multipress_start;                     // start of button press
    systime_t multipress_debounce_start;            // start of button press
    uint8_t multipress_count;
    uint8_t multipress_active;
    button_state_t multipress_state;
    uint8_t multipress_pad;
} button_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
    bool buttonInit(button_t *btn, ioportid_t port, uint8_t pin,
                    uint16_t press_delay, uint16_t auto_repeat_delay,
                    bool idle_low);
    void buttonRefresh(button_t *btn, varg_t arg);
    bool buttonKeyPress(button_t *btn, varg_t arg);
    bool buttonKeyMultiPress(button_t *btn, varg_t count);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_BUTTON */

#endif /* _BUTTON_H_ */

