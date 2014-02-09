#include <ithaca.h>

#if ITHACA_USE_RADIO || defined(__DOXYGEN__)

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
 * @brief   ...
 * @details ...
 */
bool radio_lld_init(RadioDriver *radio) {
    bool ret_val = false;
    consoleDevel("radio_lld_init() start, lld_type == %d\r\n",
                 radio->config->lld_type);
    switch (radio->config->lld_type) {
        case RADIO_LLD_TYPE_RFM12B:
            ret_val = rfm12b_lld_init(radio);
            break;
    }
    if (ret_val == true) {
        consoleDevel("radio_lld_init() end\r\n");
    } else {
        consoleDevel("radio_lld_init() failed\r\n");
    }
    return ret_val;
}

/*
 * @brief   ...
 * @details ...
 */
bool radio_lld_idle(RadioDriver *radio) {
    bool ret_val = false;
    consoleDevel("radio_lld_idle() start, lld_type == %d\r\n",
                 radio->config->lld_type);
    if (radio->config->timeout != 0) {
        radio->timeout = chTimeNow() + MS2ST(radio->config->timeout);
    } else {
        radio->timeout = 0;
    }
    switch (radio->config->lld_type) {
        case RADIO_LLD_TYPE_RFM12B:
            rfm12b_lld_idle(radio);
            ret_val = true;
            break;
    }
    if (ret_val == true) {
        consoleDevel("radio_lld_idle() end\r\n");
    } else {
        consoleDevel("radio_lld_idle() failed\r\n");
    }
    return ret_val;
}

/*
 * @brief   ...
 * @details ...
 */
bool radio_lld_receive_start(RadioDriver *radio) {
    bool ret_val = false;
    consoleDevel("radio_lld_receive_start() start, lld_type == %d\r\n",
                 radio->config->lld_type);
    if (radio->config->timeout != 0) {
        radio->timeout = chTimeNow() + MS2ST(radio->config->timeout);
    } else {
        radio->timeout = 0;
    }
    switch (radio->config->lld_type) {
        case RADIO_LLD_TYPE_RFM12B:
            rfm12b_lld_receive_start(radio);
            ret_val = true;
            break;
    }
    if (ret_val == true) {
        consoleDevel("radio_lld_receive_start() end\r\n");
    } else {
        consoleDevel("radio_lld_receive_start() failed\r\n");
    }
    return ret_val;
}

/*
 * @brief   ...
 * @details ...
 */
bool radio_lld_receive_is_completed(RadioDriver *radio) {
    bool ret_val = false;
    switch (radio->config->lld_type) {
        case RADIO_LLD_TYPE_RFM12B:
            ret_val = rfm12b_lld_receive_is_completed(radio);
            break;
    }
    if (ret_val == true) {
        consoleDevel("radio_lld_receive_is_completed() == true\r\n");
    } else {
        consoleDevel("radio_lld_receive_is_completed() == false\r\n");
    }
    return ret_val;
}

/*
 * @brief   ...
 * @details ...
 */
bool radio_lld_send_start(RadioDriver *radio) {
    bool ret_val = false;
    consoleDevel("radio_lld_send_start() start, lld_type == %d\r\n",
                 radio->config->lld_type);
    if (radio->config->timeout != 0) {
        radio->timeout = chTimeNow() + MS2ST(radio->config->timeout);
    } else {
        radio->timeout = 0;
    }
    switch (radio->config->lld_type) {
        case RADIO_LLD_TYPE_RFM12B:
            rfm12b_lld_send_start(radio);
            ret_val = true;
            break;
    }
    if (ret_val == true) {
        consoleDevel("radio_lld_send_start() end\r\n");
    } else {
        consoleWarn("radio_lld_send_start() failed\r\n");
    }
    return ret_val;
}

/*
 * @brief   ...
 * @details ...
 */
bool radio_lld_send_is_completed(RadioDriver *radio) {
    bool ret_val = false;
    switch (radio->config->lld_type) {
        case RADIO_LLD_TYPE_RFM12B:
            ret_val = rfm12b_lld_send_is_completed(radio);
            break;
    }
    if (ret_val == true) {
        consoleDevel("radio_lld_send_is_completed() == true\r\n");
    } else {
        consoleDevel("radio_lld_send_is_completed() == false\r\n");
    }
    return ret_val;
}

