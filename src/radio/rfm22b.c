#include <ithaca.h>

#if ITHACA_USE_RADIO_RFM22B || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*
 * @brief   RFM22B register definitions.
 * @details This enum contains addresses for all RFM22B registers.
            They are ORed with data (in LSBs) when communicating.
 */
typedef enum {
    RFM22B_STATUS = 0x0000,
    RFM22B_CONFIGURATION = 0x8000,
    RFM22B_POWER_MANAGEMENT = 0x8200,
    RFM22B_FREQUENCY = 0xA000,
    RFM22B_DATA_RATE = 0xC600,
    RFM22B_RECEIVER_CONTROL = 0x9000,
    RFM22B_DATA_FILTER = 0xC200,
    RFM22B_FIFO_RESET_MODE = 0xCA00,
    RFM22B_SYNCHRON_PATTERN = 0xCE00,
    RFM22B_AFC = 0xC400,
    RFM22B_TX_CONFIG = 0x9800,
    RFM22B_PLL = 0xCC00,
    RFM22B_WAKEUP_TIMER = 0xE000,
    RFM22B_LOW_DUTY_CYCLE = 0xC800,
    RFM22B_TX_WRITE = 0xB800,
    RFM22B_RX_READ = 0xB000,
    RFM22B_LOW_BATTERY_DETECTOR = 0xC000,
} rfm22b_register_t;

/*
 * @brief   Status register bits.
 * @details Some bits valuer are repeated - it's only for readability.
            Depends on current mode they meaning might be changed.
 */
typedef enum {
    // single bits
    RFM22B_STATUS_TX_READY = 0x8000,
    RFM22B_STATUS_RX_READY = 0x8000,
    RFM22B_STATUS_POWER_ON_RESET = 0x4000,
    RFM22B_STATUS_TX_UNDERRUN = 0x2000,
    RFM22B_STATUS_RX_OVERFLOW = 0x2000,
    RFM22B_STATUS_WAKEUP = 0x1000,
    RFM22B_STATUS_EXT_INT = 0x0800,
    RFM22B_STATUS_LOW_BATTERY = 0x0400,
    RFM22B_STATUS_FIFO_EMPTY = 0x0200,
    RFM22B_STATUS_RSSI = 0x0100,
    RFM22B_STATUS_ATS = 0x0100,
} rfm22b_status_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // single bits
    RFM22B_CONFIGURATION_INTERNAL_DATA_REGISTER = 0x0080,
    RFM22B_CONFIGURATION_FIFO_MODE = 0x0040,
    RFM22B_CONFIGURATION_FREQ_868 = 0x0020,
    RFM22B_CONFIGURATION_CAP_125PF = 0x0008,
    // combined settings
    RFM22B_CONFIGURATION_BAND868_IDLE = 
        RFM22B_CONFIGURATION_FREQ_868 |
        RFM22B_CONFIGURATION_CAP_125PF,
    // combined settings
    RFM22B_CONFIGURATION_BAND868_RX = 
        RFM22B_CONFIGURATION_FIFO_MODE |
        RFM22B_CONFIGURATION_FREQ_868 |
        RFM22B_CONFIGURATION_CAP_125PF,
    // combined settings
    RFM22B_CONFIGURATION_BAND868_TX = 
        RFM22B_CONFIGURATION_INTERNAL_DATA_REGISTER |
        RFM22B_CONFIGURATION_FREQ_868 |
        RFM22B_CONFIGURATION_CAP_125PF,
} rfm22b_configuration_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // single bits
    RFM22B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT = 0x0001,
    RFM22B_POWER_MANAGEMENT_ENABLE_LOW_BATTERY_DETECTOR = 0x0004,
    RFM22B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR = 0x0008,
    RFM22B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER = 0x0010,
    RFM22B_POWER_MANAGEMENT_ENABLE_TRANSMITTER = 0x0020,
    RFM22B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK = 0x0040,
    RFM22B_POWER_MANAGEMENT_ENABLE_RECEIVER = 0x0080,
    // preset idle mode
    RFM22B_POWER_MANAGEMENT_IDLE_MODE =
        RFM22B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM22B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM22B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM22B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    // preset tx mode
    RFM22B_POWER_MANAGEMENT_TX_MODE =
        RFM22B_POWER_MANAGEMENT_ENABLE_TRANSMITTER |
        RFM22B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM22B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM22B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM22B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    // preset rx mode
    RFM22B_POWER_MANAGEMENT_RX_MODE =
        RFM22B_POWER_MANAGEMENT_ENABLE_RECEIVER |
        RFM22B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM22B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM22B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM22B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
} rfm22b_power_management_t;

