#include <ithaca.hpp>

#if ITHACA_USE_WIDGET || defined(__DOXYGEN__)

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
Widget::Widget(void) {
    //
    this->lcd = NULL;
    this->parent = NULL;
    this->child = NULL;
    this->prev = NULL;
    this->next = NULL;
    //
    this->setGeometry(0, 0, 0, 0);
    this->setBgColor(Lcd::Color::Black, Lcd::Alpha::Transparent);
    //
    this->need_redraw = true;
}

/*
 * @brief   ...
 * @details ...
 */
Widget::Widget(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
               Lcd::Color c, Lcd::Alpha a) : Widget() {
    //
    this->setGeometry(x, y, w, h);
    this->setBgColor(c, a);
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::setGeometry(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    //
    this->xs = x;
    this->xe = x + w - 1;
    this->ys = y;
    this->ye = y + h - 1;
};

/*
 * @brief   ...
 * @details ...
 */
void Widget::setBgColor(Lcd::Color c, Lcd::Alpha a) {
    //
    this->bg_color = c;
    this->bg_alpha = a;
}
;
/*
 * @brief   ...
 * @details ...
 */
void Widget::updateData(void) {
    //
    this->update_data();
    //
    Widget *c;
    c = this->child;
    while (c != NULL) {
        c->updateData();
        c = c->getNext();
    }
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::redraw(bool force_redraw) {
    //
    this->self_redraw(force_redraw);
    //
    Widget *c;
    c = this->child;
    while (c != NULL) {
        c->redraw(this->need_redraw || force_redraw);
        c = c->getNext();
    }
    // mark as already drawn
    if ((this->xe <= this->lcd->getPageXE()) &&
        (this->ye <= this->lcd->getPageYE()) &&
        (this->need_redraw == true)) {
        //
        this->need_redraw = false;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::assignLcd(Lcd *lcd) {
    this->lcd = lcd;
    this->parent = NULL;
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::setParent(Widget *parent) {
    this->parent = parent;
    this->lcd = parent->lcd;
    this->xs += parent->xs;
    this->xe += parent->xs;
    this->ys += parent->ys;
    this->ye += parent->ys;
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::self_redraw(bool force_redraw) {
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::update_data(void) {
}

/*
 * @brief   ...
 * @details ...
 */
Widget *Widget::getNext(void) {
    return this->next;
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::addChild(Widget *w) {
    Widget *c = this->child;
    if (c == NULL) {
        this->child = w;
    } else {
        while (c->getNext() != NULL) {
            c = c->getNext();
        }
        c->addNeighbour(w);
    }
    w->setParent(this);
}

/*
 * @brief   ...
 * @details ...
 */
bool Widget::page_flush_needed(void) {
    //
    if ((this->xe < this->lcd->getPageXS()) || (this->xs > this->lcd->getPageXE()) ||
        (this->ye < this->lcd->getPageYS()) || (this->ys > this->lcd->getPageYE())) {
        return false;
    }
    //
    if (this->need_redraw == true) {
        return true;
    }
    //
    Widget *c = this->child;
    while (c != NULL) {
        if (c->page_flush_needed() == true) {
            return true;
        }
        c = c->getNext();
    }
    //
    return false;
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::addNeighbour(Widget *w) {
    this->next = w;
    w->prev = this;
    w->parent = this->parent;
    w->lcd = this->lcd;
}

/*
 * @brief   ...
 * @details ...
 */
void Widget::drawText(uint16_t col, uint16_t row,
                      Font *font, char *text, Lcd::TextAlign ha,
                      Lcd::Color fc, Lcd::Alpha fa,
                      Lcd::Color bc, Lcd::Alpha ba) {
    //
    if ((font == NULL) || (text == NULL)) {
        return;
    }
    //
    uint16_t x = 0, y = 0;
    uint16_t tw = font->getTextWidth(text);
    switch (ha) {
        case Lcd::TextAlign::Left:
            x = col;
            break;
        case Lcd::TextAlign::Center:
            x = col - (tw / 2) + 1;
            //y = row - (font->getHeight() / 2) + 1;
            break;
        case Lcd::TextAlign::Right:
            x = col - tw;
            //y = row - font->getHeight() + 1;
            break;
    }
    y = row;
    //
    const uint16_t xmin = MAX(this->lcd->getPageXS(), this->xs),
                   xmax = MIN(this->lcd->getPageXE(), this->xe),
                   ymin = MAX(this->lcd->getPageYS(), this->ys),
                   ymax = MIN(this->lcd->getPageYE(), this->ye);
    const uint8_t h = font->getHeight();
    //
    if ((x + tw < xmin) || (x > xmax) ||
        (y + h < ymin) || (y > ymax)) {
        return;
    }
    //
    while (*text != '\0') {
        //
        x += this->drawChar(x, y, font, *text++, fc, fa, bc, ba);
    }
    return;
}

/*
 * @brief   ...
 * @details ...
 */
uint8_t Widget::drawChar(uint16_t col, uint16_t row, Font *font, char c,
                         Lcd::Color fc, Lcd::Alpha fa, Lcd::Color bc, Lcd::Alpha ba) {
    if (font == NULL) {
        return 0;
    }
    //
    const uint8_t *bitmap = font->getCharBitmap(c);
    if (bitmap == NULL) {
        return 0;
    }
    //
    const uint16_t xmin = MAX(this->lcd->getPageXS(), this->xs),
                   xmax = MIN(this->lcd->getPageXE(), this->xe),
                   ymin = MAX(this->lcd->getPageYS(), this->ys),
                   ymax = MIN(this->lcd->getPageYE(), this->ye);
    const uint8_t w = font->getCharWidth(c),
                  h = font->getHeight();
    //
    if ((col + w < xmin) || (col > xmax) ||
        (row + h < ymin) || (row > ymax)) {
        return w;
    }
    //
    uint16_t x, y;
    uint8_t m, bit;
    m = '!';
    for (y = row; y < row + h; y++) {
        if (y < ymin) {
            bitmap += (font->getHeight() >> 3);
            continue;
        } else if (y > ymax) {
            break;
        }
        bit = 0;
        for (x = col; x < col + w; x++) {
            if (bit++ % 8 == 0) {
                m = *bitmap++;
            }
            if (x < xmin) {
                m <<= 1;
                continue;
            } else if (x > xmax) {
                break;
            } else {
                if (m & 0x80) {
                    this->lcd->putPixel(x, y, fc, fa);
                } else {
                    if (ba != Lcd::Alpha::Transparent) {
                        this->lcd->putPixel(x, y, bc, ba);
                    }
                }
                m <<= 1;
            }
        }
    }
    return w;
}

#endif /* ITHACA_USE_WIDGET */

