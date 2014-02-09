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
    RFM12B_TXFIFO_WRITE = 0xB800,
    RFM12B_RXFIFO_READ = 0xB000,
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
 * @brief   ...
 * @details ...
 */
typedef enum {
    RFM12B_FIFO_RESET_MODE_CLEAR = 0x0081,
    RFM12B_FIFO_RESET_MODE_SET = 0x0083,
} rfm12b_reset_mode_t;

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

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   RFM12B IRQ handler.
 */
static void rfm12b_lld_nirq_handler(EXTDriver *extp, expchannel_t channel);
static void rfm12b_lld_spi_handler_rx(SPIDriver *spip);
static void rfm12b_lld_spi_handler_tx(SPIDriver *spip);

/*
 * @brief   RFM12B RST line assert.
 */
void rfm12b_lld_assert_rst(const RadioDriver *radio) {
    palClearPad(radio->config->lld_config.rfm12b->rst_port,
                radio->config->lld_config.rfm12b->rst_pin);
}

/*
 * @brief   RFM12B RST line release.
 */
void rfm12b_lld_release_rst(const RadioDriver *radio) {
    palSetPad(radio->config->lld_config.rfm12b->rst_port,
              radio->config->lld_config.rfm12b->rst_pin);
}

/*
 * @brief   Exchange half-word data with RFM12B module.
 */
uint16_t rfm12b_lld_xfer(const RadioDriver *radio, const uint16_t cmd) {
    static uint16_t data;
    data = 0x00;
    ////while (!((radio->lld_driver.rfm12b.state <= RFM12B_IDLE) ||
    ////         (radio->lld_driver.rfm12b.state == RFM12B_RX_COMPLETE) ||
    ////         (radio->lld_driver.rfm12b.state == RFM12B_TX_COMPLETE) ||
    ////         (radio->lld_driver.rfm12b.state >= RFM12B_RX_ERROR))) {
    ////    chThdYield();
    ////}
    spiSelect(radio->config->lld_config.rfm12b->spi_drv);
    spiExchange(radio->config->lld_config.rfm12b->spi_drv, 1, &cmd, &data);
    spiUnselect(radio->config->lld_config.rfm12b->spi_drv);
    return data;
}

/*
 * @brief   Write one byte data to RFM12B register.
 */
void rfm12b_lld_write(const RadioDriver *radio,
                      const rfm12b_register_t reg, const uint16_t data) {
    rfm12b_lld_xfer(radio, reg | data);
}

/*
 * @brief   Read data from RFM12B register.
 * @details Only status register can be read.
            Others may return undefined results.
 */
uint8_t rfm12b_lld_read(const RadioDriver *radio) {
    //
    return (rfm12b_lld_xfer(radio, RFM12B_RXFIFO_READ) & 0xFF);
}

/*
 * @brief   Return RFM12B status.
 */
