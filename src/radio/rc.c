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
RCDriver RCD1;

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   radio thread
 */

/*
 * @brief   radio thread
 */
static msg_t RCThread(void *arg) {
    // 
    RCDriver *drv;
    systime_t wakeup_time = 0;
    //
    chRegSetThreadName("RCThread");
    // 
    drv = (RCDriver*)arg;
    chSemInit(&drv->thread_semaphore, 0);
    //
    while (true) {
        switch (drv->state) {
            case RC_UNINIT:
                if (rfm12b_lld_init(drv->config->radio_drv,
                                    drv->config->radio_cfg)) {
                    drv->state = RC_STOP;
                } else {
                    chThdSleepMilliseconds(10);
                }
                break;
            case RC_STOP:
                chSemReset(&drv->thread_semaphore, 0);
                chSemWait(&drv->thread_semaphore);
                break;
            case RC_MASTER_IDLE:
                // if (wakeup_time > chTimeNow()) {
                //     chThdSleepUntil(wakeup_time);
                // } else {
                //     wakeup_time = chTimeNow();
                // }
                drv->state = RC_MASTER_TX;
                break;
            case RC_MASTER_TX:
                if (rfm12b_lld_send(drv->config->radio_drv,
                                    &drv->rf_packet)) {
                    drv->state = RC_MASTER_RX;
                } else {
                    if (drv->config->err_cb != NULL) {
                        (drv->config->err_cb)(&drv->rf_packet);
                    }
                    drv->state = RC_MASTER_IDLE;
                }
                break;
            case RC_MASTER_RX:
                memset(&drv->rf_packet, 0, sizeof(drv->rf_packet));
                if (rfm12b_lld_recv(drv->config->radio_drv,
                                    &drv->rf_packet)) {
                    if (drv->config->cb != NULL) {
                        (drv->config->cb)(&drv->rf_packet);
                    }
                } else {
                    if (drv->config->err_cb != NULL) {
                        (drv->config->err_cb)(&drv->rf_packet);
                    }
                }
                chThdSleepMilliseconds(MS2ST(1));
                drv->state = RC_MASTER_IDLE;
                break;
            case RC_SLAVE_IDLE:
                if (wakeup_time > chTimeNow()) {
                    chThdSleepUntil(wakeup_time);
                }
                drv->state = RC_SLAVE_RX;
                break;
            case RC_SLAVE_RX:
                memset(&drv->rf_packet, 0, sizeof(drv->rf_packet));
                if (rfm12b_lld_recv(drv->config->radio_drv,
                                    &drv->rf_packet)) {
                    if (drv->config->cb != NULL) {
                        (drv->config->cb)(&drv->rf_packet);
                    }
                    chThdSleepMilliseconds(MS2ST(1));
                    drv->state = RC_SLAVE_TX;
                } else {
                    if (drv->config->err_cb != NULL) {
                        (drv->config->err_cb)(&drv->rf_packet);
                    }
                    drv->state = RC_SLAVE_IDLE;
                }
                break;
            case RC_SLAVE_TX:
                if (rfm12b_lld_send(drv->config->radio_drv,
                                    &drv->rf_packet)) {
                } else {
                    if (drv->config->err_cb != NULL) {
                        (drv->config->err_cb)(&drv->rf_packet);
                    }
                }
                drv->state = RC_SLAVE_IDLE;
                break;
        }
    //
    }
    //
    return 0;
}

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool rcInit(RCDriver *drv, RCConfig *config) {
    //
    consoleDebug("rcInit start\r\n");
    //
    drv->state = RC_UNINIT;
    drv->config = config;
    //
    chThdCreateStatic(drv->thread_wa, sizeof(drv->thread_wa),
                      NORMALPRIO + 32, RCThread, drv);
    //
    consoleDebug("rcInit end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStartMaster(RCDriver *drv) {
    //
    consoleDebug("rcStartMaster start\r\n");
    //
    if (drv->state != RC_STOP) {
        return false;
    }
    //
    drv->state = RC_MASTER_IDLE;
    chSemSignal(&drv->thread_semaphore);
    //
    consoleDebug("rcStartMaster end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStartSlave(RCDriver *drv) {
    //
    consoleDebug("rcStartSlave start\r\n");
    //
    drv->state = RC_SLAVE_IDLE;
    chSemSignal(&drv->thread_semaphore);
    //
    consoleDebug("rcStartSlave end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rcStop(RCDriver *drv) {
    //
    consoleDebug("rcStop start\r\n");
    //
    drv->state = RC_STOP;
    //
    consoleDebug("rcStop end\r\n");
    //
    return true;
}

#endif /* ITHACA_USE_RADIO_RC */

