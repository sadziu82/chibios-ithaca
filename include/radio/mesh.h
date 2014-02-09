#ifndef _RADIO_MESH_H_
#define _RADIO_MESH_H_

#if ITHACA_USE_RADIO_MESH || defined(__DOXYGEN__)

#if !ITHACA_USE_RADIO
    #error Low level radio driver not defined
#endif

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
#define MESH_PACKET_DATA_SIZE RADIO_PACKET_DATA_SIZE - 8

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
    uint16_t cmd;
    uint16_t data_len;
    uint8_t data[MESH_PACKET_DATA_SIZE];
} mesh_packet_t;

/*
 * @brief   Just forward declaration.
 */
typedef struct MeshDriver MeshDriver;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    MESH_CMD_NACK = 0x0000,
    MESH_CMD_ACK = 0x8000,
} mesh_cmd_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    MESH_UNINIT = 0,
    MESH_STOP,
    MESH_RX,
    MESH_TX,
} mesh_state_t;

/*
 * @brief   ...
 * @details ...
 */
typedef void(* mesh_callback_t)(MeshDriver *meshp, mesh_packet_t *packet);

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    uint8_t node_id;
    // callbacks
    mesh_callback_t recv_cb;
    mesh_callback_t send_cb;
    mesh_callback_t error_cb;
    // low level radio driver
    RadioDriver *radio_drv;
    RadioConfig *radio_cfg;
} MeshConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef struct MeshDriver {
    // ...
    mesh_state_t state;
    MeshConfig *config;
    // ...
    mesh_packet_t mesh_packet;
    Semaphore flag;
    lld_lock_t lock;
} MeshDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

//
extern MeshDriver MESHD1;

#ifdef __cplusplus
extern "C" {
#endif
//
bool meshInit(MeshDriver *drv, MeshConfig *config);
bool meshStart(MeshDriver *drv);
bool meshStop(MeshDriver *drv);
bool meshSend(MeshDriver *meshp, mesh_packet_t *mesh_packet);
#ifdef __cplusplus
}
#endif
#endif /* ITHACA_USE_RADIO_MESH */

#endif /* _RADIO_MESH_H_ */

