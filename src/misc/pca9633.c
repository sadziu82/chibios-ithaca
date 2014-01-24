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

/*
 * @brief   ...
 */
static uint8_t PCA9633_INIT_MODE1[] = {
    PCA9633_CONTROL_NOAI | PCA9633_REGISTER_MODE1,
    PCA9633_MODE1_AI_DISABLED,
};
 
/*
 * @brief   ...
 */
static uint8_t PCA9633_INIT_MODE2_INVERTED[] = {
    PCA9633_CONTROL_NOAI | PCA9633_REGISTER_MODE2,
    PCA9633_MODE2_INVRT | PCA9633_MODE2_OUTDRV_POLE,
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
pca9633_status_t pca9633Init(pca9633_t *pca) {
    //
    msg_t status = RDY_OK;
    //
    i2cAcquireBus(pca->i2c_driver); 
    switch (pca->mode) {
        case PCA9633_MODE_NORMAL:
            status |= i2cMasterTransmitTimeout(pca->i2c_driver,
                                               pca->i2c_addr,
                                               PCA9633_INIT_MODE1,
                                               sizeof(PCA9633_INIT_MODE1),
                                               NULL,
                                               0,
                                               MS2ST(2)); 
            status |= i2cMasterTransmitTimeout(pca->i2c_driver,
                                               pca->i2c_addr,
                                               PCA9633_INIT_MODE2_INVERTED,
                                               sizeof(PCA9633_INIT_MODE2_INVERTED),
                                               NULL,
                                               0,
                                               MS2ST(2)); 
            break;
        case PCA9633_MODE_INVERTED:
            break;
    }
    //
    if (status == RDY_OK) {
        pca->status = PCA9633_READY;
        consoleDebug("pca9633Init ok\r\n");
    } else {
        pca->status = PCA9633_UNINIT;
        consoleDebug("pca9633Init failed\r\n");
    }
    i2cReleaseBus(pca->i2c_driver); 
    //
    return pca->status;
}

/*
 * @brief   ...
 */
pca9633_channel_status_t pca9633ChannelInit(pca9633_channel_t *channel) {
    //
    msg_t status = RDY_OK;
    uint8_t buffer[2];
    //
    i2cAcquireBus(channel->pca->i2c_driver); 
    buffer[0] = PCA9633_CONTROL_NOAI | PCA9633_REGISTER_LEDOUT;
    status |= i2cMasterTransmitTimeout(channel->pca->i2c_driver,
                                       channel->pca->i2c_addr,
                                       buffer,
                                       1,
                                       buffer,
                                       2,
                                       MS2ST(2));
    buffer[1] = buffer[0];
    buffer[1] &= ~(PCA9633_LEDOUT_MASK << (channel->channel*2));
    switch (channel->mode) {
        case PCA9633_CHANNEL_MODE_SWITCH:
            buffer[1] |= PCA9633_LEDOUT_OFF << (channel->channel*2);
            break;
        case PCA9633_CHANNEL_MODE_PWM:
            buffer[1] |= PCA9633_LEDOUT_PWM << (channel->channel*2);
            break;
    }
    channel->prev = 0x00;
    channel->cur = 0x00;
    channel->next = 0x00;
    buffer[0] = PCA9633_CONTROL_NOAI | PCA9633_REGISTER_LEDOUT;
    status |= i2cMasterTransmitTimeout(channel->pca->i2c_driver,
                                       channel->pca->i2c_addr,
                                       buffer,
                                       2,
                                       NULL,
                                       0,
                                       MS2ST(2));
    if (channel->mode == PCA9633_CHANNEL_MODE_PWM) {
        buffer[0] = PCA9633_CONTROL_NOAI |
            (PCA9633_REGISTER_PWM0 + channel->channel);
        buffer[1] = 0x00;
        status |= i2cMasterTransmitTimeout(channel->pca->i2c_driver,
                                           channel->pca->i2c_addr,
                                           buffer,
                                           2,
                                           NULL,
                                           0,
                                           MS2ST(2));
    }
    //
    if (status == RDY_OK) {
        channel->status = PCA9633_CHANNEL_READY;
        consoleDebug("pca9633ChannelInit ok\r\n");
    } else {
        channel->status = PCA9633_CHANNEL_UNINIT;
        consoleDebug("pca9633ChannelInit failed\r\n");
    }
    i2cReleaseBus(channel->pca->i2c_driver); 
    //
    return channel->status;
}

/*
 * @brief   ...
 */
pca9633_channel_status_t pca9633ChannelPwmSet(pca9633_channel_t *channel,
                                              uint32_t pwm) {
    //
    consoleDebug("pca9633ChannelPwmSet start\r\n");
    switch (channel->mode) {
        case PCA9633_CHANNEL_MODE_SWITCH:
            consoleDebug("pca9633ChannelPwmSet bad mode\r\n");
            return false;
        case PCA9633_CHANNEL_MODE_PWM:
            channel->fade_start = 0;
            channel->prev = channel->cur;
            channel->next = pwm & 0xFF;
            if (channel->period == 0) {
                channel->cur = channel->next;
                consoleDebug("pca9633ChannelPwmSet immediate pwm: %X\r\n",
                             channel->cur);
            } else {
                channel->change_start = chTimeNow();
                consoleDebug("pca9633ChannelPwmSet pwm: %X\r\n",
                             channel->cur);
            }
            break;
    }
    consoleDebug("pca9633ChannelPwmSet end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 */
pca9633_channel_status_t pca9633ChannelPwmFade(pca9633_channel_t *channel,
                                               uint32_t unused) {
    //
    consoleDebug("pca9633ChannelPwmFade start\r\n");
    switch (channel->mode) {
        case PCA9633_CHANNEL_MODE_SWITCH:
            consoleDebug("pca9633ChannelPwmFade bad mode\r\n");
            return false;
        case PCA9633_CHANNEL_MODE_PWM:
            if (channel->fade_start == 0) {
                channel->fade_start = chTimeNow();
                channel->prev = channel->next = channel->cur;
            }
            break;
    }
    consoleDebug("pca9633ChannelPwmFade end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 */
pca9633_channel_status_t pca9633ChannelSwitchSet(pca9633_channel_t *channel,
                                                 uint32_t state) {
    //
    consoleDebug("pca9633ChannelSwitchSet start\r\n");
    switch (channel->mode) {
        case PCA9633_CHANNEL_MODE_SWITCH:
            channel->next = state & 0xFF;
            break;
        case PCA9633_CHANNEL_MODE_PWM:
            consoleDebug("pca9633ChannelSwitchSet bad mode\r\n");
            return false;
    }
    consoleDebug("pca9633ChannelSwitchSet end\r\n");
    //
    return false;
}

/*
 * @brief   ...
 */
pca9633_channel_status_t pca9633ChannelUpdate(pca9633_channel_t *channel, 
                                              varg_t unused) {
    //
    msg_t status = RDY_OK;
    uint8_t buffer[2], diff;
    float multiply;
    //
    consoleDebug("pca9633ChannelUpdate start\r\n");
    i2cAcquireBus(channel->pca->i2c_driver); 
    switch (channel->mode) {
        case PCA9633_CHANNEL_MODE_SWITCH:
            if (channel->cur != channel->next) {
                buffer[0] = PCA9633_CONTROL_NOAI | PCA9633_REGISTER_LEDOUT;
                status |= i2cMasterTransmitTimeout(channel->pca->i2c_driver,
                                                   channel->pca->i2c_addr,
                                                   buffer,
                                                   1,
                                                   buffer,
                                                   2,
                                                   MS2ST(2));
                buffer[1] = buffer[0];
                if (channel->next == PCA9633_SWITCH_ON) {
                    buffer[1] &= ~(PCA9633_LEDOUT_MASK << (channel->channel*2));
                            buffer[1] |= PCA9633_LEDOUT_ON << (channel->channel*2);
                    consoleDebug("LEDOUT_ON\r\n");
                } else {
                    buffer[1] &= ~(PCA9633_LEDOUT_MASK << (channel->channel*2));
                            buffer[1] |= PCA9633_LEDOUT_OFF << (channel->channel*2);
                    consoleDebug("LEDOUT_OFF\r\n");
                }
                buffer[0] = PCA9633_CONTROL_NOAI | PCA9633_REGISTER_LEDOUT;
                status |= i2cMasterTransmitTimeout(channel->pca->i2c_driver,
                                                   channel->pca->i2c_addr,
                                                   buffer,
                                                   2,
                                                   NULL,
                                                   0,
                                                   MS2ST(2));
                channel->cur = channel->next;
            }
            break;
        case PCA9633_CHANNEL_MODE_PWM:
            if (channel->next > channel->prev) {
                diff = (channel->next - channel->prev);
                multiply = (chTimeNow() - channel->change_start) / (float)MS2ST(channel->period);
                if (multiply <= 1) {
                    channel->cur = channel->prev + diff * multiply;
                }
                if (channel->cur > channel->next) {
                    channel->cur = channel->next;
                }
            } else if (channel->next < channel->prev) {
                diff = (channel->prev - channel->next);
                multiply = (chTimeNow() - channel->change_start) / (float)MS2ST(channel->period);
                if (multiply <= 1) {
                    channel->cur = channel->prev - diff * multiply;
                }
                if (channel->cur < channel->next) {
                    channel->cur = channel->next;
                }
            }
            if (channel->fade_start != 0) {
                multiply = (chTimeNow() - channel->fade_start) / (float)MS2ST(channel->fader);
                if (multiply <= 1) {
                    channel->cur = channel->next * (1 - multiply);
                }
                if (channel->cur == 0) {
                    channel->next = channel->cur;
                    channel->fade_start = 0;
                }
                consoleDebug("fader active: %2X\r\n", channel->cur);
            }
            if ((channel->prev != channel->next) ||
                (channel->fade_start != 0)) {
                buffer[0] = PCA9633_CONTROL_NOAI |
                    (PCA9633_REGISTER_PWM0 + channel->channel);
                buffer[1] = channel->cur & 0xFF;
                status |= i2cMasterTransmitTimeout(channel->pca->i2c_driver,
                                                   channel->pca->i2c_addr,
                                                   buffer,
                                                   2,
                                                   NULL,
                                                   0,
                                                   MS2ST(2));
                if (channel->cur == channel->next) {
                    channel->prev = channel->next;
                }
            }
            break;
    }
    //
    if (status == RDY_OK) {
        channel->status = PCA9633_CHANNEL_READY;
        i2cReleaseBus(channel->pca->i2c_driver); 
        consoleDebug("pca9633ChannelUpdate end\r\n");
    } else {
        channel->status = PCA9633_CHANNEL_UNINIT;
        i2cReleaseBus(channel->pca->i2c_driver); 
        consoleDebug("pca9633ChannelUpdate fail\r\n");
    }
    //
    return channel->status;
}

/*
 * @brief   ...
 */
void pca9633SequenceInit(pca9633_sequence_t *sequence) {
    //
    uint8_t i;
    //
    consoleDebug("pca9633SequenceInit start\r\n");
    if (lldLockWaitTimeout(&sequence->lock, MS2ST(5)) == true) {
        sequence->position = 0;
        sequence->timestamp = 0;
        sequence->cur_seq = NULL;
        sequence->sleep_start = 0;
        for (i = 0; i < sequence->channel_number; i++) {
            sequence->channel[i]->period = sequence->period;
            pca9633ChannelInit(sequence->channel[i]);
        }
        lldUnlock(&sequence->lock);
        consoleDebug("pca9633SequenceInit end\r\n");
    } else {
        consoleDebug("pca9633SequenceInit fail\r\n");
    }
}

/*
 * @brief   ...
 */
bool pca9633SequenceSet(pca9633_sequence_t *sequence,
                        pca9633_sequence_data_t *seq) {
    consoleDebug("pca9633SequenceSet start\r\n");
    if (lldLockWaitTimeout(&sequence->lock, MS2ST(5)) == true) {
        sequence->position = 0;
        sequence->timestamp = 0;
        sequence->cur_seq = seq;
        if (sequence->sleep_timeout != 0) {
            sequence->sleep_start = chTimeNow() + S2ST(sequence->sleep_timeout);
        }
        lldUnlock(&sequence->lock);
        consoleDebug("pca9633SequenceSet end\r\n");
    } else {
        consoleDebug("pca9633SequenceSet fail\r\n");
        return false;
    }
    return true;
}

/*
 * @brief   ...
 */
bool pca9633SequenceActivateSleep(pca9633_sequence_t *sequence) {
    consoleDebug("pca9633SequenceActivateSleep start\r\n");
    if (lldLockWaitTimeout(&sequence->lock, MS2ST(5)) == true) {
        sequence->position = 0;
        sequence->timestamp = 0;
        sequence->cur_seq = sequence->sleep_seq;
        lldUnlock(&sequence->lock);
        consoleDebug("pca9633SequenceActivateSleep end\r\n");
    } else {
        consoleDebug("pca9633SequenceActivateSleep fail\r\n");
        return false;
    }
    return true;
}

/*
 * @brief   ...
 */
void pca9633SequenceUpdate(pca9633_sequence_t *sequence, varg_t unused) {
    //
    uint8_t i;
    consoleDebug("pca9633SequenceUpdate start\r\n");
    if (lldLock(&sequence->lock) == true) {
        if ((sequence->sleep_start != 0) && (sequence->sleep_start < chTimeNow())) {
            
            lldUnlock(&sequence->lock);
            pca9633SequenceSet(sequence, sequence->sleep_seq);
            consoleDebug("pca9633SequenceUpdate sleep activated\r\n");
            return;
        }
        if (sequence->cur_seq == NULL) {
            lldUnlock(&sequence->lock);
            consoleDebug("pca9633SequenceUpdate cur_seq == NULL\r\n");
            return;
        }
        if (sequence->timestamp < chTimeNow()) {
            switch (sequence->cur_seq->data[sequence->position]) {
                case 0x00:
                    break;
                case 0xFF:
                    if (sequence->cur_seq->next != NULL) {
                        lldUnlock(&sequence->lock);
                        pca9633SequenceSet(sequence, sequence->cur_seq->next);
                        consoleDebug("pca9633SequenceUpdate next sequence activated\r\n");
                        return;
                    } else {
                        sequence->position = 0x00;
                    }
                    break;
                default:
                    for (i = 0; i < sequence->channel_number; i++) {
                        switch (sequence->channel[i]->mode) {
                            case PCA9633_CHANNEL_MODE_SWITCH:
                                pca9633ChannelSwitchSet(sequence->channel[i], sequence->cur_seq->data[sequence->position + i + 1]);
                                break;
                            case PCA9633_CHANNEL_MODE_PWM:
                                sequence->channel[i]->period = sequence->cur_seq->data[sequence->position] * sequence->period;
                                pca9633ChannelPwmSet(sequence->channel[i], sequence->cur_seq->data[sequence->position + i + 1]);
                                break;
                        }
                    }
                    sequence->timestamp = chTimeNow() + MS2ST(sequence->cur_seq->data[sequence->position] * sequence->period);
                    sequence->position += sequence->channel_number + 1;
                    break;
            }
        }
        lldUnlock(&sequence->lock);
        consoleDebug("pca9633SequenceUpdate end\r\n");
    } else {
        consoleDebug("pca9633SequenceUpdate fail\r\n");
    }
    //
    for (i = 0; i < sequence->channel_number; i++) {
        pca9633ChannelUpdate(sequence->channel[i], NULL);
    }
}

#endif /* ITHACA_USE_PCA9633 */

