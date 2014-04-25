#include <ithaca.hpp>

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
 * @brief   Acquire SPI bus and assert ChipSelect signal for ST7735 controller.
 */
inline
void RadioRFM12B::_assertCS(void) {
    spiAcquireBus(this->_spi_drv);
    palClearPad(this->_cs_port, this->_cs_pin);
};

/*
 * @brief   Release ChipSelect signal from ST7735 controller and release SPI bus.
 */
inline
void RadioRFM12B::_releaseCS(void) {
    palSetPad(this->_cs_port, this->_cs_pin);
    spiReleaseBus(this->_spi_drv);
};

/*
 * @brief   Exchange half-word data with RFM12B module.
 */
inline
uint16_t RadioRFM12B::_xfer(const uint16_t cmd) {
    uint16_t data;
    data = 0x00;
    this->_assertCS();
    spiExchange(this->_spi_drv, 1, &cmd, &data);
    this->_releaseCS();
    return data;
}

/*
 * @brief   Write one byte data to RFM12B register.
 */
inline 
void RadioRFM12B::_write(Register reg, const uint16_t data) {
    RadioRFM12B::_xfer(static_cast<uint16_t>(reg) | data);
}

/*
 * @brief   Read data from RFM12B register.
 * @details Only status register can be read.
            Others may return undefined results.
 */
inline
uint8_t RadioRFM12B::_read(void) {
    //
    return (RadioRFM12B::_xfer(static_cast<uint16_t>(Register::RxFifoRead)) & 0xFF);
}

/*
 * @brief   Return RFM12B status.
 */
inline
uint16_t RadioRFM12B::_status(void) {
    //
    return RadioRFM12B::_xfer(static_cast<uint16_t>(Register::Status));
}

/*
 * @brief   Return RFM12B status.
 */
void RadioRFM12B::nIRQ(EXTDriver *extp, expchannel_t channel) {
    RadioRFM12B *radio = (RadioRFM12B *)extp->user_ptr[channel];
    radio->wakeup();
}

/*
 * @brief   Return RFM12B nIRQ wakeup routine.
 */
