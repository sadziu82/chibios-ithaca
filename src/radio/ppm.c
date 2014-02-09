#include <ithaca.h>

#if ITHACA_USE_RADIO_PPM || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*
 * @brief   PPM register definitions.
 * @details This enum contains addresses for all PPM registers.
            They are ORed with data (in LSBs) when communicating.
 */
typedef enum {
    PPM_STATUS = 0x0000,
    PPM_CONFIGURATION = 0x8000,
    PPM_POWER_MANAGEMENT = 0x8200,
    PPM_FREQUENCY = 0xA000,
    PPM_DATA_RATE = 0xC600,
    PPM_RECEIVER_CONTROL = 0x9000,
    PPM_DATA_FILTER = 0xC200,
    PPM_FIFO_RESET_MODE = 0xCA00,
    PPM_SYNCHRON_PATTERN = 0xCE00,
    PPM_AFC = 0xC400,
    PPM_TX_CONFIG = 0x9800,
    PPM_PLL = 0xCC00,
    PPM_WAKEUP_TIMER = 0xE000,
    PPM_LOW_DUTY_CYCLE = 0xC800,
    PPM_TX_WRITE = 0xB800,
    PPM_RX_READ = 0xB000,
    PPM_LOW_BATTERY_DETECTOR = 0xC000,
} ppm_register_t;

/*
 * @brief   Status register bits.
 * @details Some bits valuer are repeated - it's only for readability.
            Depends on current mode they meaning might be changed.
 */
typedef enum {
    // single bits
    PPM_STATUS_TX_READY = 0x8000,
    PPM_STATUS_RX_READY = 0x8000,
    PPM_STATUS_POWER_ON_RESET = 0x4000,
    PPM_STATUS_TX_UNDERRUN = 0x2000,
    PPM_STATUS_RX_OVERFLOW = 0x2000,
    PPM_STATUS_WAKEUP = 0x1000,
    PPM_STATUS_EXT_INT = 0x0800,
    PPM_STATUS_LOW_BATTERY = 0x0400,
    PPM_STATUS_FIFO_EMPTY = 0x0200,
    PPM_STATUS_RSSI = 0x0100,
    PPM_STATUS_ATS = 0x0100,
} ppm_status_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // single bits
    PPM_CONFIGURATION_INTERNAL_DATA_REGISTER = 0x0080,
    PPM_CONFIGURATION_FIFO_MODE = 0x0040,
    PPM_CONFIGURATION_FREQ_868 = 0x0020,
    PPM_CONFIGURATION_CAP_125PF = 0x0008,
    // combined settings
    PPM_CONFIGURATION_BAND868_IDLE = 
        PPM_CONFIGURATION_FREQ_868 |
        PPM_CONFIGURATION_CAP_125PF,
    // combined settings
    PPM_CONFIGURATION_BAND868_RX = 
        PPM_CONFIGURATION_FIFO_MODE |
        PPM_CONFIGURATION_FREQ_868 |
        PPM_CONFIGURATION_CAP_125PF,
    // combined settings
    PPM_CONFIGURATION_BAND868_TX = 
        PPM_CONFIGURATION_INTERNAL_DATA_REGISTER |
        PPM_CONFIGURATION_FREQ_868 |
        PPM_CONFIGURATION_CAP_125PF,
} ppm_configuration_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // single bits
    PPM_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT = 0x0001,
    PPM_POWER_MANAGEMENT_ENABLE_LOW_BATTERY_DETECTOR = 0x0004,
    PPM_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR = 0x0008,
    PPM_POWER_MANAGEMENT_ENABLE_SYNTHESIZER = 0x0010,
    PPM_POWER_MANAGEMENT_ENABLE_TRANSMITTER = 0x0020,
    PPM_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK = 0x0040,
    PPM_POWER_MANAGEMENT_ENABLE_RECEIVER = 0x0080,
    // preset idle mode
    PPM_POWER_MANAGEMENT_IDLE_MODE =
        PPM_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        PPM_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        PPM_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        PPM_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    // preset tx mode
    PPM_POWER_MANAGEMENT_TX_MODE =
        PPM_POWER_MANAGEMENT_ENABLE_TRANSMITTER |
        PPM_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        PPM_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        PPM_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        PPM_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    // preset rx mode
    PPM_POWER_MANAGEMENT_RX_MODE =
        PPM_POWER_MANAGEMENT_ENABLE_RECEIVER |
        PPM_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        PPM_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        PPM_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        PPM_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
} ppm_power_management_t;

