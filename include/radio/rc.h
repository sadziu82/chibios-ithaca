#ifndef _RC_H_
#define _RC_H_

#if ITHACA_USE_RC || defined(__DOXYGEN__)

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
typedef enum {
    RC_MASTER = 0,
    RC_SLAVE,
} rc_role_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    RC_UNINIT = 0,
    RC_STOP,
    RC_MASTER_IDLE,
    RC_MASTER_TX,
    RC_MASTER_RX,
    RC_SLAVE_IDLE,
    RC_SLAVE_RX,
    RC_SLAVE_TX,
} rc_state_t;

/*
 * @brief   ...
 * @details ...
 */
typedef void(* rc_callback_t)(radio_packet_t *rf_packet);

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    uint8_t src;
    uint8_t dst;
    rc_callback_t cb;
    rc_callback_t err_cb;
#ifdef ITHACA_USE_RFM12B
    // low level radio driver
    RFM12BDriver *radio_drv;
    RFM12BConfig *radio_cfg;
#else 
    #error Low level radio driver not defined
#endif
} RCConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    // rfm12b state
    rc_state_t state;
    WORKING_AREA(thread_wa, 256);
    Semaphore thread_semaphore;
    radio_packet_t rf_packet;
    // rfm12b configuration
    RCConfig *config;
} RCDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

//
extern RCDriver RCD1;

#ifdef __cplusplus
extern "C" {
#endif
//
bool rcInit(RCDriver *drv, RCConfig *config);
bool rcStartMaster(RCDriver *drv);
bool rcStartSlave(RCDriver *drv);
bool rcStop(RCDriver *drv);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RC */

#endif /* _RC_H_ */

