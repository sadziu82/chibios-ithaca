#ifndef _WIDGET_PING_PONG_PLAYER_HPP_
#define _WIDGET_PING_PONG_PLAYER_HPP_

#if ITHACA_USE_WIDGET_PING_PONG_PLAYER || defined(__DOXYGEN__)

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
class WidgetPingPongPlayer: public WidgetFrame {
    protected:
        //
        virtual void self_redraw(bool force_redraw);
        virtual void update_data(void);
        //
        int8_t *input;
        uint16_t pos;
        //
        Lcd::Color tc;
        Lcd::Alpha ta;
    public:
        WidgetPingPongPlayer(void);
        WidgetPingPongPlayer(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    Lcd::Color bc, Lcd::Alpha ba,
                    Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                    Lcd::Color tc, Lcd::Alpha ta);
        void setInputSource(int8_t *input);
        inline uint16_t getMinPos(void);
        inline uint16_t getMaxPos(void);
};

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t WidgetPingPongPlayer::getMinPos(void) {
    return this->pos - 10;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t WidgetPingPongPlayer::getMaxPos(void) {
    return this->pos + 11;
}

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET_PING_PONG_PLAYER */

#endif /* _WIDGET_PING_PONG_PLAYER_HPP_ */