/*
 * @brief   Low-voltage battery detector presets.
 * @details Low-voltage battery detecter is the only way to check
            if RFM22B module is present. Setting threshold lower than Vcc
            should activate IRQ RFM22B line.
 */
typedef enum {
    // low voltage detection presets
    RFM22B_LOW_BATTERY_V22 = 0x0000,
    RFM22B_LOW_BATTERY_V37 = 0x000F,
} rfm22b_low_battery_t;

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*
 * @brief   RFM22B driver one instance.
 */
RFM22BDriver RFM22BD1 = {
    .lock = {
        .name = "RFM22BD1",
        .flag = false,
    },
};

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   SPI configuration used by RFM22B.
 */
static SPIConfig rfm22b_spi_cfg = {
    .end_cb = NULL,
    .cr1 = SPI_CR1_MSTR | SPI_CR1_DFF |
           SPI_CR1_BR_1,
           //SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   RFM22B IRQ handler.
 */
static void rfm22b_lld_nirq_handler(EXTDriver *extp, expchannel_t channel);

/*
 * @brief   RFM22B RST line assert.
 */
void rfm22b_lld_assert_rst(const RFM22BDriver *drv) {
    palClearPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   RFM22B RST line release.
 */
void rfm22b_lld_release_rst(const RFM22BDriver *drv) {
    palSetPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   Exchange half-word data with RFM22B module.
 */
inline
uint16_t rfm22b_lld_xfer(const RFM22BDriver *drv, const uint16_t cmd) {
    uint16_t data = 0;
    spiSelect(drv->config->spi_drv);
    spiExchange(drv->config->spi_drv, 1, &cmd, &data);
    spiUnselect(drv->config->spi_drv);
    return data;
}

/*
 * @brief   Write one byte data to RFM22B register.
 */
void rfm22b_lld_write(const RFM22BDriver *drv,
                   const rfm22b_register_t reg, const uint16_t data) {
    rfm22b_lld_xfer(drv, reg | data);
}

/*
 * @brief   Read data from RFM22B register.
 * @details Only status register can be read.
            Others may return undefined results.
 */
uint8_t rfm22b_lld_read(const RFM22BDriver *drv) {
    //
    return (rfm22b_lld_xfer(drv, RFM22B_RX_READ) & 0xFF);
}

/*
 * @brief   Return RFM22B status.
 */
inline
uint16_t rfm22b_lld_status(const RFM22BDriver *drv) {
    //
    return rfm22b_lld_xfer(drv, RFM22B_STATUS);
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   Initialize IO ports used by RFM22B.
 * @details SPI ports (MISO,MOSI,CLK) should be set in proper mode
            by user. Also EXT driver must be started by user.
            RST, CE and IRQ lines are configured by this function.
            SPI driver is configured & started also here.
 */
static bool rfm22b_lld_init_io(RFM22BDriver *drv) {
    //
    consoleDebug("rfm22b_lld_init_io start\r\n");
    // spi cs
    rfm22b_spi_cfg.ssport = drv->config->cs_port;
    rfm22b_spi_cfg.sspad = drv->config->cs_pin;
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
    drv->nirq_cfg.cb = rfm22b_lld_nirq_handler;
    // configure ext channel used by radio
    chSysLock();
    extSetChannelModeI(drv->config->ext_drv,
                       drv->config->nirq_pin, &drv->nirq_cfg);
    chSysUnlock()
    extChannelEnable(drv->config->ext_drv,
                     drv->config->nirq_pin);
    // start spi driver
    spiStart(drv->config->spi_drv, &rfm22b_spi_cfg);
    //
    consoleDebug("rfm22b_lld_init_io end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static bool rfm22b_lld_init_module(RFM22BDriver *drv) {
    //
    systime_t timeout;
    //
    consoleDebug("rfm22b_lld_init_module start\r\n");
    // reset module
    rfm22b_lld_assert_rst(drv);
    chThdSleepMilliseconds(10);
    //
    rfm22b_lld_release_rst(drv);
    chThdSleepMilliseconds(10);
    // put module into sleep mode
    rfm22b_lld_write(drv, RFM22B_POWER_MANAGEMENT,
                RFM22B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT |
                RFM22B_POWER_MANAGEMENT_ENABLE_LOW_BATTERY_DETECTOR);
    // make sure we are in fifo mode
    rfm22b_lld_write(drv, RFM22B_TX_WRITE, 0);
    //
    if (palReadPad(drv->config->nirq_port, drv->config->nirq_pin) != 0) {
        consoleDebug("rfm22b_lld_init_module nIRQ high after hardware reset\r\n");
        return false;
    }
    //
    timeout = chTimeNow() + MS2ST(250);
    while (true) {
        if ((rfm22b_lld_status(drv) & RFM22B_STATUS_POWER_ON_RESET) != 0) {
            consoleDebug("rfm22b_lld_init_module power-on-reset bit set\r\n");
            break;
        } else if (timeout <= chTimeNow()) {
            consoleDebug("rfm22b_lld_init_module power-on-reset "
                         "timeout expired\r\n");
            return false;
        }
        consoleDebug("rfm22b_lld_init_module wait-loop\r\n");
        chThdSleepMilliseconds(5);
    }
    // nIRQ pin should be now high
    if (palReadPad(drv->config->nirq_port, drv->config->nirq_pin) == 0) {
        consoleDebug("rfm22b_lld_init_module nIRQ low after status read\r\n");
        return false;
    }
    // check module presence by setting high low-battery-detector threshold
    rfm22b_lld_write(drv, RFM22B_LOW_BATTERY_DETECTOR,
                     RFM22B_LOW_BATTERY_V37);
    // wait for detector to activate
    chThdSleepMilliseconds(5);
    if ((rfm22b_lld_status(drv) & RFM22B_STATUS_LOW_BATTERY) == 0) {
        consoleDebug("rfm22b_lld_init_module low-battery-detector "
                     "test failed\r\n");
        return false;
    }
    // restore low-battery-detector threshold
    rfm22b_lld_write(drv, RFM22B_LOW_BATTERY_DETECTOR,
                     RFM22B_LOW_BATTERY_V22);
    // put module into sleep mode
    rfm22b_lld_write(drv, RFM22B_POWER_MANAGEMENT,
                     RFM22B_POWER_MANAGEMENT_IDLE_MODE);
    // change driver state
    drv->state = RFM22B_STOP;
    //
    consoleDebug("rfm22b_lld_init_module end\r\n");
    return true;
}

/*
 * @brief   rfm22b ...
 */
static bool rfm22b_lld_init_setup(RFM22BDriver *drv) {
    // initialize rf module
    consoleDebug("rfm22b_lld_init_setup start\r\n");
    rfm22b_lld_write(drv, RFM22B_CONFIGURATION,
                     RFM22B_CONFIGURATION_BAND868_IDLE);
    rfm22b_lld_write(drv, RFM22B_FREQUENCY,
                     drv->config->base_freq);
    rfm22b_lld_write(drv, RFM22B_DATA_RATE,
                     drv->config->data_rate);
    rfm22b_lld_write(drv, RFM22B_RECEIVER_CONTROL, 0x0480);
    rfm22b_lld_write(drv, RFM22B_DATA_FILTER, 0x00AC);
    rfm22b_lld_write(drv, RFM22B_FIFO_RESET_MODE, 0X0083);
    rfm22b_lld_write(drv, RFM22B_SYNCHRON_PATTERN,
                     drv->config->group_id);
    rfm22b_lld_write(drv, RFM22B_AFC, 0x0083);
    rfm22b_lld_write(drv, RFM22B_TX_CONFIG, 0x0070);
    rfm22b_lld_write(drv, RFM22B_PLL, 0x0077);
    rfm22b_lld_write(drv, RFM22B_WAKEUP_TIMER, 0x0000);
    rfm22b_lld_write(drv, RFM22B_LOW_DUTY_CYCLE, 0x0000);
    rfm22b_lld_write(drv, RFM22B_LOW_BATTERY_DETECTOR, 0x0000);
    //
    chThdSleepMilliseconds(50);
    consoleDebug("rfm22b_lld_init_setup end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static void rfm22b_lld_nirq_handler(EXTDriver *extp, expchannel_t channel) {
    //
    RFM22BDriver *drv;
    (void)extp;
    uint32_t crc;
    //
    if (RFM22BD1.config->nirq_pin == channel) {
        drv = &RFM22BD1;
    } else {
        // no driver enabled/configured, ignore interrupt
        return;
    }
    if (lldLockISR(&drv->lock) == true) {
        switch (drv->state) {
            case RFM22B_UNINIT:
            case RFM22B_STOP:
            case RFM22B_IDLE:
            case RFM22B_RX_INIT:
                break;
            case RFM22B_RX_START:
                // reset CRC
                CRC->CR = (uint32_t)0x01;
                // initiate first read
                spiUnselectI(drv->config->spi_drv);
                drv->txrx_data = RFM22B_RX_READ;
                spiSelectI(drv->config->spi_drv);
                spiStartExchangeI(drv->config->spi_drv,
                                  1, &drv->txrx_data, &drv->txrx_data);
                // single packet reception should not take long
                // TODO make this timeout user configurable
                drv->txrx_counter = 0;
                drv->task_tmout = chTimeNow() + MS2ST(100);
                drv->state = RFM22B_RX_ACTIVE;
                break;
            case RFM22B_RX_ACTIVE:
                if (drv->txrx_counter < (sizeof(drv->txrx_buff) - 6)) {
                    // store previously read data
                    drv->txrx_buff[drv->txrx_counter] = drv->txrx_data & 0xFF;
                    // read next data
                    spiUnselectI(drv->config->spi_drv);
                    drv->txrx_data = RFM22B_RX_READ;
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
                    drv->state = RFM22B_RX_COMPLETE;
                }
                break;
            case RFM22B_RX_COMPLETE:
            case RFM22B_TX_INIT:
                break;
            case RFM22B_TX_ACTIVE:
                if (drv->txrx_counter < sizeof(drv->txrx_buff)) {
                    spiUnselectI(drv->config->spi_drv);
                    drv->txrx_data = RFM22B_TX_WRITE | drv->txrx_buff[drv->txrx_counter];
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
                    drv->state = RFM22B_TX_COMPLETE;
                }
                break;
            case RFM22B_TX_COMPLETE:
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
 * @brief   RFM22B thread.
 * @details This thread performs switch between different states
            of RFM22B module. It also executes defined callbacks.
 */
//__attribute__((noreturn))
static void RFM22BThread(void *arg) {
    // 
    uint32_t crc, *crc_ptr;
    RFM22BDriver *drv = (RFM22BDriver*)arg;
    rfm22b_callback_t cb = NULL;
    //
    chRegSetThreadName("RFM22BThread");
    consoleDebug("rfm22b start\r\n");
    //
    while (!chThdShouldTerminate()) {
        chBSemWaitTimeout(&drv->flag, MS2ST(5));
        cb = NULL;
        if (lldLockWaitTimeout(&drv->lock, 2) == true) {
            chSysLock();
            if (drv->error == true) {
                drv->state = RFM22B_IDLE;
                drv->error = false;
                chSysUnlock();
                consoleDebug("\r\n\r\n\r\n ********** recovery from ISR locking error **********\r\n\r\n\r\n");
            } else {
                chSysUnlock();
            }
            switch (drv->state) {
                case RFM22B_UNINIT:
                    consoleDebug("RFM22B_UNINIT\r\n");
                    break;
                case RFM22B_STOP:
                    consoleDebug("RFM22B_STOP\r\n");
                    drv->state = RFM22B_IDLE;
                    break;
                case RFM22B_IDLE:
                    // put radio into idle mode
                    rfm22b_lld_write(drv, RFM22B_CONFIGURATION,
                                     RFM22B_CONFIGURATION_BAND868_IDLE);
                    rfm22b_lld_write(drv, RFM22B_POWER_MANAGEMENT,
                                     RFM22B_POWER_MANAGEMENT_IDLE_MODE);
                    cb = drv->config->idle_cb;
                    consoleDebug("RFM22B_IDLE\r\n");
                    break;
                case RFM22B_RX_INIT:
                    //
                    if (drv->config->recv_timeout != 0) {
                        drv->task_tmout = chTimeNow() + MS2ST(drv->config->recv_timeout);
                    } else {
                        drv->task_tmout = 0;
                    }
                    // reconfigure RFM22B module
                    rfm22b_lld_write(drv, RFM22B_CONFIGURATION,
                                     RFM22B_CONFIGURATION_BAND868_RX);
                    rfm22b_lld_write(drv, RFM22B_POWER_MANAGEMENT,
                                     RFM22B_POWER_MANAGEMENT_RX_MODE);
                    // clear interrupts
                    rfm22b_lld_status(drv);
                    // reset fifo
                    rfm22b_lld_write(drv, RFM22B_FIFO_RESET_MODE, 0x0081);
                    rfm22b_lld_write(drv, RFM22B_FIFO_RESET_MODE, 0x0083);
                    // change driver state
                    drv->txrx_counter = 0;
                    chBSemSignal(&drv->flag);
                    consoleDebug("RFM22B_RX_START\r\n");
                    drv->state = RFM22B_RX_START;
                    break;
                case RFM22B_RX_START:
                case RFM22B_RX_ACTIVE:
                    if ((drv->task_tmout != 0) &&
                        (drv->task_tmout <= chTimeNow())) {
                            cb = drv->config->recv_error_cb;
                            chBSemSignal(&drv->flag);
                            consoleDebug("RFM22B_RX_START/ACTIVE: timeout %d\r\n", drv->txrx_counter);
                            drv->state = RFM22B_IDLE;
                            break;
                    }
                    break;
                case RFM22B_RX_COMPLETE:
                    // get received crc
                    crc_ptr = (uint32_t *)(drv->txrx_buff + sizeof(radio_packet_t));
                    // read crc, disable hardware module, check if crc correct
                    crc = CRC->DR;
                    if (crc != *crc_ptr) {
                        cb = drv->config->recv_error_cb;
                        chBSemSignal(&drv->flag);
                        consoleDebug("RFM22B_RX_COMPLETE: crc != crc_recv - %8X vs %8X\r\n", crc, *crc_ptr);
                        drv->state = RFM22B_IDLE;
                    } else {
                        cb = drv->config->recv_done_cb;
                        chBSemSignal(&drv->flag);
                        consoleDebug("RFM22B_RX_COMPLETE: crc ok\r\n");
                        drv->state = RFM22B_IDLE;
                    }
                    break;
                case RFM22B_TX_INIT:
                    //
                    if (drv->config->recv_timeout != 0) {
                        drv->task_tmout = chTimeNow() + MS2ST(drv->config->recv_timeout);
                    } else {
                        drv->task_tmout = 0;
                    }
                    //
                    CRC->CR = (uint32_t)0x01;
                    // clear el, et bits
                    rfm22b_lld_write(drv, RFM22B_CONFIGURATION,
                                     RFM22B_CONFIGURATION_BAND868_IDLE);
                    rfm22b_lld_write(drv, RFM22B_POWER_MANAGEMENT,
                                     RFM22B_POWER_MANAGEMENT_IDLE_MODE);
                    // clear interrupts
                    rfm22b_lld_status(drv);
                    // initialize driver state for tx
                    drv->txrx_counter = 0;
                    chBSemSignal(&drv->flag);
                    consoleDebug("RFM22B_TX_INIT\r\n");
                    drv->state = RFM22B_TX_ACTIVE;
                    // switch to tx mode
                    rfm22b_lld_write(drv, RFM22B_CONFIGURATION,
                                     RFM22B_CONFIGURATION_BAND868_TX);
                    rfm22b_lld_write(drv, RFM22B_POWER_MANAGEMENT,
                                     RFM22B_POWER_MANAGEMENT_TX_MODE);
                    //
                    break;
                case RFM22B_TX_ACTIVE:
                    if ((drv->task_tmout != 0) &&
                        (drv->task_tmout <= chTimeNow())) {
                            cb = drv->config->send_error_cb;
                            chBSemSignal(&drv->flag);
                            consoleDebug("RFM22B_TX_ACTIVE: timeout (%d)\r\n", drv->txrx_counter);
                            drv->state = RFM22B_IDLE;
                            break;
                    }
                    break;
                case RFM22B_TX_COMPLETE:
                    // run call back
                    cb = drv->config->send_done_cb;
                    chBSemSignal(&drv->flag);
                    consoleDebug("RFM22B_TX_COMPLETE\r\n");
                    drv->state = RFM22B_IDLE;
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
 * @brief   rfm22b ...
 */
inline void rfm22bRecvStartS(RFM22BDriver *drv, void *arg) {
    drv->state = RFM22B_RX_INIT;
    drv->arg = arg;
    chBSemSignal(&drv->flag);
}

/*
 * @brief   rfm22b ...
 */
bool rfm22bRecvStart(RFM22BDriver *drv, void *arg) {
    // make sure no-one will interrupt
    if (lldLockWaitTimeout(&drv->lock, 2) == true) {
        rfm22bRecvStartS(drv, arg);
        lldUnlock(&drv->lock);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   rfm22b ...
 */
inline void rfm22bSendStartS(RFM22BDriver *drv, radio_packet_t *packet, void *arg) {
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
    drv->state = RFM22B_TX_INIT;
    chBSemSignal(&drv->flag);
}

/*
 * @brief   rfm22b ...
 */
bool rfm22bSendStart(RFM22BDriver *drv, radio_packet_t *packet, void *arg) {
    // make sure no-one will interrupt now
    if (lldLockWaitTimeout(&drv->lock, 2) == true) {
        rfm22bSendStartS(drv, packet, arg);
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
bool rfm22bInit(RFM22BDriver *drv, RFM22BConfig *config, void *arg) {
    //
    consoleDebug("rfm22bInit start\r\n");
    //
    chSysLock();
    // driver synchronization variables
    chBSemInit(&drv->flag, 0);
    // driver structures
    drv->state = RFM22B_UNINIT;
    drv->config = config;
    drv->arg = arg;
    //
    chSysUnlock();
    //
    if (drv->config->ext_drv->state != EXT_ACTIVE) {
        consoleDebug("rfm22bInit EXT driver not started\r\n");
        return false;
    }
    // enable hardware crc32
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    //
    if (!rfm22b_lld_init_io(drv) ||
        !rfm22b_lld_init_module(drv) ||
        !rfm22b_lld_init_setup(drv)) {
        consoleWarn("rfm22bInit failed\r\n");
        return false;
    }
    //
    lldLock(&drv->lock);
    chThdCreateStatic(&drv->_wa, sizeof(drv->_wa),
                      NORMALPRIO + drv->config->priority,
                      (tfunc_t)RFM22BThread, drv);
    drv->state = RFM22B_STOP;
    lldUnlock(&drv->lock);
    //
    consoleDebug("rfm22bInit end\r\n");
    return true;
}

#endif /* ITHACA_USE_RADIO_RFM22B */

