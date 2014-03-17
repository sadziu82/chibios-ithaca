#ifndef _WIDGET_FRAME_HPP_
#define _WIDGET_FRAME_HPP_

#if ITHACA_USE_WIDGET_FRAME || defined(__DOXYGEN__)

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
class WidgetFrame: public Widget {
    protected:
        Lcd::Color frame_color;
        Lcd::Alpha frame_alpha;
        Lcd::LineStyle frame_style;
        virtual void self_redraw(bool force_redraw);
    public:
        WidgetFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    Lcd::Color c, Lcd::Alpha a, Lcd::LineStyle s);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET_FRAME */

#endif /* _WIDGET_FRAME_HPP_ */

