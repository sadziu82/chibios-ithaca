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
        static const uint16_t press_delay = 200;
        //
        bool press_active;
        systime_t press_delay_end;
        uint8_t press_count;
    public:
        DigitalPushButton(ioportid_t io_port, uint8_t io_pin, bool idle_low = false);
        void refresh(void);
        bool pressed(uint8_t n = 1);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_DIGITAL_PUSH_BUTTON */

#endif /* _DIGITAL_PUSH_BUTTON_HPP_ */

