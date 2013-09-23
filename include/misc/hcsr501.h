#ifndef _HCSR501_H_
#define _HCSR501_H_

#if ITHACA_USE_HCSR501 || defined(__DOXYGEN__)

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
typedef struct {
    // hardware
    ioportid_t io_port;
    uint8_t io_pin;
    // 
    bool idle_low;
    // 
    uint8_t motion_detected;
} hcsr501_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
    bool hcsr501Init(hcsr501_t *hc, ioportid_t port, uint8_t pin,
                     bool idle_low);
    bool hcsr501Refresh(hcsr501_t *hc, varg_t arg);
    bool hcsr501MotionDetected(hcsr501_t *hc, varg_t arg);
    bool hcsr501MotionPresent(hcsr501_t *hc, varg_t arg);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_HCSR501 */

#endif /* _HCSR501_H_ */

