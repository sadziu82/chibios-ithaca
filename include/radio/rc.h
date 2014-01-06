#ifndef _RADIO_RC_H_
#define _RADIO_RC_H_

#if ITHACA_USE_RADIO_RC || defined(__DOXYGEN__)

#if ITHACA_USE_RADIO_RFM12B || ITHACA_USE_RADIO_RFM22B
#else
    #error Low level radio driver not defined
#endif

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
#define RC_CHANNEL_NUMBER 8
#define RC_PACKET_DATA_SIZE RADIO_PACKET_DATA_SIZE - RC_CHANNEL_NUMBER - 4

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
    uint16_t target_id;
    uint16_t sender_id;
    uint8_t channel[RC_CHANNEL_NUMBER];
    uint8_t data[RC_PACKET_DATA_SIZE];
} rc_packet_t;

/*
 * @brief   Just forward declaration.
 */
typedef struct RCDriver RCDriver;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    RC_CMD_NACK = 0x0000,
    RC_CMD_ACK = 0x8000,
} rc_cmd_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    RC_UNINIT = 0,
    RC_STOP,
    RC_SLAVE,
    RC_MASTER,
} rc_state_t;

/*
 * @brief   ...
 * @details ...
 */
typedef void(* rc_callback_t)(RCDriver *rcp, rc_packet_t *packet);

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    uint8_t self_id;
    uint8_t peer_id;
    // callbacks
    rc_callback_t slave_cb;
    rc_callback_t master_cb;
    rc_callback_t error_cb;
    // low level radio driver
#ifdef ITHACA_USE_RADIO_RFM12B
    RFM12BDriver *radio_drv;
    RFM12BConfig *radio_cfg;
#endif
} RCConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef struct RCDriver {
    // ...
    rc_state_t state;
    RCConfig *config;
    // ...
    rc_packet_t rc_packet;
    Semaphore flag;
    lld_lock_t lock;
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
#endif /* ITHACA_USE_RADIO_RC */

#endif /* _RADIO_RC_H_ */

