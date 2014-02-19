#include <ithaca.h>

#if ITHACA_USE_RADIO_RC || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   rc idle callback
 */
void radio_rc_idle_cb(RadioDriver *radio) {
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    consoleDebug("RC(radio_rc_idle_cb) start\r\n");
    if (ithacaLockTimeout(&rcp->lock, 3) == false) {
        consoleDebug("RC(radio_rc_idle_cb) lock fail\r\n");
        return;
    }
    // start next receive
    if (rcp->state == RC_SLAVE) {
        radioRecvStartI(radio);
        consoleDebug("RC(radio_rc_idle_cb) RC_SLAVE\r\n");
    } else if (rcp->state == RC_MASTER) {
        // TODO chThdSleepMilliseconds(2);
        rcp->rc_packet->target_id = rcp->config->peer_id;
        rcp->rc_packet->sender_id = rcp->config->self_id;
        // TODO
        chThdSleepMilliseconds(3);
        //
        radioSendStartI(radio, (radio_packet_t *)rcp->rc_packet);
        consoleDebug("RC(radio_rc_idle_cb) RC_MASTER\r\n");
    }
    consoleDebug("RC(radio_rc_idle_cb) end\r\n");
    consoleDebug("sizeof %d vs %d\r\n", sizeof(rc_packet_t), sizeof(radio_packet_t));
    ithacaUnlock(&rcp->lock);
}

/*
 * @brief   rc recv callback
 */
void radio_rc_recv_done_cb(RadioDriver *radio) {
    //
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    consoleDebug("RC(radio_rc_recv_done_cb) start\r\n");
    if (ithacaLockTimeout(&rcp->lock, 3) == false) {
        consoleDebug("RC(radio_rc_recv_done_cb) lock fail\r\n");
        return;
    }
    memcpy(rcp->rc_packet, &radio->packet, sizeof(radio_packet_t));
    if (rcp->state == RC_SLAVE) {
        if (rcp->rc_packet->target_id == rcp->config->self_id) {
            if (rcp->config->slave_cb != NULL) {
                rcp->config->slave_cb(rcp);
            }
            rcp->stats.recv_ok++;
            rcp->rc_packet->target_id = rcp->config->peer_id;
            rcp->rc_packet->sender_id = rcp->config->self_id;
            // TODO
            chThdSleepMilliseconds(3);
            //
            radioSendStartI(radio, (radio_packet_t *)rcp->rc_packet);
            consoleDebug("RC(radio_rc_recv_done_cb) RC_SLAVE got packet\r\n");
        } else {
            rcp->stats.recv_error++;
            radioRecvStartI(radio);
            consoleDebug("RC(radio_rc_recv_done_cb) RC_SLAVE ignoring packet\r\n");
        }
    } else if (rcp->state == RC_MASTER) {
        if (rcp->rc_packet->target_id == rcp->config->self_id) {
            if (rcp->config->master_cb != NULL) {
                rcp->config->master_cb(rcp);
            }
            rcp->stats.recv_ok++;
            consoleDebug("RC(radio_rc_recv_done_cb) RC_MASTER got packet\r\n");
        } else {
            rcp->stats.recv_error++;
            consoleDebug("RC(radio_rc_recv_done_cb) RC_MASTER ignoring packet\r\n");
        }
        radioIdleI(radio);
    } else {
        radioIdleI(radio);
    }
    consoleDebug("RC(radio_rc_recv_done_cb) end\r\n");
    ithacaUnlock(&rcp->lock);
}

/*
 * @brief   rc recv callback
 */
void radio_rc_recv_error_cb(RadioDriver *radio) {
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    consoleDebug("RC(radio_rc_recv_error_cb) start\r\n");
    if (ithacaLockTimeout(&rcp->lock, 3) == false) {
        consoleDebug("RC(radio_rc_recv_error_cb) lock fail\r\n");
        return;
    }
    if (rcp->config->error_cb != NULL) {
        rcp->config->error_cb(rcp);
    }
    rcp->stats.recv_error++;
    radioIdleI(radio);
    // start next receive
    consoleDebug("RC(radio_rc_recv_error_cb) end\r\n");
    ithacaUnlock(&rcp->lock);
}

