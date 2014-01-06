#include <ithaca.h>

#if ITHACA_USE_RADIO_RFM12B || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*
 * @brief   RFM12B register definitions.
 * @details This enum contains addresses for all RFM12B registers.
            They are ORed with data (in LSBs) when communicating.
 */
typedef enum {
    RFM12B_STATUS = 0x0000,
    RFM12B_CONFIGURATION = 0x8000,
    RFM12B_POWER_MANAGEMENT = 0x8200,
    RFM12B_FREQUENCY = 0xA000,
    RFM12B_DATA_RATE = 0xC600,
    RFM12B_RECEIVER_CONTROL = 0x9000,
    RFM12B_DATA_FILTER = 0xC200,
    RFM12B_FIFO_RESET_MODE = 0xCA00,
    RFM12B_SYNCHRON_PATTERN = 0xCE00,
    RFM12B_AFC = 0xC400,
    RFM12B_TX_CONFIG = 0x9800,
    RFM12B_PLL = 0xCC00,
    RFM12B_WAKEUP_TIMER = 0xE000,
    RFM12B_LOW_DUTY_CYCLE = 0xC800,
    RFM12B_TX_WRITE = 0xB800,
    RFM12B_RX_READ = 0xB000,
    RFM12B_LOW_BATTERY_DETECTOR = 0xC000,
} rfm12b_register_t;

/*
 * @brief   Status register bits.
 * @details Some bits valuer are repeated - it's only for readability.
            Depends on current mode they meaning might be changed.
 */
typedef enum {
    // single bits
    RFM12B_STATUS_TX_READY = 0x8000,
    RFM12B_STATUS_RX_READY = 0x8000,
    RFM12B_STATUS_POWER_ON_RESET = 0x4000,
    RFM12B_STATUS_TX_UNDERRUN = 0x2000,
    RFM12B_STATUS_RX_OVERFLOW = 0x2000,
    RFM12B_STATUS_WAKEUP = 0x1000,
    RFM12B_STATUS_EXT_INT = 0x0800,
    RFM12B_STATUS_LOW_BATTERY = 0x0400,
    RFM12B_STATUS_FIFO_EMPTY = 0x0200,
    RFM12B_STATUS_RSSI = 0x0100,
    RFM12B_STATUS_ATS = 0x0100,
} rfm12b_status_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // single bits
    RFM12B_CONFIGURATION_INTERNAL_DATA_REGISTER = 0x0080,
    RFM12B_CONFIGURATION_FIFO_MODE = 0x0040,
    RFM12B_CONFIGURATION_FREQ_868 = 0x0020,
    RFM12B_CONFIGURATION_CAP_125PF = 0x0008,
    // combined settings
    RFM12B_CONFIGURATION_BAND868_IDLE = 
        RFM12B_CONFIGURATION_FREQ_868 |
        RFM12B_CONFIGURATION_CAP_125PF,
    // combined settings
    RFM12B_CONFIGURATION_BAND868_RX = 
        RFM12B_CONFIGURATION_FIFO_MODE |
        RFM12B_CONFIGURATION_FREQ_868 |
        RFM12B_CONFIGURATION_CAP_125PF,
    // combined settings
    RFM12B_CONFIGURATION_BAND868_TX = 
        RFM12B_CONFIGURATION_INTERNAL_DATA_REGISTER |
        RFM12B_CONFIGURATION_FREQ_868 |
        RFM12B_CONFIGURATION_CAP_125PF,
} rfm12b_configuration_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // single bits
    RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT = 0x0001,
    RFM12B_POWER_MANAGEMENT_ENABLE_LOW_BATTERY_DETECTOR = 0x0004,
    RFM12B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR = 0x0008,
    RFM12B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER = 0x0010,
    RFM12B_POWER_MANAGEMENT_ENABLE_TRANSMITTER = 0x0020,
    RFM12B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK = 0x0040,
    RFM12B_POWER_MANAGEMENT_ENABLE_RECEIVER = 0x0080,
    // preset idle mode
    RFM12B_POWER_MANAGEMENT_IDLE_MODE =
        RFM12B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM12B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM12B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    // preset tx mode
    RFM12B_POWER_MANAGEMENT_TX_MODE =
        RFM12B_POWER_MANAGEMENT_ENABLE_TRANSMITTER |
        RFM12B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM12B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM12B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    // preset rx mode
    RFM12B_POWER_MANAGEMENT_RX_MODE =
        RFM12B_POWER_MANAGEMENT_ENABLE_RECEIVER |
        RFM12B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM12B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM12B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
} rfm12b_power_management_t;

