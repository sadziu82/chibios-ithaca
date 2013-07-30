/*
 * @brief   ...
 */

#include "radio.hpp"

char buffer[ 32 ];

/*
 * @brief   radio ....
 */
Radio::Radio(void) {
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
RadioSpi::RadioSpi(void) {
}

/*
 * @brief   radio ...
 */
bool RadioSpi::spiInit(SPIDriver *spi_drv,
                        ioportid_t cs_port, uint16_t cs_pin) {
    // driver
    this->_spi_drv = spi_drv;
    // TODO: spi pins
    palSetPadMode(GPIOA, 7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(GPIOA, 6, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(GPIOA, 5, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
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
bool Rfm22B::init(SPIDriver *spi_drv,
                  ioportid_t cs_port, uint16_t cs_pin,
                  ioportid_t nirq_port, uint16_t nirq_pin,
                  ioportid_t sdn_port, uint16_t sdn_pin,
                  systime_t tmout_period) {
    //
    systime_t timeout = 0;
    //
    this->spiInit(spi_drv, cs_port, cs_pin);
    this->ioInit(nirq_port, nirq_pin, sdn_port, sdn_pin);
    this->spiStart();
    //
    timeout = chTimeNow() + MS2ST(tmout_period);
    if (not this->reset(timeout)) return false;
    console.write("rfm22b reset completed\r\n");
    if (not this->configure(timeout)) return false;
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
    this->spiWrite(Rfm22B::Register::DataAccessControl, RF22_ENPACRX | RF22_ENPACTX | RF22_ENCRC | RF22_CRC_CRC_16_IBM);
    this->spiWrite(Rfm22B::Register::HeaderControl1, RF22_BCEN_HEADER3 | RF22_HDCH_HEADER3);
    this->spiWrite(Rfm22B::Register::HeaderControl2, RF22_HDLEN_4 | RF22_SYNCLEN_2);
    this->spiWrite(Rfm22B::Register::PreambleLength, 8); // TODO 
    // TODO
    this->spiBurstWrite(Rfm22B::Register::SyncWord3,
                        Rfm22B::SyncWords::Default,
                        sizeof(Rfm22B::SyncWords::Default));
    this->spiWrite(Rfm22B::Register::HeaderEnable3, 0x00); // TODO
    this->spiWrite(Rfm22B::Register::CheckHeader3, Radio::NodeAddress::Default);
    // transmit headers
    spiWrite(Rfm22B::Register::TransmitHeader3, Radio::NodeAddress::Default);
    spiWrite(Rfm22B::Register::TransmitHeader2, Radio::NodeAddress::Default);
    spiWrite(Rfm22B::Register::TransmitHeader1, 0xAA); // TODO
    spiWrite(Rfm22B::Register::TransmitHeader0, 0x55); // TODO
    // gpio0 -> tx state, gpio1 -> rx state
    spiWrite(Rfm22B::Register::GpioConfiguration0, 0x12 ); // TODO
    spiWrite(Rfm22B::Register::GpioConfiguration1, 0x15 ); // TODO
    // TODO interrupts
    this->spiWrite(Rfm22B::Register::InterruptEnable1, RF22_ENTXFFAEM | RF22_ENRXFFAFULL | RF22_ENPKSENT | RF22_ENPKVALID | RF22_ENCRCERROR | RF22_ENFFERR);
    this->spiWrite(Rfm22B::Register::InterruptEnable2, RF22_ENPREAVAL);
    // set frequency
    setFrequency(434.0, 0.05);
    // TODO
    rfm22b_modem_config_t config = { 0x1B, 0x03, 0x41, 0x60, 0x27, 0x52, 0x00, 0x07, 0x40, 0x0A, 0x1E, 0x80, 0x60, 0x13, 0xA9, 0x2C, 0x22, 0x3A };
    setModemConfig(&config);
    // Minimum power
    //setTxPower(RF22_TXPOW_8DBM);
    this->spiWrite(Rfm22B::Register::TxPower, RF22_TXPOW_8DBM); // TODO
    //
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setFrequency(float centre, float afcPullInRange) {
    uint8_t fbsel = RF22_SBSEL;
    uint8_t afclimiter;
    if (centre < 240.0 || centre > 960.0) // 930.0 for early silicon
    return false;
    if (centre >= 480.0)
    {
    if (afcPullInRange < 0.0 || afcPullInRange > 0.318750)
        return false;
    centre /= 2;
    fbsel |= RF22_HBSEL;
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
bool Rfm22B::setModemConfig(rfm22b_modem_config_t *config) {
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
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setRxMode(void) {
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::setTxMode(void) {
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
