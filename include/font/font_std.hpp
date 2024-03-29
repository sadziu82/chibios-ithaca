#ifndef _FONT_STD_HPP_
#define _FONT_STD_HPP_

#if ITHACA_USE_FONT_STD || defined(__DOXYGEN__)

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

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
class FontStd: public Font {
    protected:
        static const uint8_t bitmap[];
    public:
        FontStd(void);
        virtual const uint8_t *getBitmap(void);
};

#endif /* ITHACA_USE_FONT_STD */

#endif /* _FONT_STD_HPP_ */

