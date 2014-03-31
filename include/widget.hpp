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

//
typedef enum {
    
} input_event_type_t;

//
typedef struct {
    input_event_type_t type;
    uint8_t code;
} input_event_t;

/*
 * @brief   ...
 * @details ...
 */
class Widget {
    public:
        enum class DataSourceType : uint8_t;
    protected:
        //
        virtual void self_redraw(bool force_redraw);
        virtual void update_data(void);
        virtual bool process_event(input_event_t event);
        //
        Lcd *lcd;
        Widget *parent;
        Widget *child;
        Widget *prev;
        Widget *next;
        //
        uint16_t xs, ys, xe, ye;
        //
        Lcd::Color bg_color;
        Lcd::Alpha bg_alpha;
        //
        bool need_redraw;
    public:
        Widget(void);
        Widget(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
               Lcd::Color c, Lcd::Alpha a);
        void setGeometry(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void setBgColor(Lcd::Color c, Lcd::Alpha a);
        //
        bool page_flush_needed(void);
        void redraw(bool force_redraw);
        void updateData(void);
        bool processEvent(input_event_t event);
        void assignLcd(Lcd *lcd);
        void setParent(Widget *parent);
        //
        Widget *getNext(void);
        void addChild(Widget *c);
        void addNeighbour(Widget *w);
        uint8_t drawChar(uint16_t col, uint16_t row, Font *font, char c,
                         Lcd::Color fc, Lcd::Alpha fa, Lcd::Color bc, Lcd::Alpha ba);
        void  drawText(uint16_t col, uint16_t row,
                       Font *font, char *text, Lcd::TextAlign ha,
                       Lcd::Color fc, Lcd::Alpha fa,
                       Lcd::Color bc, Lcd::Alpha ba);
        inline uint16_t getXS(void);
        inline uint16_t getXE(void);
        inline uint16_t getYS(void);
        inline uint16_t getYE(void);
        void childGeometryChanged(void);
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

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Widget::getXS(void) {
    return this->xs;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Widget::getXE(void) {
    return this->xe;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Widget::getYS(void) {
    return this->ys;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Widget::getYE(void) {
    return this->ye;
}

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET */

#endif /* _WIDGET_HPP_ */

