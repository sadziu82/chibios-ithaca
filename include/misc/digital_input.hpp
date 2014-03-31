#ifndef _DIGITAL_INPUT_HPP_
#define _DIGITAL_INPUT_HPP_

#if ITHACA_USE_DIGITAL_INPUT || defined(__DOXYGEN__)

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
class DigitalInput {
    protected:
        static const uint16_t debounce_delay = 25;
    public:
        enum class State : uint8_t;
    protected:
        ioportid_t io_port;
        uint8_t io_pin;
        bool idle_low;
        //
        State state;
        uint8_t pad_state;
        systime_t debounce_end_time;;
        bool state_change;
    public:
        DigitalInput(ioportid_t io_port, uint8_t io_pin, bool idle_low = false);
        void refresh(void);
        bool changed(void);
};

/*
 * @brief   ...
 * @details ...
 */
enum class DigitalInput::State : uint8_t {
    Idle = 0x00,
    Debounce,
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_DIGITAL_INPUT */

#endif /* _DIGITAL_INPUT_HPP_ */