void RadioRFM12B::wakeup(void) {
    chSysLockFromIsr();
    // if semaprhore is taken, signal waiting thread
    chSemSignalI(&this->_radio_semaphore);
    //tmStartMeasurement(&this->tm);
    //chDbgPanic("wakeup when no one is listening");
    chSysUnlockFromIsr();
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
RadioPacket::RadioPacket(void) {
    //
    this->_raw_data = NULL;
    //
    consoleDebug("RadioPacket() created\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
RadioPacket::RadioPacket(uint8_t *raw_data): RadioPacket() {
    //
    this->setRawData(raw_data);
}

/*
 * @brief   ...
 * @details ...
 */
uint8_t RadioPacket::rawDataLength(void) {
    if (this->_raw_data == NULL) {
        return 0;
    }
    //
    return this->_raw_data[static_cast<uint8_t>(RawData::PayloadLengthHeaderOffset)] + static_cast<uint8_t>(RawData::HeadersLength);
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::setRawDataLength(uint8_t length) {
    //
    if ((length < static_cast<uint8_t>(RawData::HeadersLength)) ||
        (length + static_cast<uint8_t>(RawData::HeadersLength) > static_cast<uint8_t>(RawData::MaxLength))) {
        consoleWarn("setRawDataLength() failed to set length %d (+%d hdr)\r\n",
                    length, RawData::HeadersLength);
        return;
    }
    //
    if (this->_raw_data != NULL) {
        // release memory
        chHeapFree(this->_raw_data);
        //
        this->_raw_data = NULL;
    }
    // try to allocate memory
    this->_raw_data = (uint8_t *)chHeapAlloc(NULL, length + static_cast<uint8_t>(RawData::HeadersLength));
    // set correct length if memory allocated
    if (this->_raw_data != NULL) {
        this->_raw_data[static_cast<uint8_t>(RawData::TypeGroupIdHeaderOffset)] = static_cast<uint8_t>(Type::Empty);
        this->_raw_data[static_cast<uint8_t>(RawData::PayloadLengthHeaderOffset)] = length;
        // fill with 0x55 by default
        memset(this->_raw_data + static_cast<uint8_t>(RawData::HeadersLength), 0x55, length);
        consoleDebug("setRawDataLength() allocated _raw_data buffer, headers: %d, length: %d\r\n", static_cast<uint8_t>(RawData::HeadersLength), length);
    } else {
        consoleDebug("setRawDataLength() failed\r\n");
    }
}

/*
 * @brief   ...
 * @details ...
 */
RadioPacket::Type RadioPacket::type(void) {
    if (this->_raw_data != NULL) {
        return static_cast<Type>(this->_raw_data[static_cast<uint8_t>(RawData::TypeGroupIdHeaderOffset)] & static_cast<uint8_t>(RawData::TypeMask));
    } else {
        return Type::Empty;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::setType(Type type) {
    if (this->_raw_data != NULL) {
        this->_raw_data[static_cast<uint8_t>(RawData::TypeGroupIdHeaderOffset)] &= ~static_cast<uint8_t>(RawData::TypeMask);
        this->_raw_data[static_cast<uint8_t>(RawData::TypeGroupIdHeaderOffset)] |= static_cast<uint8_t>(type);
    }
}

/*
 * @brief   ...
 * @details ...
 */
uint8_t RadioPacket::groupId(void) {
    if (this->_raw_data != NULL) {
        return this->_raw_data[static_cast<uint8_t>(RawData::TypeGroupIdHeaderOffset)] & static_cast<uint8_t>(RawData::GroupIdMask);
    } else {
        return 0;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::setGroupId(uint8_t group_id) {
    if (this->_raw_data != NULL) {
        this->_raw_data[static_cast<uint8_t>(RawData::TypeGroupIdHeaderOffset)] &= ~static_cast<uint8_t>(RawData::GroupIdMask);
        this->_raw_data[static_cast<uint8_t>(RawData::TypeGroupIdHeaderOffset)] |= static_cast<uint8_t>(RawData::GroupIdMask) & group_id;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::getRawData(uint8_t *raw_data) {
    if (this->_raw_data != NULL) {
        memcpy(raw_data, this->_raw_data, this->rawDataLength());
    }
}

/*
 * @brief   ...
 * @details ...
 */
uint32_t RadioPacket::getRawCrc32(void) {
    if (this->_raw_data != NULL) {
        return ithacaCRC32(this->_raw_data, this->rawDataLength());
    }
    return 0;
}

/*
 * @brief   ...
 * @details ...
 */
void RadioPacket::setRawData(uint8_t *raw_data) {
    //
    uint8_t length = raw_data[static_cast<uint8_t>(RawData::PayloadLengthHeaderOffset)] & static_cast<uint8_t>(RawData::LengthMask);
    this->setRawDataLength(length);
    if (this->_raw_data != NULL) {
        memcpy(this->_raw_data, raw_data, this->rawDataLength());
    }
}

/*
 * @brief   ...
 * @details ...
 */
uint8_t &RadioPacket::operator[](uint8_t idx) {
    //
    if (idx >= this->rawDataLength()) {
        return this->_raw_trash;
    } else {
        return this->_raw_data[idx + static_cast<uint8_t>(RawData::HeadersLength)];
    }
}

/*
 * @brief   ...
 * @details ...
 */
Radio::Radio(void) {
}

/*
 * @brief   ...
 * @details ...
 */
bool Radio::init(void) {
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool Radio::send(RadioPacket *packet, systime_t tmout) {
    return false;
}

/*
 * @brief   ...
 * @details ...
 */
bool Radio::recv(RadioPacket *packet, systime_t tmout) {
    return false;
}

/*
 * @brief   ...
 * @details ...
 */
RadioRFM12B::RadioRFM12B(EXTDriver *ext_drv, uint32_t ext_ch_mode,
                         SPIDriver *spi_drv,
                         ioportid_t cs_port, uint16_t cs_pin,
                         ioportid_t nirq_port, uint16_t nirq_pin,
                         ioportid_t rst_port, uint16_t rst_pin): Radio() {
    //
    this->_busy.name = "RadioPFM12B";
    this->_busy.flag = false;
    //
    this->_ext_drv = ext_drv;
    this->_ext_ch_mode = ext_ch_mode;
    this->_spi_drv = spi_drv;
    this->_cs_port = cs_port;
    this->_cs_pin = cs_pin;
    this->_nirq_port = nirq_port;
    this->_nirq_pin = nirq_pin;
    this->_rst_port = rst_port;
    this->_rst_pin = rst_pin;
    //
    this->_syncword = 0xD4;
    this->_preamble_length = 4;
    this->_postamble_length = 2;
    this->_base_freq = Frequency::f868;
    //this->_freq_step = 0;
    this->_data_rate = DataRate::b4800;
    // init semaphore
    chSemInit(&this->_radio_semaphore, false);
    this->_thread_ptr = chThdSelf();
    tmObjectInit(&this->tm);
}

/*
 * @brief   ...
 * @details ...
 */
bool RadioRFM12B::init(void) {
    //
    consoleDebug("RadioRFM12B::init() start\r\n");
    // io ports
    palSetPadMode(this->_cs_port, this->_cs_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(this->_nirq_port, this->_nirq_pin, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(this->_rst_port, this->_rst_pin, PAL_MODE_OUTPUT_PUSHPULL);
    // interupt handler configuration
    this->_ext_drv->user_ptr[this->_nirq_pin] = this;
    this->_ext_ch_cfg.mode = EXT_CH_MODE_FALLING_EDGE | this->_ext_ch_mode;
    this->_ext_ch_cfg.cb = &RadioRFM12B::nIRQ;
    // start ext channel
    extSetChannelMode(this->_ext_drv, this->_nirq_pin, &this->_ext_ch_cfg);
    extChannelEnable(this->_ext_drv, this->_nirq_pin);
    //
    systime_t timeout;
    // reset module
    this->_assertRST();
    chThdSleepMilliseconds(10);
    //// XXX // nIRQ pin should be now high
    //// XXX if (palReadPad(this->_nirq_port, this->_nirq_pin) == 0) {
    //// XXX     consoleWarn("RadioRFM12B::init() nIRQ low under reset\r\n");
    //// XXX     return false;
    //// XXX }
    //
    this->_releaseRST();
    chThdSleepMilliseconds(10);
    // put module into sleep mode
    this->_write(Register::PowerManagement,
                 static_cast<uint16_t>(PowerManagement::DisableClockOutput) |
                 static_cast<uint16_t>(PowerManagement::EnableLowBatteryDetector));
    this->_write(Register::TxFifoWrite, 0);
    //
    if (palReadPad(this->_nirq_port, this->_nirq_pin) != 0) {
        consoleDebug("RadioRFM12B::init() nIRQ high after hardware reset\r\n");
        return false;
    }
    //
    timeout = chTimeNow() + MS2ST(500);
    while (true) {
        uint16_t status = this->_status();
        if ((status & static_cast<uint16_t>(Status::PowerOnReset)) != 0) {
            consoleDebug("RadioRFM12B::init() power-on-reset bit set\r\n");
            break;
        } else if (timeout <= chTimeNow()) {
            consoleWarn("RadioRFM12B::init() power-on-reset "
                         "timeout expired\r\n");
            return false;
        }
        consoleDevel("RadioRFM12B::init wait-loop\r\n");
        chThdSleepMilliseconds(10);
    }
    // nIRQ pin should be now high
    if (palReadPad(this->_nirq_port, this->_nirq_pin) == 0) {
        consoleWarn("RadioRFM12B::init() nIRQ low after status read: 0x%.4x\r\n", this->_status());
        return false;
    }
    // check module presence by setting high low-battery-detector threshold
    this->_write(Register::LowBatteryDetector, static_cast<uint16_t>(LowBattery::V37));
    // wait for detector to activate
    chThdSleepMilliseconds(5);
    if ((this->_status() & static_cast<uint16_t>(Status::LowBattery)) == 0) {
        consoleWarn("RadioRFM12B::init() low-battery-detector "
                     "test failed\r\n");
        return false;
    }
    consoleDebug("RadioRFM12B::init() low-battery-detector test ok\r\n");
    // restore low-battery-detector threshold and put module to sleep
    this->_write(Register::LowBatteryDetector, static_cast<uint16_t>(LowBattery::V22));
    this->_write(Register::PowerManagement, static_cast<uint16_t>(PowerManagement::IdleMode));
    consoleDebug("RadioRFM12B::init() success\r\n");
    return true;
}

/*
 * @brief   rfm12b ...
 */
bool RadioRFM12B::configure(uint8_t sw, Frequency bf, DataRate dr) {
    // initialize rf module
    this->_syncword = sw;
    this->_base_freq = bf;
    this->_data_rate = dr;
    //
    consoleDevel("RadioRFM12B::configure() start\r\n");
    this->_write(Register::Configuration, static_cast<uint16_t>(Configuration::Band868Idle));
    this->_write(Register::Frequency, static_cast<uint16_t>(this->_base_freq));
    this->_write(Register::DataRate, static_cast<uint16_t>(this->_data_rate));
    this->_write(Register::ReceiverControl, 0x0480);
    this->_write(Register::DataFilter, 0x00AC);
    this->_write(Register::FifoResetMode, static_cast<uint16_t>(FifoReset::Set));
    this->_write(Register::SynchronPattern, this->_syncword);
    this->_write(Register::Afc, 0x0083);
    this->_write(Register::TxConfig, 0x0070);
    this->_write(Register::Pll, 0x0077);
    this->_write(Register::WakeupTimer, 0x0000);
    this->_write(Register::LowDutyCycle, 0x0000);
    this->_write(Register::LowBatteryDetector, 0x0000);
    //
    chThdSleepMilliseconds(50);
    consoleDevel("RadioRFM12B::configure() end\r\n");
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool RadioRFM12B::send(RadioPacket *packet, systime_t tmout) {
    //
    systime_t timeout = chTimeNow() + MS2ST(tmout);
    //
    consoleDebug("RadioRFM12B::send() start\r\n");
    while (ithacaLock(&this->_busy) == false) {
        if (chTimeNow() < timeout) {
            consoleDebug("RadioRFM12B::send() lock failed\r\n");
            return false;
        }
    }
    // prepare data for sending
    uint32_t crc = packet->getRawCrc32();
    uint16_t status = 0;
    uint8_t idx = 0;
    uint8_t buffer_len = 4 + 2 + packet->rawDataLength() + 4 + 2;
    uint8_t *buffer = (uint8_t *)chHeapAlloc(NULL, buffer_len);
    // pre-amble
    buffer[0] = 0xAA;
    buffer[1] = 0xAA;
    buffer[2] = 0xAA;
    buffer[3] = 0xAA;
    // synchro
    buffer[4] = 0x2D;
    buffer[5] = this->_syncword;
    // data
    packet->getRawData(&buffer[6]);
    // crc
    buffer[buffer_len - 6] = (crc & 0xFF000000) >> 24;
    buffer[buffer_len - 5] = (crc & 0x00FF0000) >> 16;
    buffer[buffer_len - 4] = (crc & 0x0000FF00) >> 8;
    buffer[buffer_len - 3] = (crc & 0x000000FF);
    // post-amble
    buffer[buffer_len - 2] = 0xAB;
    buffer[buffer_len - 1] = 0xCD;
    // clear el, et bits
    this->_write(Register::Configuration, static_cast<uint16_t>(Configuration::Band868Idle));
    this->_write(Register::PowerManagement, static_cast<uint16_t>(PowerManagement::IdleMode));
    // clear interrupts and switch to tx mode
    this->_status();
    this->_write(Register::Configuration, static_cast<uint16_t>(Configuration::Band868Tx));
    this->_write(Register::PowerManagement, static_cast<uint16_t>(PowerManagement::TxMode));
    // start sending
    while (buffer_len != idx) {
        // wait for signal from interrupt
        if (chSemWaitTimeout(&this->_radio_semaphore, MS2ST(10)) != RDY_OK) {
            consoleDebug("RadioRFM12B::send() timeout during packet sending\r\n");
            break;
        }
        // read status
        status = this->_status();
        // else check for errors
        if ((status & static_cast<uint16_t>(Status::TxUnderrun)) != 0) {
            consoleDebug("RadioRFM12B::send() TxUnderrun bit set, status == 0x%.4X\r\n", status);
            break;
        // if tx ready then write next byte
        } else if ((status & static_cast<uint16_t>(Status::TxReady)) != 0) {
            consoleDevel("RadioRFM12B::send() TxReady ok, writing data: 0x%.2X\r\n", 0x55);
            this->_write(Register::TxFifoWrite, buffer[idx++]);
        // otherwise log what happend and continue
        } else {
            consoleDevel("RadioRFM12B::send() TxReady not set, status == 0x%.4X\r\n", status);
        }
    }
    //
    if (buffer_len != idx) {
        consoleDebug("RadioRFM12B::send() idx == %d, status == %.4X\r\n", idx, status);
    }
    // release memory
    chHeapFree(buffer);
    //
    ithacaUnlock(&this->_busy);
    consoleDebug("RadioRFM12B::send() end, len == %d, crc == %.8X\r\n", buffer_len, crc);
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool RadioRFM12B::recv(RadioPacket *packet, systime_t tmout) {
    //
    systime_t timeout = chTimeNow() + MS2ST(tmout);
    //
    consoleDebug("RadioRFM12B::recv() start\r\n");
    while (ithacaLock(&this->_busy) == false) {
        if (chTimeNow() < timeout) {
            consoleDebug("RadioRFM12B::recv() lock failed\r\n");
            return false;
        }
    }
    // prepare data for recving
    uint32_t crc1 = 0xAAAAAAAA, crc2 = 0x55555555;
    uint16_t status = 0;
    uint8_t idx = 0;
    uint8_t buffer_len = packet->rawDataLength() + 4;
    uint8_t *buffer = (uint8_t *)chHeapAlloc(NULL, buffer_len);
    // switch to tx mode and clear interrupts
    this->_write(Register::Configuration, static_cast<uint16_t>(Configuration::Band868Rx));
    this->_write(Register::PowerManagement, static_cast<uint16_t>(PowerManagement::RxMode));
    this->_status();
    // reset fifo
    this->_write(Register::FifoResetMode, static_cast<uint16_t>(FifoReset::Clear));
    this->_write(Register::FifoResetMode, static_cast<uint16_t>(FifoReset::Set));
    // start recving
    while (buffer_len != idx) {
        // wait for signal from interrupt
        if (chSemWaitTimeout(&this->_radio_semaphore, MS2ST(100)) != RDY_OK) {
            consoleDebug("RadioRFM12B::recv() timeout during packet recving\r\n");
            break;
        }
        // read status
        status = this->_status();
        // else check for errors
        if ((status & static_cast<uint16_t>(Status::RxOverflow)) != 0) {
            consoleDebug("RadioRFM12B::recv() RxOverflow bit set, status == 0x%.4X\r\n", status);
            break;
        // if tx ready then write next byte
        } else if ((status & static_cast<uint16_t>(Status::RxReady)) != 0) {
            consoleDevel("RadioRFM12B::recv() RxReady ok, writing data: 0x%.2X\r\n", 0x55);
            buffer[idx++] = this->_read();
        // otherwise log what happend and continue
        } else {
            consoleDevel("RadioRFM12B::recv() RxReady not set, status == 0x%.4X\r\n", status);
        }
    }
    //
    if (buffer_len != idx) {
        consoleDebug("RadioRFM12B::recv() idx == %d, status == %.4X\r\n", idx, status);
    } else {
        crc1 = ithacaCRC32(buffer, buffer_len - 4);
        crc2 = (buffer[buffer_len - 4] << 24) |
               (buffer[buffer_len - 3] << 16) |
               (buffer[buffer_len - 2] << 8) |
               buffer[buffer_len - 1];
        if (crc1 == crc2) {
            packet->setRawData(buffer);
        }
    }
    // release memory and lock
    chHeapFree(buffer);
    ithacaUnlock(&this->_busy);
    //
    if (crc1 == crc2) {
        consoleDebug("RadioRFM12B::recv() CRC ok: %.8X, length: %d\r\n", crc2, buffer_len - 4);
        return true;
    } else {
        consoleDebug("RadioRFM12B::recv() CRC mismatch: %.8X vs %.8X\r\n", crc1, crc2);
        return false;
    }
}

#endif /* ITHACA_USE_RADIO */

