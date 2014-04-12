#ifndef _DIGITAL_PUSH_BUTTON_HPP_
#define _DIGITAL_PUSH_BUTTON_HPP_

#if ITHACA_USE_DIGITAL_PUSH_BUTTON || defined(__DOXYGEN__)

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
class DigitalPushButton : public DigitalInput {
    protected:
        static const uint16_t auto_repeat_delay = 250;
        //
        bool pressed, active;
        systime_t delay_end;
    public:
        DigitalPushButton(ioportid_t io_port, uint8_t io_pin, bool idle_low = false);
        void refresh(void);
        bool keyPress(void);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_DIGITAL_PUSH_BUTTON */

#endif /* _DIGITAL_PUSH_BUTTON_HPP_ */

