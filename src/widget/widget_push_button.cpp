#include <ithaca.hpp>

#if ITHACA_USE_WIDGET_PUSH_BUTTON || defined(__DOXYGEN__)

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

/*
 * @brief   ...
 * @details ...
 */
void WidgetPushButton::self_redraw(bool force_redraw) {
    //
    if ((this->need_redraw || force_redraw) == false) {
        return;
    }
    //
    WidgetFrame::self_redraw(force_redraw);
    //
    if (this->text == NULL) {
        return;
    }
    //
    uint16_t x, y, xmin, xmax, ymin, ymax;
    //
    xmin = MAX(lcd->getPageXS(), this->xs);
    xmax = MIN(lcd->getPageXE(), this->xe);
    ymin = MAX(lcd->getPageYS(), this->ys);
    ymax = MIN(lcd->getPageYE(), this->ye);
    //
    if (this->text_alpha != Lcd::Alpha::Transparent) {
        char *str = this->text;
        x = 0;
        uint16_t tw = this->font->getTextWidth(str);
        switch (this->text_align) {
            case Lcd::TextAlign::Left:
                x = this->xs;
                break;
            case Lcd::TextAlign::Center:
                x = this->xs + ((this->xe - this->xs) / 2) - (tw / 2) + 1;
                break;
            case Lcd::TextAlign::Right:
                x = this->xe - tw;
                break;
        }
        y = this->ys + ((this->ye - this->ys) / 2) -
            (this->font->getHeight() / 2) + 1;
        while ((*str != '\0') && (x < this->xe)) {
            x += this->drawChar(x, y, this->font, *str++,
                                this->text_color, this->text_alpha,
                                this->bg_color, this->bg_alpha);
        }
        this->need_redraw = true;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPushButton::update_data(void) {
    //
    if (data == NULL) {
        return;
    }
    //
    switch (this->data_type) {
        case Widget::DataSourceType::UINT8:
            this->setText(this->fmt, *(uint8_t *)this->data);
            this->need_redraw = true;
            break;
        case Widget::DataSourceType::UINT32:
            this->setText(this->fmt, *(uint32_t *)this->data);
            this->need_redraw = true;
            break;
    }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
WidgetPushButton::WidgetPushButton(void) {
    // TODO add initializers for local data
    //this->setFont();
}

/*
 * @brief   ...
 * @details ...
 */
WidgetPushButton::WidgetPushButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                         Lcd::Color bc, Lcd::Alpha ba,
                         Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                         Font *font, Lcd::TextAlign ha, Lcd::Color tc, Lcd::Alpha ta) :
                         WidgetFrame(x, y, w, h, bc, ba, fc, fa, fs) {
    //
    consoleDebug("WidgetPushButton(%d, %d, %d, %d\r\n", x, y, w, h);
    this->text = NULL;
    this->font = font;
    this->text_align = ha;
    this->text_color = tc;
    this->text_alpha = ta;
    this->data = NULL;
    this->data_type = Widget::DataSourceType::TEXT;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPushButton::setFont(Font *font, Lcd::TextAlign ha, Lcd::Color tc, Lcd::Alpha ta) {
    this->font = font;
    this->text_align = ha;
    this->text_color = tc;
    this->text_alpha = ta;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPushButton::setText(char *fmt, ...) {
    //
    va_list args;
    //
    if (this->text != NULL) {
        chHeapFree(this->text);
        this->text = NULL;
    }
    // calculate string legth
    va_start(args, fmt);
    uint8_t len = chvsprintf(NULL, fmt, args);
    va_end(args);
    //
    this->text = (char *)chHeapAlloc(NULL, len + 1);
    //
    va_start(args, fmt);
    chvsprintf(this->text, fmt, args);
    va_end(args);
    this->text[len] = '\0';
    //consoleDebug("XXX %s XXX\r\n", this->text);
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPushButton::setDataSource(char *fmt, uint8_t *data) {
    //
    this->fmt = fmt;
    this->data = data;
    this->data_type = Widget::DataSourceType::UINT8;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPushButton::setDataSource(char *fmt, uint32_t *data) {
    //
    this->fmt = fmt;
    this->data = data;
    this->data_type = Widget::DataSourceType::UINT32;
}

//
void WidgetPushButton::update(void) {
    this->need_redraw = true;
}

#endif /* ITHACA_USE_WIDGET_PUSH_BUTTON */

