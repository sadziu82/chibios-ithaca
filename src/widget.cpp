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
Widget::Widget(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    //
    this->lcd = NULL;
    this->parent = NULL;
    this->child = NULL;
    this->prev = NULL;
    this->next = NULL;
    //
    this->xs = x;
    this->xe = x + w - 1;
    this->ys = y;
    this->ye = y + h - 1;
    //
    this->bg_color = Lcd::Color::Black;
    this->bg_alpha = Lcd::Alpha::Transparent;
    //
    this->need_redraw = true;
    //
    consoleDebug("Widget(%d, %d, %d, %d)\r\n", x, y, w, h);
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
    //
    if ((this->need_redraw == true) || (force_redraw == true)) {
        this->lcd->pageChanged();
    }
    // mark as already drawn
    if ((this->xe <= this->lcd->getPageXE()) &&
        (this->ye <= this->lcd->getPageYE())) {
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
void Widget::setBgColor(Lcd::Color c, Lcd::Alpha a) {
    this->bg_color = c;
    this->bg_alpha = a;
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
uint8_t Widget::drawChar(uint16_t col, uint16_t row, Font *font, char c,
                         Lcd::Color fc, Lcd::Alpha fa, Lcd::Color bc, Lcd::Alpha ba) {
    if (font == NULL) {
        return 0;
    }
    uint16_t x, y;
    uint8_t w, h, m, bit;
    const uint8_t *bitmap = font->getCharBitmap(c);
    w = font->getCharWidth(c);
    h = font->getHeight();
    m = '!';
    if (bitmap == NULL) {
        return 0;
    }
    for (y = row; y < row + h; y++) {
        if (y < this->ys) {
            continue;
        } else if (y > this->ye) {
            break;
        }
        bit = 0;
        for (x = col; x < col + w; x++) {
            if (bit++ % 8 == 0) {
                m = *bitmap++;
            }
            if ((x >= this->xs) && (x <= this->xe)) {
                if (m & 0x80) {
                    this->lcd->putPixel(x, y, fc, fa);
                } else {
                    if (ba != Lcd::Alpha::Transparent) {
                        this->lcd->putPixel(x, y, bc, ba);
                    }
                }
            }
            m <<= 1;
        }
    }
    return w;
}

#endif /* ITHACA_USE_WIDGET */

