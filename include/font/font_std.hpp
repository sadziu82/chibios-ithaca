#ifndef _STD_FONT_H__
#define _STD_FONT_H__

#include "font.hpp"

/*
 * @brief   ...
 */
class StdFont: public Font {
public:
    StdFont( void );
    uint8_t get_font_width( void );
    uint8_t get_font_height( void );
    uint8_t get_char_width( uint16_t c );
    uint8_t *get_char_bitmap( uint16_t c );
private:
    const static uint8_t font_width;
    const static uint8_t font_height;
    const static uint8_t char_min;
    const static uint8_t char_max;
    const static uint8_t char_matrix[];
};


#endif /* _STD_FONT_H__ */

