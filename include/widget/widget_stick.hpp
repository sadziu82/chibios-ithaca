#ifndef _WIDGET_STICK_HPP_
#define _WIDGET_STICK_HPP_

#if ITHACA_USE_WIDGET_STICK || defined(__DOXYGEN__)

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
class WidgetStick: public WidgetFrame {
    protected:
        //
        virtual void self_redraw(bool force_redraw);
        virtual void update_data(void);
        //
        int8_t *x, *y;
        uint16_t x_pos, y_pos;
        //
        Lcd::Color tc;
        Lcd::Alpha ta;
    public:
        WidgetStick(void);
        WidgetStick(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    Lcd::Color bc, Lcd::Alpha ba,
                    Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                    Lcd::Color tc, Lcd::Alpha ta);
        void setXSource(int8_t *x);
        void setYSource(int8_t *y);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET_STICK */

#endif /* _WIDGET_STICK_HPP_ */

