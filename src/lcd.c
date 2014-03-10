#include <ithaca.h>

#if ITHACA_USE_LCD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool lcdInit(LCDDriver *lcd, LCDConfig *cfg) {
    if (lcd == NULL) {
        return false;
    }
    lcd->config = cfg;
    // page buffer
    lcd->page_buffer_1 = (uint16_t *)chCoreAlloc(lcd->config->page_width * lcd->config->page_width * 2);
    lcd->page_buffer_2 = (uint16_t *)chCoreAlloc(lcd->config->page_width * lcd->config->page_width * 2);
    lcd->page_buffer_draw = lcd->page_buffer_1;
    lcd->page_buffer_flush = lcd->page_buffer_2;
    //
    lcd->bg_color = LCD_COLOR_BLACK;
    lcd->fg_color = LCD_COLOR_WHITE;
    //
    switch (lcd->config->lld_type) {
#if ITHACA_USE_LCD_ST7735
        case LCD_LLD_TYPE_ST7735:
            st7735_lld_init(lcd);
            break;
#endif
        default:
            return false;
    }
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
lcd_color_t lcdGetPixel(LCDDriver *lcd, uint16_t x, uint16_t y) {
    if (lcd == NULL) {
        return 0xA5A5;
    }
    switch (lcd->config->lld_type) {
#if ITHACA_USE_LCD_ST7735
        case LCD_LLD_TYPE_ST7735:
            #if ITHACA_USE_LCD_ST7735_GET_PIXEL
                #error st7735_lld_get_pixel not implemented for ST7735
            #else
                return LCD_COLOR_TRANSPARENT;
            #endif
            break;
#endif
        default:
            break;
    }
    return 0xA5A5;
}

/*
 * @brief   ...
 * @details ...
 */
