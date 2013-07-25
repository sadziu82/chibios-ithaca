/*
 * @brief   ...
 */

#include "radio.hpp"

char buffer[ 32 ];

/*
 * @brief   radio ....
 */
Radio::Radio(void) {
    this->_state = RadioStateUninit;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::init(SPIDriver *spi_drv,
                  ioportid_t ce_port,
                  uint16_t ce_pin,
                  ioportid_t nirq_port,
                  uint16_t nirq_pin,
                  ioportid_t sdn_port,
                  uint16_t sdn_pin,
                  systime_t tmout) {
    //
    uint8_t status = 0;
    //
    this->io_init(spi_drv, ce_port, ce_pin, nirq_port, nirq_pin, sdn_port, sdn_pin);
    this->start_spi();
    // reset module
    this->assert_sdn();
    chThdSleepMilliseconds(1);
    this->release_sdn();
    console.write("rfm22b: reset completed\r\n");
    // check module response
    this->_state = RadioStateUninit;
    while (--tmout > 0) {
        status = this->read(0x00);
        if (status == 0x08) {
            break;
        } else {
            console.write("rfm22b: not ready, status == %x, sleeping\r\n", status);
            chThdSleepMilliseconds(1);
        }
    }
    if (tmout == 0) {
        console.write("rfm22b: init timeout, last status == %x\r\n", status);
        return false;
    }
    this->_state = RadioStatePresent;
    console.write("rfm22b: module present, status == %x\r\n", status);
    this->write(0x07, 0x80);
    while (--tmout > 0) {
        status = this->read(0x00);
        if (status == 0x08) {
            break;
        } else {
            console.write("rfm22b: not ready, status == %x, sleeping\r\n", status);
            chThdSleepMilliseconds(1);
        }
    }
    if (tmout == 0) {
        console.write("rfm22b: reset timeout, last status == %x\r\n", status);
        return false;
    }
    console.write("rfm22b: reset ok, status == %x, version = %x\r\n", status, this->read(0x01));
    // 
    this->write(RF22_REG_7D_TX_FIFO_CONTROL2, RF22_TXFFAEM_THRESHOLD);
    this->write(RF22_REG_7E_RX_FIFO_CONTROL,  RF22_RXFFAFULL_THRESHOLD);
    this->write(RF22_REG_30_DATA_ACCESS_CONTROL, RF22_ENPACRX | RF22_ENPACTX | RF22_ENCRC | RF22_CRC_CRC_16_IBM);
    this->write(RF22_REG_32_HEADER_CONTROL1, RF22_BCEN_HEADER3 | RF22_HDCH_HEADER3);
    this->write(RF22_REG_33_HEADER_CONTROL2, RF22_HDLEN_4 | RF22_SYNCLEN_2);
    setPreambleLength(8);
    uint8_t syncwords[] = { 0x2d, 0xd4 };
    setSyncWords(syncwords, sizeof(syncwords));
    setPromiscuous(false); 
    this->write(RF22_REG_3F_CHECK_HEADER3, RF22_DEFAULT_NODE_ADDRESS);
    setHeaderTo(RF22_DEFAULT_NODE_ADDRESS);
    setHeaderFrom(RF22_DEFAULT_NODE_ADDRESS);
    setHeaderId(0);
    setHeaderFlags(0);
    this->write(RF22_REG_0B_GPIO_CONFIGURATION0, 0x12) ; // TX state
    this->write(RF22_REG_0C_GPIO_CONFIGURATION1, 0x15) ; // RX state
    this->write(RF22_REG_05_INTERRUPT_ENABLE1, RF22_ENTXFFAEM | RF22_ENRXFFAFULL | RF22_ENPKSENT | RF22_ENPKVALID | RF22_ENCRCERROR | RF22_ENFFERR);
    this->write(RF22_REG_06_INTERRUPT_ENABLE2, RF22_ENPREAVAL);
    setFrequency(434.0, 0.05);
    setModemConfig(FSK_Rb2_4Fd36);
    setTxPower(RF22_TXPOW_8DBM);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::io_init(SPIDriver *spi_drv,
                     ioportid_t ce_port,
                     uint16_t ce_pin,
                     ioportid_t nirq_port,
                     uint16_t nirq_pin,
                     ioportid_t sdn_port,
                     uint16_t sdn_pin) {
    // spi pins
    this->_spi_drv = spi_drv;
    palSetPadMode(GPIOA, 7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(GPIOA, 6, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetPadMode(GPIOA, 5, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    // ce, nirq, sdn
    this->_ce_port = ce_port;
    this->_ce_pin = ce_pin;
    this->_nirq_port = nirq_port;
    this->_nirq_pin = nirq_pin;
    this->_sdn_port = sdn_port;
    this->_sdn_pin = sdn_pin;
    palSetPadMode(this->_ce_port, this->_ce_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(this->_nirq_port, this->_nirq_pin, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(this->_sdn_port, this->_sdn_pin, PAL_MODE_OUTPUT_PUSHPULL);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::start_spi(void) {
    this->_spi_cfg.end_cb = NULL;
    this->_spi_cfg.ssport = this->_ce_port;
    this->_spi_cfg.sspad = this->_ce_pin;
    this->_spi_cfg.cr1 = SPI_CR1_DFF | SPI_CR1_MSTR | SPI_CR1_BR_1;
    spiStart(this->_spi_drv, &this->_spi_cfg);
    return true;
}

/*
 * @brief   rfm22b ...
 */
bool Rfm22B::stop_spi(void) {
    spiStop(this->_spi_drv);
    return true;
}

/*
 * @brief   rfm22b ...
 */
void Rfm22B::shutdown_mode(void) {
    this->assert_sdn();
}

/*
 * @brief   rfm22b ...
 */
void Rfm22B::idle_mode(void) {
    this->release_sdn();
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
uint16_t Rfm22B::xfer(uint16_t tx_data) {
    uint16_t rx_data = 0;
    spiSelect(this->_spi_drv);
    spiExchange(this->_spi_drv, 1, &tx_data, &rx_data );
    spiUnselect(this->_spi_drv );
    return rx_data;
}

/*
 * @brief   rfm22b ...
 */
void Rfm22B::write(const uint8_t reg, const uint8_t data) {
    this->xfer(0x8000 | (reg << 8) | data);
}

/*
 * @brief   rfm22b ...
 */
uint8_t Rfm22B::read(const uint8_t reg) {
    uint16_t data = ( reg & 0x7F ) << 8;
    return this->xfer(data) & 0xFF;
}

// REVISIT: top bit is in Header Control 2 0x33
void Rfm22B::setPreambleLength(uint8_t nibbles)
{
    this->write(RF22_REG_34_PREAMBLE_LENGTH, nibbles);
}

// Caution doesnt set sync word len in Header Control 2 0x33
void Rfm22B::setSyncWords(const uint8_t* syncWords, uint8_t len)
{
    spiBurstWrite(RF22_REG_36_SYNC_WORD3, syncWords, len);
}

void Rfm22B::setPromiscuous(bool promiscuous)
{
    this->write(RF22_REG_43_HEADER_ENABLE3, promiscuous ? 0x00 : 0xff);
}

void Rfm22B::setHeaderTo(uint8_t to)
{
    this->write(RF22_REG_3A_TRANSMIT_HEADER3, to);
}

void Rfm22B::setHeaderFrom(uint8_t from)
{
    this->write(RF22_REG_3B_TRANSMIT_HEADER2, from);
}

void Rfm22B::setHeaderId(uint8_t id)
{
    this->write(RF22_REG_3C_TRANSMIT_HEADER1, id);
}

void Rfm22B::setHeaderFlags(uint8_t flags)
{
    this->write(RF22_REG_3D_TRANSMIT_HEADER0, flags);
}

uint8_t Rfm22B::headerTo()
{
    return spiRead(RF22_REG_47_RECEIVED_HEADER3);
}

uint8_t Rfm22B::headerFrom()
{
    return spiRead(RF22_REG_48_RECEIVED_HEADER2);
}

uint8_t Rfm22B::headerId()
{
    return spiRead(RF22_REG_49_RECEIVED_HEADER1);
}

uint8_t Rfm22B::headerFlags()
{
    return spiRead(RF22_REG_4A_RECEIVED_HEADER0);
}
// Returns true if centre + (fhch * fhs) is within limits
// Caution, different versions of the RF22 support different max freq
// so YMMV
boolean RF22::setFrequency(float centre, float afcPullInRange)
{
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
    spiWrite(RF22_REG_73_FREQUENCY_OFFSET1, 0);  // REVISIT
    spiWrite(RF22_REG_74_FREQUENCY_OFFSET2, 0);
    spiWrite(RF22_REG_75_FREQUENCY_BAND_SELECT, fbsel);
    spiWrite(RF22_REG_76_NOMINAL_CARRIER_FREQUENCY1, fc >> 8);
    spiWrite(RF22_REG_77_NOMINAL_CARRIER_FREQUENCY0, fc & 0xff);
    spiWrite(RF22_REG_2A_AFC_LIMITER, afclimiter);
    return !(statusRead() & RF22_FREQERR);
}

// Step size in 10kHz increments
// Returns true if centre + (fhch * fhs) is within limits
boolean RF22::setFHStepSize(uint8_t fhs)
{
    spiWrite(RF22_REG_7A_FREQUENCY_HOPPING_STEP_SIZE, fhs);
    return !(statusRead() & RF22_FREQERR);
}

// Adds fhch * fhs to centre frequency
// Returns true if centre + (fhch * fhs) is within limits
boolean RF22::setFHChannel(uint8_t fhch)
{
    spiWrite(RF22_REG_79_FREQUENCY_HOPPING_CHANNEL_SELECT, fhch);
    return !(statusRead() & RF22_FREQERR);
}

uint8_t RF22::rssiRead()
{
    return spiRead(RF22_REG_26_RSSI);
}

uint8_t RF22::ezmacStatusRead()
{
    return spiRead(RF22_REG_31_EZMAC_STATUS);
}

void RF22::setMode(uint8_t mode)
{
    spiWrite(RF22_REG_07_OPERATING_MODE1, mode);
}

void RF22::setModeIdle()
{
    if (_mode != RF22_MODE_IDLE)
    {
    setMode(_idleMode);
    _mode = RF22_MODE_IDLE;
    }
}

void RF22::setModeRx()
{
    if (_mode != RF22_MODE_RX)
    {
    setMode(_idleMode | RF22_RXON);
    _mode = RF22_MODE_RX;
    }
}

void RF22::setModeTx()
{
    if (_mode != RF22_MODE_TX)
    {
    setMode(_idleMode | RF22_TXON);
    _mode = RF22_MODE_TX;
    // Hmmm, if you dont clear the RX FIFO here, then it appears that going
    // to transmit mode in the middle of a receive can corrupt the
    // RX FIFO
    resetRxFifo();
//  clearRxBuf();
    }
}

uint8_t  RF22::mode()
{
    return _mode;
}

void RF22::setTxPower(uint8_t power)
{
    spiWrite(RF22_REG_6D_TX_POWER, power);
}

