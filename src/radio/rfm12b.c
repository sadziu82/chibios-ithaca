#include <ithaca.h>

#if ITHACA_USE_RADIO_RFM12B || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
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
 * @brief   ...
 * @details ...
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
    RFM12B_CONFIGURATION_BAND868 = 
        RFM12B_CONFIGURATION_FREQ_868 |
        RFM12B_CONFIGURATION_CAP_125PF,
    // combined settings
    RFM12B_CONFIGURATION_BAND868_RX = 
        RFM12B_CONFIGURATION_INTERNAL_DATA_REGISTER |
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
    // combined settings
    RFM12B_POWER_MANAGEMENT_IDLE_MODE =
        // TODO
        RFM12B_POWER_MANAGEMENT_ENABLE_RECEIVER |
        RFM12B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM12B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM12B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    RFM12B_POWER_MANAGEMENT_TX_MODE =
        RFM12B_POWER_MANAGEMENT_ENABLE_TRANSMITTER |
        RFM12B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM12B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM12B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
    RFM12B_POWER_MANAGEMENT_RX_MODE =
        RFM12B_POWER_MANAGEMENT_ENABLE_RECEIVER |
        RFM12B_POWER_MANAGEMENT_ENABLE_BASE_BAND_BLOCK |
        RFM12B_POWER_MANAGEMENT_ENABLE_SYNTHESIZER |
        RFM12B_POWER_MANAGEMENT_ENABLE_CRYSTAL_OSCILLATOR |
        RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT,
} rfm12b_power_management_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // combined settings
    RFM12B_LOW_BATTERY_V22 = 0x0000,
    RFM12B_LOW_BATTERY_V37 = 0x000F,
} rfm12b_low_battery_t;

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
RFM12BDriver RFM12BD1;

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
static SPIConfig rfm12b_spi_cfg = {
    .end_cb = NULL,
    .cr1 = SPI_CR1_MSTR | SPI_CR1_DFF |
           SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   rfm12b ...
 */
void rfm12b_lld_assert_rst(const RFM12BDriver *drv) {
    palClearPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   rfm12b ...
 */
void rfm12b_lld_release_rst(const RFM12BDriver *drv) {
    palSetPad(drv->config->rst_port, drv->config->rst_pin);
}

/*
 * @brief   rfm12b ...
 */
uint16_t rfm12b_lld_xfer(const RFM12BDriver *drv, const uint16_t cmd) {
    uint16_t data = 0;
    spiSelect(drv->config->spi_drv);
    spiExchange(drv->config->spi_drv, 1, &cmd, &data);
    spiUnselect(drv->config->spi_drv);
    return data;
}

/*
 * @brief   rfm12b ...
 */
void rfm12b_lld_write(const RFM12BDriver *drv,
                   const rfm12b_register_t reg, const uint16_t data) {
    rfm12b_lld_xfer(drv, reg | data);
}

/*
 * @brief   rfm12b ...
 */
void rfm12b_lld_start_write(const RFM12BDriver *drv,
                            const rfm12b_register_t reg, const uint16_t data) {
    rfm12b_lld_xfer(drv, reg | data);
}

/*
 * @brief   rfm12b ...
 */
uint8_t rfm12b_lld_read(const RFM12BDriver *drv) {
    //
    return (rfm12b_lld_xfer(drv, RFM12B_RX_READ) & 0xFF);
}

/*
 * @brief   rfm12b ...
 */
uint16_t rfm12b_lld_status(const RFM12BDriver *drv) {
    //
    return rfm12b_lld_xfer(drv, RFM12B_STATUS);
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
static void rfm12b_lld_nirq_handler(EXTDriver *extp,
                                    expchannel_t channel) {
    RFM12BDriver *drv;
    //
    (void)extp;
    //
    if ((RFM12BD1.state > RFM12B_UNINIT) &&
        (RFM12BD1.config->nirq_pin == channel)) {
        drv = &RFM12BD1;
    // no driver enabled/configured, ignore interrupt
    } else {
        return;
    }
    switch (drv->state) {
        case RFM12B_UNINIT:
        case RFM12B_STOP:
            return;
        case RFM12B_IDLE:
            break;
        case RFM12B_IDLE_RX:
            break;
        case RFM12B_ACTIVE_RX:
            if (drv->counter < (sizeof(drv->buffer) - 6)) {
                drv->buffer[drv->counter] = drv->txrx_data & 0xFF;
                spiUnselectI(drv->config->spi_drv);
                drv->txrx_data = RFM12B_RX_READ;
                spiSelectI(drv->config->spi_drv);
                spiStartExchangeI(drv->config->spi_drv,
                                  1, &drv->txrx_data, &drv->txrx_data);
                drv->counter++;
            } else {
                drv->state = RFM12B_IDLE;
                chSemSignalI(&drv->semaphore);
            }
            break;
        case RFM12B_ACTIVE_TX:
            if (drv->counter < sizeof(drv->buffer)) {
                spiUnselectI(drv->config->spi_drv);
                drv->txrx_data = RFM12B_TX_WRITE | drv->buffer[drv->counter];
                spiSelectI(drv->config->spi_drv);
                spiStartSendI(drv->config->spi_drv,
                              1, &drv->txrx_data);
                drv->counter++;
            } else {
                drv->state = RFM12B_IDLE;
                chSemSignalI(&drv->semaphore);
            }
            break;
    }
} 

/*
 * @brief   ...
 * @details ...
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
                     RFM12B_CONFIGURATION_BAND868);
    rfm12b_lld_write(drv, RFM12B_FREQUENCY,
                     drv->config->frequency);
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
    chThdSleepMilliseconds( 50 );
    consoleDebug("rfm12b_lld_init_setup end\r\n");
    //
    return true;
}

/*
 * @brief   rfm12b ...
 */
bool rfm12b_lld_idle(RFM12BDriver *drv) {
    // change drivar state
    drv->state = RFM12B_IDLE;
    // switch to sleep mode
    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_IDLE_MODE);
    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868);
    return true;
}

/*
 * @brief   rfm12b ...
 */
bool rfm12b_lld_send(RFM12BDriver *drv, radio_packet_t *packet) {
    //
    uint8_t i, j;
    uint32_t crc;
    //
    consoleDebug("rfm12b_lld_send start\r\n");
    // initialize hardware crc
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    //
    i = 0;
    drv->buffer[i++] = 0xAA;
    drv->buffer[i++] = 0xAA;
    drv->buffer[i++] = 0xAA;
    drv->buffer[i++] = 0x2D;
    drv->buffer[i++] = drv->config->group_id;
    drv->buffer[i++] = packet->dst;
    drv->buffer[i++] = packet->src;
    drv->buffer[i++] = packet->cmd;
    drv->buffer[i++] = packet->param;
    // reset crc32 and write first data
    CRC->CR = (uint32_t)0x01;
    CRC->DR = (packet->dst << 24) +
              (packet->src << 16) +
              (packet->cmd << 8) +
              packet->param;
#if RADIO_PACKET_DATA_SIZE > 0
    crc = 0;
    //
    for (j = 0; j < sizeof(packet->data); j++) {
        drv->buffer[i++] = packet->data[j];
        crc <<= 8;
        crc += packet->data[j];
        if (j % 3 == 0) {
            CRC->DR = crc;
        }
    }
#endif
    // read crc from hardware and disable crc module
    // XXX in case of crc errors please check this crc32 timing
    crc = CRC->DR;
    RCC->AHBENR &= ~RCC_AHBENR_CRCEN;
    // add crc to send data
    drv->buffer[i++] = (crc >> 24) & 0xFF;
    drv->buffer[i++] = (crc >> 16) & 0xFF;
    drv->buffer[i++] = (crc >> 8) & 0xFF;
    drv->buffer[i++] = crc & 0xFF;
    // send postamble
    drv->buffer[i++] = 0xAA;
    drv->buffer[i++] = 0xAA;
    drv->buffer[i++] = 0xAA;
    // start transmission
    rfm12b_lld_status(drv);
    chSemReset(&drv->semaphore, 0);
    drv->counter = 0;
    drv->state = RFM12B_ACTIVE_TX;
    // TODO frequency hopping
    //rfm12b_lld_write(drv, RFM12B_FREQUENCY,
    //                 drv->config->frequency + 0x100 * freq_chan);
    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_TX);
    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_TX_MODE);
    if (chSemWaitTimeout(&drv->semaphore, MS2ST(10)) != RDY_OK) {
        rfm12b_lld_idle(drv);
        consoleWarn("rfm12b_lld_send timeout sending data\r\n");
        return false;
    }
    consoleDebug("rfm12b_lld_send end\r\n");
    // switch to sleep mode
    rfm12b_lld_idle(drv);
    //
    return true;
}

