#ifndef _WIDGET_PING_PONG_BALL_HPP_
#define _WIDGET_PING_PONG_BALL_HPP_

#if ITHACA_USE_WIDGET_PING_PONG_BALL || defined(__DOXYGEN__)

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
class WidgetPingPongBall: public Widget {
    protected:
        //
        virtual void self_redraw(bool force_redraw);
        virtual void update_data(void);
        //
        Widget *top, *bottom;
        WidgetPingPongPlayer *left, *right;
        uint16_t xmin, xmax, ymin, ymax, xcur, ycur;
        int8_t dx, dy;
        systime_t restart;
        //
        Lcd::Color c;
        Lcd::Alpha a;
    public:
        WidgetPingPongBall(void);
        WidgetPingPongBall(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    Lcd::Color bc, Lcd::Alpha ba,
                    Lcd::Color c, Lcd::Alpha a);
        void setEnv(Widget *top, Widget *bottom,
                    WidgetPingPongPlayer *left, WidgetPingPongPlayer *right);
        void move(int8_t dx, int8_t dy);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET_PING_PONG_BALL */

#endif /* _WIDGET_PING_PONG_BALL_HPP_ */

