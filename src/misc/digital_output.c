#include <ithaca.h>

#if ITHACA_USE_DIGITAL_OUTPUT || defined(__DOXYGEN__)

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
bool digitalOutputInit(digital_output_t *out,
                       ioportid_t port, uint8_t pin,
                       bool open_drain_mode) {
    out->io_port = port;
    out->io_pin = pin;
    if (open_drain_mode) {
        palSetPadMode(out->io_port, out->io_pin, PAL_MODE_OUTPUT_OPENDRAIN);
    } else {
        palSetPadMode(out->io_port, out->io_pin, PAL_MODE_OUTPUT_PUSHPULL);
    }
    out->is_set = palReadPad(out->io_port, out->io_pin);
    //
    return true;
}

/*
 * @brief   ...
 */
bool digitalOutputSet(digital_output_t *out, varg_t arg) {
    // unused in this function
    (void) arg;
    // set pad and return true
    out->is_set = true;
    // 
    return true;
}

/*
 * @brief   ...
 */
bool digitalOutputClear(digital_output_t *out, varg_t arg) {
    // unused in this function
    (void) arg;
    // set pad and return true
    out->is_set = false;
    return true;
}

/*
 * @brief   ...
 */
bool digitalOutputToggle(digital_output_t *out, varg_t arg) {
    // unused in this function
    (void) arg;
    // toggle pad and return true
    out->is_set = ! out->is_set;
    // 
    return true;
}

/*
 * @brief   ...
 */
bool digitalOutputUpdate(digital_output_t *out, varg_t arg) {
    // unused in this function
    (void) arg;
    if (out->is_set) {
        palSetPad(out->io_port, out->io_pin);
    } else {
        palClearPad(out->io_port, out->io_pin);
    }
    // set pad and return true
    return true;
}

#endif /* ITHACA_USE_DIGITAL_OUTPUT */

