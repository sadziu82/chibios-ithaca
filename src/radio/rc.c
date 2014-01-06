#include <ithaca.h>

#if ITHACA_USE_RADIO_RC || defined(__DOXYGEN__)

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
RCDriver RCD1 = {
    .lock = {
        .name = "RCD1",
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
 * @brief   rc idle callback
 */
#ifdef ITHACA_USE_RADIO_RFM12B
void radio_rc_idle_cb(RFM12BDriver *drv,
                   radio_packet_t *rf_packet,
                   void *arg) {
#endif
    (void)rf_packet;
    RCDriver *rcp = (RCDriver *)arg;
    lldLock(&rcp->lock);
    // start next receive
    if (rcp->state == RC_SLAVE) {
        consoleDebug("RC(radio_rc_idle_cb)\r\n");
#ifdef ITHACA_USE_RADIO_RFM12B
        rfm12bRecvStartS(drv, arg);
#endif
    } else if (rcp->state == RC_MASTER) {
        chThdSleepMilliseconds(2);
        consoleDebug("RC(radio_rc_idle_cb) RC_MASTER\r\n");
#ifdef ITHACA_USE_RADIO_RFM12B
        rfm12bSendStartS(drv, (radio_packet_t *)&rcp->rc_packet, arg);
#endif
    }
    lldUnlock(&rcp->lock);
}

/*
 * @brief   rc recv callback
 */
#ifdef ITHACA_USE_RADIO_RFM12B
void radio_rc_recv_done_cb(RFM12BDriver *drv,
                        radio_packet_t *rf_packet,
                        void *arg) {
#endif
    //
    (void)rf_packet;
    RCDriver *rcp = (RCDriver *)arg;
    if (lldLock(&rcp->lock) == true) {
        memcpy(&rcp->rc_packet, rf_packet, sizeof(rc_packet_t));
        if (rcp->state == RC_SLAVE) {
            if (rcp->rc_packet.target_id == rcp->config->self_id) {
                if (rcp->config->slave_cb != NULL) {
                    rcp->config->slave_cb(rcp, &rcp->rc_packet);
                }
                rcp->rc_packet.target_id = rcp->config->peer_id;
                rcp->rc_packet.sender_id = rcp->config->self_id;
                consoleDebug("RC(radio_rc_recv_done_cb) RC_SLAVE got packet\r\n");
#ifdef ITHACA_USE_RADIO_RFM12B
                // allow remote side to switch from tx to rx
                chThdSleepMilliseconds(2);
                rfm12bSendStartS(drv, (radio_packet_t *)&rcp->rc_packet, arg);
#endif
            } else {
                consoleDebug("RC(radio_rc_recv_done_cb) RC_SLAVE ignoring packet\r\n");
////     #ifdef ITHACA_USE_RADIO_RFM12B
////                 rfm12bRecvStartS(drv, arg);
////     #endif
            }
        } else if (rcp->state == RC_MASTER) {
            if (rcp->rc_packet.target_id == rcp->config->self_id) {
                if (rcp->config->master_cb != NULL) {
                    rcp->config->master_cb(rcp, &rcp->rc_packet);
                }
                consoleDebug("RC(radio_rc_recv_done_cb) RC_MASTER got packet\r\n");
            } else {
                consoleDebug("RC(radio_rc_recv_done_cb) RC_MASTER ignoring packet\r\n");
            }
////     #ifdef ITHACA_USE_RADIO_RFM12B
////             drv->config->recv_timeout = 0;
////             rfm12bRecvStartS(drv, arg);
////     #endif
        }
        lldUnlock(&rcp->lock);
    }
}

/*
 * @brief   rc recv callback
 */
#ifdef ITHACA_USE_RADIO_RFM12B
void radio_rc_recv_error_cb(RFM12BDriver *drv,
                          radio_packet_t *rf_packet,
                          void *arg) {
#endif
    (void)rf_packet;
    RCDriver *rcp = (RCDriver *)arg;
    if (lldLock(&rcp->lock) == true) {
        if (rcp->config->error_cb != NULL) {
            rcp->config->error_cb(rcp, NULL);
        }
        // start next receive
        consoleDebug("RC(radio_rc_recv_error_cb)\r\n");
//// #ifdef ITHACA_USE_RADIO_RFM12B
////         drv->config->recv_timeout = 0;
////         rfm12bRecvStartS(drv, arg);
//// #endif
        lldUnlock(&rcp->lock);
    }
}

/*
 * @brief   rc send callback
 */
#ifdef ITHACA_USE_RADIO_RFM12B
void radio_rc_send_done_cb(RFM12BDriver *drv,
                         radio_packet_t *rf_packet,
                         void *arg) {
#endif
    (void)rf_packet;
    RCDriver *rcp = (RCDriver *)arg;
    if (lldLock(&rcp->lock) == true) {
        if (rcp->state == RC_MASTER) {
#ifdef ITHACA_USE_RADIO_RFM12B
            rfm12bRecvStartS(drv, arg);
#endif
        }
        consoleDebug("RC(radio_rc_send_done_cb)\r\n");
        lldUnlock(&rcp->lock);
    }
}

/*
 * @brief   rc send callback
 */
#ifdef ITHACA_USE_RADIO_RFM12B
void radio_rc_send_error_cb(RFM12BDriver *drv,
                          radio_packet_t *rf_packet,
                          void *arg) {
#endif
    (void)rf_packet;
    RCDriver *rcp = (RCDriver *)arg;
    if (lldLock(&rcp->lock) == true) {
        if (rcp->config->error_cb != NULL) {
            rcp->config->error_cb(rcp, NULL);
        }
        consoleDebug("RC(radio_rc_send_error_cb)\r\n");
        lldUnlock(&rcp->lock);
    }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool rcInit(RCDriver *rcp, RCConfig *config) {
    //
    consoleDebug("rcInit start\r\n");
    //
    chSysLock();
    rcp->config = config;
    rcp->config->radio_cfg->idle_cb = radio_rc_idle_cb;
    rcp->config->radio_cfg->recv_done_cb = radio_rc_recv_done_cb;
    rcp->config->radio_cfg->recv_error_cb = radio_rc_recv_error_cb;
    rcp->config->radio_cfg->send_done_cb = radio_rc_send_done_cb;
    rcp->config->radio_cfg->send_error_cb = radio_rc_send_error_cb;
    rcp->state = RC_STOP;
    chSysUnlock();
    lldLock(&rcp->lock);
#ifdef ITHACA_USE_RADIO_RFM12B
    rfm12bInit(rcp->config->radio_drv, rcp->config->radio_cfg, rcp);
#endif
    lldUnlock(&rcp->lock);
    //
    consoleDebug("rcInit end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStartMaster(RCDriver *rcp) {
    //
    consoleDebug("rcStartMaster start\r\n");
    //
    lldLock(&rcp->lock);
    if ((rcp->state != RC_STOP) &&
        (rcp->state != RC_SLAVE)) {
        consoleDebug("rcStartSlave failed\r\n");
        lldUnlock(&rcp->lock);
        return false;
    }
    //
//// #ifdef ITHACA_USE_RADIO_RFM12B
////     rfm12bRecvStart(rcp->config->radio_drv, rcp);
//// #endif
    consoleDebug("rcStartMaster end\r\n");
    rcp->rc_packet.target_id = rcp->config->peer_id;
    rcp->rc_packet.sender_id = rcp->config->self_id;
    rcp->config->radio_cfg->recv_timeout = 20;
    rcp->state = RC_MASTER;
    lldUnlock(&rcp->lock);
    //
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStartSlave(RCDriver *rcp) {
    //
    consoleDebug("rcStartSlave start\r\n");
    //
    lldLock(&rcp->lock);
    if ((rcp->state != RC_STOP) &&
        (rcp->state != RC_SLAVE)) {
        consoleDebug("rcStartSlave failed\r\n");
        lldUnlock(&rcp->lock);
        return false;
    }
    //
#ifdef ITHACA_USE_RADIO_RFM12B
    rfm12bRecvStart(rcp->config->radio_drv, rcp);
#endif
    consoleDebug("rcStartSlave end\r\n");
    rcp->config->radio_cfg->recv_timeout = 50;
    rcp->state = RC_SLAVE;
    lldUnlock(&rcp->lock);
    //
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStop(RCDriver *rcp) {
    //
    consoleDebug("rcStop start\r\n");
    //
    lldLock(&rcp->lock);
    rcp->state = RC_STOP;
    lldUnlock(&rcp->lock);
    //
    consoleDebug("rcStop end\r\n");
    //
    return true;
}

#endif /* ITHACA_USE_RADIO_RC */

