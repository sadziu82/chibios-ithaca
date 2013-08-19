/*
 * @brief   RFM12B module
 */

/*
 * @brief   radio ....
 */
#include "radio_rfm12b.hpp"

/*
 * @brief   rfm12b ...
 */
bool Rfm12B::init(SPIDriver *spi_drv,
                  ioportid_t miso_port, uint16_t miso_pin,
                  ioportid_t mosi_port, uint16_t mosi_pin,
                  ioportid_t scl_port, uint16_t scl_pin,
                  ioportid_t cs_port, uint16_t cs_pin,
                  ioportid_t nirq_port, uint16_t nirq_pin,
                  ioportid_t rst_port, uint16_t rst_pin,
                  systime_t tmout_period) {
    //
    systime_t timeout = chTimeNow() + MS2ST(tmout_period);
    // nirq, rst/reset
    this->_nirq_port = nirq_port;
    this->_nirq_pin = nirq_pin;
    this->_rst_port = rst_port;
    this->_rst_pin = rst_pin;
    // 
    palSetPadMode(this->_nirq_port, this->_nirq_pin, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(this->_rst_port, this->_rst_pin, PAL_MODE_OUTPUT_PUSHPULL);
    // driver
    this->_spi_cfg.end_cb = NULL;
    this->_spi_cfg.ssport = cs_port;
    this->_spi_cfg.sspad = cs_pin;
    this->_spi_cfg.cr1 = SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_DFF;
    this->_spi_drv = spi_drv;
    // TODO: spi pins
    palSetPadMode(miso_port, miso_pin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(mosi_port, mosi_pin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(scl_port, scl_pin, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(cs_port, cs_pin, PAL_MODE_OUTPUT_PUSHPULL);
    // 
    spiAcquireBus(this->_spi_drv);
    spiStart(this->_spi_drv, &this->_spi_cfg);
    console.write("rfm12b io&spi initialized\r\n");
    // perform module reset
    if (not this->reset(timeout)) {
        spiStop(this->_spi_drv);
        spiReleaseBus(this->_spi_drv);
        return false;
    }
    console.write("rfm12b reset completed\r\n");
    // configure module
    if (not this->configure(timeout)) {
        spiStop(this->_spi_drv);
        spiReleaseBus(this->_spi_drv);
        return false;
    }
    console.write("rfm12b configure completed\r\n");
    // 
    this->idleMode();
    console.write("rfm12b initialized\r\n");
    // 
    return true;
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::assertReset(void) {
    palClearPad(this->_rst_port, this->_rst_pin);
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::releaseReset(void) {
    palSetPad(this->_rst_port, this->_rst_pin);
}

/*
 * @brief   rfm12b ...
 */
uint16_t Rfm12B::xfer(const uint16_t cmd) {
    uint16_t data = 0;
    spiSelect(this->_spi_drv);
    spiExchange(this->_spi_drv, 1, &cmd, &data);
    spiUnselect(this->_spi_drv);
    return data;
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::write(const uint16_t reg, const uint16_t data) {
    this->xfer(reg | data);
}

/*
 * @brief   rfm12b ...
 */
uint16_t Rfm12B::status(void) {
    // 
    return this->xfer(Rfm12B::Register::Status);
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::sleepMode(void) {
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::idleMode(void) {
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::rxMode(void) {
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::txMode(void) {
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::resetTxFifio(void) {
}

/*
 * @brief   rfm12b ...
 */
void Rfm12B::resetRxFifio(void) {
}

/*
 * @brief   rfm12b ...
 */
bool Rfm12B::reset(systime_t tmout) {
    // reset module
    this->assertReset();
    chThdSleepMilliseconds(10);
    // 
    this->releaseReset();
    chThdSleepMilliseconds(10);
    // put module into sleep mode
    this->write(Rfm12B::Register::PowerManagement,
                Rfm12B::PowerManagement::DisableClockOutput |
                Rfm12B::PowerManagement::EnableLowBatteryDetector);
    // make sure we are in fifo mode
    this->write(Rfm12B::Register::TxWrite, 0);
    this->setState(Radio::State::Uninit);
    // 
    if (palReadPad(this->_nirq_port, this->_nirq_pin) != 0) {
        console.write("nIRQ high after reset\r\n");
        return false;
    }
    //
    while ((this->getState() != Radio::State::Present) && (tmout > chTimeNow())) {
        if ((this->status() & Rfm12B::Status::PowerOnReset) != 0) {
            this->setState(Radio::State::Present);
        } else {
            chThdSleepMilliseconds(1);
            console.write("hardware reset loop\r\n");
        }
    }
    // if timeout then return false
    if (this->getState() != Radio::State::Present) {
        return false;
    }
    // check if nIRQ goes high after status read
    if (palReadPad(this->_nirq_port, this->_nirq_pin) == 0) {
        console.write("nIRQ low after status read\r\n");
        return false;
    }
    // check module presence by setting high low-battery-detector threshold
    this->write(Rfm12B::Register::LowBatteryDetector, Rfm12B::LowBattery::v37);
    chThdSleepMilliseconds(1);
    if ((this->status() & Rfm12B::Status::LowBattery) == 0) {
        console.write("rfm12b low-battery-detector test failed\r\n");
        return false;
    }
    // 
    return true;
}

/*
 * @brief   rfm12b ...
 */
bool Rfm12B::configure(systime_t tmout) {
    // initialize rf module
    this->write(Rfm12B::Register::Configuration, 0x00E7); // config
    this->write(Rfm12B::Register::Frequency, 0x0CFF); // frequency - 876.63 MHz
    this->write(Rfm12B::Register::DataRate, 0x0006); // bit rate, 49 kbps
    this->write(Rfm12B::Register::ReceiverControl, 0x04A2 ); // rx config
    this->write(Rfm12B::Register::DataFilter, 0x00AC); // data filter
    this->write(Rfm12B::Register::FifoResetMode, 0x0083); // synchro 1-st
    this->write(Rfm12B::Register::SynchronPattern, 0x0055); // synchro 2-nd
    //this->write( 0x8209 ); // power
    //this->write( 0xC647 ); // bit rate, 4.8 kbps
    //this->write( 0xC647 ); // bit rate, 9.6 kbps
    //this->write( 0xC608 ); // bit rate, 38 kbps
    //this->write( 0xC605 ); // bit rate, 57.6 kbps
    //this->write( 0x94A0 ); // rx config
    //this->write( 0x9424 ); // rx config
    //this->write( 0xC2AC ); // data filter
    //this->write( 0xC2EC ); // data filter
    //this->write( 0xCEFC ); // synchro
    //this->write( 0xC487 ); // afc
    this->write(Rfm12B::Register::AFC, 0x0083); // afc
    this->write(Rfm12B::Register::TxConfig, 0x0050); // tx config
    this->write(Rfm12B::Register::PLL, 0x0077);
    this->write(Rfm12B::Register::WakeUpTimer, 0x0000);
    this->write(Rfm12B::Register::LowDutyCycle, 0x0000);
    this->write(Rfm12B::Register::LowBatteryDetector, 0x0000);
    // 
    chThdSleepMilliseconds( 50 );
    // 
    return true;
}

/*
 * @brief   rfm12b ...
 */
bool Rfm12B::send(RadioPacket *packet, systime_t tmout_period) {
    // 
    return true;
}

/*
 * @brief   rfm12b ...
 */
bool Rfm12B::recv(RadioPacket *packet, systime_t tmout_period) {
    // 
    return true;
}

