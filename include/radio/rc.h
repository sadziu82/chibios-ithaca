#ifndef _RADIO_RC_H_
#define _RADIO_RC_H_

#if ITHACA_USE_RADIO_RC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
#define RC_CHANNEL_NUMBER 4
#define RC_PACKET_DATA_SIZE RADIO_PACKET_DATA_SIZE - RC_CHANNEL_NUMBER - 4
#define RC_SLAVE_PACKET_DATA_SIZE RADIO_PACKET_DATA_SIZE - 4

/*
 * @brief   ...
 * @details ...
 */
#if RC_PACKET_DATA_SIZE < 0
#error RC_PACKET_DATA_SIZE cant be less than zero
#endif

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
#if RC_PACKET_DATA_SIZE > 0
    uint8_t data[RC_PACKET_DATA_SIZE];
#endif
} rc_packet_t;

/*
 * @brief   Alias for rc_packet_t
 * @details ...
 */
typedef rc_packet_t rc_master_packet_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    uint16_t target_id;
    uint16_t sender_id;
#if RC_SLAVE_PACKET_DATA_SIZE > 0
    uint8_t data[RC_SLAVE_PACKET_DATA_SIZE];
#endif
} rc_slave_packet_t;

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
typedef void(* rc_callback_t)(RCDriver *rcp);

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    uint16_t self_id;
    uint16_t peer_id;
    // callbacks
    rc_callback_t slave_cb;
    rc_callback_t master_cb;
    rc_callback_t error_cb;
    // radio driver
    RadioConfig *radio_cfg;
    RadioDriver *radio_drv;
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
    rc_packet_t *rc_packet;
    ithaca_lock_t lock;
} RCDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
//
bool rcInit(RCDriver *drv, RCConfig *config);
bool rcStartMaster(RCDriver *drv, rc_packet_t *packet);
bool rcStartSlave(RCDriver *drv, rc_packet_t *packet);
bool rcStop(RCDriver *drv);
#ifdef __cplusplus
}
#endif
#endif /* ITHACA_USE_RADIO_RC */

#endif /* _RADIO_RC_H_ */

