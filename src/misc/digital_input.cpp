#include <ithaca.hpp>

#if ITHACA_USE_DIGITAL_INPUT || defined(__DOXYGEN__)

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
DigitalInput::DigitalInput(ioportid_t io_port, uint8_t io_pin, bool idle_low) {
    //
    this->io_port = io_port;
    this->io_pin = io_pin;
    this->idle_low = idle_low;
    // 
    if (this->idle_low) {
        palSetPadMode(this->io_port, this->io_pin, PAL_MODE_INPUT_PULLDOWN);
    } else {
        palSetPadMode(this->io_port, this->io_pin, PAL_MODE_INPUT_PULLUP);
    }
    //
    this->state = State::Idle;
    this->pad_state = palReadPad(this->io_port, this->io_pin);
    this->debounce_end_time = 0;
}

/*
 * @brief   ...
 * @details ...
 */
void DigitalInput::refresh(void) {
    //
    uint8_t pad_state = palReadPad(this->io_port, this->io_pin);
    this->state_change = false;
    //
    switch (this->state) {
        case DigitalInput::State::Idle:
            if (this->pad_state != pad_state) {
                this->debounce_end_time = chTimeNow() + MS2ST(this->debounce_delay);
                this->state = State::Debounce;
            }
            break;
        case DigitalInput::State::Debounce:
            if (this->pad_state != pad_state) {
                if (this->debounce_end_time < chTimeNow()) {
                    this->pad_state = pad_state;
                    this->state = State::Idle;
                    this->state_change = true;
                }
            } else {
                this->state = State::Idle;
            }
            break;
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool DigitalInput::changed(void) {
    if (this->state_change == true) {
        //consoleDebug("input state changed\r\n");
        return true;
    } else {
        return false;
    }
}

#endif /* ITHACA_USE_DIGITAL_INPUT */