void lcdPutPixel(LCDDriver *lcd, uint16_t x, uint16_t y, lcd_color_t c) {
    if (lcd == NULL) {
        return;
    }
    switch (lcd->config->lld_type) {
#if ITHACA_USE_LCD_ST7735
        case LCD_LLD_TYPE_ST7735:
            st7735_lld_put_pixel(&lcd->config->lld_driver.st7735, x, y, c);
            break;
#endif
        default:
            break;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void lcdFlushPage(LCDDriver *lcd) {
    if (lcd->page_buffer_1 == lcd->page_buffer_draw) {
        lcd->page_buffer_draw = lcd->page_buffer_2;
        lcd->page_buffer_flush = lcd->page_buffer_1;
    } else {
        lcd->page_buffer_draw = lcd->page_buffer_1;
        lcd->page_buffer_flush = lcd->page_buffer_2;
    }
    switch (lcd->config->lld_type) {
#if ITHACA_USE_LCD_ST7735
        case LCD_LLD_TYPE_ST7735:
            st7735_lld_set_viewport(&lcd->config->lld_driver.st7735,
                                    lcd->page_xs, lcd->page_ys,
                                    lcd->config->page_width, lcd->config->page_height);
            st7735_lld_start_data_write(&lcd->config->lld_driver.st7735,
                                        lcd->config->page_width * lcd->config->page_height * 2,
                                        lcd->page_buffer_flush);
            break;
#endif
        default:
            break;
    }
}

//
inline void lcdPagePutPixel(LCDDriver *lcd,
                            uint16_t x, uint16_t y, lcd_color_t color) {
    //
    if ((x < lcd->page_xs) || (x > lcd->page_xe) ||
        (y < lcd->page_ys) || (y > lcd->page_ye)) {
        return;
    }
    //
    x -= lcd->page_xs;
    y -= lcd->page_ys;
    lcd->page_buffer_draw[x * lcd->config->page_height + y] = color;
}

//
inline void lcdPagePutAlphaPixel(LCDDriver *lcd,
                            uint16_t x, uint16_t y,
                            lcd_color_t color, lcd_alpha_t alpha) {
    //
    uint16_t idx, c1, alpha_norm, alpha_inv;
    uint8_t r1, g1, b1, r2, g2, b2;
    //
    if ((x < lcd->page_xs) || (x > lcd->page_xe) ||
        (y < lcd->page_ys) || (y > lcd->page_ye)) {
        return;
    }
    //
    x -= lcd->page_xs;
    y -= lcd->page_ys;
    idx = x * lcd->config->page_height + y;
    c1 = lcd->page_buffer_draw[idx];
    //
    r1 = (c1 & 0x00F8) >> 3;
    g1 = ((c1 & 0x0007) << 3) | ((c1 & 0xE000) >> 13);
    b1 = (c1 & 0x1F00) >> 8;
    r2 = (color & 0x00F8) >> 3;
    g2 = ((color & 0x0007) << 3) | ((color & 0xE000) >> 13);
    b2 = (color & 0x1F00) >> 8;
    //
    alpha_norm = alpha + 1;
    alpha_inv = 0x100 - alpha_norm;
    //
    r2 = (r2 * alpha_norm + r1 * alpha_inv) >> 8;
    g2 = (g2 * alpha_norm + g1 * alpha_inv) >> 8;
    b2 = (b2 * alpha_norm + b1 * alpha_inv) >> 8;
    color = (r2 << 3) | ((g2 & 0x38) >> 3) | ((g2 & 0x07) << 13) | (b2 << 8);
    //consoleDebug("%2X %2X %2X\r\n", r2, g2, b2);
    lcd->page_buffer_draw[idx] = color;
}

//
uint16_t lcdGetTextWidth(char *text, const font_t *font) {
    uint16_t w = 0;
    while (*text != '\0') {
        w += fontGetCharWidth(font, *text++);
    }
    return w;
}

//
uint16_t lcdDrawChar(LCDDriver *lcd, char c, uint16_t xs, uint16_t ys, const font_t *font,
                     lcd_color_t fg_color, lcd_alpha_t fg_alpha,
                     lcd_color_t bg_color, lcd_alpha_t bg_alpha) {
    uint16_t x, y;
    uint8_t w, h, *bitmap, m, bit;
    consoleDebug("%c\r\n", c);
    w = fontGetCharWidth(font, c);
    h = fontGetHeight(font);
    bitmap = fontGetCharBitmap(font, c);
    m = '!';
    for (y = ys; y < ys + h; y++) {
        bit = 0;
        for (x = xs; x < xs + w; x++) {
            if (bit++ % 8 == 0) {
                m = *bitmap++;
            }
            if (m & 0x80) {
                lcdPagePutAlphaPixel(lcd, x, y, fg_color, fg_alpha);
            } else {
                lcdPagePutAlphaPixel(lcd, x, y, bg_color, bg_alpha);
            }
            m <<= 1;
        }
    }
    return w;
}

//
void lcdWidgetRootRedraw(LCDDriver *lcd, LCDWidget *root) {
    uint16_t page_size, i;
    page_size = lcd->config->page_width * lcd->config->page_height;
    for (i = 0; i < page_size; i++) {
        lcd->page_buffer_draw[i] = lcd->bg_color;
    }
    consoleDebug("root window redraw\r\n");
}

//
void lcdWidgetStaticLabelRedraw(LCDDriver *lcd, LCDWidget *widget) {
    //
    uint16_t x, y, tw;
    char *text;
    LCDWidgetStaticLabel *wdata = widget->data;
    //
    for (x = widget->xs; x <= widget->xe; x++) {
        lcdPagePutPixel(lcd, x, widget->ys, lcd->fg_color);
        lcdPagePutPixel(lcd, x, widget->ye, lcd->fg_color);
        lcdPagePutPixel(lcd, x, widget->ye - 5, lcd->fg_color);
    }
    //
    for (y = widget->ys + 1; y < widget->ye; y++) {
        lcdPagePutPixel(lcd, widget->xs, y, lcd->fg_color);
        lcdPagePutPixel(lcd, widget->xe, y, lcd->fg_color);
    }
    //
    text = wdata->text;
    tw = lcdGetTextWidth(wdata->text, wdata->font);
    x = widget->xs + ((widget->xe - widget->xs) / 2) - (tw / 2) + 1;
    while ((*text != '\0') && (x < widget->xe)) {
        x += lcdDrawChar(lcd, *text++, x, widget->ys + 2, wdata->font,
                         widget->fg_color, widget->fg_alpha,
                         widget->bg_color, widget->bg_alpha);
    }
    //
    consoleDebug("%s\r\n", wdata->text);
}

//
void lcdWidgetLabelRedraw(LCDDriver *lcd, LCDWidget *widget) {
    //
    uint16_t x, y, tw;
    char *text;
    LCDWidgetLabel *wdata = widget->data;
    //
    for (x = widget->xs; x <= widget->xe; x++) {
        lcdPagePutPixel(lcd, x, widget->ys, lcd->fg_color);
        lcdPagePutPixel(lcd, x, widget->ye, lcd->fg_color);
        lcdPagePutPixel(lcd, x, widget->ye - 5, lcd->fg_color);
    }
    //
    for (y = widget->ys + 1; y < widget->ye; y++) {
        lcdPagePutPixel(lcd, widget->xs, y, lcd->fg_color);
        lcdPagePutPixel(lcd, widget->xe, y, lcd->fg_color);
    }
    //
    text = wdata->text;
    tw = lcdGetTextWidth(wdata->text, wdata->font);
    x = widget->xs + ((widget->xe - widget->xs) / 2) - (tw / 2) + 1;
    while ((*text != '\0') && (x < widget->xe)) {
        x += lcdDrawChar(lcd, *text++, x, widget->ys + 2, wdata->font,
                         widget->fg_color, widget->fg_alpha,
                         widget->bg_color, widget->bg_alpha);
    }
    //
    consoleDebug("%s\r\n", wdata->text);
}

//
void lcdWidgetRedraw(LCDDriver *lcd, LCDWidget *widget, bool force_redraw) {
    //
    LCDWidget *child;
    //
    if ((lcd->page_xs > widget->xe) || (lcd->page_xe < widget->xs) ||
        (lcd->page_ys > widget->ye) || (lcd->page_ye < widget->ys)) {
        return;
    }
    // 
    if (force_redraw == true) {
        widget->need_redraw = true;
    }
    // redraw if needed
    if (widget->need_redraw == true) {
        //
        switch (widget->type) {
            case LCD_WIDGET_ROOT:
                lcdWidgetRootRedraw(lcd, widget);
                break;
            case LCD_WIDGET_STATIC_LABEL:
                lcdWidgetStaticLabelRedraw(lcd, widget);
                consoleDebug("STATIC_LABEL\r\n");
                break;
            case LCD_WIDGET_LABEL:
                lcdWidgetLabelRedraw(lcd, widget);
                consoleDebug("XXXXXXXXXXXx\r\n");
                break;
        }
    }
    //
    child = widget->child;
    while (child != NULL) {
        consoleDebug("child redraw will be called\r\n");
        lcdWidgetRedraw(lcd, child, widget->need_redraw);
        child = child->focus_next;
    }
    //
    if ((widget->xe <= lcd->page_xe) && (widget->ye <= lcd->page_ye)) {
        widget->need_redraw = false;
    }
}

//
void lcdScreenUpdate(LCDDriver *lcd, LCDWidget *root) {
    for (lcd->page_xs = root->xs; lcd->page_xs <= root->xe;
         lcd->page_xs += lcd->config->page_width) {
        lcd->page_xe = lcd->page_xs + lcd->config->page_width - 1;
        for (lcd->page_ys = root->ys; lcd->page_ys <= root->ye;
             lcd->page_ys += lcd->config->page_height) {
            lcd->page_ye = lcd->page_ys + lcd->config->page_height - 1;
            //lcdPutPixel(lcd, px, py, LCD_COLOR_RED);
            lcdWidgetRedraw(lcd, root, true);
            lcdFlushPage(lcd);
        }
    }
}

//
LCDWidget *lcdWidgetCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    LCDWidget *widget = chCoreAlloc(sizeof(LCDWidget));
    //widget->parent = NULL;
    widget->child = NULL;
    widget->focus_prev = NULL;
    widget->focus_next = NULL;
    widget->xs = x;
    widget->ys = y;
    widget->xe = x + w - 1;
    widget->ye = y + h - 1;
    widget->need_redraw = true;
    widget->data = NULL;
    return widget;
}

//
LCDWidget *lcdWidgetRootCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    LCDWidget *widget = lcdWidgetCreate(x, y, w, h);
    widget->type = LCD_WIDGET_ROOT;
    return widget;
}

