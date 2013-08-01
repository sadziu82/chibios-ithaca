/*
 * @brief   ...
 */

#include "radio.hpp"

char buffer[ 32 ];

/*
 * @brief   radio ....
 */
RadioPacket::RadioPacket(void) {
}

/*
 * @brief   radio ....
 */
uint8_t *RadioPacket::data(void) {
    return this->_data;
}

/*
 * @brief   radio ....
 */
uint8_t RadioPacket::len(void) {
    return this->_len;
}

/*
 * @brief   radio ....
 */
Radio::Radio(void) {
    chMtxInit(&this->_state_mtx);
    // 
    this->setState(Radio::State::Uninit);
}

/*
 * @brief   radio ....
 */
radio_state_t Radio::getState(void) {
    return this->_state;
}

/*
 * @brief   radio ....
 */
radio_state_t Radio::setState(radio_state_t new_state) {
    radio_state_t old_state = this->_state;
    this->_state = new_state;
    return old_state;
}

/*
 * @brief   radio ....
 */
bool Radio::lock(void) {
    return chMtxTryLock(&this->_state_mtx);
}

/*
 * @brief   radio ....
 */
bool Radio::unlock(void) {
    return chMtxUnlock();
}

/*
 * @brief   radio ....
 */
RadioSpi::RadioSpi(void) {
}

/*
 * @brief   radio ...
 */
