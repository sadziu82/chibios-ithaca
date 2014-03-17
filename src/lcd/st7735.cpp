#include <ithaca.hpp>

#if ITHACA_USE_LCD_ST7735 || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 */
enum class LcdST7735::Register {
    //
    Nop = 0x00,
    SwReset = 0x01,
    RddId = 0x04,
    RddSt = 0x09,
    //
    SlpIn = 0x10,
    SlpOut = 0x11,
    PtlOn = 0x12,
    NorOn = 0x13,
    //
    InvOff = 0x20,
    InvOn = 0x21,
    DispOff = 0x28,
    DispOn = 0x29,
    CaSet = 0x2A,
    RaSet = 0x2B,
    RamWr = 0x2C,
    RamRd = 0x2E,
    //
    PtlAr = 0x30,
    ColMod = 0x3A,
    MadCtl = 0x36,
    //
    FrmCtr1 = 0xB1,
    FrmCtr2 = 0xB2,
    FrmCtr3 = 0xB3,
    InvCtr = 0xB4,
    DisSet5 = 0xB6,
    //
    PwCtr1 = 0xC0,
    PwCtr2 = 0xC1,
    PwCtr3 = 0xC2,
    PwCtr4 = 0xC3,
    PwCtr5 = 0xC4,
    VmCtr1 = 0xC5,
    //
    RdId1 = 0xDA,
    RdId2 = 0xDB,
    RdId3 = 0xDC,
    RdId4 = 0xDD,
    //
    PwCtr6 = 0xFC,
    //
    GmCtrP1 = 0xE0,
    GmCtrN1 = 0xE1,
    //
    MadCtl_MY = 0x80,
    MadCtl_MX = 0x40,
    MadCtl_MV = 0x20,
    MadCtl_ML = 0x10,
    MadCtl_RGB = 0x00,
    MadCtl_BGR = 0x08,
    MadCtl_MH = 0x04,
};

/*
 * @brief   ...
 */
const uint8_t LcdST7735::init_seq[] = {
    static_cast<uint8_t>(LcdST7735::Register::SwReset), 0, 50,
    static_cast<uint8_t>(LcdST7735::Register::SlpOut), 0, 50,
    static_cast<uint8_t>(LcdST7735::Register::FrmCtr1), 3, 0x01, 0x2C, 0x2D, 0,
    static_cast<uint8_t>(LcdST7735::Register::FrmCtr2), 3, 0x01, 0x2C, 0x2D, 0,
    static_cast<uint8_t>(LcdST7735::Register::FrmCtr3), 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, 0,
    static_cast<uint8_t>(LcdST7735::Register::InvCtr), 1, 0x07, 0,
    static_cast<uint8_t>(LcdST7735::Register::PwCtr1), 3, 0xA2, 0x02, 0x84, 0,
    static_cast<uint8_t>(LcdST7735::Register::PwCtr2), 1, 0xC5, 0,
    static_cast<uint8_t>(LcdST7735::Register::PwCtr3), 2, 0x0A, 0x00, 0,
    static_cast<uint8_t>(LcdST7735::Register::PwCtr4), 2, 0x8A, 0x2A, 0,
    static_cast<uint8_t>(LcdST7735::Register::PwCtr5), 2, 0x8A, 0xEE, 0,
    static_cast<uint8_t>(LcdST7735::Register::VmCtr1), 1, 0x0E, 0,
    static_cast<uint8_t>(LcdST7735::Register::InvOff), 0, 0,
    static_cast<uint8_t>(LcdST7735::Register::ColMod), 1, 0x05, 0,
    static_cast<uint8_t>(LcdST7735::Register::GmCtrP1), 16,
          0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 
          0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, 
        0,
    static_cast<uint8_t>(LcdST7735::Register::GmCtrN1), 16,
          0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
          0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
        5,
    //  last
    static_cast<uint8_t>(LcdST7735::Register::NorOn), 0, 10,
    static_cast<uint8_t>(LcdST7735::Register::DispOn), 0, 20,
    0,
};

/*
 * @brief   ...
 */
uint8_t LcdST7735::rotation_seq[] = {
    static_cast<uint8_t>(LcdST7735::Register::MadCtl), 1, 0x40, 0,
    0,
};

/*
 * @brief   ...
 */
