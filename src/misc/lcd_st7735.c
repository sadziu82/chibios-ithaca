#include <ithaca.h>

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
uint8_t lcd_st7735_init_sequence[] = {
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
    ST7735_MADCTL, 1, 0x60, 0,
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

/*
 * @brief   ...
 */
uint8_t lcd_st7735_set_viewport_sequence[] = {
    ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x9F, 0,
    ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x7F, 0,
    ST7735_RAMWR, 0, 0,
    0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   ...
 */
inline void lcdST7735AssertCS(lcd_st7735_t *lcd) {
    palClearPad(lcd->cs_port, lcd->cs_pin);
}

/*
 * @brief   ...
 */
inline void lcdST7735ReleaseCS(lcd_st7735_t *lcd) {
    palSetPad(lcd->cs_port, lcd->cs_pin);
}

/*
 * @brief   ...
 */
inline void lcdST7735AssertDC(lcd_st7735_t *lcd) {
    palClearPad(lcd->dc_port, lcd->dc_pin);
}

/*
 * @brief   ...
 */
inline void lcdST7735ReleaseDC(lcd_st7735_t *lcd) {
    palSetPad(lcd->dc_port, lcd->dc_pin);
}

/*
 * @brief   ...
 */
inline void lcdST7735AssertRST(lcd_st7735_t *lcd) {
    palClearPad(lcd->rst_port, lcd->rst_pin);
}

/*
 * @brief   ...
 */
inline void lcdST7735ReleaseRST(lcd_st7735_t *lcd) {
    palSetPad(lcd->rst_port, lcd->rst_pin);
}

/*
 * @brief   ...
 * @details ...
 */
bool lcdST7735SendCommand(lcd_st7735_t *lcd, uint8_t command_sequence[]) {
    //
    uint16_t i = 0, argc = 0, delay = 0;
    // process sequence
    do {
        // assert chip select
        lcdST7735AssertCS(lcd);
        // send command
        lcdST7735AssertDC(lcd);
        spiSend(lcd->spi_driver, 1, &command_sequence[i++]);
        // send arguments if any
        argc = command_sequence[i++];
        if (argc > 0) {
            lcdST7735ReleaseDC(lcd);
            spiSend(lcd->spi_driver, argc, &command_sequence[i]);
            i += argc;
        }
        // wwait if necessary
        delay = command_sequence[i++];
        if (delay > 0) {
            chThdSleepMilliseconds(delay);
        }
        // reselese chip select
        lcdST7735ReleaseCS(lcd);
    } while (command_sequence[i] != 0);
    //
    return true;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/
    
/*
 * @brief   ...
 * @details ...
 */
void lcdST7735DataWrite(lcd_st7735_t *lcd, uint16_t len, uint16_t data[]) {
    // assert chip select, data will be sent
    lcdST7735ReleaseDC(lcd);
    lcdST7735AssertCS(lcd);
    // send data using hardware spi
    spiSend(lcd->spi_driver, len, data);
    // release chip select
    lcdST7735ReleaseCS(lcd);
}

/*
 * @brief   ...
 * @details ...
 */
void lcdST7735SetViewport(lcd_st7735_t *lcd,
                          uint16_t y, uint16_t x,
                          uint16_t h, uint16_t w) {
    lcd->viewport_x1 = x;
    lcd->viewport_x2 = x + w - 1;
    lcd->viewport_y1 = y;
    lcd->viewport_y2 = y + h - 1;
    lcd_st7735_set_viewport_sequence[ 3 ] = lcd->viewport_x1;
    lcd_st7735_set_viewport_sequence[ 5 ] = lcd->viewport_x2;
    lcd_st7735_set_viewport_sequence[ 10 ] = lcd->viewport_y1;
    lcd_st7735_set_viewport_sequence[ 12 ] = lcd->viewport_y2;
    lcdST7735SendCommand(lcd, lcd_st7735_set_viewport_sequence);
}

/*
 * @brief   ...
 * @details ...
 */
void lcdST7735PutPixel(lcd_st7735_t *lcd,
                uint16_t x, uint16_t y, lcd_color_t color) {
    //
    uint16_t offset;
    //
    if ((x < lcd->framebuffer_x1) ||
        (x > lcd->framebuffer_x2) ||
        (y < lcd->framebuffer_y1) ||
        (y > lcd->framebuffer_y2)) {
        return;
    }
    //
    offset = (y - lcd->framebuffer_y1) * LCD_FRAMEBUFFER_WIDTH +
             (x - lcd->framebuffer_x1);
    lcd->shadow_framebuffer[offset] = color;
}

/*
 * @brief   ...
 * @details ...
 */
void lcdST7735ClearScreen(lcd_st7735_t *lcd, uint16_t color) {
    //
    uint16_t i;
    uint16_t row[160];
    // prepare one row of data
    for (i = 0; i < 160; i++) {
        row[i] = color;
    }
    // set viewport and send data
    lcdST7735SetViewport(lcd, 0, 0, 160, 128);
    for (i = 0; i < 128; i++) {
        lcdST7735DataWrite(lcd, sizeof(row), row);
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool lcdST7735Init(lcd_st7735_t *lcd) {
    //
    lcd->framebuffer_x1 = 0;
    lcd->framebuffer_x2 = LCD_FRAMEBUFFER_WIDTH - 1;
    lcd->framebuffer_y1 = 0;
    lcd->framebuffer_y2 = LCD_FRAMEBUFFER_HEIGHT - 1;
    lcd->active_framebuffer = lcd->framebuffer1;
    lcd->shadow_framebuffer = lcd->framebuffer2;
    //
    memset(lcd->framebuffer1, 0, LCD_FRAMEBUFFER_SIZE * sizeof(lcd_pixel_t));
    memset(lcd->framebuffer1, 0, LCD_FRAMEBUFFER_SIZE * sizeof(lcd_pixel_t));
    // take spi bus
    spiAcquireBus(lcd->spi_driver);
    // reset chip
    lcdST7735AssertRST(lcd);
    chThdSleepMilliseconds( 5 );
    lcdST7735ReleaseRST(lcd);
    consoleWarn("initializing lcd\r\n");
    // power on sequence
    lcdST7735SendCommand(lcd, lcd_st7735_init_sequence);
    lcdST7735ClearScreen(lcd, 0x0000);
    // 
    consoleWarn("commands send\r\n");
    // release spi bus
    spiReleaseBus(lcd->spi_driver);
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
uint8_t lcdST7735DrawChar(lcd_st7735_t *lcd, uint16_t x, uint16_t y,
                          char c) {
    uint8_t width, height, font_width, *bitmap;
    uint8_t row, column, column_offset, m;
    uint16_t offset, x_start;
    //
    height = fontGetHeight(lcd->font);
    width = fontGetCharWidth(lcd->font, c);
    // in active frame buffer?
    if ((x + width - 1 < lcd->framebuffer_x1) ||
        (x > lcd->framebuffer_x2) ||
        (y + height - 1 < lcd->framebuffer_y1) ||
        (y > lcd->framebuffer_y2)) {
        return width;
    }
    //
    m = 0;
    font_width = fontGetWidth(lcd->font);
    bitmap = fontGetCharBitmap(lcd->font, c);
    //
    for (row = 0; row < height; row++) {
        if ((y >= lcd->framebuffer_y1) &&
            (y <= lcd->framebuffer_y2)) {
            // offset at the beggining
            offset = (y - lcd->framebuffer_y1) * LCD_FRAMEBUFFER_WIDTH + (x - lcd->framebuffer_x1);
            column_offset = 0;
            for (column = 0; column < width; column++) {
                if (column % 8 == 0) {
                    m = *(bitmap + column_offset++);
                }
                if ((x + column >= lcd->framebuffer_x1) &&
                    (x + column <= lcd->framebuffer_x2)) {
                    if (m & 0x80) {
                        lcd->shadow_framebuffer[offset] = lcd->fg_color;
                    //} else if (bg != Lcd::Color::Transparent) {
                    } else {
                        //buffer[row * width + column] = lcd->bg_color;
                        //lcd->shadow_framebuffer[offset] = 0;
                    }
                }
                offset++;
                m <<= 1;
            }
        }
        bitmap += font_width >> 3;
        y++;
    }
    //lcdST7735SetViewport(lcd, x, y, width, height);
    //lcdST7735DataWrite(lcd, height * width * 2, buffer);
    return width;
}

uint16_t lcdGetStringWidth(char *s, font_t *font) {
    uint16_t len = 0;
    while ( *s != '\0' ) {
        len += fontGetCharWidth(font, *s);
        s++;
    }
    return len;
}

uint16_t lcdST7735DrawString(lcd_st7735_t *lcd, uint16_t x, uint16_t y,
                             char *s, lcd_align_t align) {
    //
    uint16_t length;
    length = lcdGetStringWidth(s, lcd->font);
    // if ( not this->in_viewport( x, y, x + length - 1, y + this->font->get_font_height() ) ) {
    //     return 0;
    // }
    if (align == LCD_ALIGN_CENTER) {
        x -= length / 2;
    } else if (align == LCD_ALIGN_RIGHT) {
        x -= length;
    }
    // 
    while ( *s != '\0' ) {
        x += lcdST7735DrawChar(lcd, x, y, *s);
        s++;
    }
    return x;
}

/*
 * @brief   ...
 * @details ...
 */
bool lcdSetFgColor(lcd_st7735_t *lcd, varg_t arg) {
    // 
    if (lcd != NULL) {
        lcd->fg_color = (lcd_color_t)arg;
    }
    // by default return false
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool lcdSetBgColor(lcd_st7735_t *lcd, varg_t arg) {
    // 
    if (lcd != NULL) {
        lcd->bg_color = (lcd_color_t)arg;
    }
    // by default return false
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool lcdSetFont(lcd_st7735_t *lcd, varg_t arg) {
    // 
    if (lcd != NULL) {
        lcd->font = (font_t *)arg;
    }
    // by default return false
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool lcdST7735FramebufferSwap(lcd_st7735_t *lcd, varg_t arg) {
    // 
    (void)arg;
    //
    if (lcd->active_framebuffer == lcd->framebuffer1) {
        lcd->active_framebuffer = lcd->framebuffer2;
        lcd->shadow_framebuffer = lcd->framebuffer1;
    } else {
        lcd->active_framebuffer = lcd->framebuffer1;
        lcd->shadow_framebuffer = lcd->framebuffer2;
    }
    //
    lcdST7735SetViewport(lcd,
                         lcd->framebuffer_x1, lcd->framebuffer_y1,
                         LCD_FRAMEBUFFER_WIDTH, LCD_FRAMEBUFFER_HEIGHT);
    lcdST7735DataWrite(lcd,
                       LCD_FRAMEBUFFER_SIZE * sizeof(lcd_pixel_t),
                       lcd->active_framebuffer);
    // 
    lcd->framebuffer_x1 += LCD_FRAMEBUFFER_WIDTH;
    lcd->framebuffer_x2 += LCD_FRAMEBUFFER_WIDTH;
    if (lcd->framebuffer_x1 >= LCD_ST7735_WIDTH) {
        lcd->framebuffer_x1 = 0;
        lcd->framebuffer_x2 = LCD_FRAMEBUFFER_WIDTH - 1;
        // 
        lcd->framebuffer_y1 += LCD_FRAMEBUFFER_HEIGHT;
        lcd->framebuffer_y2 += LCD_FRAMEBUFFER_HEIGHT;
        if (lcd->framebuffer_y1 >= LCD_ST7735_HEIGHT) {
            lcd->framebuffer_y1 = 0;
            lcd->framebuffer_y2 = LCD_FRAMEBUFFER_HEIGHT - 1;
        }
    }
    // FIXME allow custom background 
    memset(lcd->shadow_framebuffer, 0, LCD_FRAMEBUFFER_SIZE * sizeof(lcd_pixel_t));
    // by default return false
    return true;
}

#endif /* ITHACA_USE_LCD_ST7735 */

