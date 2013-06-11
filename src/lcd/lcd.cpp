/*
 * @brief   ...
 */

#include "lcd.hpp"

void Lcd::set_font( Font *font ) {
    this->font = font;
}

Lcd::Lcd( void ) {
    uint8_t i = 0;
    for ( i = 0; i < this->paint_functions_number; i++ ) {
        this->paint_functions[ i ] = NULL;
    }
}

void Lcd::draw_pixel( uint16_t x, uint16_t y,
                             LcdColor color ) {
}

bool Lcd::in_viewport( uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye ) {
    if ( ( xs <= this->viewport_x2 ) && ( this->viewport_x1 <= xe ) &&
         ( ys <= this->viewport_y2 ) && ( this->viewport_y1 <= ye ) ) {
        return true;
    } else {
        return false;
    }
}

uint16_t Lcd::get_string_width( char *s, Font *f ) {
    uint16_t len = 0;
    while ( *s != '\0' ) {
        len += f->get_char_width( *s );
        s++;
    }
    return len;
}

uint16_t Lcd::draw_string( uint16_t x, uint16_t y, char *s,
                           LcdColor fg, LcdColor bg, LcdAlign align ) {
    //
    uint16_t length;
    length = this->get_string_width( s, this->font );
    // if ( not this->in_viewport( x, y, x + length - 1, y + this->font->get_font_height() ) ) {
    //     return 0;
    // }
    if ( align == Lcd::Align::Center ) {
        x -= length / 2;
    } else if ( align == Lcd::Align::Right ) {
        x -= length;
    }
    // 
    while ( *s != '\0' ) {
        x += this->draw_char( x, y, *s, fg, bg );
        s++;
    }
    return x;
}

uint8_t Lcd::draw_char( uint16_t x, uint16_t y, char c,
                        LcdColor fg, LcdColor bg ) {
    // 
    uint8_t font_width, font_height, char_width;
    uint8_t *char_bitmap, m;
    uint8_t row, column, column_offset, x_start;
    //
    if ( font == NULL ) {
        return 0;
    }
    //
    font_width = this->font->get_font_width();
    font_height = this->font->get_font_height();
    char_width = this->font->get_char_width( c );
    if ( char_width == 0 ) {
        return 0;
    }
    // check if we are in the current viewport or char is not in this font
    if ( not this->in_viewport( x, y, x + char_width - 1, y + font_height - 1 ) ) {
        return char_width;
    }
    //
    char_bitmap = this->font->get_char_bitmap( c );
    x_start = x;
    m = 0;
    //
    for ( row = 0; row < font_height; row++ ) {
        column_offset = 0;
        for ( column = 0; column < char_width; column++ ) {
            if ( column % 8 == 0 ) {
                m = *(char_bitmap + column_offset++);
            }
            if ( m & 0x80 ) {
                draw_pixel( x, y, fg );
            } else if ( bg != Lcd::Color::Transparent ) {
                draw_pixel( x, y, bg );
            }
            m <<= 1;
            x += 1;
        }
        char_bitmap += font_width >> 3;
        y += 1;
        x = x_start;
    }
    return char_width;
}

/*
 * @brief   Class for SPI based LCD
 */
bool LcdSpi::init( SPIDriver *spi_drv,
                   ioportid_t cs_port, uint16_t cs_pin ) {
    this->spi_drv = spi_drv;
    this->cs_port = cs_port;
    this->cs_pin = cs_pin;
    return true;
}

inline void LcdSpi::acquire_bus( void ) {
    spiAcquireBus( this->spi_drv );
}

inline void LcdSpi::release_bus( void ) {
    spiReleaseBus( this->spi_drv );
}

inline void LcdSpi::assert_cs( void ) {
    palClearPad( this->cs_port, this->cs_pin );
}

inline void LcdSpi::release_cs( void ) {
    palSetPad( this->cs_port, this->cs_pin );
}

