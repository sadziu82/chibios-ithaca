#ifndef _WIDGET_LABEL_HPP_
#define _WIDGET_LABEL_HPP_

#if ITHACA_USE_WIDGET_LABEL || defined(__DOXYGEN__)

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
class WidgetLabel: public WidgetFrame {
    protected:
        //
        virtual void self_redraw(bool force_redraw);
        virtual void update_data(void);
        //
        char *text;
        Font *font;
        //
        char *fmt;
        void *data;
        //
        Lcd::TextAlign text_align;
        Lcd::Color text_color;
        Lcd::Alpha text_alpha;
        //
        Widget::DataSourceType data_type;
    public:
        WidgetLabel(void);
        WidgetLabel(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    Lcd::Color bc, Lcd::Alpha ba,
                    Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                    Font *font, Lcd::TextAlign ha, Lcd::Color tc, Lcd::Alpha ta);
        void setFont(Font *font, Lcd::TextAlign ha, Lcd::Color tc, Lcd::Alpha ta);
        void setText(char *fmt, ...);
        void setDataSource(char *fmt, uint8_t *data);
        void setDataSource(char *fmt, uint32_t *data);
        void update(void);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET_LABEL */

#endif /* _WIDGET_LABEL_HPP_ */