inline
uint16_t rfm12b_lld_status(const RadioDriver *radio) {
    //
    return rfm12b_lld_xfer(radio, RFM12B_STATUS);
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
static bool rfm12b_lld_init_io(RadioDriver *radio) {
    //
    consoleDevel("rfm12b_lld_init_io() start\r\n");
    // spi
    radio->lld_driver.rfm12b.spi_cfg.end_cb = NULL;
    // max spi clock 2.5 MHz for RFM12B fifo read
    radio->lld_driver.rfm12b.spi_cfg.cr1 = SPI_CR1_MSTR | SPI_CR1_DFF | SPI_CR1_BR_1 | SPI_CR1_BR_0;
    radio->lld_driver.rfm12b.spi_cfg.ssport = radio->config->lld_config.rfm12b->cs_port;
    radio->lld_driver.rfm12b.spi_cfg.sspad = radio->config->lld_config.rfm12b->cs_pin;
    radio->config->lld_config.rfm12b->spi_drv->user_ptr = radio;
    // io ports
    palSetPadMode(radio->config->lld_config.rfm12b->cs_port,
                  radio->config->lld_config.rfm12b->cs_pin,
                  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(radio->config->lld_config.rfm12b->rst_port,
                  radio->config->lld_config.rfm12b->rst_pin,
                  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(radio->config->lld_config.rfm12b->nirq_port,
                  radio->config->lld_config.rfm12b->nirq_pin,
                  PAL_MODE_INPUT_PULLUP);
    // start spi driver
    spiStart(radio->config->lld_config.rfm12b->spi_drv,
             &radio->lld_driver.rfm12b.spi_cfg);
    // interupt handler
    radio->config->lld_config.rfm12b->ext_drv->user_ptr = radio;
    radio->lld_driver.rfm12b.nirq_cfg.mode = EXT_CH_MODE_FALLING_EDGE |
                                             radio->config->lld_config.rfm12b->ext_mode;
    radio->lld_driver.rfm12b.nirq_cfg.cb = rfm12b_lld_nirq_handler;
    // start ext channel
    extSetChannelMode(radio->config->lld_config.rfm12b->ext_drv,
                      radio->config->lld_config.rfm12b->nirq_pin,
                      &radio->lld_driver.rfm12b.nirq_cfg);
    extChannelEnable(radio->config->lld_config.rfm12b->ext_drv,
                     radio->config->lld_config.rfm12b->nirq_pin);
    //
    consoleDevel("rfm12b_lld_init_io() end\r\n");
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static bool rfm12b_lld_init_module(RadioDriver *radio) {
    //
    systime_t timeout;
    //
    consoleDevel("rfm12b_lld_init_module() start\r\n");
    // reset module
    rfm12b_lld_assert_rst(radio);
    chThdSleepMilliseconds(10);
    rfm12b_lld_release_rst(radio);
    chThdSleepMilliseconds(10);
    // put module into sleep mode
    rfm12b_lld_write(radio, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_DISABLE_CLOCK_OUTPUT |
                     RFM12B_POWER_MANAGEMENT_ENABLE_LOW_BATTERY_DETECTOR);
    //
    rfm12b_lld_write(radio, RFM12B_TXFIFO_WRITE, 0);
    //
    if (palReadPad(radio->config->lld_config.rfm12b->nirq_port,
                   radio->config->lld_config.rfm12b->nirq_pin) != 0) {
        consoleDevel("rfm12b_lld_init_module() nIRQ high after hardware reset\r\n");
        return false;
    }
    //
    timeout = chTimeNow() + MS2ST(250);
    while (true) {
        if ((rfm12b_lld_status(radio) & RFM12B_STATUS_POWER_ON_RESET) != 0) {
            consoleDevel("rfm12b_lld_init_module() power-on-reset bit set\r\n");
            break;
        } else if (timeout <= chTimeNow()) {
            consoleWarn("rfm12b_lld_init_module() power-on-reset "
                         "timeout expired\r\n");
            return false;
        }
        consoleDevel("rfm12b_lld_init_module wait-loop\r\n");
        chThdSleepMilliseconds(10);
    }
    // nIRQ pin should be now high
    if (palReadPad(radio->config->lld_config.rfm12b->nirq_port,
                   radio->config->lld_config.rfm12b->nirq_pin) == 0) {
        consoleWarn("rfm12b_lld_init_module() nIRQ low after status read\r\n");
        return false;
    }
    // check module presence by setting high low-battery-detector threshold
    rfm12b_lld_write(radio, RFM12B_LOW_BATTERY_DETECTOR,
                     RFM12B_LOW_BATTERY_V37);
    // wait for detector to activate
    chThdSleepMilliseconds(5);
    if ((rfm12b_lld_status(radio) & RFM12B_STATUS_LOW_BATTERY) == 0) {
        consoleWarn("rfm12b_lld_init_module() low-battery-detector "
                     "test failed\r\n");
        return false;
    }
    consoleDevel("rfm12b_lld_init_module() low-battery-detector test ok\r\n");
    // restore low-battery-detector threshold
    rfm12b_lld_write(radio, RFM12B_LOW_BATTERY_DETECTOR,
                     RFM12B_LOW_BATTERY_V22);
    // put module into sleep mode
    rfm12b_lld_write(radio, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_IDLE_MODE);
    //
    consoleDevel("rfm12b_lld_init_module() end\r\n");
    return true;
}

/*
 * @brief   rfm12b ...
 */
static bool rfm12b_lld_init_setup(RadioDriver *radio) {
    // initialize rf module
    consoleDevel("rfm12b_lld_init_setup() start\r\n");
    rfm12b_lld_write(radio, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_IDLE);
    rfm12b_lld_write(radio, RFM12B_FREQUENCY,
                     radio->config->lld_config.rfm12b->base_freq);
    rfm12b_lld_write(radio, RFM12B_DATA_RATE,
                     radio->config->lld_config.rfm12b->data_rate);
    rfm12b_lld_write(radio, RFM12B_RECEIVER_CONTROL, 0x0480);
    rfm12b_lld_write(radio, RFM12B_DATA_FILTER, 0x00AC);
    rfm12b_lld_write(radio, RFM12B_FIFO_RESET_MODE,
                     RFM12B_FIFO_RESET_MODE_SET);
    rfm12b_lld_write(radio, RFM12B_SYNCHRON_PATTERN,
                     radio->config->lld_config.rfm12b->syncword);
    rfm12b_lld_write(radio, RFM12B_AFC, 0x0083);
    rfm12b_lld_write(radio, RFM12B_TX_CONFIG, 0x0070);
    rfm12b_lld_write(radio, RFM12B_PLL, 0x0077);
    rfm12b_lld_write(radio, RFM12B_WAKEUP_TIMER, 0x0000);
    rfm12b_lld_write(radio, RFM12B_LOW_DUTY_CYCLE, 0x0000);
    rfm12b_lld_write(radio, RFM12B_LOW_BATTERY_DETECTOR, 0x0000);
    //
    chThdSleepMilliseconds(50);
    consoleDevel("rfm12b_lld_init_setup() end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool rfm12b_lld_is_error(RadioDriver *radio) {
    if ((radio->lld_driver.rfm12b.state == RFM12B_RX_ERROR) &&
        (radio->lld_driver.rfm12b.state == RFM12B_TX_ERROR)) {
        consoleDevel("rfm12b_lld_is_error() == true\r\n");
        return true;
    } else {
        consoleDevel("rfm12b_lld_is_error() == false\r\n");
        return false;
    }
}

/*
 * @brief   rfm12b send trigger
 * @details radio is locked - no need to lock here
 */
void rfm12b_lld_idle(RadioDriver *radio) {
    //
    consoleDevel("rfm12b_lld_idle() start\r\n");
    if (ithacaLockTimeout(&radio->lld_driver.rfm12b.lock, 5) == false) {
        consoleDevel("rfm12b_lld_idle() lock failed\r\n");
        return;
    }
    // clear el, et, er bits
    rfm12b_lld_write(radio, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_IDLE);
    rfm12b_lld_write(radio, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_IDLE_MODE);
    // clear interrupts
    rfm12b_lld_status(radio);
    // set idle state
    radio->lld_driver.rfm12b.state = RFM12B_IDLE;
    //
    ithacaUnlock(&radio->lld_driver.rfm12b.lock);
    consoleDevel("rfm12b_lld_idle() end\r\n");
}

/*
 * @brief   rfm12b receive trigger
 * @details radio is locked - no need to lock here
 */
void rfm12b_lld_receive_start(RadioDriver *radio) {
    //
    consoleDevel("rfm12b_lld_receive_start() start\r\n");
    if (ithacaLockTimeout(&radio->lld_driver.rfm12b.lock, 5) == false) {
        consoleDevel("rfm12b_lld_receive_start() lock failed\r\n");
        return;
    }
    // change driver state
    radio->lld_driver.rfm12b.state = RFM12B_IDLE;
    // switch to rx mode
    rfm12b_lld_write(radio, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_RX);
    rfm12b_lld_write(radio, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_RX_MODE);
    // clear interrupts
    rfm12b_lld_status(radio);
    // make sure hardware crc32 module is enabled
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    // reset fifo to start receive
    rfm12b_lld_write(radio, RFM12B_FIFO_RESET_MODE,
                     RFM12B_FIFO_RESET_MODE_CLEAR);
    rfm12b_lld_write(radio, RFM12B_FIFO_RESET_MODE,
                     RFM12B_FIFO_RESET_MODE_SET);
    // reset hardware crc32
    CRC->CR = (uint32_t)0x01;
    // change driver state
    radio->lld_driver.rfm12b.state = RFM12B_RX_START;
    //
    ithacaUnlock(&radio->lld_driver.rfm12b.lock);
    consoleDevel("rfm12b_lld_receive_start() end\r\n");
}

/*
 * @brief   rfm12b receive trigger
 * @details radio is locked - no need to lock here
 */
bool rfm12b_lld_receive_is_completed(RadioDriver *radio) {
    if (radio->lld_driver.rfm12b.state == RFM12B_RX_COMPLETE) {
        consoleDevel("rfm12b_lld_receive_is_completed() == true\r\n");
        consoleDebug("%8X vs %8X\r\n", radio->lld_driver.rfm12b.crc_self, radio->lld_driver.rfm12b.crc_recv);
        return true;
    } else {
        consoleDevel("rfm12b_lld_receive_is_completed() == false\r\n");
        return false;
    }
}

/*
 * @brief   rfm12b send trigger
 * @details radio is locked - no need to lock here
 */
void rfm12b_lld_send_start(RadioDriver *radio) {
    //
    consoleDevel("rfm12b_lld_send_start() start\r\n");
    if (ithacaLockTimeout(&radio->lld_driver.rfm12b.lock, 5) == false) {
        consoleDevel("rfm12b_lld_send_start() lock failed\r\n");
        return;
    }
    // clear el, et bits
    rfm12b_lld_write(radio, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_IDLE);
    rfm12b_lld_write(radio, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_IDLE_MODE);
    // clear interrupts
    rfm12b_lld_status(radio);
    // switch to tx mode
    rfm12b_lld_write(radio, RFM12B_CONFIGURATION,
                     RFM12B_CONFIGURATION_BAND868_TX);
    rfm12b_lld_write(radio, RFM12B_POWER_MANAGEMENT,
                     RFM12B_POWER_MANAGEMENT_TX_MODE);
    // change driver state
    radio->lld_driver.rfm12b.state = RFM12B_TX_START;
    //
    ithacaUnlock(&radio->lld_driver.rfm12b.lock);
    consoleDevel("rfm12b_lld_send_start() end\r\n");
}

/*
 * @brief   rfm12b receive trigger
 * @details radio is locked - no need to lock here
 */
bool rfm12b_lld_send_is_completed(RadioDriver *radio) {
    if (radio->lld_driver.rfm12b.state == RFM12B_TX_COMPLETE) {
        consoleDevel("rfm12b_lld_send_is_completed() == true\r\n");
        return true;
    } else {
        consoleDevel("rfm12b_lld_send_is_completed() == false\r\n");
        return false;
    }
}

/*
 * @brief   ...
 * @details ...
 */
static void rfm12b_lld_spi_handler_tx(SPIDriver *spip) {
    //
    RadioDriver *radio;
    //
    radio = spip->user_ptr;
    if (radio == NULL) {
        return;
    }
    // end spi transmission
    spiUnselectI(radio->config->lld_config.rfm12b->spi_drv);
    if (radio->lld_driver.rfm12b.spi_xfer_last == true) {
        radio->lld_driver.rfm12b.spi_cfg.end_cb = NULL;
        return;
    } else {
        radio->lld_driver.rfm12b.spi_xfer_last = true;
    }
    //
    if (ithacaLockISR(&radio->lld_driver.rfm12b.lock) == false) {
        return;
    }
    //
    if ((radio->lld_driver.rfm12b.state >= RFM12B_TX_START) &&
        (radio->lld_driver.rfm12b.state <= RFM12B_TX_COMPLETE)) {
        if (radio->lld_driver.rfm12b.txrx_data & RFM12B_STATUS_TX_UNDERRUN) {
            radio->packet.data[1]++;
        } else if (radio->lld_driver.rfm12b.txrx_data & RFM12B_STATUS_TX_READY) {
            radio->packet.data[0]++;
            switch (radio->lld_driver.rfm12b.state) {
                case RFM12B_TX_START:
                    radio->lld_driver.rfm12b.txrx_counter = 2;
                    radio->lld_driver.rfm12b.state = RFM12B_TX_PREAMBLE;
                    // no break here
                case RFM12B_TX_PREAMBLE:
                    radio->lld_driver.rfm12b.txrx_data = RFM12B_TXFIFO_WRITE | 0xAA;
                    if (++radio->lld_driver.rfm12b.txrx_counter >= radio->config->lld_config.rfm12b->preamble_length) {
                        radio->lld_driver.rfm12b.state = RFM12B_TX_SYNC1;
                    }
                    break;
                case RFM12B_TX_SYNC1:
                    // make sure hardware crc32 module is enabled
                    RCC->AHBENR |= RCC_AHBENR_CRCEN;
                    //
                    radio->lld_driver.rfm12b.txrx_data = RFM12B_TXFIFO_WRITE | 0x2D;
                    radio->lld_driver.rfm12b.state = RFM12B_TX_SYNC2;
                    break;
                case RFM12B_TX_SYNC2:
                    // reset hardware crc32
                    CRC->CR = (uint32_t)0x01;
                    //
                    radio->lld_driver.rfm12b.txrx_counter = 0;
                    radio->lld_driver.rfm12b.txrx_data = RFM12B_TXFIFO_WRITE | radio->config->lld_config.rfm12b->syncword;
                    radio->lld_driver.rfm12b.state = RFM12B_TX_DATA;
                    break;
                case RFM12B_TX_DATA:
                    // update crc data
                    CRC->DR = radio->packet.data[radio->lld_driver.rfm12b.txrx_counter];
                    // TODO write real data here
                    radio->lld_driver.rfm12b.txrx_data = RFM12B_TXFIFO_WRITE | radio->packet.data[radio->lld_driver.rfm12b.txrx_counter];
                    if (++radio->lld_driver.rfm12b.txrx_counter >= RADIO_PACKET_DATA_SIZE) {
                        radio->lld_driver.rfm12b.state = RFM12B_TX_CRC;
                        radio->lld_driver.rfm12b.txrx_counter = 0;
                        // read crc and disable module
                        radio->lld_driver.rfm12b.crc_self = CRC->DR;
                        RCC->AHBENR &= ~RCC_AHBENR_CRCEN;
                    }
                    break;
                case RFM12B_TX_CRC:
                    //
                    radio->lld_driver.rfm12b.txrx_data = RFM12B_TXFIFO_WRITE | ((radio->lld_driver.rfm12b.crc_self & 0xFF000000) >> 24);
                    radio->lld_driver.rfm12b.crc_self <<= 8;
                    if (++radio->lld_driver.rfm12b.txrx_counter >= sizeof(radio->lld_driver.rfm12b.crc_self)) {
                        radio->lld_driver.rfm12b.state = RFM12B_TX_POSTAMBLE;
                        radio->lld_driver.rfm12b.txrx_counter = 0;
                    }
                    break;
                case RFM12B_TX_POSTAMBLE:
                    radio->lld_driver.rfm12b.txrx_data = RFM12B_TXFIFO_WRITE | 0xAA;
                    if (++radio->lld_driver.rfm12b.txrx_counter >= radio->config->lld_config.rfm12b->postamble_length) {
                        radio->lld_driver.rfm12b.txrx_data = 0;
                        radio->lld_driver.rfm12b.state = RFM12B_TX_COMPLETE;
                    }
                    break;
                default:
                    radio->packet.data[1]++;
                    radio->lld_driver.rfm12b.txrx_data = 0;
                    break;
            }
            if (radio->lld_driver.rfm12b.txrx_data != 0) {
                spiSelectI(radio->config->lld_config.rfm12b->spi_drv);
                spiStartExchangeI(radio->config->lld_config.rfm12b->spi_drv,
                    1, &radio->lld_driver.rfm12b.txrx_data,
                    &radio->lld_driver.rfm12b.txrx_data);
            }
        } else {
            radio->lld_driver.rfm12b.txrx_data = RFM12B_STATUS;
            spiSelectI(radio->config->lld_config.rfm12b->spi_drv);
            spiStartExchangeI(radio->config->lld_config.rfm12b->spi_drv,
                1, &radio->lld_driver.rfm12b.txrx_data,
                &radio->lld_driver.rfm12b.txrx_data);
        }
    }
    ithacaUnlockISR(&radio->lld_driver.rfm12b.lock);
}

/*
 * @brief   ...
 * @details ...
 */
static void rfm12b_lld_spi_handler_rx(SPIDriver *spip) {
    //
    RadioDriver *radio;
    //
    radio = spip->user_ptr;
    if (radio == NULL) {
        return;
    }
    // end spi transmission
    spiUnselectI(radio->config->lld_config.rfm12b->spi_drv);
    if (ithacaLockISR(&radio->lld_driver.rfm12b.lock) == false) {
        return;
    }
    //
    if (radio->lld_driver.rfm12b.spi_xfer_last == true) {
        radio->lld_driver.rfm12b.spi_cfg.end_cb = NULL;
        switch (radio->lld_driver.rfm12b.state) {
            case RFM12B_RX_START:
            case RFM12B_RX_WAIT:
                // update crc data
                CRC->DR = radio->lld_driver.rfm12b.txrx_data & 0xFF;
                //
                radio->packet.data[0] = radio->lld_driver.rfm12b.txrx_data & 0xFF;
                radio->lld_driver.rfm12b.txrx_counter = 1;
                radio->lld_driver.rfm12b.state = RFM12B_RX_DATA;
                radio->lld_driver.rfm12b.txrx_data = RFM12B_RXFIFO_READ;
                break;
            case RFM12B_RX_DATA:
                // update crc data
                CRC->DR = radio->lld_driver.rfm12b.txrx_data & 0xFF;
                //
                radio->packet.data[radio->lld_driver.rfm12b.txrx_counter] = radio->lld_driver.rfm12b.txrx_data & 0xFF;
                radio->lld_driver.rfm12b.txrx_data = RFM12B_RXFIFO_READ;
                if (++radio->lld_driver.rfm12b.txrx_counter >= RADIO_PACKET_DATA_SIZE) {
                    radio->lld_driver.rfm12b.txrx_counter = 0;
                    radio->lld_driver.rfm12b.state = RFM12B_RX_CRC;
                    // store crc and disable hardware module
                    radio->lld_driver.rfm12b.crc_self = CRC->DR;
                    RCC->AHBENR &= ~RCC_AHBENR_CRCEN;
                }
                break;
            case RFM12B_RX_CRC:
                //radio->packet.data[radio->lld_driver.rfm12b.txrx_counter] = radio->lld_driver.rfm12b.txrx_data;
                radio->lld_driver.rfm12b.crc_recv <<= 8;
                radio->lld_driver.rfm12b.crc_recv |= radio->lld_driver.rfm12b.txrx_data & 0xFF;
                radio->lld_driver.rfm12b.txrx_data = RFM12B_RXFIFO_READ;
                if (++radio->lld_driver.rfm12b.txrx_counter >= sizeof(radio->lld_driver.rfm12b.crc_self)) {
                    radio->lld_driver.rfm12b.txrx_counter = 0;
                    radio->lld_driver.rfm12b.state = RFM12B_RX_POSTAMBLE;
                }
                break;
            case RFM12B_RX_POSTAMBLE:
                //radio->packet.data[radio->lld_driver.rfm12b.txrx_counter] = radio->lld_driver.rfm12b.txrx_data;
                radio->lld_driver.rfm12b.txrx_data = RFM12B_RXFIFO_READ;
                if (++radio->lld_driver.rfm12b.txrx_counter >= radio->config->lld_config.rfm12b->postamble_length) {
                    radio->lld_driver.rfm12b.txrx_counter = 0;
                    radio->lld_driver.rfm12b.state = RFM12B_RX_COMPLETE;
                }
                break;
            default:
                radio->lld_driver.rfm12b.txrx_data = 0;
                break;
        }
    } else {
        // rfm12b status in txrx_data
        if (radio->lld_driver.rfm12b.txrx_data & RFM12B_STATUS_RX_READY) {
            radio->lld_driver.rfm12b.txrx_data = RFM12B_RXFIFO_READ;
            radio->lld_driver.rfm12b.spi_xfer_last = true;
        } else {
            radio->lld_driver.rfm12b.txrx_data = RFM12B_STATUS;
        }
        spiSelectI(radio->config->lld_config.rfm12b->spi_drv);
        spiStartExchangeI(radio->config->lld_config.rfm12b->spi_drv,
            1, &radio->lld_driver.rfm12b.txrx_data,
            &radio->lld_driver.rfm12b.txrx_data);
    }
    ithacaUnlockISR(&radio->lld_driver.rfm12b.lock);
}

/*
 * @brief   ...
 * @details ...
 */
static void rfm12b_lld_nirq_handler(EXTDriver *extp, expchannel_t channel) {
    //
    (void)channel;
    RadioDriver *radio;
    //
    radio = extp->user_ptr;
    if (radio == NULL) {
        return;
    }
    if (ithacaLockISR(&radio->lld_driver.rfm12b.lock) == false) {
        return;
    }
    //
    if (((radio->lld_driver.rfm12b.state >= RFM12B_RX_START) &&
         (radio->lld_driver.rfm12b.state < RFM12B_RX_COMPLETE)) ||
        ((radio->lld_driver.rfm12b.state >= RFM12B_TX_START) &&
         (radio->lld_driver.rfm12b.state < RFM12B_TX_COMPLETE))) {
        if ((radio->lld_driver.rfm12b.state >= RFM12B_TX_START) &&
            (radio->lld_driver.rfm12b.state < RFM12B_TX_COMPLETE)) {
            radio->lld_driver.rfm12b.spi_cfg.end_cb = rfm12b_lld_spi_handler_tx;
        } else {
            radio->lld_driver.rfm12b.spi_cfg.end_cb = rfm12b_lld_spi_handler_rx;
        }
        radio->lld_driver.rfm12b.spi_xfer_last = false;
        radio->lld_driver.rfm12b.txrx_data = RFM12B_STATUS;
        spiSelectI(radio->config->lld_config.rfm12b->spi_drv);
        spiStartExchangeI(radio->config->lld_config.rfm12b->spi_drv,
            1, &radio->lld_driver.rfm12b.txrx_data,
            &radio->lld_driver.rfm12b.txrx_data);
    }
    ithacaUnlockISR(&radio->lld_driver.rfm12b.lock);
}

/*
 * @brief   ...
 * @details ...
 */
bool rfm12b_lld_init(RadioDriver *radio) {
    //
    consoleDevel("rfm12b_lld_init() start\r\n");
    if (ithacaLockTimeout(&radio->lld_driver.rfm12b.lock, 5) == false) {
        consoleDevel("rfm12b_lld_init() lock failed\r\n");
        return false;
    }
    radio->lld_driver.rfm12b.state = RFM12B_UNINIT;
    //
    if (radio->config->lld_config.rfm12b->ext_drv->state != EXT_ACTIVE) {
        ithacaUnlock(&radio->lld_driver.rfm12b.lock);
        consoleDevel("rfm12b_lld_init() EXT driver not started\r\n");
        return false;
    }
    if (radio->config->lld_config.rfm12b->preamble_length < 3) {
        radio->config->lld_config.rfm12b->preamble_length = 3;
    }
    if (radio->config->lld_config.rfm12b->postamble_length < 1) {
        radio->config->lld_config.rfm12b->postamble_length = 1;
    }
    //
    if (!rfm12b_lld_init_io(radio) ||
        !rfm12b_lld_init_module(radio) ||
        !rfm12b_lld_init_setup(radio)) {
        ithacaUnlock(&radio->lld_driver.rfm12b.lock);
        consoleWarn("rfm12b_lld_init() failed\r\n");
        return false;
    }
    radio->lld_driver.rfm12b.state = RFM12B_STOP;
    //
    ithacaUnlock(&radio->lld_driver.rfm12b.lock);
    consoleDevel("rfm12b_lld_init() end\r\n");
    return true;
}

#endif /* ITHACA_USE_RADIO_RFM12B */

