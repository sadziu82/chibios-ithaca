#ifndef _FONT_H_
#define _FONT_H_

#if ITHACA_USE_FONT || defined(__DOXYGEN__)

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
typedef struct {
    uint8_t width;
    uint8_t height;
    uint8_t min;
    uint8_t max;
    uint8_t bitmap[];
} font_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
    uint8_t fontGetWidth(const font_t *);
    uint8_t fontGetHeight(const font_t *);
    uint8_t fontGetCharWidth(const font_t *, const char c);
    uint8_t *fontGetCharBitmap(const font_t *, const char c);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_FONT */

#endif /* _FONT_H_ */

