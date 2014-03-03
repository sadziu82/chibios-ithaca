#include <ithaca.h>

#if ITHACA_USE_HCSR501 || defined(__DOXYGEN__)

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
 */
bool hcsr501Init(hcsr501_t *hc, ioportid_t port, uint8_t pin,
                 bool idle_low) {
    // hardware io
    hc->io_port = port;
    hc->io_pin = pin;
    // 
    hc->idle_low = idle_low;
    hc->motion_detected = 0;
    //
    if (hc->idle_low) {
        palSetPadMode(hc->io_port, hc->io_pin, PAL_MODE_INPUT_PULLDOWN);
    } else {
        palSetPadMode(hc->io_port, hc->io_pin, PAL_MODE_INPUT_PULLUP);
    }
    //
    return true;
}

/*
 * @brief   ...
 */
bool hcsr501Refresh(hcsr501_t *hc, varg_t arg) {
    //
    (void)arg;
    // 
    if (palReadPad(hc->io_port, hc->io_pin) != hc->idle_low) {
        switch (hc->motion_detected) {
            case 0:
                hc->motion_detected = 1;
                break;
            case 1:
                hc->motion_detected = 2;
                break;
            default:
                break;
        }
    } else {
        hc->motion_detected = 0;
    }
    //
    return true;
}

/*
 * @brief   ...
 */
bool hcsr501MotionDetected(hcsr501_t *hc, varg_t arg) {
    //
    (void)arg;
    // 
    if (hc->motion_detected == 1) {
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool hcsr501MotionPresent(hcsr501_t *hc, varg_t arg) {
    //
    (void)arg;
    // 
    if (hc->motion_detected != 0) {
        return true;
    } else {
        return false;
    }
}

#endif /* ITHACA_USE_HCSR501 */

