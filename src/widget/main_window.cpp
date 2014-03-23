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
WidgetMainWindow::WidgetMainWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                                   Lcd::Color c, Lcd::Alpha a) :
                                   Widget(x, y, w, h, c, a) {
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
    this->lcd->clearPage(this->bg_color);
}

#endif /* ITHACA_USE_WIDGET_MAIN_WINDOW */