/*
 * @brief   rfm12b ...
 */
bool rfm12b_lld_recv(RFM12BDriver *drv, radio_packet_t *packet) {
    //
    (void)drv;
    (void)packet;
    //
    uint32_t crc, crc_recv;
    uint8_t i, j;
    //
    consoleDebug("rfm12b_lld_recv start\r\n");
    // prepare variables
    chSemReset(&drv->semaphore, 0);
    drv->counter = 0;
    // clear all interrupts
    rfm12b_lld_status(drv);
    drv->state = RFM12B_ACTIVE_RX;
    // enter rx mode
    rfm12b_lld_write(drv, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_RX);
    rfm12b_lld_write(drv, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_RX_MODE);
    // reset fifo
    rfm12b_lld_write(drv, RFM12B_FIFO_RESET_MODE, 0x0081); // synchro 1-st
    rfm12b_lld_write(drv, RFM12B_FIFO_RESET_MODE, 0x0083); // synchro 1-st
    if (chSemWaitTimeout(&drv->semaphore,
                         MS2ST(drv->config->rx_timeout)) != RDY_OK) {
        rfm12b_lld_idle(drv);
        consoleWarn("rfm12b_lld_recv timeout\r\n");
        return false;
    }
    // enable crc32, timing is important here
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    // we start at one this time
    i = 1;
    // get values
    packet->dst = drv->buffer[i++];
    packet->src = drv->buffer[i++];
    packet->cmd = drv->buffer[i++];
    packet->param = drv->buffer[i++];
    // initialize hardware crc
    CRC->CR = (uint32_t)0x01;
    CRC->DR = (packet->dst << 24) +
              (packet->src << 16) +
              (packet->cmd << 8) +
              packet->param;
    crc = 0;
    //
    for (j = 0; j < sizeof(packet->data); j++ ) {
        packet->data[j] = drv->buffer[i++];
        // TODO calculate crc
        crc <<= 8;
        crc += packet->data[j];
        if (j % 3 == 0) {
            CRC->DR = crc;
        }
    }
    // get received CRC
    crc_recv = (drv->buffer[i++] << 24);
    crc_recv += (drv->buffer[i++] << 16);
    crc_recv += (drv->buffer[i++] << 8);
    crc_recv += drv->buffer[i++];
    // read crc from hardware and disable clock
    crc = CRC->DR;
    RCC->AHBENR &= ~RCC_AHBENR_CRCEN;
    //
    consoleDebug("DATA[0]: %2X, DATA[1]: %2X\r\n", packet->data[0], packet->data[1]);
    consoleDebug("CRC: %8X, CRC_RECV: %8X\r\n", crc, crc_recv);
    //
    if (crc != crc_recv) {
        rfm12b_lld_idle(drv);
        consoleWarn("rfm12b_lld_recv crc mismatch\r\n");
        return false;
    }
    consoleDebug("rfm12b_lld_recv end\r\n");
    rfm12b_lld_idle(drv);
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rfm12b_lld_init(RFM12BDriver *drv, RFM12BConfig *config) {
    //
    consoleDebug("rfm12b_lld_init start\r\n");
    //
    drv->state = RFM12B_UNINIT;
    drv->config = config;
    chSemInit(&drv->semaphore, 0);
    //
    if (drv->config->ext_drv->state != EXT_ACTIVE) {
        consoleDebug("rfm12b_lld_init EXT driver not started\r\n");
        return false;
    }
    //
    if (!rfm12b_lld_init_io(drv) ||
        !rfm12b_lld_init_module(drv) ||
        !rfm12b_lld_init_setup(drv)) {
        return false;
    }
    //
    consoleDebug("rfm12b_lld_init end\r\n");
    //
    return true;
}

#endif /* ITHACA_USE_RADIO_RFM12B */

