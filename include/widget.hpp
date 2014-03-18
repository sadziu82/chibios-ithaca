#ifndef _WIDGET_HPP_
#define _WIDGET_HPP_

#if ITHACA_USE_WIDGET || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

//
class Lcd;

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
class Widget {
    public:
        enum class DataSourceType : uint8_t;
    protected:
        Lcd *lcd;
        Widget *parent;
        Widget *child;
        Widget *prev;
        Widget *next;
        //
        Lcd::Color bg_color;
        Lcd::Alpha bg_alpha;
        //
        uint16_t xs, ys, xe, ye;
        bool need_redraw;
        virtual void self_redraw(bool force_redraw);
        virtual void update_data(void);
    public:
        bool page_flush_needed(void);
        Widget(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void redraw(bool force_redraw);
        void updateData(void);
        void assignLcd(Lcd *lcd);
        void setParent(Widget *parent);
        //
        void setBgColor(Lcd::Color c, Lcd::Alpha a);
        //
        Widget *getNext(void);
        void addChild(Widget *c);
        void addNeighbour(Widget *w);
        uint8_t drawChar(uint16_t col, uint16_t row, Font *font, char c,
                         Lcd::Color fc, Lcd::Alpha fa, Lcd::Color bc, Lcd::Alpha ba);
};

//
enum class Widget::DataSourceType : uint8_t {
    TEXT,
    UINT8,
    INT8,
    UINT16,
    INT16,
    UINT32,
    INT32,
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET */

#endif /* _WIDGET_HPP_ */

