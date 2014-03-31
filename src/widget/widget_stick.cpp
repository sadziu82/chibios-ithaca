#include <ithaca.hpp>

#if ITHACA_USE_WIDGET_STICK || defined(__DOXYGEN__)

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
void WidgetStick::self_redraw(bool force_redraw) {
    //
    if ((this->need_redraw || force_redraw) == false) {
        return;
    }
    //
    WidgetFrame::self_redraw(force_redraw);
    this->lcd->drawDot(this->x_pos, this->y_pos, 2, Lcd::Color::White, Lcd::Alpha::Solid);
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetStick::update_data(void) {
    //
    if (this->x != NULL) {
        this->need_redraw = true;
        this->x_pos = this->xe - 2 - (this->xe - this->xs - 4) * (*this->x / 100.0);
    } else {
        this->x_pos = this->xs + (this->xe - this->xs) / 2;
    }
    //
    if (this->x != NULL) {
        this->need_redraw = true;
        this->y_pos = this->ye - 2 - (this->ye - this->ys - 4) * (*this->y / 100.0);
    } else {
        this->y_pos = this->ys + (this->ye - this->ys) / 2;
    }
    this->need_redraw = true;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
WidgetStick::WidgetStick(void) {
    // TODO add initializers for local data
    //this->setFont();
    this->x = NULL;
    this->y = NULL;
}

/*
 * @brief   ...
 * @details ...
 */
WidgetStick::WidgetStick(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     Lcd::Color bc, Lcd::Alpha ba,
                     Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                     Lcd::Color tc, Lcd::Alpha ta) :
                     WidgetFrame(x, y, w, h, bc, ba, fc, fa, fs) {
    //
    consoleDebug("WidgetStick(%d, %d, %d, %d\r\n", x, y, w, h);
    this->tc = tc;
    this->ta = ta;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetStick::setXSource(int8_t *x) {
    //
    this->x = x;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetStick::setYSource(int8_t *y) {
    //
    this->y = y;
}

#endif /* ITHACA_USE_WIDGET_STICK */

