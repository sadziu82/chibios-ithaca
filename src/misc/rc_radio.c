#include <ithaca.h>

#if ITHACA_USE_RC_RADIO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define RC_RADIO_PACKET_SIZE 0x40

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 */
typedef struct {
    //
    uint8_t addr;
} rc_radio_t;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   working area for pca9633Player threads
 */
WORKING_AREA(waRcRadio, 1024);

/*
 * @brief   PCA9633 processing thread
 */
msg_t rcRadioThread(void *arg) {
    //
    (void)arg;
    //
    return 0;
}

/*
 * @brief   ...
 * @details ...
 */
bool rc_radio_ldd_init(const rc_radio_t *cfg) {
    //
    return true;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool rcRadioInit(const rc_radio_t *cfg) {
    //
    rc_radio_ldd_init(cfg);
    // create radio thread
    chThdCreateStatic(waRcRadio, sizeof(waRcRadio), NORMALPRIO,
                      rcRadioThread, NULL);
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcRadioStart(const rc_radio_t *cfg) {
    //
    rc_radio_ldd_start(cfg);
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcRadioStop(const rc_radio_t *cfg) {
    //
    rc_radio_ldd_stop(cfg);
    //
    return true;
}

#endif /* ITHACA_USE_RC_RADIO */