//
LCDWidget *lcdWidgetStaticLabelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    LCDWidget *widget = lcdWidgetCreate(x, y, w, h);
    widget->type = LCD_WIDGET_STATIC_LABEL;
    widget->data = chCoreAlloc(sizeof(LCDWidgetStaticLabel));
    return widget;
}

//
LCDWidget *lcdWidgetLabelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    LCDWidget *widget = lcdWidgetCreate(x, y, w, h);
    LCDWidgetLabel *label;
    widget->type = LCD_WIDGET_LABEL;
    widget->data = chCoreAlloc(sizeof(LCDWidgetLabel));
    label = widget->data;
    label->text = NULL;
    return widget;
}

//
void lcdWidgetAddChild(LCDWidget *parent, LCDWidget *child) {
    if (parent->child == NULL) {
        parent->child = child;
        child->xs += parent->xs;
        child->ys += parent->ys;
        child->xe += parent->xs;
        child->ye += parent->ys;
    } else {
    }
}

//
void lcdSetBgColor(LCDDriver *lcd, lcd_color_t color) {
    lcd->bg_color = color;
}

//
void lcdSetFgColor(LCDDriver *lcd, lcd_color_t color) {
    lcd->fg_color = color;
}

//
void lcdStaticLabelSetText(LCDWidget *widget, char *text,
                           const font_t *font, lcd_text_align_t align) {
    LCDWidgetStaticLabel *static_label = widget->data;
    static_label->text = text;
    static_label->font = (font_t *)font;
    static_label->align = align;
}