/*
 * @brief   ...
 * @details ...
 */
bool radio_lld_is_error(RadioDriver *radio) {
    bool ret_val = false;
    switch (radio->config->lld_type) {
        case RADIO_LLD_TYPE_RFM12B:
            ret_val = rfm12b_lld_is_error(radio);
            break;
    }
    if (ret_val == true) {
        consoleDevel("radio_lld_is_error() == true\r\n");
    } else {
        consoleDevel("radio_lld_is_error() == false\r\n");
    }
    return ret_val;
}

/*
 * @brief   ...
 * @details ...
 */
bool radio_lld_is_timeout_expired(RadioDriver *radio) {
    if ((radio->timeout != 0) && (radio->timeout <= chTimeNow())) {
        consoleDevel("radio_lld_is_timeout_expired() == true\r\n");
        return true;
    } else {
        consoleDevel("radio_lld_is_timeout_expired() == false\r\n");
        return false;
    }
}

/*
 * @brief   Radio thread.
 * @details This thread performs switch between different states
            of radio module. It also executes defined callbacks.
 */
static void radio_thread(void *radio_driver) {
    // 
    RadioDriver *radio = (RadioDriver *)radio_driver;
    radio_callback_t cb = NULL;
    //
    if (radio->config->name != NULL) {
        chRegSetThreadName(radio->config->name);
    }
    consoleDebug("started\r\n");
    tmObjectInit(&radio->tm);
    //
    while (true) {
        consoleDevel("loop start\r\n");
        cb = NULL;
        if (ithacaLock(&radio->lock) == true) {
            switch (radio->state) {
                case RADIO_UNINIT:
                    consoleDebug("RADIO_UNINIT\r\n");
                    if (radio_lld_init(radio) == true) {
                        radio->state = RADIO_STOP;
                        consoleDevel("init ok\r\n");
                    } else {
                        consoleDebug("init failed\r\n");
                    }
                    break;
                case RADIO_STOP:
                    consoleDebug("RADIO_STOP\r\n");
                    if (radioIdleI(radio, radio->user_arg) == true) {
                        consoleDevel("stop -> idle ok\r\n");
                    } else {
                        consoleWarn("stop -> idle failed\r\n");
                        radio->state = RADIO_UNINIT;
                    }
                    break;
                case RADIO_IDLE:
                    consoleDebug("RADIO_IDLE\r\n");
                    if (radio->config->idle_cb != NULL) {
                        cb = radio->config->idle_cb;
                    } else {
                        consoleDevel("idle_cb == NULL\r\n");
                    }
                    break;
                case RADIO_RX:
                    consoleDebug("RADIO_RX\r\n");
                    if ((radio_lld_is_error(radio) == true) ||
                        (radio_lld_is_timeout_expired(radio) == true)) {
                        if (radio->config->recv_error_cb != NULL) {
                            cb = radio->config->recv_error_cb;
                        } else {
                            radio->state = RADIO_ERROR;
                            consoleDevel("recv_error_cb == NULL\r\n");
                        }
                        consoleWarn("rx failed\r\n");
                    } else if (radio_lld_receive_is_completed(radio) == true) {
                        radio->state = RADIO_IDLE;
                        consoleDebug("rx completed ok\r\n");
                    } else {
                        consoleDevel("rx in progress\r\n");
                    }
                    break;
                case RADIO_TX:
                    consoleDebug("RADIO_TX\r\n");
                    if ((radio_lld_is_error(radio) == true) ||
                        (radio_lld_is_timeout_expired(radio) == true)) {
                        if (radio->config->send_error_cb != NULL) {
                            cb = radio->config->send_error_cb;
                        } else {
                            radio->state = RADIO_ERROR;
                            consoleDevel("send_error_cb == NULL\r\n");
                        }
                        consoleWarn("tx failed\r\n");
                    } else if (radio_lld_send_is_completed(radio) == true) {
                        if (radio->config->send_done_cb != NULL) {
                            cb = radio->config->send_done_cb;
                        } else {
                            radio->state = RADIO_IDLE;
                        }
                        consoleDebug("tx completed ok\r\n");
                    } else {
                        consoleDevel("tx in progress\r\n");
                    }
                    break;
                case RADIO_ERROR:
                    consoleDebug("RADIO_ERROR\r\n");
                    if (radioIdleI(radio, radio->user_arg) == true) {
                        consoleDevel("error -> idle ok\r\n");
                    } else {
                        consoleWarn("eror -> idle failed\r\n");
                        radio->state = RADIO_UNINIT;
                    }
                    break;
            }
            if (cb != NULL ) {
                consoleDevel("callback execute\r\n");
                cb(radio, &radio->packet);
            }
            ithacaUnlock(&radio->lock);
        }
        consoleDevel("loop yield\r\n");
        chThdYield();
    }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
void radioInit(RadioDriver *radio, RadioConfig *config, void *user_arg) {
    //
    consoleDevel("radioInit() start\r\n");
    if (ithacaLock(&radio->lock) == true) {
        radio->state = RADIO_UNINIT;
        if (radio->tp == NULL) {
            consoleDevel("radioInit() tp == NULL\r\n");
            radio->config = config;
            radio->user_arg = user_arg;
            radio->tp = chThdCreateStatic(
                &radio->radio_thread_working_area,
                sizeof(radio->radio_thread_working_area),
                NORMALPRIO, (tfunc_t)radio_thread, radio);
        } else {
            consoleDebug("radioInit() tp != NULL\r\n");
        }
        ithacaUnlock(&radio->lock);
    }
    consoleDevel("radioInit() end\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
bool radioIdleI(RadioDriver *radio, void *arg) {
    consoleDevel("radioIdleI() start\r\n");
    if (radio_lld_idle(radio) == true) {
        radio->state = RADIO_IDLE;
        consoleDevel("radioIdleI() end\r\n");
        return true;
    } else {
        radio->state = RADIO_ERROR;
        consoleWarn("radioIdleI() failed\r\n");
        return false;
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool radioIdle(RadioDriver *radio, void *arg) {
    //
    bool ret;
    //
    consoleDevel("radioIdle() start\r\n");
    if (ithacaLockTimeout(&radio->lock, 5) == false) {
        consoleWarn("radioIdle() failed\r\n");
        return false;
    }
    ret = radioIdleI(radio, arg);
    ithacaUnlock(&radio->lock);
    consoleDevel("radioIdle() end == %s\r\n", ret ? "true" : "false");
    return ret;
}

/*
 * @brief   ...
 * @details ...
 */
bool radioRecvStartI(RadioDriver *radio, void *arg) {
    consoleDevel("radioRecvStartS() start\r\n");
    if (radio_lld_receive_start(radio) == true) {
        radio->state = RADIO_RX;
        consoleDevel("radioRecvStartS() end\r\n");
        return true;
    } else {
        radio->state = RADIO_ERROR;
        consoleWarn("radioRecvStartS() failed\r\n");
        return false;
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool radioRecvStart(RadioDriver *radio, void *arg) {
    //
    bool ret;
    //
    consoleDevel("radioRecvStart() start\r\n");
    if (ithacaLockTimeout(&radio->lock, 5) == false) {
        consoleWarn("radioRecvStart() failed\r\n");
        return false;
    }
    ret = radioRecvStartI(radio, arg);
    ithacaUnlock(&radio->lock);
    consoleDevel("radioRecvStart() end == %s\r\n", ret ? "true" : "false");
    return ret;
}

/*
 * @brief   ...
 * @details ...
 */
bool radioSendStartI(RadioDriver *radio, radio_packet_t *packet, void *arg) {
    consoleDevel("radioSendStartS() start\r\n");
    if (radio_lld_send_start(radio) == true) {
        radio->state = RADIO_TX;
        consoleDevel("radioSendStartS() end\r\n");
        return true;
    } else {
        radio->state = RADIO_ERROR;
        consoleWarn("radioSendStartS() failed\r\n");
        return false;
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool radioSendStart(RadioDriver *radio, radio_packet_t *packet, void *arg) {
    //
    bool ret;
    //
    consoleDevel("radioSendStart() start\r\n");
    if (ithacaLockTimeout(&radio->lock, 5) == false) {
        consoleWarn("radioSendStart() failed\r\n");
        return false;
    }
    ret = radioSendStartI(radio, packet, arg);
    ithacaUnlock(&radio->lock);
    consoleDevel("radioSendStart() end == %s\r\n", ret ? "true" : "false");
    return ret;
}

#endif /* ITHACA_USE_RADIO */