/*
 * @brief   rc send callback
 */
void radio_rc_send_done_cb(RadioDriver *radio) {
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    consoleDebug("RC(radio_rc_send_done_cb) start\r\n");
    if (ithacaLock(&rcp->lock) == false) {
        consoleDebug("RC(radio_rc_send_done_cb) lock fail\r\n");
        return;
    }
    if (rcp->state == RC_MASTER) {
        radioRecvStartI(radio);
    } else {
        radioIdleI(radio);
    }
    rcp->stats.send_ok++;
    consoleDebug("RC(radio_rc_send_done_cb) end\r\n");
    ithacaUnlock(&rcp->lock);
}

/*
 * @brief   rc send callback
 */
void radio_rc_send_error_cb(RadioDriver *radio) {
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    consoleDebug("RC(radio_rc_send_error_cb) start\r\n");
    if (ithacaLock(&rcp->lock) == false) {
        consoleDebug("RC(radio_rc_send_error_cb) lock fail\r\n");
        return;
    }
    if (rcp->config->error_cb != NULL) {
        rcp->config->error_cb(rcp);
    }
    rcp->stats.send_error++;
    radioIdleI(radio);
    consoleDebug("RC(radio_rc_send_error_cb) end\r\n");
    ithacaUnlock(&rcp->lock);
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
    ithacaLock(&rcp->lock);
    radioInit(rcp->config->radio_drv, rcp->config->radio_cfg, rcp);
    // TODO add wait for init to complete
    ithacaUnlock(&rcp->lock);
    //
    consoleDebug("rcInit end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStartMaster(RCDriver *rcp, rc_packet_t *packet) {
    //
    consoleDebug("rcStartMaster start\r\n");
    //
    if (ithacaLock(&rcp->lock) == false) {
        consoleDebug("rcStartMaster lock fail\r\n");
        return false;
    }
    if ((rcp->state != RC_STOP) &&
        (rcp->state != RC_SLAVE)) {
        consoleDebug("rcStartMaster failed\r\n");
        ithacaUnlock(&rcp->lock);
        return false;
    }
    //
    rcp->rc_packet = packet;
    rcp->rc_packet->target_id = rcp->config->peer_id;
    rcp->rc_packet->sender_id = rcp->config->self_id;
    radioSetTimeout(rcp->config->radio_drv, 20);
    radioIdleI(rcp->config->radio_drv);
    RadioDriver *radio = rcp->config->radio_drv;
    consoleDebug("RC(radio_rc_idle_cb) RC_MASTER IDLE\r\n");
    rcp->state = RC_MASTER;
    ithacaUnlock(&rcp->lock);
    //
    consoleDebug("rcStartMaster end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStartSlave(RCDriver *rcp, rc_packet_t *packet) {
    //
    consoleDebug("rcStartSlave start\r\n");
    //
    if (ithacaLockTimeout(&rcp->lock, 3) == false) {
        consoleDebug("rcStartSlave lock fail\r\n");
        return false;
    }
    if ((rcp->state != RC_STOP) &&
        (rcp->state != RC_MASTER)) {
        consoleDebug("rcStartSlave failed\r\n");
        ithacaUnlock(&rcp->lock);
        return false;
    }
    //
    rcp->rc_packet = packet;
    radioSetTimeout(rcp->config->radio_drv, 50);
    rcp->state = RC_SLAVE;
    ithacaUnlock(&rcp->lock);
    consoleDebug("rcStartSlave end\r\n");
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
    if (ithacaLockTimeout(&rcp->lock, 3) == false) {
        consoleDebug("rcStop failed\r\n");
        return false;
    }
    radioIdle(rcp->config->radio_drv);
    rcp->state = RC_STOP;
    ithacaUnlock(&rcp->lock);
    //
    consoleDebug("rcStop end\r\n");
    //
    return true;
}

#endif /* ITHACA_USE_RADIO_RC */