inline void LcdSpi::send_to_lcd( uint8_t *data, uint16_t len ) {
    spiSend( this->spi_drv, len, data );
}


/*
 * @brief   Display driver for ST7735R
 */
bool LcdST7735R::init( SPIDriver *spi_drv,
               ioportid_t ce_port, uint16_t ce_pin,
               ioportid_t dc_port, uint16_t dc_pin,
               ioportid_t rst_port, uint16_t rst_pin ) {
    // fail if spi fails to initialize
    if ( not LcdSpi::init( spi_drv, ce_port, ce_pin ) ) {
        return false;
    }
    // this->init_sequenceure pins
    this->dc_port = dc_port;
    this->dc_pin = dc_pin;
    this->rst_port = rst_port;
    this->rst_pin = rst_pin;
    // take spi bus
    this->acquire_bus();
    // reset chip
    this->assert_rst();
    chThdSleepMilliseconds( 5 );
    this->release_rst();
    // power on sequence
    this->send_commands_sequence( this->init_sequence );
    // release spi bus
    this->release_bus();
    return true;
}

void LcdST7735R::set_rotation( LcdRotation rotation ) {
}

// void LcdST7735R::clear_screen( LcdColor color ) {
//     this->acquire_bus();
//     this->set_viewport( 0, 0, 0xA0, 0x10 );
//     this->send_prepared_data();
//     this->release_bus();
// }

uint8_t LcdST7735R::init_sequence[] = {
    ST7735_SWRESET, 0, 50,
    ST7735_SLPOUT, 0, 50,
    ST7735_FRMCTR1, 3, 0x01, 0x2C, 0x2D, 0,
    ST7735_FRMCTR2, 3, 0x01, 0x2C, 0x2D, 0,
    ST7735_FRMCTR3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, 0,
    ST7735_INVCTR, 1, 0x07, 0,
    ST7735_PWCTR1, 3, 0xA2, 0x02, 0x84, 0,
    ST7735_PWCTR2, 1, 0xC5, 0,
    ST7735_PWCTR3, 2, 0x0A, 0x00, 0,
    ST7735_PWCTR4, 2, 0x8A, 0x2A, 0,
    ST7735_PWCTR5, 2, 0x8A, 0xEE, 0,
    ST7735_VMCTR1, 1, 0x0E, 0,
    ST7735_INVOFF, 0, 0,
    ST7735_MADCTL, 1, 0x44, 0,
    ST7735_COLMOD, 1, 0x05, 0,
    //ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x7F, 0,
    //ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x9F, 0,
    ST7735_GMCTRP1, 16,
          0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 
          0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, 
        0,
    ST7735_GMCTRN1, 16,
          0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
          0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
        5,
    //  last
    ST7735_NORON, 0, 10,
    ST7735_DISPON, 0, 20,
    0,
};

uint8_t LcdST7735R::viewport_sequence[] = {
    ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x9F, 0,
    ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x7F, 0,
    ST7735_RAMWR, 0, 0,
    0,
};

uint8_t LcdST7735R::pixel_sequence[] = {
    ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x00, 0,
    ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x00, 0,
    ST7735_RAMWR, 2, 0xFF, 0xFF, 0,
    0,
};

inline void LcdST7735R::assert_dc( void ) {
    palClearPad( this->dc_port, this->dc_pin );
}

inline void LcdST7735R::release_dc( void ) {
    palSetPad( this->dc_port, this->dc_pin );
}

inline void LcdST7735R::assert_rst( void ) {
    palClearPad( this->rst_port, this->rst_pin );
}

inline void LcdST7735R::release_rst( void ) {
    palSetPad( this->rst_port, this->rst_pin );
}

