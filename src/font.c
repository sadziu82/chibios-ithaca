#include <ithaca.h>

#if ITHACA_USE_FONT || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

uint8_t fontGetWidth(const font_t *font) {
    return font->width;
}

uint8_t fontGetHeight(const font_t *font) {
    return font->height;
}

uint8_t fontGetCharWidth(const font_t *font, const char c) {
    uint16_t offset;
    offset = (c - font->min) * (font->height * (font->width >> 3) + 1);
    return font->bitmap[offset];
}

uint8_t *fontGetCharBitmap(const font_t *font, const char c) {
    uint16_t offset;
    offset = (c - font->min) * (font->height * (font->width >> 3) + 1) + 1;
    return (uint8_t*)font->bitmap + offset;
}

#endif /* ITHACA_USE_FONT */

