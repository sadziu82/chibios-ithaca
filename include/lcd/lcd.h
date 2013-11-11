#ifndef _ST7735R_H__
#define _ST7735R_H__

/*
 * @brief   Radio library
 */

#include "ch.h"
#include "hal.h"
#include "chsprintf.h"
#include "font.hpp"


/*
 * @brief   SPI driver to use for this radio
 */
#define ST7735R_SPI          SPID2

/*
 * @brief   Radio spi/rst/irq config
 */
#define ST7735R_RST_PORT     GPIOA
#define ST7735R_RST_PIN      9
#define ST7735R_DC_PORT      GPIOA
#define ST7735R_DC_PIN       10
#define ST7735R_CE_PORT      GPIOB
#define ST7735R_CE_PIN       12

#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

/*
 * @brief   Base display class
 */
typedef uint8_t LcdRotation;
typedef uint16_t LcdColor;
typedef uint8_t LcdAlign;
/*
 * @brief   Base display class
 */
class Lcd {
public:
    /*
    * @brief   Allowed LCD rotation
    */
    class Rotation {
    public:
        static const LcdRotation Normal = 0;
        static const LcdRotation MirrorX = 1;
        static const LcdRotation MirrorY = 2;
        static const LcdRotation Flip = 3;
    };
    /*
    * @brief   Allowed LCD Colors
    */
    class Color {
    public:
        static const LcdColor Transparent = 0x0821;
        static const LcdColor Black = 0x0000;
        static const LcdColor Yellow = 0x87FF;
        static const LcdColor Red = 0x00F8;
        static const LcdColor Green = 0xE007;
        static const LcdColor Blue = 0x1F00;
        static const LcdColor LightBlue = 0x0F00;
        static const LcdColor White = 0xFFFF;
    };
    class Align {
    public:
        static const LcdAlign Left = 0;
        static const LcdAlign Center = 1;
        static const LcdAlign Right = 2;
    };
    void draw_circle( uint16_t x, uint16_t y, uint16_t r,
                              LcdColor color );
    virtual void draw_pixel( uint16_t viewport_x, uint16_t viewport_y,
                     LcdColor color );
    void set_font( Font *font );
    uint16_t get_string_width( char *s, Font *f );
    uint16_t draw_string( uint16_t x, uint16_t y, char *s,
                          LcdColor color, LcdColor bg = Lcd::Color::Transparent, LcdAlign a = Lcd::Align::Left );
    uint8_t draw_char( uint16_t x, uint16_t y, char c,
                     LcdColor fg, LcdColor bg = Lcd::Color::Transparent );
    bool add_paint_function( void (func)( Lcd *, bool ) );
    Lcd( void );
protected:
    bool in_viewport( uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye );
    LcdColor bg_color;
    uint16_t viewport_x1;
    uint16_t viewport_x2;
    uint16_t viewport_y1;
    uint16_t viewport_y2;
    Font *font;
    static const uint8_t paint_functions_number = 5;;
    void (*paint_functions[5])(Lcd *, bool);
};

/*
 * @brief   class for displays with spi interface
 */
class LcdSpi: public Lcd {
public:
    bool init( SPIDriver *spi_drv,
               ioportid_t cs_port, uint16_t cs_pin );
protected:
    void acquire_bus( void );
    void release_bus( void );
    void assert_cs( void );
    void release_cs( void );
    void send_to_lcd( uint8_t *data, uint16_t len = 1 );
private:
    SPIDriver *spi_drv;
    ioportid_t cs_port;
    uint16_t cs_pin;
};

/*
 * @brief   Display driver for ST7735R
 */
class LcdST7735R: public LcdSpi {
public:
    bool init( SPIDriver *spi_drv,
               ioportid_t ce_port, uint16_t ce_pin,
               ioportid_t dc_port, uint16_t dc_pin,
               ioportid_t rst_port, uint16_t rst_pin );
    void set_rotation( LcdRotation rotation );
    void draw_pixel( uint16_t viewport_x, uint16_t viewport_y,
                     LcdColor color );
    void update_lcd( void );
    void set_bg_color( LcdColor color );
private:
    // framebuffer
    static const uint16_t lcd_width = 160;
    static const uint16_t lcd_height = 128;
    static const uint16_t framebuffer_width = 32;
    static const uint16_t framebuffer_height = 32;
    static const uint16_t framebuffer_size = LcdST7735R::framebuffer_width * LcdST7735R::framebuffer_height;
    uint16_t framebuffer[ LcdST7735R::framebuffer_size ];
    // command sequences
    static uint8_t init_sequence[];
    static uint8_t viewport_sequence[];
    static uint8_t pixel_sequence[];
    ioportid_t dc_port;
    uint16_t dc_pin;
    ioportid_t rst_port;
    uint16_t rst_pin;
    void assert_dc( void );
    void release_dc( void );
    void assert_rst( void );
    void release_rst( void );
    void send_commands_sequence( uint8_t[] );
    void set_viewport( uint16_t viewport_x, uint16_t viewport_y,
                       uint16_t viewport_w, uint16_t viewport_h );
    void prepare_framebuffer( void );
    void send_framebuffer( void );
    //void send_prepared_data( void );
    void paint_screen( bool kay_frame );
    systime_t time_start, time_end;
};
/*
 * @brief   RFM12B ...
 */
void test( void );

#endif /* _ST7735R_H__ */