void LcdST7735R::send_commands_sequence( uint8_t sequence[] ) {
    //
    uint16_t i = 0, argc = 0, delay = 0;
    // process sequence
    do {
        // assert chip select
        this->assert_cs();
        // send command
        this->assert_dc();
        this->send_to_lcd( &sequence[ i++ ] );
        // send arguments if any
        argc = sequence[ i++ ];
        if ( argc > 0 ) {
            this->release_dc();
            this->send_to_lcd( &sequence[ i ], argc );
            i += argc;
        }
        // wwait if necessary
        delay = sequence[ i++ ];
        if ( delay > 0 ) {
            chThdSleepMilliseconds( delay );
        }
        // reselese chip select
        this->release_cs();
    } while ( sequence[ i ] != 0 );
}

void LcdST7735R::set_viewport( uint16_t x,
                               uint16_t y,
                               uint16_t w,
                               uint16_t h ) {
    this->viewport_x1 = x;
    this->viewport_x2 = x + w - 1;
    this->viewport_y1 = y;
    this->viewport_y2 = y + h - 1;
    this->viewport_sequence[ 3 ] = this->viewport_x1;
    this->viewport_sequence[ 5 ] = this->viewport_x2;
    this->viewport_sequence[ 10 ] = this->viewport_y1;
    this->viewport_sequence[ 12 ] = this->viewport_y2;
    this->send_commands_sequence( this->viewport_sequence );
}

void LcdST7735R::draw_pixel( uint16_t x, uint16_t y,
                             LcdColor color ) {
    uint16_t pos;
    if ( this->in_viewport( x, y, x, y ) ) {
        x -= this->viewport_x1;
        y -= this->viewport_y1;
        pos = x * this->framebuffer_height + y;
        this->framebuffer[ pos ] = color;
    }
}

typedef int16_t coord_t;

void Lcd::draw_circle( uint16_t x, uint16_t y, uint16_t r,
                              LcdColor color ) {
        coord_t a, b, P;

        a = 0;
        b = r;
        P = 1 - r;

        do {
            this->draw_pixel(x+a, y+b, color);
            this->draw_pixel(x+b, y+a, color);
            this->draw_pixel(x-a, y+b, color);
            this->draw_pixel(x-b, y+a, color);
            this->draw_pixel(x+b, y-a, color);
            this->draw_pixel(x+a, y-b, color);
            this->draw_pixel(x-a, y-b, color);
            this->draw_pixel(x-b, y-a, color);
            if (P < 0)
                P += 3 + 2*a++;
            else
                P += 5 + 2*(a++ - b--);
        } while(a <= b);

}

void LcdST7735R::prepare_framebuffer( void ) {
    uint16_t i;
    for ( i = 0; i < this->framebuffer_size; i++ ) {
        this->framebuffer[ i ] = this->bg_color;
    }
}

void LcdST7735R::set_bg_color( LcdColor color ) {
    this->bg_color = color;
}

void LcdST7735R::send_framebuffer( void ) {
    // assert chip select, data will be sent
    this->time_start = chTimeNow();
    this->release_dc();
    this->assert_cs();
    // 
    this->send_to_lcd( (uint8_t *)this->framebuffer,
                       this->framebuffer_size * 2 );
    // release chip select
    this->release_cs();
    this->time_end = chTimeNow();
}

bool Lcd::add_paint_function( void (func)( Lcd *, bool ) ) {
    this->paint_functions[ 0 ] = func;
    return true;
}

void LcdST7735R::update_lcd( void ) {
    uint8_t x, y, i;
    bool key_frame = true;
    for ( y = 0; y < this->lcd_height; y += this->framebuffer_height ) {
        for ( x = 0; x < this->lcd_width; x += this->framebuffer_width ) {
            this->set_viewport( x, y, this->framebuffer_width, this->framebuffer_height );
            this->prepare_framebuffer();
            this->paint_screen( key_frame );
            i = 0;
            while ( this->paint_functions[ i ] != NULL ) {
                (*this->paint_functions[ i++ ])( this, key_frame );
            }
            this->send_framebuffer();
            key_frame = false;
        }
    }
}