/*
 * @brief   Low-voltage battery detector presets.
 * @details Low-voltage battery detecter is the only way to check
            if PPM module is present. Setting threshold lower than Vcc
            should activate IRQ PPM line.
 */
typedef enum {
    // low voltage detection presets
    PPM_LOW_BATTERY_V22 = 0x0000,
    PPM_LOW_BATTERY_V37 = 0x000F,
} ppm_low_battery_t;

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*
 * @brief   PPM driver one instance.
 */
PPMDriver PPMD1 = {
    .lock = {
        .name = "PPMD1",
        .flag = false,
    },
};

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   SPI configuration used by PPM.
 */
static SPIConfig ppm_spi_cfg = {
    .end_cb = NULL,
    .cr1 = SPI_CR1_MSTR | SPI_CR1_DFF |
           SPI_CR1_BR_1,
           //SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   PPM IRQ handler.
 */
static void ppm_lld_nirq_handler(EXTDriver *extp, expchannel_t channel);

/*
 * @brief   PPM RST line assert.
 */
void ppm_lld_assert_rst(const PPMDriver *drv) {
    palClearPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   PPM RST line release.
 */
void ppm_lld_release_rst(const PPMDriver *drv) {
    palSetPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   Exchange half-word data with PPM module.
 */
inline
uint16_t ppm_lld_xfer(const PPMDriver *drv, const uint16_t cmd) {
    uint16_t data = 0;
    spiSelect(drv->config->spi_drv);
    spiExchange(drv->config->spi_drv, 1, &cmd, &data);
    spiUnselect(drv->config->spi_drv);
    return data;
}

/*
 * @brief   Write one byte data to PPM register.
 */
void ppm_lld_write(const PPMDriver *drv,
                   const ppm_register_t reg, const uint16_t data) {
    ppm_lld_xfer(drv, reg | data);
}

/*
 * @brief   Read data from PPM register.
 * @details Only status register can be read.
            Others may return undefined results.
 */
uint8_t ppm_lld_read(const PPMDriver *drv) {
    //
    return (ppm_lld_xfer(drv, PPM_RX_READ) & 0xFF);
}

/*
 * @brief   Return PPM status.
 */
inline
uint16_t ppm_lld_status(const PPMDriver *drv) {
    //
    return ppm_lld_xfer(drv, PPM_STATUS);
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   Initialize IO ports used by PPM.
 * @details SPI ports (MISO,MOSI,CLK) should be set in proper mode
            by user. Also EXT driver must be started by user.
            RST, CE and IRQ lines are configured by this function.
            SPI driver is configured & started also here.
 */
static bool ppm_lld_init_io(PPMDriver *drv) {
    //
    consoleDebug("ppm_lld_init_io start\r\n");
    // spi cs
    ppm_spi_cfg.ssport = drv->config->cs_port;
    ppm_spi_cfg.sspad = drv->config->cs_pin;
    palSetPadMode(drv->config->cs_port, drv->config->cs_pin,
                  PAL_MODE_OUTPUT_PUSHPULL);
    // reset
    palSetPadMode(drv->config->rst_port, drv->config->rst_pin,
                  PAL_MODE_OUTPUT_PUSHPULL);
    // nirq
    palSetPadMode(drv->config->nirq_port, drv->config->nirq_pin,
                  PAL_MODE_INPUT_PULLUP);
    drv->nirq_cfg.mode = EXT_CH_MODE_FALLING_EDGE |
                         drv->config->ext_mode;
    drv->nirq_cfg.cb = ppm_lld_nirq_handler;
    // configure ext channel used by radio
    chSysLock();
    extSetChannelModeI(drv->config->ext_drv,
                       drv->config->nirq_pin, &drv->nirq_cfg);
    chSysUnlock()
    extChannelEnable(drv->config->ext_drv,
                     drv->config->nirq_pin);
    // start spi driver
    spiStart(drv->config->spi_drv, &ppm_spi_cfg);
    //
    consoleDebug("ppm_lld_init_io end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static bool ppm_lld_init_module(PPMDriver *drv) {
    //
    systime_t timeout;
    //
    consoleDebug("ppm_lld_init_module start\r\n");
    // reset module
    ppm_lld_assert_rst(drv);
    chThdSleepMilliseconds(10);
    //
    ppm_lld_release_rst(drv);
    chThdSleepMilliseconds(10);
    // put module into sleep mode
    ppm_lld_write(drv, PPM_POWER_MANAGEMENT,
                PPM_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT |
                PPM_POWER_MANAGEMENT_ENABLE_LOW_BATTERY_DETECTOR);
    // make sure we are in fifo mode
    ppm_lld_write(drv, PPM_TX_WRITE, 0);
    //
    if (palReadPad(drv->config->nirq_port, drv->config->nirq_pin) != 0) {
        consoleDebug("ppm_lld_init_module nIRQ high after hardware reset\r\n");
        return false;
    }
    //
    timeout = chTimeNow() + MS2ST(250);
    while (true) {
        if ((ppm_lld_status(drv) & PPM_STATUS_POWER_ON_RESET) != 0) {
            consoleDebug("ppm_lld_init_module power-on-reset bit set\r\n");
            break;
        } else if (timeout <= chTimeNow()) {
            consoleDebug("ppm_lld_init_module power-on-reset "
                         "timeout expired\r\n");
            return false;
        }
        consoleDebug("ppm_lld_init_module wait-loop\r\n");
        chThdSleepMilliseconds(5);
    }
    // nIRQ pin should be now high
    if (palReadPad(drv->config->nirq_port, drv->config->nirq_pin) == 0) {
        consoleDebug("ppm_lld_init_module nIRQ low after status read\r\n");
        return false;
    }
    // check module presence by setting high low-battery-detector threshold
    ppm_lld_write(drv, PPM_LOW_BATTERY_DETECTOR,
                     PPM_LOW_BATTERY_V37);
    // wait for detector to activate
    chThdSleepMilliseconds(5);
    if ((ppm_lld_status(drv) & PPM_STATUS_LOW_BATTERY) == 0) {
        consoleDebug("ppm_lld_init_module low-battery-detector "
                     "test failed\r\n");
        return false;
    }
    // restore low-battery-detector threshold
    ppm_lld_write(drv, PPM_LOW_BATTERY_DETECTOR,
                     PPM_LOW_BATTERY_V22);
    // put module into sleep mode
    ppm_lld_write(drv, PPM_POWER_MANAGEMENT,
                     PPM_POWER_MANAGEMENT_IDLE_MODE);
    // change driver state
    drv->state = PPM_STOP;
    //
    consoleDebug("ppm_lld_init_module end\r\n");
    return true;
}

/*
 * @brief   ppm ...
 */
static bool ppm_lld_init_setup(PPMDriver *drv) {
    // initialize rf module
    consoleDebug("ppm_lld_init_setup start\r\n");
    ppm_lld_write(drv, PPM_CONFIGURATION,
                     PPM_CONFIGURATION_BAND868_IDLE);
    ppm_lld_write(drv, PPM_FREQUENCY,
                     drv->config->base_freq);
    ppm_lld_write(drv, PPM_DATA_RATE,
                     drv->config->data_rate);
    ppm_lld_write(drv, PPM_RECEIVER_CONTROL, 0x0480);
    ppm_lld_write(drv, PPM_DATA_FILTER, 0x00AC);
    ppm_lld_write(drv, PPM_FIFO_RESET_MODE, 0X0083);
    ppm_lld_write(drv, PPM_SYNCHRON_PATTERN,
                     drv->config->group_id);
    ppm_lld_write(drv, PPM_AFC, 0x0083);
    ppm_lld_write(drv, PPM_TX_CONFIG, 0x0070);
    ppm_lld_write(drv, PPM_PLL, 0x0077);
    ppm_lld_write(drv, PPM_WAKEUP_TIMER, 0x0000);
    ppm_lld_write(drv, PPM_LOW_DUTY_CYCLE, 0x0000);
    ppm_lld_write(drv, PPM_LOW_BATTERY_DETECTOR, 0x0000);
    //
    chThdSleepMilliseconds(50);
    consoleDebug("ppm_lld_init_setup end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static void ppm_lld_nirq_handler(EXTDriver *extp, expchannel_t channel) {
    //
    PPMDriver *drv;
    (void)extp;
    uint32_t crc;
    //
    if (PPMD1.config->nirq_pin == channel) {
        drv = &PPMD1;
    } else {
        // no driver enabled/configured, ignore interrupt
        return;
    }
    if (lldLockISR(&drv->lock) == true) {
        switch (drv->state) {
            case PPM_UNINIT:
            case PPM_STOP:
            case PPM_IDLE:
            case PPM_RX_INIT:
                break;
            case PPM_RX_START:
                // reset CRC
                CRC->CR = (uint32_t)0x01;
                // initiate first read
                spiUnselectI(drv->config->spi_drv);
                drv->txrx_data = PPM_RX_READ;
                spiSelectI(drv->config->spi_drv);
                spiStartExchangeI(drv->config->spi_drv,
                                  1, &drv->txrx_data, &drv->txrx_data);
                // single packet reception should not take long
                // TODO make this timeout user configurable
                drv->txrx_counter = 0;
                drv->task_tmout = chTimeNow() + MS2ST(100);
                drv->state = PPM_RX_ACTIVE;
                break;
            case PPM_RX_ACTIVE:
                if (drv->txrx_counter < (sizeof(drv->txrx_buff) - 6)) {
                    // store previously read data
                    drv->txrx_buff[drv->txrx_counter] = drv->txrx_data & 0xFF;
                    // read next data
                    spiUnselectI(drv->config->spi_drv);
                    drv->txrx_data = PPM_RX_READ;
                    spiSelectI(drv->config->spi_drv);
                    spiStartExchangeI(drv->config->spi_drv,
                                      1, &drv->txrx_data, &drv->txrx_data);
                    // calculate crc
                    if (drv->txrx_counter < sizeof(radio_packet_t)) {
                        CRC->DR = drv->txrx_buff[drv->txrx_counter];
                    }
                    drv->txrx_counter++;
                } else {
                    spiUnselectI(drv->config->spi_drv);
                    chBSemSignalI(&drv->flag);
                    drv->state = PPM_RX_COMPLETE;
                }
                break;
            case PPM_RX_COMPLETE:
            case PPM_TX_INIT:
                break;
            case PPM_TX_ACTIVE:
                if (drv->txrx_counter < sizeof(drv->txrx_buff)) {
                    spiUnselectI(drv->config->spi_drv);
                    drv->txrx_data = PPM_TX_WRITE | drv->txrx_buff[drv->txrx_counter];
                    spiSelectI(drv->config->spi_drv);
                    spiStartSendI(drv->config->spi_drv,
                                  1, &drv->txrx_data);
                    drv->txrx_counter++;
                    // calculate crc
                    if (drv->txrx_counter <= sizeof(radio_packet_t)) {
                        CRC->DR = drv->txrx_buff[drv->txrx_counter + 5];
                    } else if (drv->txrx_counter == sizeof(radio_packet_t) + 1) {
                        crc = CRC->DR;
                        drv->txrx_buff[drv->txrx_counter + 8] = (crc >> 24) & 0xFF;
                        drv->txrx_buff[drv->txrx_counter + 7] = (crc >> 16) & 0xFF;
                        drv->txrx_buff[drv->txrx_counter + 6] = (crc >> 8) & 0xFF;
                        drv->txrx_buff[drv->txrx_counter + 5] = crc & 0xFF;
                    }
                } else {
                    spiUnselectI(drv->config->spi_drv);
                    chBSemSignalI(&drv->flag);
                    drv->state = PPM_TX_COMPLETE;
                }
                break;
            case PPM_TX_COMPLETE:
                break;
        }
        lldUnlockISR(&drv->lock);
    } else {
        // in case something went wrong
        chSysLockFromIsr();
        drv->error = true;
        chSysUnlockFromIsr();
        chBSemSignalI(&drv->flag);
    }
} 

/*
 * @brief   PPM thread.
 * @details This thread performs switch between different states
            of PPM module. It also executes defined callbacks.
 */
//__attribute__((noreturn))
static void PPMThread(void *arg) {
    // 
    uint32_t crc, *crc_ptr;
    PPMDriver *drv = (PPMDriver*)arg;
    ppm_callback_t cb = NULL;
    //
    chRegSetThreadName("PPMThread");
    consoleDebug("ppm start\r\n");
    //
    while (!chThdShouldTerminate()) {
        chBSemWaitTimeout(&drv->flag, MS2ST(5));
        cb = NULL;
        if (lldLockWaitTimeout(&drv->lock, 2) == true) {
            chSysLock();
            if (drv->error == true) {
                drv->state = PPM_IDLE;
                drv->error = false;
                chSysUnlock();
                consoleDebug("\r\n\r\n\r\n ********** recovery from ISR locking error **********\r\n\r\n\r\n");
            } else {
                chSysUnlock();
            }
            switch (drv->state) {
                case PPM_UNINIT:
                    consoleDebug("PPM_UNINIT\r\n");
                    break;
                case PPM_STOP:
                    consoleDebug("PPM_STOP\r\n");
                    drv->state = PPM_IDLE;
                    break;
                case PPM_IDLE:
                    // put radio into idle mode
                    ppm_lld_write(drv, PPM_CONFIGURATION,
                                     PPM_CONFIGURATION_BAND868_IDLE);
                    ppm_lld_write(drv, PPM_POWER_MANAGEMENT,
                                     PPM_POWER_MANAGEMENT_IDLE_MODE);
                    cb = drv->config->idle_cb;
                    consoleDebug("PPM_IDLE\r\n");
                    break;
                case PPM_RX_INIT:
                    //
                    if (drv->config->recv_timeout != 0) {
                        drv->task_tmout = chTimeNow() + MS2ST(drv->config->recv_timeout);
                    } else {
                        drv->task_tmout = 0;
                    }
                    // reconfigure PPM module
                    ppm_lld_write(drv, PPM_CONFIGURATION,
                                     PPM_CONFIGURATION_BAND868_RX);
                    ppm_lld_write(drv, PPM_POWER_MANAGEMENT,
                                     PPM_POWER_MANAGEMENT_RX_MODE);
                    // clear interrupts
                    ppm_lld_status(drv);
                    // reset fifo
                    ppm_lld_write(drv, PPM_FIFO_RESET_MODE, 0x0081);
                    ppm_lld_write(drv, PPM_FIFO_RESET_MODE, 0x0083);
                    // change driver state
                    drv->txrx_counter = 0;
                    chBSemSignal(&drv->flag);
                    consoleDebug("PPM_RX_START\r\n");
                    drv->state = PPM_RX_START;
                    break;
                case PPM_RX_START:
                case PPM_RX_ACTIVE:
                    if ((drv->task_tmout != 0) &&
                        (drv->task_tmout <= chTimeNow())) {
                            cb = drv->config->recv_error_cb;
                            chBSemSignal(&drv->flag);
                            consoleDebug("PPM_RX_START/ACTIVE: timeout %d\r\n", drv->txrx_counter);
                            drv->state = PPM_IDLE;
                            break;
                    }
                    break;
                case PPM_RX_COMPLETE:
                    // get received crc
                    crc_ptr = (uint32_t *)(drv->txrx_buff + sizeof(radio_packet_t));
                    // read crc, disable hardware module, check if crc correct
                    crc = CRC->DR;
                    if (crc != *crc_ptr) {
                        cb = drv->config->recv_error_cb;
                        chBSemSignal(&drv->flag);
                        consoleDebug("PPM_RX_COMPLETE: crc != crc_recv - %8X vs %8X\r\n", crc, *crc_ptr);
                        drv->state = PPM_IDLE;
                    } else {
                        cb = drv->config->recv_done_cb;
                        chBSemSignal(&drv->flag);
                        consoleDebug("PPM_RX_COMPLETE: crc ok\r\n");
                        drv->state = PPM_IDLE;
                    }
                    break;
                case PPM_TX_INIT:
                    //
                    if (drv->config->recv_timeout != 0) {
                        drv->task_tmout = chTimeNow() + MS2ST(drv->config->recv_timeout);
                    } else {
                        drv->task_tmout = 0;
                    }
                    //
                    CRC->CR = (uint32_t)0x01;
                    // clear el, et bits
                    ppm_lld_write(drv, PPM_CONFIGURATION,
                                     PPM_CONFIGURATION_BAND868_IDLE);
                    ppm_lld_write(drv, PPM_POWER_MANAGEMENT,
                                     PPM_POWER_MANAGEMENT_IDLE_MODE);
                    // clear interrupts
                    ppm_lld_status(drv);
                    // initialize driver state for tx
                    drv->txrx_counter = 0;
                    chBSemSignal(&drv->flag);
                    consoleDebug("PPM_TX_INIT\r\n");
                    drv->state = PPM_TX_ACTIVE;
                    // switch to tx mode
                    ppm_lld_write(drv, PPM_CONFIGURATION,
                                     PPM_CONFIGURATION_BAND868_TX);
                    ppm_lld_write(drv, PPM_POWER_MANAGEMENT,
                                     PPM_POWER_MANAGEMENT_TX_MODE);
                    //
                    break;
                case PPM_TX_ACTIVE:
                    if ((drv->task_tmout != 0) &&
                        (drv->task_tmout <= chTimeNow())) {
                            cb = drv->config->send_error_cb;
                            chBSemSignal(&drv->flag);
                            consoleDebug("PPM_TX_ACTIVE: timeout (%d)\r\n", drv->txrx_counter);
                            drv->state = PPM_IDLE;
                            break;
                    }
                    break;
                case PPM_TX_COMPLETE:
                    // run call back
                    cb = drv->config->send_done_cb;
                    chBSemSignal(&drv->flag);
                    consoleDebug("PPM_TX_COMPLETE\r\n");
                    drv->state = PPM_IDLE;
                    break;
            }
            if (cb != NULL ) {
                cb(drv, (radio_packet_t *)drv->txrx_buff, drv->arg);
            }
            lldUnlock(&drv->lock);
        } else {
            chBSemSignal(&drv->flag);
        }
    }
    //
    chThdExit(1);
}

/*
 * @brief   ppm ...
 */
inline void ppmRecvStartS(PPMDriver *drv, void *arg) {
    drv->state = PPM_RX_INIT;
    drv->arg = arg;
    chBSemSignal(&drv->flag);
}

/*
 * @brief   ppm ...
 */
bool ppmRecvStart(PPMDriver *drv, void *arg) {
    // make sure no-one will interrupt
    if (lldLockWaitTimeout(&drv->lock, 2) == true) {
        ppmRecvStartS(drv, arg);
        lldUnlock(&drv->lock);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ppm ...
 */
inline void ppmSendStartS(PPMDriver *drv, radio_packet_t *packet, void *arg) {
    //
    uint8_t i;
    // prepare data to send
    i = 0;
    drv->txrx_buff[i++] = 0xAA;
    drv->txrx_buff[i++] = 0xAA;
    drv->txrx_buff[i++] = 0xAA;
    drv->txrx_buff[i++] = 0xAA;
    drv->txrx_buff[i++] = 0x2D;
    drv->txrx_buff[i++] = drv->config->group_id;
    memcpy(drv->txrx_buff + i, packet, sizeof(radio_packet_t));
    //
    drv->arg = arg;
    drv->state = PPM_TX_INIT;
    chBSemSignal(&drv->flag);
}

/*
 * @brief   ppm ...
 */
bool ppmSendStart(PPMDriver *drv, radio_packet_t *packet, void *arg) {
    // make sure no-one will interrupt now
    if (lldLockWaitTimeout(&drv->lock, 2) == true) {
        ppmSendStartS(drv, packet, arg);
        lldUnlock(&drv->lock);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool ppmInit(PPMDriver *drv, PPMConfig *config, void *arg) {
    //
    consoleDebug("ppmInit start\r\n");
    //
    chSysLock();
    // driver synchronization variables
    chBSemInit(&drv->flag, 0);
    // driver structures
    drv->state = PPM_UNINIT;
    drv->config = config;
    drv->arg = arg;
    //
    chSysUnlock();
    //
    if (drv->config->ext_drv->state != EXT_ACTIVE) {
        consoleDebug("ppmInit EXT driver not started\r\n");
        return false;
    }
    // enable hardware crc32
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    //
    if (!ppm_lld_init_io(drv) ||
        !ppm_lld_init_module(drv) ||
        !ppm_lld_init_setup(drv)) {
        consoleWarn("ppmInit failed\r\n");
        return false;
    }
    //
    lldLock(&drv->lock);
    chThdCreateStatic(&drv->_wa, sizeof(drv->_wa),
                      NORMALPRIO + drv->config->priority,
                      (tfunc_t)PPMThread, drv);
    drv->state = PPM_STOP;
    lldUnlock(&drv->lock);
    //
    consoleDebug("ppmInit end\r\n");
    return true;
}

#endif /* ITHACA_USE_RADIO_PPM */

