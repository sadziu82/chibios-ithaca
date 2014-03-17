#include <ithaca.hpp>

#if ITHACA_USE_WIDGET_MAIN_WINDOW || defined(__DOXYGEN__)

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
WidgetMainWindow::WidgetMainWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) :
                                   Widget(x, y, w, h) {
    //
    consoleDebug("WidgetMainWindow(%d, %d, %d, %d\r\n", x, y, w, h);
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetMainWindow::self_redraw(bool force_redraw) {
    //
    if ((this->need_redraw || force_redraw) == false) {
        return;
    }
    //
    Widget::self_redraw(force_redraw);
    //
    uint16_t x, y;
    //
    for (y = this->lcd->getPageYS(); y <= this->lcd->getPageYE(); y++) {
        for (x = this->lcd->getPageXS(); x <= this->lcd->getPageXE(); x++) {
            this->lcd->putPixel(x, y, this->bg_color, Lcd::Alpha::Solid);
        }
    }
}

#endif /* ITHACA_USE_WIDGET_MAIN_WINDOW */

