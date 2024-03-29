/*
 * @brief   ...
 */

#include "radio_rfm22b.hpp"

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
    ::spiAcquireBus(this->_spi_drv);
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
    ::spiReleaseBus(this->_spi_drv);
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
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb2Fd5 = {
    .IfFilterBandwidth = 0x2b,
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0xf4,
    .ClockRecoveryOffset2 = 0x20,
    .ClockRecoveryOffset1 = 0x41,
    .ClockRecoveryOffset0 = 0x89,
    .ClockRecoveryTimingLoopGain1 = 0x00,
    .ClockRecoveryTimingLoopGain0 = 0x36,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0a,
    .SlicerPeakHold = 0x1d,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x10,
    .TxDataRate0 = 0x62,
    .ModulationControl1 = 0x2c,
    .ModulationControl2 = 0x23,
    .FrequencyDeviation = 0x08
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb2_4Fd36 = {
    .IfFilterBandwidth = 0x1b,
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0x41,
    .ClockRecoveryOffset2 = 0x60,
    .ClockRecoveryOffset1 = 0x27,
    .ClockRecoveryOffset0 = 0x52,
    .ClockRecoveryTimingLoopGain1 = 0x00,
    .ClockRecoveryTimingLoopGain0 = 0x07,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0a,
    .SlicerPeakHold = 0x1e,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x13,
    .TxDataRate0 = 0xa9,
    .ModulationControl1 = 0x2c,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0x3a
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb4_8Fd45 = {
    .IfFilterBandwidth = 0x1d, 
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0xa1,
    .ClockRecoveryOffset2 = 0x20,
    .ClockRecoveryOffset1 = 0x4e,
    .ClockRecoveryOffset0 = 0xa5,
    .ClockRecoveryTimingLoopGain1 = 0x00,
    .ClockRecoveryTimingLoopGain0 = 0x13,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0a,
    .SlicerPeakHold = 0x1e,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x27,
    .TxDataRate0 = 0x52,
    .ModulationControl1 = 0x2c,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0x48
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb9_6Fd45 = {
    .IfFilterBandwidth = 0x1e, 
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0xd0,
    .ClockRecoveryOffset2 = 0x00,
    .ClockRecoveryOffset1 = 0x9d,
    .ClockRecoveryOffset0 = 0x49,
    .ClockRecoveryTimingLoopGain1 = 0x00,
    .ClockRecoveryTimingLoopGain0 = 0x45,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0a,
    .SlicerPeakHold = 0x20,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x4e,
    .TxDataRate0 = 0xa5,
    .ModulationControl1 = 0x2c,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0x48
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb19_2Fd9_6 = {
    .IfFilterBandwidth = 0x2b, 
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0x34,
    .ClockRecoveryOffset2 = 0x02,
    .ClockRecoveryOffset1 = 0x75,
    .ClockRecoveryOffset0 = 0x25,
    .ClockRecoveryTimingLoopGain1 = 0x07,
    .ClockRecoveryTimingLoopGain0 = 0xff,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0a,
    .SlicerPeakHold = 0x1b,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x9d,
    .TxDataRate0 = 0x49,
    .ModulationControl1 = 0x2c,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0x0f
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb38_4Fd19_6 = {
    .IfFilterBandwidth = 0x02, 
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0x68,
    .ClockRecoveryOffset2 = 0x01,
    .ClockRecoveryOffset1 = 0x3a,
    .ClockRecoveryOffset0 = 0x93,
    .ClockRecoveryTimingLoopGain1 = 0x04,
    .ClockRecoveryTimingLoopGain0 = 0xd5,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0a,
    .SlicerPeakHold = 0x1e,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x09,
    .TxDataRate0 = 0xd5,
    .ModulationControl1 = 0x0c,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0x1f
};

/*
 * @brief   rfm22b ...
 */
const rfm22b_modem_config_t Rfm22B::ModemConfig::FSK_Rb57_6Fd28_8 = {
    .IfFilterBandwidth = 0x06, 
    .ClockRecoveryGearshiftOverride = 0x03,
    .ClockRecoveryOversamplingRate = 0x45,
    .ClockRecoveryOffset2 = 0x01,
    .ClockRecoveryOffset1 = 0xd7,
    .ClockRecoveryOffset0 = 0xdc,
    .ClockRecoveryTimingLoopGain1 = 0x07,
    .ClockRecoveryTimingLoopGain0 = 0x6e,
    .OokCounterValue1 = 0x40,
    .OokCounterValue2 = 0x0a,
    .SlicerPeakHold = 0x2d,
    .ChargePumpCurrentTrimming = 0x80,
    .AgcOverride1 = 0x60,
    .TxDataRate1 = 0x0e,
    .TxDataRate0 = 0xbf,
    .ModulationControl1 = 0x0c,
    .ModulationControl2 = 0x22,
    .FrequencyDeviation = 0x2e
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
    //this->setFrequency(434.0, 0.05);
    this->setFrequency(434.0, 0.05);
    // TODO
    //this->setModemConfig(&Rfm22B::ModemConfig::Default);
    this->setModemConfig(&Rfm22B::ModemConfig::FSK_Rb38_4Fd19_6);
    // Minimum power
    //setTxPower(RF22_TXPOW_8DBM);
    //this->spiWrite(Rfm22B::Register::TxPower, Rfm22B::TxPower::TXPOW_8DBM); // TODO
    this->spiWrite(Rfm22B::Register::TxPower, Rfm22B::TxPower::TXPOW_20DBM); // TODO
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
        // console.write("rfm22b sending timeout\r\n");
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
    spiBurstRead(Rfm22B::Register::FifoAccess, packet->data(), sizeof(packet->len()));
    this->setIdleMode();
    if (packet_received == false) {
        //console.write("rfm22b receiving timeout\r\n");
        return false;
    }
    //console.write("rfm22b received\r\n");
    return true;
}

