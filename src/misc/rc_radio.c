#include <ithaca.h>

#if ITHACA_USE_RC_RADIO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef RC_RADIO_EXTRA_CHANNELS
#define RC_RADIO_EXTRA_CHANNELS 0x00
#endif

#ifndef RC_RADIO_PACKET_SIZE
#define RC_RADIO_PACKET_SIZE 0x20
#endif

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
    uint8_t dst;
    uint8_t src;
    //
    uint8_t control1;
    uint8_t control2;
    // 
    uint8_t throttle;
    uint8_t yaw;
    uint8_t pitch;
    uint8_t roll;
#if RC_RADIO_EXTRA_CHANNELS > 0
    // extra channels
    uint8_t exch[RC_RADIO_EXTRA_CHANNELS];
#endif
#if RC_RADIO_PACKET_SIZE - RC_RADIO_EXTRA_CHANNELS - 10 > 0
    // additional data, will be transmitted without modifications
    uint8_t aux_data[RC_RADIO_PACKET_SIZE - RC_RADIO_EXTRA_CHANNELS - 10];
#endif
    //
    uint8_t crc1;
    uint8_t crc2;
} rc_master_packet_t;

/*
 * @brief   ...
 */
typedef enum {
    RC_RADIO_TYPE_NULL = 0,
    RC_RADIO_TYPE_RFM12B,
    RC_RADIO_TYPE_RFM22B,
    RC_RADIO_TYPE_SI4432,
} rc_radio_type_t;

/*
 * @brief   ...
 */
typedef struct {
    rc_radio_type_t type;
} rc_radio_t;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   working area for rc radio thread
 */
WORKING_AREA(waRcRadio, 256);

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

