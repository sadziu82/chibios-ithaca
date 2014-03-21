#include <ithaca.hpp>

#if ITHACA_USE_WIDGET_FRAME || defined(__DOXYGEN__)

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
WidgetFrame::WidgetFrame(void) : Widget() {
    //
    this->setFrame(Lcd::Color::White, Lcd::Alpha::Solid, Lcd::LineStyle::Dotted);
}

/*
 * @brief   ...
 * @details ...
 */
WidgetFrame::WidgetFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                         Lcd::Color bc, Lcd::Alpha ba,
                         Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs) :
                         Widget(x, y, w, h, bc, ba) {
    //consoleDebug("WidgetFrame(%d, %d, %d, %d\r\n", x, y, w, h);
    this->setFrame(fc, fa, fs);
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetFrame::setFrame(Lcd::Color c, Lcd::Alpha a, Lcd::LineStyle s) {
    //
    this->frame_color = c;
    this->frame_alpha = a;
    this->frame_style = s;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetFrame::self_redraw(bool force_redraw) {
    //
    if ((this->need_redraw || force_redraw) == false) {
        return;
    }
    //
    uint8_t style_len, style_space;
    //
    Widget::self_redraw(force_redraw);
    //
    uint16_t x, y, xmin, xmax, ymin, ymax;
    //
    xmin = MAX(lcd->getPageXS(), this->xs);
    xmax = MIN(lcd->getPageXE(), this->xe);
    ymin = MAX(lcd->getPageYS(), this->ys);
    ymax = MIN(lcd->getPageYE(), this->ye);
    //
    switch (this->frame_style) {
        case Lcd::LineStyle::None:
            style_len = 1;
            style_space = 1;
            break;
        case Lcd::LineStyle::Solid:
            style_len = 1;
            style_space = 0;
            break;
        case Lcd::LineStyle::Dotted:
            style_len = 2;
            style_space = 1;
            break;
        case Lcd::LineStyle::Dashed:
            style_len = 4;
            style_space = 2;
            break;
        case Lcd::LineStyle::DDashed:
            style_len = 6;
            style_space = 3;
            break;
    }
    //
    if (this->bg_alpha != Lcd::Alpha::Transparent) {
        for (y = ymin; y <= ymax; y++) {
            for (x = xmin; x <= xmax; x++) {
                lcd->putPixel(x, y, this->bg_color, this->bg_alpha);
            }
        }
    }
    //
    if (this->frame_alpha != Lcd::Alpha::Transparent) {
        if ((this->ys >= this->lcd->getPageYS()) &&
            (this->ys <= this->lcd->getPageYE())) {
            for (x = xmin; x <= xmax; x++) {
                if (x % style_len >= style_space) {
                    lcd->putPixel(x, this->ys, this->frame_color, this->frame_alpha);
                }
            }
        }
        if ((this->ye >= this->lcd->getPageYS()) &&
            (this->ye <= this->lcd->getPageYE())) {
            for (x = xmin; x <= xmax; x++) {
                if (x % style_len >= style_space) {
                    lcd->putPixel(x, this->ye, this->frame_color, this->frame_alpha);
                }
            }
        }
        if ((this->xs >= this->lcd->getPageXS()) &&
            (this->xs <= this->lcd->getPageXE())) {
            for (y = ymin; y <= ymax; y++) {
                if (y % style_len >= style_space) {
                    lcd->putPixel(this->xs, y, this->frame_color, this->frame_alpha);
                }
            }
        }
        if ((this->xe >= this->lcd->getPageXS()) &&
            (this->xe <= this->lcd->getPageXE())) {
            for (y = ymin; y <= ymax; y++) {
                if (y % style_len >= style_space) {
                    lcd->putPixel(this->xe, y, this->frame_color, this->frame_alpha);
                }
            }
        }
    }
}

#endif /* ITHACA_USE_WIDGET_FRAME */

