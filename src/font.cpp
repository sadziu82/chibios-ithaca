#include <ithaca.hpp>

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

//
Font::Font(void) {
    this->width = 0;
    this->height = 0;
    this->min = 0;
    this->max = 0;
}

//
uint8_t Font::getWidth(void) {
    return this->width;
}

//
uint8_t Font::getHeight(void) {
    return this->height;
}

//
uint8_t Font::getCharWidth(char c) {
    uint16_t offset;
    const uint8_t *bitmap = this->getBitmap();
    offset = (c - this->min) * (this->height * (this->width >> 3) + 1);
    return bitmap[offset];
}

//
uint16_t Font::getTextWidth(char *text) {
    uint16_t width = 0;
    while (*text != '\0') {
        width += this->getCharWidth(*text++);
    }
    return width;
}

//
uint8_t *Font::getCharBitmap(char c) {
    uint16_t offset;
    const uint8_t *bitmap = this->getBitmap();
    offset = (c - this->min) * (this->height * (this->width >> 3) + 1) + 1;
    return (uint8_t*)bitmap + offset;
}

//
const uint8_t *Font::getBitmap(void) {
    return NULL;
}

#endif /* ITHACA_USE_FONT */