/*
 * @brief   Low-voltage battery detector presets.
 * @details Low-voltage battery detecter is the only way to check
            if RFM12B module is present. Setting threshold lower than Vcc
            should activate IRQ RFM12B line.
 */
typedef enum {
    // low voltage detection presets
    RFM12B_LOW_BATTERY_V22 = 0x0000,
    RFM12B_LOW_BATTERY_V37 = 0x000F,
} rfm12b_low_battery_t;

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*
 * @brief   RFM12B driver one instance.
 */
RFM12BDriver RFM12BD1 = {
    .lock = {
        .name = "RFM12BD1",
        .flag = false,
    },
};

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   SPI configuration used by RFM12B.
 */
static SPIConfig rfm12b_spi_cfg = {
    .end_cb = NULL,
    .cr1 = SPI_CR1_MSTR | SPI_CR1_DFF |
           SPI_CR1_BR_1,
           //SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   RFM12B IRQ handler.
 */
static void rfm12b_lld_nirq_handler(EXTDriver *extp, expchannel_t channel);

/*
 * @brief   RFM12B RST line assert.
 */
void rfm12b_lld_assert_rst(const RFM12BDriver *drv) {
    palClearPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   RFM12B RST line release.
 */
void rfm12b_lld_release_rst(const RFM12BDriver *drv) {
    palSetPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   Exchange half-word data with RFM12B module.
 */
inline
uint16_t rfm12b_lld_xfer(const RFM12BDriver *drv, const uint16_t cmd) {
    uint16_t data = 0;
    spiSelect(drv->config->spi_drv);
    spiExchange(drv->config->spi_drv, 1, &cmd, &data);
    spiUnselect(drv->config->spi_drv);
    return data;
}

/*
 * @brief   Write one byte data to RFM12B register.
 */
void rfm12b_lld_write(const RFM12BDriver *drv,
                   const rfm12b_register_t reg, const uint16_t data) {
    rfm12b_lld_xfer(drv, reg | data);
}

/*
 * @brief   Read data from RFM12B register.
 * @details Only status register can be read.
            Others may return undefined results.
 */
uint8_t rfm12b_lld_read(const RFM12BDriver *drv) {
    //
    return (rfm12b_lld_xfer(drv, RFM12B_RX_READ) & 0xFF);
}

/*
 * @brief   Return RFM12B status.
 */
inline
uint16_t rfm12b_lld_status(const RFM12BDriver *drv) {
    //
    return rfm12b_lld_xfer(drv, RFM12B_STATUS);
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   Initialize IO ports used by RFM12B.
 * @details SPI ports (MISO,MOSI,CLK) should be set in proper mode
            by user. Also EXT driver must be started by user.
            RST, CE and IRQ lines are configured by this function.
            SPI driver is configured & started also here.
 */
static bool rfm12b_lld_init_io(RFM12BDriver *drv) {
    //
    consoleDebug("rfm12b_lld_init_io start\r\n");
    // spi cs
    rfm12b_spi_cfg.ssport = drv->config->cs_port;
    rfm12b_spi_cfg.sspad = drv->config->cs_pin;
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
    drv->nirq_cfg.cb = rfm12b_lld_nirq_handler;
    // configure ext channel used by radio
    chSysLock();
    extSetChannelModeI(drv->config->ext_drv,
                       drv->config->nirq_pin, &drv->nirq_cfg);
    chSysUnlock()
    extChannelEnable(drv->config->ext_drv,
                     drv->config->nirq_pin);
    // start spi driver
    spiStart(drv->config->spi_drv, &rfm12b_spi_cfg);
    //
    consoleDebug("rfm12b_lld_init_io end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static bool rfm12b_lld_init_module(RFM12BDriver *drv) {
    //
    systime_t timeout;
    //
    consoleDebug("rfm12b_lld_init_module start\r\n");
    // reset module
    rfm12b_lld_assert_rst(drv);
    chThdSleepMilliseconds(10);
    //
    rfm12b_lld_release_rst(drv);
    chThdSleepMilliseconds(10);
    // put module into sleep mode
    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT |
                RFM12B_POWER_MANAGEMENT_ENABLE_LOW_BATTERY_DETECTOR);
    // make sure we are in fifo mode
    rfm12b_lld_write(drv, RFM12B_TX_WRITE, 0);
    //
    if (palReadPad(drv->config->nirq_port, drv->config->nirq_pin) != 0) {
        consoleDebug("rfm12b_lld_init_module nIRQ high after hardware reset\r\n");
        return false;
    }
    //
    timeout = chTimeNow() + MS2ST(250);
    while (true) {
        if ((rfm12b_lld_status(drv) & RFM12B_STATUS_POWER_ON_RESET) != 0) {
            consoleDebug("rfm12b_lld_init_module power-on-reset bit set\r\n");
            break;
        } else if (timeout <= chTimeNow()) {
            consoleDebug("rfm12b_lld_init_module power-on-reset "
                         "timeout expired\r\n");
            return false;
        }
        consoleDebug("rfm12b_lld_init_module wait-loop\r\n");
        chThdSleepMilliseconds(5);
    }
    // nIRQ pin should be now high
    if (palReadPad(drv->config->nirq_port, drv->config->nirq_pin) == 0) {
        consoleDebug("rfm12b_lld_init_module nIRQ low after status read\r\n");
        return false;
    }
    // check module presence by setting high low-battery-detector threshold
    rfm12b_lld_write(drv, RFM12B_LOW_BATTERY_DETECTOR,
                     RFM12B_LOW_BATTERY_V37);
    // wait for detector to activate
    chThdSleepMilliseconds(5);
    if ((rfm12b_lld_status(drv) & RFM12B_STATUS_LOW_BATTERY) == 0) {
        consoleDebug("rfm12b_lld_init_module low-battery-detector "
                     "test failed\r\n");
        return false;
    }
    // restore low-battery-detector threshold
    rfm12b_lld_write(drv, RFM12B_LOW_BATTERY_DETECTOR,
                     RFM12B_LOW_BATTERY_V22);
    // put module into sleep mode
    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_IDLE_MODE);
    // change driver state
    drv->state = RFM12B_STOP;
    //
    consoleDebug("rfm12b_lld_init_module end\r\n");
    return true;
}

/*
 * @brief   rfm12b ...
 */
static bool rfm12b_lld_init_setup(RFM12BDriver *drv) {
    // initialize rf module
    consoleDebug("rfm12b_lld_init_setup start\r\n");
    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_IDLE);
    rfm12b_lld_write(drv, RFM12B_FREQUENCY,
                     drv->config->base_freq);
    rfm12b_lld_write(drv, RFM12B_DATA_RATE,
                     drv->config->data_rate);
    rfm12b_lld_write(drv, RFM12B_RECEIVER_CONTROL, 0x0480);
    rfm12b_lld_write(drv, RFM12B_DATA_FILTER, 0x00AC);
    rfm12b_lld_write(drv, RFM12B_FIFO_RESET_MODE, 0X0083);
    rfm12b_lld_write(drv, RFM12B_SYNCHRON_PATTERN,
                     drv->config->group_id);
    rfm12b_lld_write(drv, RFM12B_AFC, 0x0083);
    rfm12b_lld_write(drv, RFM12B_TX_CONFIG, 0x0070);
    rfm12b_lld_write(drv, RFM12B_PLL, 0x0077);
    rfm12b_lld_write(drv, RFM12B_WAKEUP_TIMER, 0x0000);
    rfm12b_lld_write(drv, RFM12B_LOW_DUTY_CYCLE, 0x0000);
    rfm12b_lld_write(drv, RFM12B_LOW_BATTERY_DETECTOR, 0x0000);
    //
    chThdSleepMilliseconds(50);
    consoleDebug("rfm12b_lld_init_setup end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static void rfm12b_lld_nirq_handler(EXTDriver *extp, expchannel_t channel) {
    //
    RFM12BDriver *drv;
    (void)extp;
    uint32_t crc;
    //
    if (RFM12BD1.config->nirq_pin == channel) {
        drv = &RFM12BD1;
    } else {
        // no driver enabled/configured, ignore interrupt
        return;
    }
    if (lldLockISR(&drv->lock) == true) {
        switch (drv->state) {
            case RFM12B_UNINIT:
            case RFM12B_STOP:
            case RFM12B_IDLE:
            case RFM12B_RX_INIT:
                break;
            case RFM12B_RX_START:
                // reset CRC
                CRC->CR = (uint32_t)0x01;
                // initiate first read
                spiUnselectI(drv->config->spi_drv);
                drv->txrx_data = RFM12B_RX_READ;
                spiSelectI(drv->config->spi_drv);
                spiStartExchangeI(drv->config->spi_drv,
                                  1, &drv->txrx_data, &drv->txrx_data);
                // single packet reception should not take long
                // TODO make this timeout user configurable
                drv->txrx_counter = 0;
                drv->task_tmout = chTimeNow() + MS2ST(100);
                drv->state = RFM12B_RX_ACTIVE;
                break;
            case RFM12B_RX_ACTIVE:
                if (drv->txrx_counter < (sizeof(drv->txrx_buff) - 6)) {
                    // store previously read data
                    drv->txrx_buff[drv->txrx_counter] = drv->txrx_data & 0xFF;
                    // read next data
                    spiUnselectI(drv->config->spi_drv);
                    drv->txrx_data = RFM12B_RX_READ;
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
                    drv->state = RFM12B_RX_COMPLETE;
                }
                break;
            case RFM12B_RX_COMPLETE:
            case RFM12B_TX_INIT:
                break;
            case RFM12B_TX_ACTIVE:
                if (drv->txrx_counter < sizeof(drv->txrx_buff)) {
                    spiUnselectI(drv->config->spi_drv);
                    drv->txrx_data = RFM12B_TX_WRITE | drv->txrx_buff[drv->txrx_counter];
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
                    drv->state = RFM12B_TX_COMPLETE;
                }
                break;
            case RFM12B_TX_COMPLETE:
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
 * @brief   RFM12B thread.
 * @details This thread performs switch between different states
            of RFM12B module. It also executes defined callbacks.
 */
//__attribute__((noreturn))
static void RFM12BThread(void *arg) {
    // 
    uint32_t crc, *crc_ptr;
    RFM12BDriver *drv = (RFM12BDriver*)arg;
    radio_callback_t cb = NULL;
    //
    chRegSetThreadName("RFM12BThread");
    consoleDebug("rfm12b start\r\n");
    //
    while (!chThdShouldTerminate()) {
        chBSemWaitTimeout(&drv->flag, MS2ST(5));
        cb = NULL;
        if (lldLockWaitTimeout(&drv->lock, 2) == true) {
            chSysLock();
            if (drv->error == true) {
                drv->state = RFM12B_IDLE;
                drv->error = false;
                chSysUnlock();
                consoleDebug("\r\n\r\n\r\n ********** recovery from ISR locking error **********\r\n\r\n\r\n");
            } else {
                chSysUnlock();
            }
            switch (drv->state) {
                case RFM12B_UNINIT:
                    consoleDebug("RFM12B_UNINIT\r\n");
                    break;
                case RFM12B_STOP:
                    consoleDebug("RFM12B_STOP\r\n");
                    drv->state = RFM12B_IDLE;
                    break;
                case RFM12B_IDLE:
                    // put radio into idle mode
                    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                                     RFM12B_CONFIGURATION_BAND868_IDLE);
                    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                                     RFM12B_POWER_MANAGEMENT_IDLE_MODE);
                    cb = drv->config->idle_cb;
                    consoleDebug("RFM12B_IDLE\r\n");
                    break;
                case RFM12B_RX_INIT:
                    //
                    if (drv->config->recv_timeout != 0) {
                        drv->task_tmout = chTimeNow() + MS2ST(drv->config->recv_timeout);
                    } else {
                        drv->task_tmout = 0;
                    }
                    // reconfigure RFM12B module
                    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                                     RFM12B_CONFIGURATION_BAND868_RX);
                    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                                     RFM12B_POWER_MANAGEMENT_RX_MODE);
                    // clear interrupts
                    rfm12b_lld_status(drv);
                    // reset fifo
                    rfm12b_lld_write(drv, RFM12B_FIFO_RESET_MODE, 0x0081);
                    rfm12b_lld_write(drv, RFM12B_FIFO_RESET_MODE, 0x0083);
                    // change driver state
                    drv->txrx_counter = 0;
                    chBSemSignal(&drv->flag);
                    consoleDebug("RFM12B_RX_START\r\n");
                    drv->state = RFM12B_RX_START;
                    break;
                case RFM12B_RX_START:
                case RFM12B_RX_ACTIVE:
                    if ((drv->task_tmout != 0) &&
                        (drv->task_tmout <= chTimeNow())) {
                            cb = drv->config->recv_error_cb;
                            chBSemSignal(&drv->flag);
                            consoleDebug("RFM12B_RX_START/ACTIVE: timeout %d\r\n", drv->txrx_counter);
                            drv->state = RFM12B_IDLE;
                            break;
                    }
                    break;
                case RFM12B_RX_COMPLETE:
                    // get received crc
                    crc_ptr = (uint32_t *)(drv->txrx_buff + sizeof(radio_packet_t));
                    // read crc, disable hardware module, check if crc correct
                    crc = CRC->DR;
                    if (crc != *crc_ptr) {
                        cb = drv->config->recv_error_cb;
                        chBSemSignal(&drv->flag);
                        consoleDebug("RFM12B_RX_COMPLETE: crc != crc_recv - %8X vs %8X\r\n", crc, *crc_ptr);
                        drv->state = RFM12B_IDLE;
                    } else {
                        cb = drv->config->recv_done_cb;
                        chBSemSignal(&drv->flag);
                        consoleDebug("RFM12B_RX_COMPLETE: crc ok\r\n");
                        drv->state = RFM12B_IDLE;
                    }
                    break;
                case RFM12B_TX_INIT:
                    //
                    if (drv->config->recv_timeout != 0) {
                        drv->task_tmout = chTimeNow() + MS2ST(drv->config->recv_timeout);
                    } else {
                        drv->task_tmout = 0;
                    }
                    //
                    CRC->CR = (uint32_t)0x01;
                    // clear el, et bits
                    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                                     RFM12B_CONFIGURATION_BAND868_IDLE);
                    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                                     RFM12B_POWER_MANAGEMENT_IDLE_MODE);
                    // clear interrupts
                    rfm12b_lld_status(drv);
                    // initialize driver state for tx
                    drv->txrx_counter = 0;
                    chBSemSignal(&drv->flag);
                    consoleDebug("RFM12B_TX_INIT\r\n");
                    drv->state = RFM12B_TX_ACTIVE;
                    // switch to tx mode
                    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                                     RFM12B_CONFIGURATION_BAND868_TX);
                    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                                     RFM12B_POWER_MANAGEMENT_TX_MODE);
                    //
                    break;
                case RFM12B_TX_ACTIVE:
                    if ((drv->task_tmout != 0) &&
                        (drv->task_tmout <= chTimeNow())) {
                            cb = drv->config->send_error_cb;
                            chBSemSignal(&drv->flag);
                            consoleDebug("RFM12B_TX_ACTIVE: timeout (%d)\r\n", drv->txrx_counter);
                            drv->state = RFM12B_IDLE;
                            break;
                    }
                    break;
                case RFM12B_TX_COMPLETE:
                    // run call back
                    cb = drv->config->send_done_cb;
                    chBSemSignal(&drv->flag);
                    consoleDebug("RFM12B_TX_COMPLETE\r\n");
                    drv->state = RFM12B_IDLE;
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
 * @brief   rfm12b ...
 */
inline void rfm12bRecvStartS(RFM12BDriver *drv, void *arg) {
    drv->state = RFM12B_RX_INIT;
    drv->arg = arg;
    chBSemSignal(&drv->flag);
}

/*
 * @brief   rfm12b ...
 */
bool rfm12bRecvStart(RFM12BDriver *drv, void *arg) {
    // make sure no-one will interrupt
    if (lldLockWaitTimeout(&drv->lock, 2) == true) {
        rfm12bRecvStartS(drv, arg);
        lldUnlock(&drv->lock);
        return true;
    } else {
        return false;
    }
}

/*
 * @brief   rfm12b ...
 */
inline void rfm12bSendStartS(RFM12BDriver *drv, radio_packet_t *packet, void *arg) {
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
    drv->state = RFM12B_TX_INIT;
    chBSemSignal(&drv->flag);
}

/*
 * @brief   rfm12b ...
 */
bool rfm12bSendStart(RFM12BDriver *drv, radio_packet_t *packet, void *arg) {
    // make sure no-one will interrupt now
    if (lldLockWaitTimeout(&drv->lock, 2) == true) {
        rfm12bSendStartS(drv, packet, arg);
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
bool rfm12bInit(RFM12BDriver *drv, RFM12BConfig *config, void *arg) {
    //
    consoleDebug("rfm12bInit start\r\n");
    //
    chSysLock();
    // driver synchronization variables
    chBSemInit(&drv->flag, 0);
    // driver structures
    drv->state = RFM12B_UNINIT;
    drv->config = config;
    drv->arg = arg;
    //
    chSysUnlock();
    //
    if (drv->config->ext_drv->state != EXT_ACTIVE) {
        consoleDebug("rfm12bInit EXT driver not started\r\n");
        return false;
    }
    // enable hardware crc32
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    //
    if (!rfm12b_lld_init_io(drv) ||
        !rfm12b_lld_init_module(drv) ||
        !rfm12b_lld_init_setup(drv)) {
        consoleWarn("rfm12bInit failed\r\n");
        return false;
    }
    //
    lldLock(&drv->lock);
    chThdCreateStatic(&drv->_wa, sizeof(drv->_wa),
                      NORMALPRIO + drv->config->priority,
                      (tfunc_t)RFM12BThread, drv);
    drv->state = RFM12B_STOP;
    lldUnlock(&drv->lock);
    //
    consoleDebug("rfm12bInit end\r\n");
    return true;
}

#endif /* ITHACA_USE_RADIO_RFM12B */

