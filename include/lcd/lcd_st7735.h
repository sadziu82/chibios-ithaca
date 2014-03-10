#ifndef _LCD_ST7735_H_
#define _LCD_ST7735_H_

#if ITHACA_USE_LCD_ST7735 || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#define LCD_FRAMEBUFFER_WIDTH       32
#define LCD_FRAMEBUFFER_HEIGHT      32
#define LCD_FRAMEBUFFER_SIZE        LCD_FRAMEBUFFER_WIDTH * LCD_FRAMEBUFFER_HEIGHT

#define LCD_ST7735_WIDTH            128
#define LCD_ST7735_HEIGHT           160

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    // spi driver
    SPIDriver *spi_drv;
    // spi cs/ce
    ioportid_t cs_port;
    uint16_t cs_pin;
    // d/c pin
    ioportid_t dc_port;
    uint16_t dc_pin;
    // rst pin
    ioportid_t rst_port;
    uint16_t rst_pin;
} ST7735Config;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    ST7735Config *config;
    lcd_rotation_t rotation;
    uint16_t width;
    uint16_t height;
} ST7735Driver;

//// /*
////  * @brief   ...
////  * @details ...
////  */
//// typedef uint16_t lcd_pixel_t;
//// 
//// /*
////  * @brief   ...
////  * @details ...
////  */
//// typedef enum {
////     LCD_ALIGN_LEFT = 0,
////     LCD_ALIGN_CENTER,
////     LCD_ALIGN_RIGHT,
//// } lcd_align_t;
//// 
//// /*
////  * @brief   ...
////  * @details ...
////  */
//// typedef struct {
////     //
////     SPIDriver *spi_driver;
////     ioportid_t cs_port;
////     uint16_t cs_pin;
////     ioportid_t dc_port;
////     uint16_t dc_pin;
////     ioportid_t rst_port;
////     uint16_t rst_pin;
////     // 
////     uint16_t viewport_x1;
////     uint16_t viewport_y1;
////     uint16_t viewport_x2;
////     uint16_t viewport_y2;
////     //
////     lcd_pixel_t framebuffer1[LCD_FRAMEBUFFER_SIZE];
////     lcd_pixel_t framebuffer2[LCD_FRAMEBUFFER_SIZE];
////     lcd_pixel_t *active_framebuffer;
////     lcd_pixel_t *shadow_framebuffer;
////     //
////     uint16_t framebuffer_x1;
////     uint16_t framebuffer_x2;
////     uint16_t framebuffer_y1;
////     uint16_t framebuffer_y2;
////     //
////     lcd_color_t bg_color;
////     lcd_color_t fg_color;
////     font_t *font;
//// } lcd_st7735_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
bool st7735_lld_init(LCDDriver *lcd);
void st7735_lld_put_pixel(ST7735Driver *st7735,
                          uint16_t x, uint16_t y,
                          lcd_color_t color);
lcd_color_t st7735_lld_get_pixel(ST7735Driver *st7735,
                                 uint16_t x, uint16_t y);
void st7735_lld_set_viewport(ST7735Driver *st7735,
                             uint16_t x, uint16_t y,
                             uint16_t w, uint16_t h);
void st7735_lld_data_write(ST7735Driver *st7735, uint16_t len, uint16_t data[]);
void st7735_lld_start_data_write(ST7735Driver *st7735, uint16_t len, uint16_t data[]);
////    void lcdST7735ClearScreen(lcd_st7735_t *lcd, uint16_t color);
////    uint8_t lcdST7735DrawChar(lcd_st7735_t *lcd, uint16_t x, uint16_t y,
////                              char c);
////    uint16_t lcdST7735DrawString(lcd_st7735_t *lcd, uint16_t x, uint16_t y,
////                                 char *s, lcd_align_t align);
////    bool lcdSetBgColor(lcd_st7735_t *lcd, varg_t arg);
////    bool lcdSetFgColor(lcd_st7735_t *lcd, varg_t arg);
////    bool lcdSetFont(lcd_st7735_t *lcd, varg_t arg);
////    void lcdST7735PutPixel(lcd_st7735_t *lcd,
////                           uint16_t y, uint16_t x, lcd_color_t c);
////    bool lcdST7735FramebufferSwap(lcd_st7735_t *lcd, varg_t arg);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_LCD_ST7735 */

#endif /* _LCD_ST7735_H_ */

