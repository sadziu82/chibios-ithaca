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
    if (ithacaLockTimeout(&rcp->lock, 5) == false) {
        return;
    }
    // start next receive
    if (rcp->state == RC_SLAVE) {
        consoleDebug("RC(radio_rc_idle_cb)\r\n");
        radioRecvStartI(radio);
    } else if (rcp->state == RC_MASTER) {
        // TODO chThdSleepMilliseconds(2);
        consoleDebug("RC(radio_rc_idle_cb) RC_MASTER\r\n");
        radioSendStartI(radio, (radio_packet_t *)&rcp->rc_packet);
    }
    ithacaUnlock(&rcp->lock);
}

/*
 * @brief   rc recv callback
 */
void radio_rc_recv_done_cb(RadioDriver *radio) {
    //
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    if (ithacaLockTimeout(&rcp->lock, 5) == false) {
        return;
    }
    // TODO memcpy(&rcp->rc_packet, rf_packet, sizeof(rc_packet_t));
    if (rcp->state == RC_SLAVE) {
        if (rcp->rc_packet.target_id == rcp->config->self_id) {
            if (rcp->config->slave_cb != NULL) {
                rcp->config->slave_cb(radio);
            }
            rcp->rc_packet.target_id = rcp->config->peer_id;
            rcp->rc_packet.sender_id = rcp->config->self_id;
            consoleDebug("RC(radio_rc_recv_done_cb) RC_SLAVE got packet\r\n");
            // TODO chThdSleepMilliseconds(2);
            radioSendStartI(radio, (radio_packet_t *)&rcp->rc_packet);
        } else {
            consoleDebug("RC(radio_rc_recv_done_cb) RC_SLAVE ignoring packet\r\n");
        }
    } else if (rcp->state == RC_MASTER) {
        if (rcp->rc_packet.target_id == rcp->config->self_id) {
            if (rcp->config->master_cb != NULL) {
                rcp->config->master_cb(radio);
            }
            consoleDebug("RC(radio_rc_recv_done_cb) RC_MASTER got packet\r\n");
        } else {
            consoleDebug("RC(radio_rc_recv_done_cb) RC_MASTER ignoring packet\r\n");
        }
    }
    ithacaUnlock(&rcp->lock);
}

/*
 * @brief   rc recv callback
 */
void radio_rc_recv_error_cb(RadioDriver *radio) {
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    if (ithacaLock(&rcp->lock) == true) {
        if (rcp->config->error_cb != NULL) {
            rcp->config->error_cb(radio);
        } else {
            radioIdleI(radio);
        }
        // start next receive
        consoleDebug("RC(radio_rc_recv_error_cb)\r\n");
        ithacaUnlock(&rcp->lock);
    }
}

/*
 * @brief   rc send callback
 */
void radio_rc_send_done_cb(RadioDriver *radio) {
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    if (ithacaLock(&rcp->lock) == true) {
        if (rcp->state == RC_MASTER) {
            radioRecvStartI(radio);
        }
        consoleDebug("RC(radio_rc_send_done_cb)\r\n");
        ithacaUnlock(&rcp->lock);
    }
}

/*
 * @brief   rc send callback
 */
void radio_rc_send_error_cb(RadioDriver *radio) {
    RCDriver *rcp = (RCDriver *)radio->user_arg;
    if (ithacaLock(&rcp->lock) == true) {
        if (rcp->config->error_cb != NULL) {
            rcp->config->error_cb(radio);
        } else {
            radioIdleI(radio);
        }
        consoleDebug("RC(radio_rc_send_error_cb)\r\n");
        ithacaUnlock(&rcp->lock);
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
    ithacaLock(&rcp->lock);
    radioInit(rcp->config->radio_drv, rcp->config->radio_cfg, rcp);
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
bool rcStartMaster(RCDriver *rcp) {
    //
    consoleDebug("rcStartMaster start\r\n");
    //
    ithacaLock(&rcp->lock);
    if ((rcp->state != RC_STOP) &&
        (rcp->state != RC_SLAVE)) {
        consoleDebug("rcStartSlave failed\r\n");
        ithacaUnlock(&rcp->lock);
        return false;
    }
    //
    consoleDebug("rcStartMaster end\r\n");
    rcp->rc_packet.target_id = rcp->config->peer_id;
    rcp->rc_packet.sender_id = rcp->config->self_id;
    radioSetTimeout(rcp->config->radio_drv, 20);
    rcp->state = RC_MASTER;
    ithacaUnlock(&rcp->lock);
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
    ithacaLock(&rcp->lock);
    if ((rcp->state != RC_STOP) &&
        (rcp->state != RC_SLAVE)) {
        consoleDebug("rcStartSlave failed\r\n");
        ithacaUnlock(&rcp->lock);
        return false;
    }
    //
    radioRecvStart(rcp->config->radio_drv);
    consoleDebug("rcStartSlave end\r\n");
    radioSetTimeout(rcp->config->radio_drv, 50);
    rcp->state = RC_SLAVE;
    ithacaUnlock(&rcp->lock);
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
    ithacaLock(&rcp->lock);
    radioIdle(rcp->config->radio_drv);
    rcp->state = RC_STOP;
    ithacaUnlock(&rcp->lock);
    //
    consoleDebug("rcStop end\r\n");
    //
    return true;
}

#endif /* ITHACA_USE_RADIO_RC */

