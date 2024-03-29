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
    this->pressed = false;
    this->active = false;
    this->delay_end = 0;
}

/*
 * @brief   ...
 * @details ...
 */
void DigitalPushButton::refresh(void) {
    DigitalInput::refresh();
    //
    this->active = false;
    //
    if (this->pressed) {
        if (this->changed()) {
            this->pressed = false;
            this->delay_end = 0;
        } else if (this->delay_end < chTimeNow()) {
            this->active = true;
            this->delay_end = chTimeNow() + MS2ST(this->auto_repeat_delay);
        }
    } else if (this->changed()) {
        this->pressed = true;
        this->active = true;
        this->delay_end = chTimeNow() + MS2ST(2 * this->auto_repeat_delay);
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool DigitalPushButton::keyPress(void) {
    return this->active;
}

#endif /* ITHACA_USE_DIGITAL_PUSH_BUTTON */

