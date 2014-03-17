#ifndef _LCD_ST7735_HPP_
#define _LCD_ST7735_HPP_


#if ITHACA_USE_LCD_ST7735 || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
enum {
    //
    ST7735_NOP = 0x00,
    ST7735_SWRESET = 0x01,
    ST7735_RDDID = 0x04,
    ST7735_RDDST = 0x09,
    //
    ST7735_SLPIN = 0x10,
    ST7735_SLPOUT = 0x11,
    ST7735_PTLON = 0x12,
    ST7735_NORON = 0x13,
    //
    ST7735_INVOFF = 0x20,
    ST7735_INVON = 0x21,
    ST7735_DISPOFF = 0x28,
    ST7735_DISPON = 0x29,
    ST7735_CASET = 0x2A,
    ST7735_RASET = 0x2B,
    ST7735_RAMWR = 0x2C,
    ST7735_RAMRD = 0x2E,
    //
    ST7735_PTLAR = 0x30,
    ST7735_COLMOD = 0x3A,
    ST7735_MADCTL = 0x36,
    //
    ST7735_FRMCTR1 = 0xB1,
    ST7735_FRMCTR2 = 0xB2,
    ST7735_FRMCTR3 = 0xB3,
    ST7735_INVCTR = 0xB4,
    ST7735_DISSET5 = 0xB6,
    //
    ST7735_PWCTR1 = 0xC0,
    ST7735_PWCTR2 = 0xC1,
    ST7735_PWCTR3 = 0xC2,
    ST7735_PWCTR4 = 0xC3,
    ST7735_PWCTR5 = 0xC4,
    ST7735_VMCTR1 = 0xC5,
    //
    ST7735_RDID1 = 0xDA,
    ST7735_RDID2 = 0xDB,
    ST7735_RDID3 = 0xDC,
    ST7735_RDID4 = 0xDD,
    //
    ST7735_PWCTR6 = 0xFC,
    //
    ST7735_GMCTRP1 = 0xE0,
    ST7735_GMCTRN1 = 0xE1,
    //
    ST7735_MY = 0x80,
    ST7735_MX = 0x40,
    ST7735_MV = 0x20,
    ST7735_ML = 0x10,
    ST7735_RGB = 0x00,
    ST7735_BGR = 0x08,
    ST7735_MH = 0x04,
};

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
enum class LcdColor {
    Black = 0x0000,
    Red = 0x00F8,
    Green = 0xE007,
    Blue = 0x1F00,
    White = 0xFFFF,
};

/*
 * @brief   ...
 * @details ...
 */
class LcdST7735: public Lcd {
    protected:
        // registers
        enum class Register;
        // hardware
        SPIDriver *spi_drv;
        SPIConfig *spi_cfg;
        ioportid_t cs_port;
        uint16_t cs_pin;
        ioportid_t dc_port;
        uint16_t dc_pin;
        ioportid_t rst_port;
        uint16_t rst_pin;
        //
        // hardware related
        void assert_cs(void);
        void release_cs(void);
        void assert_dc(void);
        void release_dc(void);
        void assert_rst(void);
        void release_rst(void);
        // ~const
        static const uint8_t init_seq[];
        static uint8_t rotation_seq[];
        static uint8_t viewport_seq[];
        // functions
        virtual void sendCommand(const uint8_t seq[]);
        virtual bool flushPage(void);
        ithaca_lock_t spi_lock;
    public:
        void flushEnd(void);
        LcdST7735(uint16_t width, uint16_t height,
                  uint16_t page_width, uint16_t page_height,
                  SPIDriver *spi_drv, SPIConfig *spi_cfg,
                  ioportid_t cs_port, uint16_t cs_pin,
                  ioportid_t dc_port, uint16_t dc_pin,
                  ioportid_t rst_port, uint16_t rst_pin);
        void init(void);
        void setRotation(Rotation r);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_LCD_ST7735 */

#endif /* _LCD_ST7735_HPP_ */

