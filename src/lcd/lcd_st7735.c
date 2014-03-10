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
uint8_t st7735_lld_lcd_init_sequence[] = {
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
    //ST7735_MADCTL, 1, 0x40, 0,
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
uint8_t st7735_lld_set_rotation_sequence[] = {
    ST7735_MADCTL, 1, 0x40, 0,
    0,
};

/*
 * @brief   ...
 */
uint8_t st7735_lld_set_viewport_sequence[] = {
    ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x9F, 0,
    ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x7F, 0,
    ST7735_RAMWR, 0, 0,
    0,
};

/*
 * @brief   ...
 */
uint8_t st7735_lld_put_pixel_sequence[] = {
    ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x9F, 0,
    ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x7F, 0,
    ST7735_RAMWR, 2, 0xFF, 0xFF, 0,
    0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   ...
 */
inline void st7735_lld_assert_cs(ST7735Driver *st7735) {
    // take spi bus
    spiAcquireBus(st7735->config->spi_drv);
    // assert CS
    palClearPad(st7735->config->cs_port, st7735->config->cs_pin);
}

/*
 * @brief   ...
 */
inline void st7735_lld_release_cs(ST7735Driver *st7735) {
    // release CS
    palSetPad(st7735->config->cs_port, st7735->config->cs_pin);
    // release spi bus
    spiReleaseBus(st7735->config->spi_drv);
}

/*
 * @brief   ...
 */
inline void st7735_lld_assert_dc(ST7735Driver *st7735) {
    palClearPad(st7735->config->dc_port, st7735->config->dc_pin);
}

/*
 * @brief   ...
 */
inline void st7735_lld_release_dc(ST7735Driver *st7735) {
    palSetPad(st7735->config->dc_port, st7735->config->dc_pin);
}

/*
 * @brief   ...
 */
inline void st7735_lld_assert_rst(ST7735Driver *st7735) {
    palClearPad(st7735->config->rst_port, st7735->config->rst_pin);
}

/*
 * @brief   ...
 */
inline void st7735_lld_release_rst(ST7735Driver *st7735) {
    palSetPad(st7735->config->rst_port, st7735->config->rst_pin);
}

/*
 * @brief   ...
 * @details ...
 */
bool st7735_lld_send_command(ST7735Driver *st7735, uint8_t command_sequence[]) {
    //
    uint16_t i = 0, argc = 0, delay = 0;
    // process sequence
    do {
        // send command
        st7735_lld_assert_dc(st7735);
        // assert chip select
        st7735_lld_assert_cs(st7735);
        // send command
        spiSend(st7735->config->spi_drv, 1, &command_sequence[i++]);
        // send arguments if any
        argc = command_sequence[i++];
        if (argc > 0) {
            st7735_lld_release_dc(st7735);
            spiSend(st7735->config->spi_drv, argc, &command_sequence[i]);
            i += argc;
        }
        // wwait if necessary
        delay = command_sequence[i++];
        if (delay > 0) {
            chThdSleepMilliseconds(delay);
        }
        // reselese chip select
        st7735_lld_release_cs(st7735);
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
void st7735_lld_data_write(ST7735Driver *st7735, uint16_t len, uint16_t data[]) {
    // assert chip select, data will be sent
    st7735_lld_release_dc(st7735);
    st7735_lld_assert_cs(st7735);
    // send data using hardware spi
    spiSend(st7735->config->spi_drv, len, data);
    // release chip select
    st7735_lld_release_cs(st7735);
}

/*
 * @brief   ...
 * @details ...
 */
void st7735_lld_start_data_write(ST7735Driver *st7735, uint16_t len, uint16_t data[]) {
    // assert chip select, data will be sent
    st7735_lld_release_dc(st7735);
    st7735_lld_assert_cs(st7735);
    // send data using hardware spi
    // FIXME spiStartSend(st7735->config->spi_drv, len, data);
    spiSend(st7735->config->spi_drv, len, data);
    // release chip select
    st7735_lld_release_cs(st7735);
}

/*
 * @brief   ...
 * @details ...
 */
void st7735_lld_set_viewport(ST7735Driver *st7735,
                             uint16_t x, uint16_t y,
                             uint16_t w, uint16_t h) {
    //
    switch (st7735->rotation) {
        case LCD_ROTATION_0:
            //
            st7735_lld_set_viewport_sequence[3] = y;
            st7735_lld_set_viewport_sequence[5] = y + h - 1;
            st7735_lld_set_viewport_sequence[10] = x;
            st7735_lld_set_viewport_sequence[12] = x + w - 1;
            break;
        case LCD_ROTATION_90:
            //
            st7735_lld_set_viewport_sequence[3] = x;
            st7735_lld_set_viewport_sequence[5] = x + w - 1;
            st7735_lld_set_viewport_sequence[10] = y;
            st7735_lld_set_viewport_sequence[12] = y + h - 1;
            break;
        case LCD_ROTATION_180:
            break;
        case LCD_ROTATION_270:
            break;
    }
    st7735_lld_send_command(st7735, st7735_lld_set_viewport_sequence);
}

/*
 * @brief   ...
 * @details ...
 */
void st7735_lld_put_pixel(ST7735Driver *st7735,
                          uint16_t y, uint16_t x,
                          lcd_color_t color) {
    //
    consoleDevel("st7735_lld_put_pixel() start\r\n");
    //
    st7735_lld_put_pixel_sequence[16] = color & 0xFF;
    st7735_lld_put_pixel_sequence[17] = (color & 0xFF00) >> 8;
    //
    switch (st7735->rotation) {
        case LCD_ROTATION_0:
            //
            st7735_lld_put_pixel_sequence[3] = x;
            st7735_lld_put_pixel_sequence[5] = x;
            st7735_lld_put_pixel_sequence[10] = y;
            st7735_lld_put_pixel_sequence[12] = y;
            break;
        case LCD_ROTATION_90:
            //
            st7735_lld_put_pixel_sequence[3] = y;
            st7735_lld_put_pixel_sequence[5] = y;
            st7735_lld_put_pixel_sequence[10] = x;
            st7735_lld_put_pixel_sequence[12] = x;
            break;
        case LCD_ROTATION_180:
            break;
        case LCD_ROTATION_270:
            break;
    }
    //
    st7735_lld_send_command(st7735, st7735_lld_put_pixel_sequence);
    //
    consoleDevel("st7735_lld_put_pixel() end\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
void st7735_lld_clear_screen(ST7735Driver *st7735, uint16_t color) {
    //
    uint16_t i;
    uint16_t row[128];
    // prepare one row of data
    for (i = 0; i < 128; i++) {
        row[i] = color;
    }
    consoleDebug("st7735_lld_clear_screen() start\r\n");

    // DMA setup
    //dmaStreamAllocate(uartp->dmarx, STM32_SPI_SPI1_IRQ_PRIORITY, (stm32_dmaisr_t)spi_lld_serve_rx_interrupt, (void *)spip);
    //dmaStreamSetPeripheral(spip->dmarx, &spip->spi->DR);
    //dmsStreamSetMemory0(spip->dmarx, rxbuf);
    //dmaStreamSetTransactionSize(spip->dmarx, n);
    //dmaStreamSetMode(spip->dmarx, spip->rxdmamode| STM32_DMA_CR_MINC);
    //dmaStreamEnable(spip->dmarx);
    //dmaStreamDisable(spip->dmatx);
    //dmaStreamRelease(spip->dmarx);

    // set viewport and send data
    st7735_lld_set_viewport(st7735, 0, 0, st7735->width, st7735->height);
    for (i = 0; i < 160; i++) {
        st7735_lld_data_write(st7735, sizeof(row), row);
    }
    consoleDebug("st7735_lld_clear_screen() end\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
void st7735_lld_set_rotation(ST7735Driver *st7735, lcd_rotation_t rotation) {
    //
    st7735->rotation = rotation;
    // prepare sequnce
    switch (st7735->rotation) {
        case LCD_ROTATION_0:
            st7735_lld_set_rotation_sequence[2] = 0x20;
            st7735->width = LCD_ST7735_WIDTH;
            st7735->height = LCD_ST7735_HEIGHT;
            break;
        case LCD_ROTATION_90:
            st7735_lld_set_rotation_sequence[2] = 0x40;
            st7735->width = LCD_ST7735_HEIGHT;
            st7735->height = LCD_ST7735_WIDTH;
            break;
        case LCD_ROTATION_180:
            st7735_lld_set_rotation_sequence[2] = 0xE0;
            st7735->width = LCD_ST7735_WIDTH;
            st7735->height = LCD_ST7735_HEIGHT;
            break;
        case LCD_ROTATION_270:
            st7735_lld_set_rotation_sequence[2] = 0x80;
            st7735->width = LCD_ST7735_HEIGHT;
            st7735->height = LCD_ST7735_WIDTH;
            break;
    }
    // send sequnce to lcd
    st7735_lld_send_command(st7735, st7735_lld_set_rotation_sequence);
}

/*
 * @brief   ...
 * @details ...
 */
bool st7735_lld_init(LCDDriver *lcd) {
    //
    ST7735Driver *st7735 = &lcd->config->lld_driver.st7735;
    st7735->config = lcd->config->lld_config.st7735;
    //
    consoleDebug("st7735_lld_init() start\r\n");
    // io ports
    palSetPadMode(lcd->config->lld_config.st7735->cs_port,
                  lcd->config->lld_config.st7735->cs_pin,
                  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(lcd->config->lld_config.st7735->dc_port,
                  lcd->config->lld_config.st7735->dc_pin,
                  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(lcd->config->lld_config.st7735->rst_port,
                  lcd->config->lld_config.st7735->rst_pin,
                  PAL_MODE_OUTPUT_PUSHPULL);
    // reset chip
    st7735_lld_assert_rst(st7735);
    chThdSleepMilliseconds(5);
    st7735_lld_release_rst(st7735);
    consoleDebug("initializing lcd\r\n");
    // power on sequence
    st7735_lld_send_command(st7735, st7735_lld_lcd_init_sequence);
    // set rotation
    st7735_lld_set_rotation(st7735, lcd->config->rotation);
    // clear screen to black
    st7735_lld_clear_screen(st7735, LCD_COLOR_BLACK);
    // 
    consoleDebug("st7735_lld_init() end\r\n");
    //
    return true;
}

////// /*
//////  * @brief   ...
//////  * @details ...
//////  */
////// uint8_t lcdST7735DrawChar(lcd_st7735_t *lcd, uint16_t x, uint16_t y,
//////                           char c) {
//////     uint8_t width, height, font_width, *bitmap;
//////     uint8_t row, column, column_offset, m;
//////     uint16_t offset, x_start;
//////     //
//////     height = fontGetHeight(lcd->font);
//////     width = fontGetCharWidth(lcd->font, c);
//////     // in active frame buffer?
//////     if ((x + width - 1 < lcd->framebuffer_x1) ||
//////         (x > lcd->framebuffer_x2) ||
//////         (y + height - 1 < lcd->framebuffer_y1) ||
//////         (y > lcd->framebuffer_y2)) {
//////         return width;
//////     }
//////     //
//////     m = 0;
//////     font_width = fontGetWidth(lcd->font);
//////     bitmap = fontGetCharBitmap(lcd->font, c);
//////     //
//////     for (row = 0; row < height; row++) {
//////         if ((y >= lcd->framebuffer_y1) &&
//////             (y <= lcd->framebuffer_y2)) {
//////             // offset at the beggining
//////             offset = (y - lcd->framebuffer_y1) * LCD_FRAMEBUFFER_WIDTH + (x - lcd->framebuffer_x1);
//////             column_offset = 0;
//////             for (column = 0; column < width; column++) {
//////                 if (column % 8 == 0) {
//////                     m = *(bitmap + column_offset++);
//////                 }
//////                 if ((x + column >= lcd->framebuffer_x1) &&
//////                     (x + column <= lcd->framebuffer_x2)) {
//////                     if (m & 0x80) {
//////                         lcd->shadow_framebuffer[offset] = lcd->fg_color;
//////                     //} else if (bg != Lcd::Color::Transparent) {
//////                     } else {
//////                         //buffer[row * width + column] = lcd->bg_color;
//////                         //lcd->shadow_framebuffer[offset] = 0;
//////                     }
//////                 }
//////                 offset++;
//////                 m <<= 1;
//////             }
//////         }
//////         bitmap += font_width >> 3;
//////         y++;
//////     }
//////     //lcdST7735SetViewport(lcd, x, y, width, height);
//////     //lcdST7735DataWrite(lcd, height * width * 2, buffer);
//////     return width;
////// }
////// 
////// uint16_t lcdGetStringWidth(char *s, font_t *font) {
//////     uint16_t len = 0;
//////     while ( *s != '\0' ) {
//////         len += fontGetCharWidth(font, *s);
//////         s++;
//////     }
//////     return len;
////// }
////// 
////// uint16_t lcdST7735DrawString(lcd_st7735_t *lcd, uint16_t x, uint16_t y,
//////                              char *s, lcd_align_t align) {
//////     //
//////     uint16_t length;
//////     length = lcdGetStringWidth(s, lcd->font);
//////     // if ( not this->in_viewport( x, y, x + length - 1, y + this->font->get_font_height() ) ) {
//////     //     return 0;
//////     // }
//////     if (align == LCD_ALIGN_CENTER) {
//////         x -= length / 2;
//////     } else if (align == LCD_ALIGN_RIGHT) {
//////         x -= length;
//////     }
//////     // 
//////     while ( *s != '\0' ) {
//////         x += lcdST7735DrawChar(lcd, x, y, *s);
//////         s++;
//////     }
//////     return x;
////// }
////// 
////// /*
//////  * @brief   ...
//////  * @details ...
//////  */
////// bool lcdSetFgColor(lcd_st7735_t *lcd, varg_t arg) {
//////     // 
//////     if (lcd != NULL) {
//////         lcd->fg_color = (lcd_color_t)arg;
//////     }
//////     // by default return false
//////     return true;
////// }
////// 
////// /*
//////  * @brief   ...
//////  * @details ...
//////  */
////// bool lcdSetBgColor(lcd_st7735_t *lcd, varg_t arg) {
//////     // 
//////     if (lcd != NULL) {
//////         lcd->bg_color = (lcd_color_t)arg;
//////     }
//////     // by default return false
//////     return true;
////// }
////// 
////// /*
//////  * @brief   ...
//////  * @details ...
//////  */
////// bool lcdSetFont(lcd_st7735_t *lcd, varg_t arg) {
//////     // 
//////     if (lcd != NULL) {
//////         lcd->font = (font_t *)arg;
//////     }
//////     // by default return false
//////     return true;
////// }
////// 
////// /*
//////  * @brief   ...
//////  * @details ...
//////  */
////// bool lcdST7735FramebufferSwap(lcd_st7735_t *lcd, varg_t arg) {
//////     // 
//////     (void)arg;
//////     //
//////     if (lcd->active_framebuffer == lcd->framebuffer1) {
//////         lcd->active_framebuffer = lcd->framebuffer2;
//////         lcd->shadow_framebuffer = lcd->framebuffer1;
//////     } else {
//////         lcd->active_framebuffer = lcd->framebuffer1;
//////         lcd->shadow_framebuffer = lcd->framebuffer2;
//////     }
//////     //
//////     lcdST7735SetViewport(lcd,
//////                          lcd->framebuffer_x1, lcd->framebuffer_y1,
//////                          LCD_FRAMEBUFFER_WIDTH, LCD_FRAMEBUFFER_HEIGHT);
//////     lcdST7735DataWrite(lcd,
//////                        LCD_FRAMEBUFFER_SIZE * sizeof(lcd_pixel_t),
//////                        lcd->active_framebuffer);
//////     // 
//////     lcd->framebuffer_x1 += LCD_FRAMEBUFFER_WIDTH;
//////     lcd->framebuffer_x2 += LCD_FRAMEBUFFER_WIDTH;
//////     if (lcd->framebuffer_x1 >= LCD_ST7735_WIDTH) {
//////         lcd->framebuffer_x1 = 0;
//////         lcd->framebuffer_x2 = LCD_FRAMEBUFFER_WIDTH - 1;
//////         // 
//////         lcd->framebuffer_y1 += LCD_FRAMEBUFFER_HEIGHT;
//////         lcd->framebuffer_y2 += LCD_FRAMEBUFFER_HEIGHT;
//////         if (lcd->framebuffer_y1 >= LCD_ST7735_HEIGHT) {
//////             lcd->framebuffer_y1 = 0;
//////             lcd->framebuffer_y2 = LCD_FRAMEBUFFER_HEIGHT - 1;
//////         }
//////     }
//////     // FIXME allow custom background 
//////     memset(lcd->shadow_framebuffer, 0, LCD_FRAMEBUFFER_SIZE * sizeof(lcd_pixel_t));
//////     // by default return false
//////     return true;
////// }

#endif /* ITHACA_USE_LCD_ST7735 */

