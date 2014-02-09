#include <ithaca.h>

#if ITHACA_USE_RADIO_MESH || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
MeshDriver MESHD1 = {
    .lock = {
        .name = "MESHD1",
        .flag = false,
    },
};

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   mesh idle callback
 */
void radio_mesh_idle_cb(RadioDriver *radio,
                   radio_packet_t *rf_packet,
                   void *arg) {
    (void)rf_packet;
    MeshDriver *meshp = (MeshDriver *)arg;
    lldLock(&meshp->lock);
    // start next receive
    consoleDebug("Mesh(radio_mesh_idle_cb)\r\n");
    meshp->state = MESH_RX;
    radioRecvStartS(radio, arg);
    lldUnlock(&meshp->lock);
}

/*
 * @brief   mesh recv callback
 */
void radio_mesh_recv_done_cb(RadioDriver *radio,
                        radio_packet_t *rf_packet,
                        void *arg) {
    //
    (void)rf_packet;
    MeshDriver *meshp = (MeshDriver *)arg;
    lldLock(&meshp->lock);
    memcpy(&meshp->mesh_packet, rf_packet, sizeof(mesh_packet_t));
    if (meshp->state == MESH_RX) {
        if ((meshp->mesh_packet.target_id == meshp->config->node_id) ||
            (meshp->mesh_packet.target_id == 0xFFFF)) {
            if (meshp->config->recv_cb != NULL) {
                meshp->config->recv_cb(meshp, &meshp->mesh_packet);
            }
            meshp->mesh_packet.target_id = meshp->mesh_packet.sender_id;
            meshp->mesh_packet.sender_id = meshp->config->node_id;
            consoleDebug("Mesh(radio_mesh_recv_done_cb) MESH_RX got packet\r\n");
            // allow remote side to switch from tx to rx
            // move to low-lever driver: chThdSleepMilliseconds(2);
            radioSendRecvPeerDelay(radio);
            radioSendStartS(radio, (radio_packet_t *)&meshp->mesh_packet, arg);
        } else {
            consoleDebug("Mesh(radio_mesh_recv_done_cb) MESH_RX ignoring packet\r\n");
            radioRecvStartS(radio, arg);
        }
    } else if (meshp->state == MESH_TX) {
        if (meshp->mesh_packet.target_id == meshp->config->node_id) {
            if (meshp->config->send_cb != NULL) {
                meshp->config->send_cb(meshp, &meshp->mesh_packet);
            }
            consoleDebug("Mesh(radio_mesh_recv_done_cb) MESH_TX got packet\r\n");
        } else {
            consoleDebug("Mesh(radio_mesh_recv_done_cb) MESH_TX ignoring packet\r\n");
        }
        meshp->state = MESH_RX;
        radioSetRecvTimeout(radio, 0);
        radioRecvStartS(radio, arg);
    }
    lldUnlock(&meshp->lock);
}

/*
 * @brief   mesh recv callback
 */
void radio_mesh_recv_error_cb(RadioDriver *radio,
                          radio_packet_t *rf_packet,
                          void *arg) {
    (void)rf_packet;
    MeshDriver *meshp = (MeshDriver *)arg;
    if (lldLock(&meshp->lock) == true) {
        // start next receive
        consoleDebug("Mesh(radio_mesh_recv_error_cb)\r\n");
        meshp->state = MESH_RX;
        radioSetRecvTimeout(radio, 0);
        radioRecvStartS(radio, arg);
        lldUnlock(&meshp->lock);
    }
}

/*
 * @brief   mesh send callback
 */
void radio_mesh_send_done_cb(RadioDriver *radio,
                         radio_packet_t *rf_packet,
                         void *arg) {
    (void)rf_packet;
    MeshDriver *meshp = (MeshDriver *)arg;
    if (lldLock(&meshp->lock) == true) {
        if (meshp->state == MESH_TX) {
            radioSetRecvTimeout(radio, 50);
        }
        consoleDebug("Mesh(radio_mesh_send_done_cb)\r\n");
        // start next receive
        radioRecvStartS(radio, arg);
        lldUnlock(&meshp->lock);
    }
}

/*
 * @brief   mesh send callback
 */
void radio_mesh_send_error_cb(RadioDriver *radio,
                          radio_packet_t *rf_packet,
                          void *arg) {
    (void)rf_packet;
    MeshDriver *meshp = (MeshDriver *)arg;
    if (lldLock(&meshp->lock) == true) {
        // start next receive
        consoleDebug("Mesh(radio_mesh_send_error_cb)\r\n");
        meshp->state = MESH_RX;
        radioRecvStartS(radio, arg);
        lldUnlock(&meshp->lock);
    }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool meshInit(MeshDriver *meshp, MeshConfig *config) {
    //
    consoleDebug("meshInit start\r\n");
    //
    chSysLock();
    meshp->config = config;
    meshp->config->radio_cfg->idle_cb = radio_mesh_idle_cb;
    meshp->config->radio_cfg->recv_done_cb = radio_mesh_recv_done_cb;
    meshp->config->radio_cfg->recv_error_cb = radio_mesh_recv_error_cb;
    meshp->config->radio_cfg->send_done_cb = radio_mesh_send_done_cb;
    meshp->config->radio_cfg->send_error_cb = radio_mesh_send_error_cb;
    meshp->state = MESH_STOP;
    chSysUnlock();
    lldLock(&meshp->lock);
    radioInit(meshp->config->radio_drv, meshp->config->radio_cfg, meshp);
    lldUnlock(&meshp->lock);
    //
    consoleDebug("meshInit end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool meshStart(MeshDriver *meshp) {
    //
    consoleDebug("meshStartMaster start\r\n");
    //
    lldLock(&meshp->lock);
    if ((meshp->state != MESH_STOP) &&
        (meshp->state != MESH_RX)) {
        consoleDebug("meshStart failed\r\n");
        lldUnlock(&meshp->lock);
        return false;
    }
    //
    radioRecvStart(meshp->config->radio_drv, meshp);
    consoleDebug("meshStart end\r\n");
    meshp->state = MESH_RX;
    lldUnlock(&meshp->lock);
    //
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool meshStop(MeshDriver *meshp) {
    //
    consoleDebug("meshStop start\r\n");
    //
    lldLock(&meshp->lock);
    meshp->state = MESH_STOP;
    lldUnlock(&meshp->lock);
    //
    consoleDebug("meshStop end\r\n");
    //
    return true;
}

/*
 * @brief   mesh recv callback
 */
bool meshSend(MeshDriver *meshp, mesh_packet_t *mesh_packet) {
    //
    lldLock(&meshp->lock);
    consoleDebug("meshSend start\r\n");
    memcpy(&meshp->mesh_packet, mesh_packet, sizeof(mesh_packet_t));
    meshp->mesh_packet.sender_id = meshp->config->node_id;
    meshp->state = MESH_TX;
    radioSendStart(meshp->config->radio_drv,
                    (radio_packet_t *)&meshp->mesh_packet,
                    meshp);
    consoleDebug("meshSend end\r\n");
    lldUnlock(&meshp->lock);
    return true;
}

#endif /* ITHACA_USE_RADIO_MESH */

