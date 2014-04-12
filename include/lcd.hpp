#ifndef _LCD_HPP_
#define _LCD_HPP_

#if ITHACA_USE_LCD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
class Widget;

/*
 * @brief   ...
 * @details ...
 */
class Font;

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
class Lcd {
    public:
        //
        enum class Rotation : uint8_t;
        enum class Color : uint16_t;
        enum class Alpha : uint8_t;
        enum class LineStyle : uint8_t;
        enum class TextAlign : uint8_t;
    protected:
        // physical size of lcd
        uint16_t raw_width, raw_height,
                 width, height;
        // page buffer
        uint16_t *page_buffer_1,
                 *page_buffer_2,
                 *page_buffer_flush,
                 *page_buffer_draw;
        uint16_t raw_page_size, page_size,
                 raw_page_width, raw_page_height,
                 page_width, page_height,
                 page_xs, page_xe,
                 page_ys, page_ye;
        bool page_need_flush;
        //
        Rotation rotation;
        //
        void setRotation(Rotation r);
        virtual bool flushPage(void);
        //
        InputEventQueue *event_queue;
        Widget *root_window;
    public:
        Lcd(uint16_t width, uint16_t height,
            uint16_t page_width, uint16_t page_height);
        void init(void);
        void setRootWindow(Widget *widget);
        void setEventQueue(InputEventQueue *queue);
        void updateScreen(void);
        inline uint16_t getPageXS(void);
        inline uint16_t getPageXE(void);
        inline uint16_t getPageYS(void);
        inline uint16_t getPageYE(void);
        inline uint16_t getPageWidth(void);
        inline uint16_t getPageHeight(void);
        inline uint16_t getWidth(void);
        inline uint16_t getHeight(void);
        inline void putPixel(uint16_t x, uint16_t y, Color c, Alpha a);
        void clearPage(Color c);
        void drawHLine(uint16_t x, uint16_t y, uint16_t l, Color c, Alpha a);
        void drawVLine(uint16_t x, uint16_t y, uint16_t l, Color c, Alpha a);
        void drawDot(uint16_t x, uint16_t y, uint16_t r, Color c, Alpha a);
};

/*
 * @brief   ...
 * @details ...
 */
enum class Lcd::Rotation : uint8_t {
    Normal = 0x01,
    Right = 0x02,
    Flip = 0x03,
    Left = 0x04,
};

/*
 * @brief   ...
 * @details ...
 */
enum class Lcd::Color : uint16_t {
    Black = 0x0000,
    Red = 0x00F8,
    Green = 0xE007,
    Blue = 0x1F00,
    Grey = 0xF79E,
    White = 0xFFFF,
};

/*
 * @brief   ...
 * @details ...
 */
enum class Lcd::Alpha : uint8_t {
    Transparent = 0x00,
    Half = 0x80,
    Solid = 0xFF,
};

/*
 * @brief   ...
 * @details ...
 */
enum class Lcd::LineStyle : uint8_t {
    None,
    Solid,
    Dotted,
    Dashed,
    DDashed,
};

/*
 * @brief   ...
 * @details ...
 */
enum class Lcd::TextAlign : uint8_t {
    Left,
    Center,
    Right,
};

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getPageXS(void) {
    return this->page_xs;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getPageXE(void) {
    return this->page_xe;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getPageYS(void) {
    return this->page_ys;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getPageYE(void) {
    return this->page_ye;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getPageWidth(void) {
    return this->page_width;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getPageHeight(void) {
    return this->page_height;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getWidth(void) {
    return this->width;
}

/*
 * @brief   ...
 * @details ...
 */
inline uint16_t Lcd::getHeight(void) {
    return this->height;
}

/*
 * @brief   ...
 * @details ...
 */
inline void Lcd::putPixel(uint16_t x, uint16_t y, Color c, Alpha a) {
    // FIXME add alpha handling
    (void)a;
    //
    x &= 0x1F;
    y &= 0x1F;
    //
    this->page_buffer_draw[(y * this->page_width) + x] = static_cast<uint16_t>(c);
}

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_LCD */

#endif /* _LCD_HPP_ */

