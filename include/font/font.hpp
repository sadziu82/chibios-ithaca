#ifndef _FONT_H__
#define _FONT_H__

#include "ch.h"

/*
 * @brief   ...
 */
class Font {
public:
    virtual uint8_t get_font_width( void );
    virtual uint8_t get_font_height( void );
    virtual uint8_t get_char_width( uint16_t c );
    virtual uint8_t *get_char_bitmap( uint16_t c );
};


#endif /* _FONT_H__ */

