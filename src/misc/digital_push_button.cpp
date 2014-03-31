#include <ithaca.hpp>

#if ITHACA_USE_DIGITAL_PUSH_BUTTON || defined(__DOXYGEN__)

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
DigitalPushButton::DigitalPushButton(ioportid_t io_port, uint8_t io_pin, bool idle_low) :
              DigitalInput(io_port, io_pin, idle_low) {
    //
    this->press_active = false;
    this->press_delay_end = 0;
    this->press_count = 0;
}

/*
 * @brief   ...
 * @details ...
 */
void DigitalPushButton::refresh(void) {
    DigitalInput::refresh();
    this->press_active = false;
    //
    if ((this->press_delay_end != 0) && (this->press_delay_end < chTimeNow())) {
        this->press_active = true;
        this->press_delay_end = 0;
    } else {
        if (this->changed()) {
            this->press_delay_end = chTimeNow() + MS2ST(this->press_delay);
            this->press_count++;
        } else if (this->press_delay_end == 0) {
            this->press_count = 0;
        }
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool DigitalPushButton::pressed(uint8_t n) {
    if ((this->press_active == true) && (press_count == n)) {
        return true;
    } else {
        return false;
    }
}

#endif /* ITHACA_USE_DIGITAL_PUSH_BUTTON */