uint8_t LcdST7735::viewport_seq[] = {
    static_cast<uint8_t>(LcdST7735::Register::RaSet), 4, 0x00, 0x00, 0x00, 0x9F, 0,
    static_cast<uint8_t>(LcdST7735::Register::CaSet), 4, 0x00, 0x00, 0x00, 0x7F, 0,
    static_cast<uint8_t>(LcdST7735::Register::RamWr), 0, 0,
    0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   ...
 */
inline void LcdST7735::assert_cs(void) {
    // take spi bus and assert chip select
    //spiAcquireBus(this->spi_drv);
    palClearPad(this->cs_port, this->cs_pin);
}

/*
 * @brief   ...
 */
inline void LcdST7735::release_cs(void) {
    // release chip select and spi  bus
    palSetPad(this->cs_port, this->cs_pin);
    //spiReleaseBus(this->spi_drv);
}

/*
 * @brief   ...
 */
inline void LcdST7735::assert_dc(void) {
    palClearPad(this->dc_port, this->dc_pin);
}

/*
 * @brief   ...
 */
inline void LcdST7735::release_dc(void) {
    palSetPad(this->dc_port, this->dc_pin);
}

/*
 * @brief   ...
 */
inline void LcdST7735::assert_rst(void) {
    palClearPad(this->rst_port, this->rst_pin);
}

/*
 * @brief   ...
 */
inline void LcdST7735::release_rst(void) {
    palSetPad(this->rst_port, this->rst_pin);
}

/*
 * @brief   ...
 * @details ...
 */
void LcdST7735::sendCommand(const uint8_t seq[]) {
    //
    uint16_t i = 0, argc = 0, delay = 0;
    // process sequence
    do {
        // send command
        this->assert_dc();
        // assert chip select
        this->assert_cs();
        // send command
        spiSend(this->spi_drv, 1, &seq[i++]);
        // send arguments if any
        argc = seq[i++];
        if (argc > 0) {
            this->release_dc();
            spiSend(this->spi_drv, argc, &seq[i]);
            i += argc;
        }
        // wwait if necessary
        delay = seq[i++];
        if (delay > 0) {
            //consoleDebug("delay: %d\r\n", delay);
            chThdSleepMilliseconds(delay);
        }
        // reselese chip select
        this->release_cs();
    } while (seq[i] != 0);
};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
LcdST7735::LcdST7735(uint16_t width, uint16_t height,
                     uint16_t page_width, uint16_t page_height,
                     SPIDriver *spi_drv, SPIConfig *spi_cfg,
                     ioportid_t cs_port, uint16_t cs_pin,
                     ioportid_t dc_port, uint16_t dc_pin,
                     ioportid_t rst_port, uint16_t rst_pin) : Lcd(width, height, page_width, page_height) {
    //::Lcd(width, height, page_width, page_height);
    this->spi_drv = spi_drv;
    this->spi_cfg = spi_cfg;
    this->cs_port = cs_port;
    this->cs_pin = cs_pin;
    this->dc_port = dc_port;
    this->dc_pin = dc_pin;
    this->rst_port = rst_port;
    this->rst_pin = rst_pin;
    ithacaUnlock(&this->spi_lock);
}

/*
 * @brief   ...
 * @details ...
 */
void LcdST7735::init(void) {
    // initialize base class
    Lcd::init();
    // spi validation
    if (this->spi_drv == NULL) {
        return;
    }
    // io ports
    palSetPadMode(this->cs_port, this->cs_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(this->dc_port, this->dc_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(this->rst_port, this->rst_pin, PAL_MODE_OUTPUT_PUSHPULL);
    // reset chip
    palClearPad(this->rst_port, this->rst_pin);
    chThdSleepMilliseconds(5);
    palSetPad(this->rst_port, this->rst_pin);
    // send power on sequence
    this->sendCommand(this->init_seq);
    // set default rotation
    this->setRotation(Rotation::Normal);
};

/*
 * @brief   ...
 * @details ...
 */
void LcdST7735::setRotation(Rotation r) {
    Lcd::setRotation(r);
    //
    switch (this->rotation) {
        case Lcd::Rotation::Normal:
            this->rotation_seq[2] = 0x00;
            break;
        case Lcd::Rotation::Right:
            this->rotation_seq[2] = 0x60;
            break;
        case Lcd::Rotation::Flip:
            this->rotation_seq[2] = 0xC0;
            break;
        case Lcd::Rotation::Left:
            this->rotation_seq[2] = 0xA0;
            break;
    }
    this->sendCommand(this->rotation_seq);
}

//
void LcdST7735::flushEnd(void) {
    this->release_cs();
    this->spi_cfg->end_cb = NULL;
    ithacaUnlockISR(&this->spi_lock);
}

//
void ST7735SpiCB(SPIDriver *spip) {
    LcdST7735 *st7735 = (LcdST7735 *)spip->user_ptr;
    st7735->flushEnd();
}

/*
 * @brief   ...
 * @details ...
 */
bool LcdST7735::flushPage(void) {
    //
    if (Lcd::flushPage() == false) {
        return false;
    }
    //
    if (ithacaLockTimeout(&this->spi_lock, 50) == false) {
        return false;
    }
    //
    switch (this->rotation) {
        case Lcd::Rotation::Normal:
        case Lcd::Rotation::Flip:
            //
            this->viewport_seq[3] = this->page_ys;
            this->viewport_seq[5] = this->page_ye;
            this->viewport_seq[10] = this->page_xs;
            this->viewport_seq[12] = this->page_xe;
            break;
        case Lcd::Rotation::Right:
        case Lcd::Rotation::Left:
            //
            this->viewport_seq[3] = this->page_ys;
            this->viewport_seq[5] = this->page_ye;
            this->viewport_seq[10] = this->page_xs;
            this->viewport_seq[12] = this->page_xe;
            break;
    }
    this->sendCommand(this->viewport_seq);
    // assert chip select, data will be sent
    this->release_dc();
    this->assert_cs();
    //
    this->spi_drv->user_ptr = this;
    this->spi_cfg->end_cb = ST7735SpiCB;
    // send data using hardware spi
    spiStartSend(this->spi_drv, this->raw_page_size, this->page_buffer_flush);
    //
    return true;
};

#endif /* ITHACA_USE_LCD_ST7735 */


