#include <ithaca.hpp>

#if ITHACA_USE_WIDGET_PING_PONG_PLAYER || defined(__DOXYGEN__)

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
void WidgetPingPongPlayer::self_redraw(bool force_redraw) {
    //
    if ((this->need_redraw || force_redraw) == false) {
        return;
    }
    //
    WidgetFrame::self_redraw(force_redraw);
    uint16_t y;
    for (y = this->pos - 10; y < this->pos + 11; y++) {
        this->lcd->drawHLine(this->xs, y, this->xe - this->xs, Lcd::Color::White, Lcd::Alpha::Solid);
    }
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPingPongPlayer::update_data(void) {
    //
    if (this->input != NULL) {
        this->need_redraw = true;
        this->pos = this->ye - 10 - (this->ye - this->ys - 20) * (*this->input / 100.0);
    } else {
        this->pos = this->ys + (this->ye - this->ys) / 2;
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
WidgetPingPongPlayer::WidgetPingPongPlayer(void) {
    // TODO add initializers for local data
    //this->setFont();
    this->input = NULL;
}

/*
 * @brief   ...
 * @details ...
 */
WidgetPingPongPlayer::WidgetPingPongPlayer(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     Lcd::Color bc, Lcd::Alpha ba,
                     Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                     Lcd::Color tc, Lcd::Alpha ta) :
                     WidgetFrame(x, y, w, h, bc, ba, fc, fa, fs) {
    //
    consoleDebug("WidgetPingPongPlayer(%d, %d, %d, %d\r\n", x, y, w, h);
    this->tc = tc;
    this->ta = ta;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPingPongPlayer::setInputSource(int8_t *input) {
    //
    this->input = input;
}

#endif /* ITHACA_USE_WIDGET_PING_PONG_PLAYER */