//
void lcdLabelSetText(LCDWidget *widget, char *text,
                           const font_t *font, lcd_text_align_t align) {
    LCDWidgetLabel *label = widget->data;
    uint8_t len = strlen(text);
    if (label->text != NULL) {
        chHeapFree(label->text);
    }
    label->text = chHeapAlloc(NULL, len + 1);
    memcpy(label->text, text, len);
    label->text[len] = 0;
    label->font = (font_t *)font;
    label->align = align;
}

//
void lcdWidgetSetFgColor(LCDWidget *widget, lcd_color_t color, uint8_t alpha) {
    widget->fg_color = color;
    widget->fg_alpha = alpha;
}

//
void lcdWidgetSetBgColor(LCDWidget *widget, lcd_color_t color, uint8_t alpha) {
    widget->bg_color = color;
    widget->bg_alpha = alpha;
}

//
uint16_t lcdGetWidth(LCDDriver *lcd) {
    //
    switch (lcd->config->lld_type) {
#if ITHACA_USE_LCD_ST7735
        case LCD_LLD_TYPE_ST7735:
            return lcd->config->lld_driver.st7735.width;
#endif
        default:
            break;
    }
    return 0;
}

//
uint16_t lcdGetHeight(LCDDriver *lcd) {
    //
    switch (lcd->config->lld_type) {
#if ITHACA_USE_LCD_ST7735
        case LCD_LLD_TYPE_ST7735:
            return lcd->config->lld_driver.st7735.height;
#endif
        default:
            break;
    }
    return 0;
}
#endif /* ITHACA_USE_LCD */


