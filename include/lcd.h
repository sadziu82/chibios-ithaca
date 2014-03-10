#ifndef _LCD_H_
#define _LCD_H_


#if ITHACA_USE_LCD || defined(__DOXYGEN__)

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
 * @brief   Forward declarations
 */
typedef struct LCDConfig LCDConfig;
typedef struct LCDDriver LCDDriver;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    LCD_ROTATION_0,
    LCD_ROTATION_90,
    LCD_ROTATION_180,
    LCD_ROTATION_270,
} lcd_rotation_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    LCD_COLOR_TRANSPARENT = 0x0001,
    LCD_COLOR_BLACK = 0x0000,
    LCD_COLOR_WHITE = 0xFFFF,
    LCD_COLOR_RED = 0x00F8,
    LCD_COLOR_GREEN = 0xE007,
    LCD_COLOR_BLUE = 0x1F00,
} lcd_color_t;

/*
 * @brief   ...
 * @details ...
 */
#include <lcd/lcd_st7735.h>

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
#if ITHACA_USE_LCD_ST7735
    LCD_LLD_TYPE_ST7735,
#endif
} lcd_lld_type_t;

/*
 * @brief   ...
 * @details ...
 */
typedef union {
#if ITHACA_USE_LCD_ST7735
    ST7735Config *st7735;
#endif
} lcd_lld_config_t;

/*
 * @brief   ...
 * @details ...
 */
typedef union {
#if ITHACA_USE_LCD_ST7735
    ST7735Driver st7735;
#endif
} lcd_lld_driver_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct LCDConfig {
    //
    char *name;
    lcd_rotation_t rotation;
    uint16_t page_width, page_height;
    //
    lcd_lld_type_t lld_type;
    lcd_lld_driver_t lld_driver;
    lcd_lld_config_t lld_config;
    //
} LCDConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef struct LCDDriver {
    LCDConfig *config;
    uint16_t *page_buffer_1,
             *page_buffer_2;
    uint16_t *page_buffer_flush,
             *page_buffer_draw;
    uint16_t page_xs, page_ys,
             page_xe, page_ye;
    lcd_color_t bg_color,
                fg_color;
    //ithaca_lock_t lock;
    //WORKING_AREA(radio_thread_working_area, 256);
    //Thread *tp;
    //void *user_arg;
} LCDDriver;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    LCD_WIDGET_ROOT,
    LCD_WIDGET_STATIC_LABEL,
    LCD_WIDGET_LABEL,
} lcd_widget_type_t;


/*
 * @brief   ...
 * @details ...
 */
typedef struct LCDWidget LCDWidget;
typedef struct LCDWidget {
    //
    lcd_widget_type_t type;
    uint16_t xs, ys, xe, ye;
    bool need_redraw;
    LCDWidget *child;
    LCDWidget *focus_prev;
    LCDWidget *focus_next;
    //
    lcd_color_t fg_color;
    lcd_color_t bg_color;
    uint8_t fg_alpha;
    uint8_t bg_alpha;
    //
    void *data;
} LCDWidget;

/*
 * @brief   ...
 * @details ...
 */
typedef uint8_t lcd_alpha_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    LCD_TEXT_ALIGN_LEFT,
    LCD_TEXT_ALIGN_CENTER,
    LCD_TEXT_ALIGN_RIGHT,
} lcd_text_align_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    char *text;
    font_t *font;
    lcd_text_align_t align;
} LCDWidgetStaticLabel;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    char *text;
    font_t *font;
    lcd_text_align_t align;
} LCDWidgetLabel;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
bool lcdInit(LCDDriver *lcd, LCDConfig *cfg);
void lcdPutPixel(LCDDriver *lcd, uint16_t x, uint16_t y, lcd_color_t c);
lcd_color_t lcdGetPixel(LCDDriver *lcd, uint16_t x, uint16_t y);
void lcdFlushPage(LCDDriver *lcd);
inline void lcdPagePutPixel(LCDDriver *lcd,
                            uint16_t x, uint16_t y, lcd_color_t color);
inline void lcdPagePutAlphaPixel(LCDDriver *lcd,
                            uint16_t x, uint16_t y,
                            lcd_color_t color, lcd_alpha_t alpha);
uint16_t lcdGetTextWidth(char *text, const font_t *font);
uint16_t lcdDrawChar(LCDDriver *lcd, char c, uint16_t xs, uint16_t ys, const font_t *font,
                     lcd_color_t fg_color, lcd_alpha_t fg_alpha,
                     lcd_color_t bg_color, lcd_alpha_t bg_alpha);
void lcdWidgetRootRedraw(LCDDriver *lcd, LCDWidget *root);
void lcdWidgetStaticLabelRedraw(LCDDriver *lcd, LCDWidget *widget);
void lcdWidgetLabelRedraw(LCDDriver *lcd, LCDWidget *widget);
void lcdWidgetRedraw(LCDDriver *lcd, LCDWidget *widget, bool force_redraw);
void lcdScreenUpdate(LCDDriver *lcd, LCDWidget *root);
LCDWidget *lcdWidgetCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
LCDWidget *lcdWidgetRootCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
LCDWidget *lcdWidgetStaticLabelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
LCDWidget *lcdWidgetLabelCreate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void lcdWidgetAddChild(LCDWidget *parent, LCDWidget *child);
void lcdSetBgColor(LCDDriver *lcd, lcd_color_t color);
void lcdSetFgColor(LCDDriver *lcd, lcd_color_t color);
void lcdStaticLabelSetText(LCDWidget *widget, char *text,
                           const font_t *font, lcd_text_align_t align);
void lcdLabelSetText(LCDWidget *widget, char *text,
                           const font_t *font, lcd_text_align_t align);
void lcdWidgetSetFgColor(LCDWidget *widget, lcd_color_t color, uint8_t alpha);
void lcdWidgetSetBgColor(LCDWidget *widget, lcd_color_t color, uint8_t alpha);
uint16_t lcdGetWidth(LCDDriver *lcd);
uint16_t lcdGetHeight(LCDDriver *lcd);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_LCD */

#endif /* _LCD_H_ */