bool RadioSpi::spiInit(SPIDriver *spi_drv,
                        ioportid_t miso_port, uint16_t miso_pin,
                        ioportid_t mosi_port, uint16_t mosi_pin,
                        ioportid_t scl_port, uint16_t scl_pin,
                        ioportid_t cs_port, uint16_t cs_pin) {
    // driver
    this->_spi_drv = spi_drv;
    // TODO: spi pins
    palSetPadMode(miso_port, miso_pin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(mosi_port, mosi_pin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(scl_port, scl_pin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    // 
    palSetPadMode(cs_port, cs_pin, PAL_MODE_OUTPUT_PUSHPULL);
    // 
    this->_spi_cfg.end_cb = NULL;
    this->_spi_cfg.ssport = cs_port;
    this->_spi_cfg.sspad = cs_pin;
    //this->_spi_cfg.cr1 = SPI_CR1_DFF | SPI_CR1_MSTR | SPI_CR1_BR_1;
    this->_spi_cfg.cr1 = SPI_CR1_MSTR | SPI_CR1_BR_1;
    //
    return true;
}

/*
 * @brief   radio ...
 */
bool RadioSpi::spiStart(void) {
    //
    spiAcquireBus(this->_spi_drv);
    ::spiStart(this->_spi_drv, &this->_spi_cfg);
    //
    return true;
}

/*
 * @brief   radio ...
 */
bool RadioSpi::spiStop(void) {
    //
    ::spiStop(this->_spi_drv);
    spiReleaseBus(this->_spi_drv);
    //
    return true;
}

/*
 * @brief   radio ...
 */
void RadioSpi::spiWrite(uint8_t reg, uint8_t data) {
    reg = 0x80 | reg;
    spiSelect(this->_spi_drv);
    spiSend(this->_spi_drv, 1, &reg);
    spiSend(this->_spi_drv, 1, &data);
    spiUnselect(this->_spi_drv );
}

/*
 * @brief   radio ...
 */
uint8_t RadioSpi::spiRead(uint8_t reg) {
    reg = 0x7F & reg;
    spiSelect(this->_spi_drv);
    spiSend(this->_spi_drv, 1, &reg);
    spiReceive(this->_spi_drv, 1, &reg);
    spiUnselect(this->_spi_drv);
    return reg;
}

/*
 * @brief   radio ...
 */
void RadioSpi::spiBurstWrite(uint8_t reg, const uint8_t *data, uint8_t len) {
    reg = 0x80 | reg;
    spiSelect(this->_spi_drv);
    spiSend(this->_spi_drv, 1, &reg);
    spiSend(this->_spi_drv, len, data);
    spiUnselect(this->_spi_drv );
}

/*
 * @brief   radio ...
 */
uint8_t RadioSpi::spiBurstRead(uint8_t reg, uint8_t *data, uint8_t len) {
    reg = 0x7F & reg;
    spiSelect(this->_spi_drv);
    spiSend(this->_spi_drv, 1, &reg);
    spiReceive(this->_spi_drv, len, data);
    spiUnselect(this->_spi_drv);
    return 0;
}

/*
 * @brief   rfm22b ...
 */
const rfm22b_sync_words_t Rfm22B::SyncWords::Default[] = { 0x2D, 0xD4 };

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::Default = {
    .IfFilterBandwidth = 0x1B,
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0x41,
    .ClockRecoveryOffset2 = 0x60,
    .ClockRecoveryOffset1 = 0x27,
    .ClockRecoveryOffset0 = 0x52,
    .ClockRecoveryTimingLoopGain1 = 0x00,
    .ClockRecoveryTimingLoopGain0 = 0x07,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0A,
    .SlicerPeakHold = 0x1E,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x13,
    .TxDataRate0 = 0xA9,
    .ModulationControl1 = 0x2C,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0x3A
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb125Fd125 = {
    .IfFilterBandwidth = 0x8A,
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0x60,
    .ClockRecoveryOffset2 = 0x01,
    .ClockRecoveryOffset1 = 0x55,
    .ClockRecoveryOffset0 = 0x55,
    .ClockRecoveryTimingLoopGain1 = 0x02,
    .ClockRecoveryTimingLoopGain0 = 0xAD,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0A,
    .SlicerPeakHold = 0x50,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x20,
    .TxDataRate0 = 0x00,
    .ModulationControl1 = 0x0C,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0xC8
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::GFSK_Rb125Fd125 = {
    .IfFilterBandwidth = 0x8A,
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0x60,
    .ClockRecoveryOffset2 = 0x01,
    .ClockRecoveryOffset1 = 0x55,
    .ClockRecoveryOffset0 = 0x55,
    .ClockRecoveryTimingLoopGain1 = 0x02,
    .ClockRecoveryTimingLoopGain0 = 0xAD,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0A,
    .SlicerPeakHold = 0x50,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x20,
    .TxDataRate0 = 0x00,
    .ModulationControl1 = 0x0C,
    .ModulationControl2 = 0x23,
    .FrequencyDeviation = 0xC8
};


/*
 * @brief   rfm22b ...
 */
bool Rfm22B::init(SPIDriver *spi_drv,
                  ioportid_t miso_port, uint16_t miso_pin,
                  ioportid_t mosi_port, uint16_t mosi_pin,
                  ioportid_t scl_port, uint16_t scl_pin,
                  ioportid_t cs_port, uint16_t cs_pin,
                  ioportid_t nirq_port, uint16_t nirq_pin,
                  ioportid_t sdn_port, uint16_t sdn_pin,
                  systime_t tmout_period) {
    //
    systime_t timeout = 0;
    //
    this->spiInit(spi_drv, miso_port, miso_pin, mosi_port, mosi_pin,
                  scl_port, scl_pin, cs_port, cs_pin);
    this->ioInit(nirq_port, nirq_pin, sdn_port, sdn_pin);
    this->spiStart();
    //
    timeout = chTimeNow() + MS2ST(tmout_period);
    if (not this->reset(timeout)) return false;
    console.write("rfm22b reset completed\r\n");
    if (not this->configure(timeout)) return false;
    this->setState(Radio::State::Idle);
    console.write("rfm22b configure completed\r\n");
    // 
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::reset(systime_t tmout) {
    // check module response
    this->setState(Radio::State::Uninit);
    // hardware reset
    this->assert_sdn();
    chThdSleepMilliseconds(1);
    this->release_sdn();
    // wait for module
    console.write("%d vs %d\r\n", tmout, chTimeNow());
    while ((this->getState() != Radio::State::Present) && (tmout > chTimeNow())) {
        if (this->spiRead(Rfm22B::Register::DeviceType) == Rfm22B::DeviceType::RxTx) {
            this->setState(Radio::State::Present);
        } else {
            chThdSleepMilliseconds(1);
            console.write("hardware reset loop\r\n");
        }
    }
    if (this->getState() != Radio::State::Present) return false;
    console.write("hardware reset completed: %x, %x\r\n", this->spiRead(Rfm22B::Register::DeviceType), this->spiRead(Rfm22B::Register::VersionCode));
    // software reset
    this->spiWrite(Rfm22B::Register::OperatingMode1, Rfm22B::Command::SwReset);
    console.write("software reset initiated\r\n");
    // wait for module
    while ((this->getState() != Radio::State::Ready) && (tmout > chTimeNow())) {
        if (this->spiRead(Rfm22B::Register::DeviceType) == Rfm22B::DeviceType::RxTx) {
            console.write("software reset loop\r\n");
            this->setState(Radio::State::Ready);
        } else {
            console.write("software reset loop\r\n");
            chThdSleepMilliseconds(1);
        }
    }
    if (this->getState() != Radio::State::Ready) return false;
    console.write("software reset completed: %x, %x\r\n", this->spiRead(Rfm22B::Register::DeviceType), this->spiRead(Rfm22B::Register::VersionCode));
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::configure(systime_t tmout) {
    //
    if (this->getState() != Radio::State::Ready) return false;
    //
    this->spiWrite(Rfm22B::Register::TxFifoControl2, RF22_TXFFAEM_THRESHOLD);
    this->spiWrite(Rfm22B::Register::RxFifoControl,  RF22_RXFFAFULL_THRESHOLD);
    this->spiWrite(Rfm22B::Register::DataAccessControl,
                   Rfm22B::DataAccessControl::ENPACRX |
                   Rfm22B::DataAccessControl::ENPACTX |
                   Rfm22B::DataAccessControl::Crc |
                   Rfm22B::DataAccessControl::Crc16Ibm);
    this->spiWrite(Rfm22B::Register::HeaderControl1,
                   Rfm22B::HeaderControl1::BCEN_HEADER3 |
                   Rfm22B::HeaderControl1::HDCH_HEADER3);
    this->spiWrite(Rfm22B::Register::HeaderControl2,
                   Rfm22B::HeaderControl2::HDLEN_4 |
                   Rfm22B::HeaderControl2::SYNCLEN_2);
    this->spiWrite(Rfm22B::Register::PreambleLength, 8); // TODO 
    // TODO
    this->spiBurstWrite(Rfm22B::Register::SyncWord3,
                        Rfm22B::SyncWords::Default,
                        sizeof(Rfm22B::SyncWords::Default));
    this->spiWrite(Rfm22B::Register::HeaderEnable3, 0x00); // TODO
    this->spiWrite(Rfm22B::Register::CheckHeader3, Radio::NodeAddress::Default);
    // transmit headers
    this->spiWrite(Rfm22B::Register::TransmitHeader3, Radio::NodeAddress::Default);
    this->spiWrite(Rfm22B::Register::TransmitHeader2, Radio::NodeAddress::Default);
    this->spiWrite(Rfm22B::Register::TransmitHeader1, 0xAA); // TODO
    this->spiWrite(Rfm22B::Register::TransmitHeader0, 0x55); // TODO
    // gpio0 -> tx state, gpio1 -> rx state
    this->spiWrite(Rfm22B::Register::GpioConfiguration0, 0x12 ); // TODO
    this->spiWrite(Rfm22B::Register::GpioConfiguration1, 0x15 ); // TODO
    // TODO interrupts
    this->spiWrite(Rfm22B::Register::InterruptEnable1,
                   Rfm22B::InterruptEnable1::FifoError |
                   Rfm22B::InterruptEnable1::TxFifoAlmostEmpty |
                   Rfm22B::InterruptEnable1::RxFifoAlmostFull |
                   Rfm22B::InterruptEnable1::PacketSent |
                   Rfm22B::InterruptEnable1::PacketValid |
                   Rfm22B::InterruptEnable1::CrcError);
    this->spiWrite(Rfm22B::Register::InterruptEnable2,
                   Rfm22B::InterruptEnable2::PreambleValid);
    // set frequency
    this->setFrequency(434.0, 0.05);
    // TODO
    //this->setModemConfig(&Rfm22B::ModemConfig::Default);
    this->setModemConfig(&Rfm22B::ModemConfig::GFSK_Rb125Fd125);
    // Minimum power
    //setTxPower(RF22_TXPOW_8DBM);
    this->spiWrite(Rfm22B::Register::TxPower, Rfm22B::TxPower::TXPOW_8DBM); // TODO
    //
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setFrequency(float centre, float afcPullInRange) {
    uint8_t fbsel = Rfm22B::FrequencyBandSelect::SBSEL;
    uint8_t afclimiter;
    if (centre < 240.0 || centre > 960.0) // 930.0 for early silicon
    return false;
    if (centre >= 480.0)
    {
    if (afcPullInRange < 0.0 || afcPullInRange > 0.318750)
        return false;
    centre /= 2;
    fbsel |= Rfm22B::FrequencyBandSelect::HBSEL;
    afclimiter = afcPullInRange * 1000000.0 / 1250.0;
    }
    else
    {
    if (afcPullInRange < 0.0 || afcPullInRange > 0.159375)
        return false;
    afclimiter = afcPullInRange * 1000000.0 / 625.0;
    }
    centre /= 10.0;
    float integerPart = floor(centre);
    float fractionalPart = centre - integerPart;

    uint8_t fb = (uint8_t)integerPart - 24; // Range 0 to 23
    fbsel |= fb;
    uint16_t fc = fractionalPart * 64000;
    spiWrite(Rfm22B::Register::FrequencyOffset1, 0);  // REVISIT
    spiWrite(Rfm22B::Register::FrequencyOffset2, 0);
    spiWrite(Rfm22B::Register::FrequencyBandSelect, fbsel);
    spiWrite(Rfm22B::Register::NominalCarrierFrequency1, fc >> 8);
    spiWrite(Rfm22B::Register::NominalCarrierFrequency0, fc & 0xff);
    spiWrite(Rfm22B::Register::AfcLimiter, afclimiter);
    return !(this->spiRead(Rfm22B::Register::DeviceStatus) & Rfm22B::DeviceStatus::FrequencyError);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setModemConfig(const rfm22b_modem_config_t *config) {
    this->spiWrite(Rfm22B::Register::IfFilterBandwidth, config->IfFilterBandwidth);
    this->spiWrite(Rfm22B::Register::ClockRecoveryGearshiftOverride, config->ClockRecoveryGearshiftOverride);
    this->spiBurstWrite(Rfm22B::Register::ClockRecoveryOversamplingRate, &config->ClockRecoveryOversamplingRate, 6);
    this->spiBurstWrite(Rfm22B::Register::OokCounterValue1, &config->OokCounterValue1, 3);
    this->spiWrite(Rfm22B::Register::ChargePumpCurrentTrimming, config->ChargePumpCurrentTrimming);
    this->spiWrite(Rfm22B::Register::AgcOverride1, config->AgcOverride1);
    this->spiBurstWrite(Rfm22B::Register::TxDataRate1, &config->TxDataRate1, 5);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setShutdownMode(void) {
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setIdleMode(void) {
    this->spiWrite(Rfm22B::Register::OperatingMode1,
                   Rfm22B::OperatingMode1::XtalOn);
    this->setState(Radio::State::Idle);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setRxMode(void) {
    this->resetRxFifio();
    this->spiWrite(Rfm22B::Register::OperatingMode1,
                   Rfm22B::OperatingMode1::XtalOn |
                   Rfm22B::OperatingMode1::RxOn);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setTxMode(void) {
    this->spiWrite(Rfm22B::Register::OperatingMode1,
                   Rfm22B::OperatingMode1::XtalOn |
                   Rfm22B::OperatingMode1::TxOn);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::ioInit(ioportid_t nirq_port, uint16_t nirq_pin,
                     ioportid_t sdn_port, uint16_t sdn_pin) {
    // nirq, sdn
    this->_nirq_port = nirq_port;
    this->_nirq_pin = nirq_pin;
    this->_sdn_port = sdn_port;
    this->_sdn_pin = sdn_pin;
    // 
    palSetPadMode(this->_nirq_port, this->_nirq_pin, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(this->_sdn_port, this->_sdn_pin, PAL_MODE_OUTPUT_PUSHPULL);
    return true;
}

/*
 * @brief   rfm22b ...
 */
void Rfm22B::assert_sdn(void) {
    palSetPad(this->_sdn_port, this->_sdn_pin);
}

/*
 * @brief   rfm22b ...
 */
void Rfm22B::release_sdn(void) {
    palClearPad(this->_sdn_port, this->_sdn_pin);
}

/*
 * @brief   rfm22b ...
 */
void Rfm22B::resetTxFifio(void) {
    spiWrite(Rfm22B::Register::OperatingMode2, Rfm22B::OperatingMode2::FifoClearTx);
    spiWrite(Rfm22B::Register::OperatingMode2, 0);
}

/*
 * @brief   rfm22b ...
 */
void Rfm22B::resetRxFifio(void) {
    spiWrite(Rfm22B::Register::OperatingMode2, Rfm22B::OperatingMode2::FifoClearRx);
    spiWrite(Rfm22B::Register::OperatingMode2, 0);
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::send(RadioPacket *packet, systime_t tmout_period) {
    //
    bool packet_sent = false;
    systime_t tmout = chTimeNow() + tmout_period;
    rfm22b_register_bits_t interrupt_status_1;
    //
    //console.write("rfm22b sending packet\r\n");
    if (not this->lock()) {
        console.write("rfm22b unable to lock\r\n");
        return false;
    }
    if (this->getState() != Radio::State::Idle) {
        console.write("rfm22b not in idle state\r\n");
        this->unlock();
        return false;
    }
    this->setState(Radio::State::Sending);
    this->unlock();
    this->resetTxFifio();
    spiBurstWrite(Rfm22B::Register::FifoAccess, packet->data(), sizeof(packet->len()));
    spiWrite(Rfm22B::Register::PacketLength, packet->len());
    this->setTxMode();
    while ((not packet_sent) and (tmout > chTimeNow())) {
        interrupt_status_1 = this->spiRead(Rfm22B::Register::InterruptStatus1);
        if ((interrupt_status_1 & Rfm22B::InterruptStatus1::PacketSent) != 0) {
            packet_sent = true;
        } else {
            //console.write("rfm22b wait for PacketSent\r\n");
            chThdSleepMilliseconds(1);
        }
    }
    this->setIdleMode();
    if (packet_sent == false) {
        console.write("rfm22b sending timeout\r\n");
        return false;
    }
    //console.write("rfm22b sent\r\n");
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::recv(RadioPacket *packet, systime_t tmout_period) {
    //
    bool packet_received = false;
    systime_t tmout = chTimeNow() + tmout_period;
    rfm22b_register_bits_t interrupt_status_1;
    //
    //console.write("rfm22b receiving packet\r\n");
    //
    if (not this->lock()) {
        console.write("rfm22b unable to lock\r\n");
        return false;
    }
    if (this->getState() != Radio::State::Idle) {
        console.write("rfm22b not in idle state\r\n");
        this->unlock();
        return false;
    }
    //
    this->setState(Radio::State::Receiving);
    this->unlock();
    this->setRxMode();
    while ((not packet_received) and (tmout > chTimeNow())) {
        interrupt_status_1 = this->spiRead(Rfm22B::Register::InterruptStatus1);
        if ((interrupt_status_1 & Rfm22B::InterruptStatus1::PacketValid) != 0) {
            packet_received = true;
        } else {
            //console.write("rfm22b wait for PacketValid\r\n");
            chThdSleepMilliseconds(1);
        }
    }
    this->setIdleMode();
    if (packet_received == false) {
        console.write("rfm22b receiving timeout\r\n");
        return false;
    }
    //console.write("rfm22b received\r\n");
    return true;
}

//////////// // REVISIT: top bit is in Header Control 2 0x33
//////////// void Rfm22B::setPreambleLength(uint8_t nibbles)
//////////// {
////////////     this->write(RF22_REG_34_PREAMBLE_LENGTH, nibbles);
//////////// }
//////////// 
//////////// // Caution doesnt set sync word len in Header Control 2 0x33
//////////// void Rfm22B::setSyncWords(const uint8_t* syncWords, uint8_t len)
//////////// {
////////////     spiBurstWrite(RF22_REG_36_SYNC_WORD3, syncWords, len);
//////////// }
//////////// 
//////////// void Rfm22B::setPromiscuous(bool promiscuous)
//////////// {
////////////     this->write(RF22_REG_43_HEADER_ENABLE3, promiscuous ? 0x00 : 0xff);
//////////// }
//////////// 
//////////// void Rfm22B::setHeaderTo(uint8_t to)
//////////// {
////////////     this->write(RF22_REG_3A_TRANSMIT_HEADER3, to);
//////////// }
//////////// 
//////////// void Rfm22B::setHeaderFrom(uint8_t from)
//////////// {
////////////     this->write(RF22_REG_3B_TRANSMIT_HEADER2, from);
//////////// }
//////////// 
//////////// void Rfm22B::setHeaderId(uint8_t id)
//////////// {
////////////     this->write(RF22_REG_3C_TRANSMIT_HEADER1, id);
//////////// }
//////////// 
//////////// void Rfm22B::setHeaderFlags(uint8_t flags)
//////////// {
////////////     this->write(RF22_REG_3D_TRANSMIT_HEADER0, flags);
//////////// }
//////////// 
//////////// uint8_t Rfm22B::headerTo()
//////////// {
////////////     return spiRead(RF22_REG_47_RECEIVED_HEADER3);
//////////// }
//////////// 
//////////// uint8_t Rfm22B::headerFrom()
//////////// {
////////////     return spiRead(RF22_REG_48_RECEIVED_HEADER2);
//////////// }
//////////// 
//////////// uint8_t Rfm22B::headerId()
//////////// {
////////////     return spiRead(RF22_REG_49_RECEIVED_HEADER1);
//////////// }
//////////// 
//////////// uint8_t Rfm22B::headerFlags()
//////////// {
////////////     return spiRead(RF22_REG_4A_RECEIVED_HEADER0);
//////////// }
//////////// // Returns true if centre + (fhch * fhs) is within limits
//////////// // Caution, different versions of the RF22 support different max freq
//////////// // so YMMV
//////////// boolean RF22::setFrequency(float centre, float afcPullInRange)
//////////// {
////////////     uint8_t fbsel = RF22_SBSEL;
////////////     uint8_t afclimiter;
////////////     if (centre < 240.0 || centre > 960.0) // 930.0 for early silicon
////////////     return false;
////////////     if (centre >= 480.0)
////////////     {
////////////     if (afcPullInRange < 0.0 || afcPullInRange > 0.318750)
////////////         return false;
////////////     centre /= 2;
////////////     fbsel |= RF22_HBSEL;
////////////     afclimiter = afcPullInRange * 1000000.0 / 1250.0;
////////////     }
////////////     else
////////////     {
////////////     if (afcPullInRange < 0.0 || afcPullInRange > 0.159375)
////////////         return false;
////////////     afclimiter = afcPullInRange * 1000000.0 / 625.0;
////////////     }
////////////     centre /= 10.0;
////////////     float integerPart = floor(centre);
////////////     float fractionalPart = centre - integerPart;
//////////// 
////////////     uint8_t fb = (uint8_t)integerPart - 24; // Range 0 to 23
////////////     fbsel |= fb;
////////////     uint16_t fc = fractionalPart * 64000;
////////////     spiWrite(RF22_REG_73_FREQUENCY_OFFSET1, 0);  // REVISIT
////////////     spiWrite(RF22_REG_74_FREQUENCY_OFFSET2, 0);
////////////     spiWrite(RF22_REG_75_FREQUENCY_BAND_SELECT, fbsel);
////////////     spiWrite(RF22_REG_76_NOMINAL_CARRIER_FREQUENCY1, fc >> 8);
////////////     spiWrite(RF22_REG_77_NOMINAL_CARRIER_FREQUENCY0, fc & 0xff);
////////////     spiWrite(RF22_REG_2A_AFC_LIMITER, afclimiter);
////////////     return !(statusRead() & RF22_FREQERR);
//////////// }
//////////// 
//////////// // Step size in 10kHz increments
//////////// // Returns true if centre + (fhch * fhs) is within limits
//////////// boolean RF22::setFHStepSize(uint8_t fhs)
//////////// {
////////////     spiWrite(RF22_REG_7A_FREQUENCY_HOPPING_STEP_SIZE, fhs);
////////////     return !(statusRead() & RF22_FREQERR);
//////////// }
//////////// 
//////////// // Adds fhch * fhs to centre frequency
//////////// // Returns true if centre + (fhch * fhs) is within limits
//////////// boolean RF22::setFHChannel(uint8_t fhch)
//////////// {
////////////     spiWrite(RF22_REG_79_FREQUENCY_HOPPING_CHANNEL_SELECT, fhch);
////////////     return !(statusRead() & RF22_FREQERR);
//////////// }
//////////// 
//////////// uint8_t RF22::rssiRead()
//////////// {
////////////     return spiRead(RF22_REG_26_RSSI);
//////////// }
//////////// 
//////////// uint8_t RF22::ezmacStatusRead()
//////////// {
////////////     return spiRead(RF22_REG_31_EZMAC_STATUS);
//////////// }
//////////// 
//////////// void RF22::setMode(uint8_t mode)
//////////// {
////////////     spiWrite(RF22_REG_07_OPERATING_MODE1, mode);
//////////// }
//////////// 
//////////// void RF22::setModeIdle()
//////////// {
////////////     if (_mode != RF22_MODE_IDLE)
////////////     {
////////////     setMode(_idleMode);
////////////     _mode = RF22_MODE_IDLE;
////////////     }
//////////// }
//////////// 
//////////// void RF22::setModeRx()
//////////// {
////////////     if (_mode != RF22_MODE_RX)
////////////     {
////////////     setMode(_idleMode | RF22_RXON);
////////////     _mode = RF22_MODE_RX;
////////////     }
//////////// }
//////////// 
//////////// void RF22::setModeTx()
//////////// {
////////////     if (_mode != RF22_MODE_TX)
////////////     {
////////////     setMode(_idleMode | RF22_TXON);
////////////     _mode = RF22_MODE_TX;
////////////     // Hmmm, if you dont clear the RX FIFO here, then it appears that going
////////////     // to transmit mode in the middle of a receive can corrupt the
////////////     // RX FIFO
////////////     resetRxFifo();
//////////// //  clearRxBuf();
////////////     }
//////////// }
//////////// 
//////////// uint8_t  RF22::mode()
//////////// {
////////////     return _mode;
//////////// }
//////////// 
//////////// void RF22::setTxPower(uint8_t power)
//////////// {
////////////     spiWrite(RF22_REG_6D_TX_POWER, power);
//////////// }
//////////// 
