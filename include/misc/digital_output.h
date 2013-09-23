#ifndef _DIGITAL_OUTPUT_H_
#define _DIGITAL_OUTPUT_H_

#if ITHACA_USE_DIGITAL_OUTPUT || defined(__DOXYGEN__)

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
 */
typedef struct {
    // hardware
    ioportid_t io_port;
    uint8_t io_pin;
    // 
    bool is_set;
} digital_output_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
    bool digitalOutputInit(digital_output_t *out,
                           ioportid_t port, uint8_t pin,
                           bool open_drain_mode);
    bool digitalOutputSet(digital_output_t *out, varg_t arg);
    bool digitalOutputClear(digital_output_t *out, varg_t arg);
    bool digitalOutputToggle(digital_output_t *out, varg_t arg);
    bool digitalOutputUpdate(digital_output_t *out, varg_t arg);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_DIGITAL_OUTPUT */

#endif /* _DIGITAL_OUTPUT_H_ */

