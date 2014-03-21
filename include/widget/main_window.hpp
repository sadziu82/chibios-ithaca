#ifndef _WIDGET_MAIN_WINDOW_HPP_
#define _WIDGET_MAIN_WINDOW_HPP_

#if ITHACA_USE_WIDGET_MAIN_WINDOW || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
class WidgetMainWindow : public Widget {
    protected:
        virtual void self_redraw(bool force_redraw);
    public:
        WidgetMainWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                         Lcd::Color c, Lcd::Alpha a);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET_MAIN_WINDOW */

#endif /* _WIDGET_MAIN_WINDOW_HPP_ */

