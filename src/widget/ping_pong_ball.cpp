#include <ithaca.hpp>

#if ITHACA_USE_WIDGET_PING_PONG_BALL || defined(__DOXYGEN__)

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
void WidgetPingPongBall::self_redraw(bool force_redraw) {
    //
    if ((this->need_redraw || force_redraw) == false) {
        return;
    }
    //
    Widget::self_redraw(force_redraw);
    this->lcd->drawDot(this->xs + 4, this->ys + 4, 7, this->c, this->a);
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPingPongBall::update_data(void) {
    if ((this->top != NULL) && (restart < chTimeNow())) {
        this->setGeometry(this->xcur - 4, this->ycur - 4, 8, 8);
        //
        this->xcur += dx;
        if ((this->xcur + 4 >= xmax) && (dx > 0)) {
            if ((this->right->getMinPos() > this->ycur + 7) ||
                (this->right->getMaxPos() < this->ycur + 1)) {
                restart = chTimeNow() + S2ST(3);
                this->xcur = (this->xmin + this->xmax) / 2;
                this->ycur = (this->ymin + this->ymax) / 2;
                //chSysHalt();
            }
            dx = -(halGetCounterValue() % 3) - 2;
            if (dy > 0) {
                dy = halGetCounterValue() % 2 + 1;
            } else {
                dy = -(halGetCounterValue() % 2) - 1;
            }
        } else if ((this->xcur - 4 <= xmin) && (dx < 0)) {
            if ((this->left->getMinPos() > this->ycur + 7) ||
                (this->left->getMaxPos() < this->ycur + 1)) {
                restart = chTimeNow() + S2ST(3);
                this->xcur = (this->xmin + this->xmax) / 2;
                this->ycur = (this->ymin + this->ymax) / 2;
                //chSysHalt();
            }
            dx = halGetCounterValue() % 3 + 2;
            if (dy > 0) {
                dy = halGetCounterValue() % 2 + 1;
            } else {
                dy = -(halGetCounterValue() % 2) - 1;
            }
        }
        //
        this->ycur += dy;
        if ((this->ycur + 8 >= ymax) && (dy > 0)) {
            dy = -dy;
        } else if ((this->ycur - 8 <= ymin) && (dy < 0)) {
            dy = -dy;
        }
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
WidgetPingPongBall::WidgetPingPongBall(void) {
    // TODO add initializers for local data
    //this->setFont();
}

/*
 * @brief   ...
 * @details ...
 */
WidgetPingPongBall::WidgetPingPongBall(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     Lcd::Color bc, Lcd::Alpha ba,
                     Lcd::Color c, Lcd::Alpha a) :
                     Widget(x, y, w, h, bc, ba) {
    //
    consoleDebug("WidgetPingPongBall(%d, %d, %d, %d\r\n", x, y, w, h);
    this->c = c;
    this->a = a;
    this->top = NULL;
    this->bottom = NULL;
    this->left = NULL;
    this->right = NULL;
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPingPongBall::setEnv(Widget *top, Widget *bottom,
                                WidgetPingPongPlayer *left, WidgetPingPongPlayer *right) {
    //
    this->top = top;
    this->bottom = bottom;
    this->left = left;
    this->right = right;
    //
    this->ymin = this->top->getYE();
    this->ymax = this->bottom->getYS();
    this->xmin = this->left->getXE();
    this->xmax = this->right->getXS();
    //
    this->xcur = (this->xmin + this->xmax) / 2;
    this->ycur = (this->ymin + this->ymax) / 2;
    //
    this->dx = 2;
    this->dy = 1;
    //
    this->restart = chTimeNow() + S2ST(5);
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetPingPongBall::move(int8_t dx, int8_t dy) {
}

#endif /* ITHACA_USE_WIDGET_PING_PONG_BALL */

