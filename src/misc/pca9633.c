#include <ithaca.h>

#if ITHACA_USE_PCA9633 || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

//
static color_t pca9633_sequence_empty[] = {
    { 0x00, 0x00, 0x00, 0x00, },
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 */
bool pca9633Init(pca9633_t *pca) {
    //
    static uint8_t mode1[] = { 0x00, 0x00 }; 
    static uint8_t mode2[] = { 0x01, 0x14 }; 
    static uint8_t ledout[] = { 0x08, 0xAA }; 
    uint8_t data[] = { 0xC2, 0x00, 0x00, 0x00, 0x00, };
    msg_t status;
    // 
    pca->active = false;
    pca->sequence = (color_t *)pca9633_sequence_empty;
    pca->sequence_length = 1;
    pca->prev_color.red = 0x00;
    pca->prev_color.green = 0x00;
    pca->prev_color.blue = 0x00;
    pca->prev_color.amber = 0x00;
    //
    status = RDY_OK;
    i2cAcquireBus(pca->i2c_driver); 
    status |= i2cMasterTransmitTimeout(pca->i2c_driver, pca->i2c_addr,
                                       mode1, sizeof(mode1), NULL, 0, MS2ST(2)); 
    status |= i2cMasterTransmitTimeout(pca->i2c_driver, pca->i2c_addr,
                                       mode2, sizeof(mode2), NULL, 0, MS2ST(2)); 
    status |= i2cMasterTransmitTimeout(pca->i2c_driver, pca->i2c_addr,
                                       ledout, sizeof(ledout), NULL, 0, MS2ST(2)); 
    status |= i2cMasterTransmitTimeout(pca->i2c_driver, pca->i2c_addr,
                                       data, sizeof(data), NULL, 0, MS2ST(3)); 
    i2cReleaseBus(pca->i2c_driver); 
    //
    if (status == RDY_OK) {
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool pca9633SleepReset(pca9633_t *pca, varg_t unused) {
    //
    (void)unused;
    // 
    if (pca->sequence != NULL && pca->sequence_length > 0 ) {
        pca->sleep_start = chTimeNow() + S2ST(pca->sleep_timeout);
        pca->sleep_end = chTimeNow() + S2ST(pca->sleep_timeout) + S2ST(pca->fader_timeout);
        pca->seq_i = pca->seq_j = 0;
        pca->active = true;
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool pca9633SleepActivate(pca9633_t *pca, varg_t unused) {
    //
    (void)unused;
    // 
    if (pca->active == true) {
        pca->sleep_start = chTimeNow();
        pca->sleep_end = chTimeNow() + S2ST(pca->fader_timeout);
    }
    return true;
}

/*
 * @brief   ...
 */
color_t pca9633AdvanceSequence(pca9633_t *pca) {
    color_t value;
    if (pca->sequence == NULL) {
        return value;
    }
    if (++pca->seq_j >= pca->step) {
        pca->seq_j = 0;
        pca->prev_color = pca->sequence[pca->seq_i];
        if (++pca->seq_i >= pca->sequence_length) {
            pca->seq_i = 0;
        }
    }
    // red
    if (pca->prev_color.red < pca->sequence[pca->seq_i].red) {
        value.red = pca->prev_color.red +
                    ((pca->sequence[pca->seq_i].red - pca->prev_color.red) *
                     (pca->seq_j / pca->step));
    } else if (pca->prev_color.red > pca->sequence[pca->seq_i].red) {
        value.red = pca->prev_color.red -
                    ((pca->prev_color.red - pca->sequence[pca->seq_i].red) *
                     (pca->seq_j / pca->step));
    } else {
        value.red = pca->sequence[pca->seq_i].red;
    }
    // green
    if (pca->prev_color.green < pca->sequence[pca->seq_i].green) {
        value.green = pca->prev_color.green +
                    ((pca->sequence[pca->seq_i].green - pca->prev_color.green) *
                     (pca->seq_j / pca->step));
    } else if (pca->prev_color.green > pca->sequence[pca->seq_i].green) {
        value.green = pca->prev_color.green -
                    ((pca->prev_color.green - pca->sequence[pca->seq_i].green) *
                     (pca->seq_j / pca->step));
    } else {
        value.green = pca->sequence[pca->seq_i].green;
    }
    // blue
    if (pca->prev_color.blue < pca->sequence[pca->seq_i].blue) {
        value.blue = pca->prev_color.blue +
                    ((pca->sequence[pca->seq_i].blue - pca->prev_color.blue) *
                     (pca->seq_j / pca->step));
    } else if (pca->prev_color.blue > pca->sequence[pca->seq_i].blue) {
        value.blue = pca->prev_color.blue -
                    ((pca->prev_color.blue - pca->sequence[pca->seq_i].blue) *
                     (pca->seq_j / pca->step));
    } else {
        value.blue = pca->sequence[pca->seq_i].blue;
    }
    // amber
    if (pca->prev_color.amber < pca->sequence[pca->seq_i].amber) {
        value.amber = pca->prev_color.amber +
                    ((pca->sequence[pca->seq_i].amber - pca->prev_color.amber) *
                     (pca->seq_j / pca->step));
    } else if (pca->prev_color.amber > pca->sequence[pca->seq_i].amber) {
        value.amber = pca->prev_color.amber -
                    ((pca->prev_color.amber - pca->sequence[pca->seq_i].amber) *
                     (pca->seq_j / pca->step));
    } else {
        value.amber = pca->sequence[pca->seq_i].amber;
    }
    // 
    return value;
}

/*
 * @brief   core processing thread
 */
bool pca9633Play(pca9633_t *pca) {
    // i2c operation status
    msg_t i2c_status;
    // write four registers starting from 0x02 using auto-increment (0xC0)
    uint8_t data[5] = { 0xC2, };
    float fader;
    color_t color;
    //
    if (pca->active == false) {
        return true;
    }
    //
    color = pca9633AdvanceSequence(pca);
    if (chTimeNow() <= pca->sleep_start) {
        data[1] = color.red;
        data[2] = color.green;
        data[3] = color.blue;
        data[4] = color.amber;
    } else if (chTimeNow() <= pca->sleep_end && pca->fader_timeout > 0) {
        fader = S2ST(pca->fader_timeout);
        fader = (fader - (chTimeNow() - pca->sleep_start)) / fader;
        if (fader >= 0 && fader <= 1) {
            data[1] = color.red * fader;
            data[2] = color.green * fader;
            data[3] = color.blue * fader;
            data[4] = color.amber * fader;
        } else {
            data[1] = data[2] = data[3] = data[4] = 0;
            pca->prev_color.red = pca->prev_color.green = 
                pca->prev_color.blue = pca->prev_color.amber = 0;
            pca->active = false;
        }
    } else {
        data[1] = data[2] = data[3] = data[4] = 0;
        pca->prev_color.red = pca->prev_color.green = 
            pca->prev_color.blue = pca->prev_color.amber = 0;
        pca->active = false;
    }
    //
    i2cAcquireBus(pca->i2c_driver); 
    i2c_status = i2cMasterTransmitTimeout(pca->i2c_driver, pca->i2c_addr,
                                          data, sizeof(data), NULL, 0, MS2ST(1)); 
    i2cReleaseBus(pca->i2c_driver); 
    //
    if (i2c_status == RDY_OK) {
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool pca9633SequenceSet(pca9633_t *pca, const color_t *sequence) {
    //
    pca->sequence_length = 0;
    pca->sequence = (color_t *)sequence;
    //
    return true;
}

/*
 * @brief   ...
 */
bool pca9633SequenceLengthSet(pca9633_t *pca, uint32_t length) {
    //
    if (pca->sequence != NULL) {
        pca->sequence_length = (uint32_t)length;
        pca->seq_i = 0;
        pca->seq_j = 0;
        pca9633SleepReset(pca, NULL);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool pca9633SetRed(pca9633_t *pca, uint32_t red) {
    //
    if ((pca->sequence != NULL) && (pca->sequence_length > 0)) {
        pca->sequence[0].red = red;
        pca9633SleepReset(pca, NULL);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool pca9633SetGreen(pca9633_t *pca, uint32_t green) {
    //
    if ((pca->sequence != NULL) && (pca->sequence_length > 0)) {
        pca->sequence[0].green = green;
        pca9633SleepReset(pca, NULL);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool pca9633SetBlue(pca9633_t *pca, uint32_t blue) {
    //
    if ((pca->sequence != NULL) && (pca->sequence_length > 0)) {
        pca->sequence[0].blue = blue;
        pca9633SleepReset(pca, NULL);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 */
bool pca9633SetAmber(pca9633_t *pca, uint32_t amber) {
    //
    if ((pca->sequence != NULL) && (pca->sequence_length > 0)) {
        pca->sequence[0].amber = amber;
        pca9633SleepReset(pca, NULL);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   PCA9633 processing thread
 */
msg_t pca9633Player(void *arg) {
    //
    systime_t wakeup_time;
    pca9633_t *pca;
    //
    pca = (pca9633_t *)arg; 
    chRegSetThreadName(pca->desc);
    //
    while (true) {
        // initialize pca9633
        while (!pca9633Init(pca)) {
            //
            consoleError("initialization failed (0x%X), retrying\r\n",
                         pca->i2c_addr);
            chThdSleepMilliseconds(50);
        }
        //
        consoleDebug("initialized (0x%X)\r\n", pca->i2c_addr);
        // play!
        wakeup_time = chTimeNow();
        while (pca9633Play(pca)) {
            //consoleWrite("play failed, retrying\r\n");
            wakeup_time += MS2ST(pca->play_delay);
            if (wakeup_time > chTimeNow()) {
                chThdSleepUntil(wakeup_time);
            } else {
                consoleError("loop time exceeded\r\n");
                wakeup_time = chTimeNow();
            }
        }
    }
    //
    return 0;
}

#endif /* ITHACA_USE_PCA9633 */

