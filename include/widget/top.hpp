#ifndef _WIDGET_TOP_HPP_
#define _WIDGET_TOP_HPP_

#if ITHACA_USE_WIDGET_TOP || defined(__DOXYGEN__)

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
class WidgetTop: public WidgetFrame {
    protected:
        //
        virtual void self_redraw(bool force_redraw);
        virtual void update_data(void);
        //
        Font *font = NULL;
        char *cpu_pct = NULL,
             *free_mem = NULL,
             *batt_volt = NULL,
             *uptime = NULL;
        char *stats[6][4] = {
            {NULL, NULL, NULL, NULL,},
            {NULL, NULL, NULL, NULL,},
            {NULL, NULL, NULL, NULL,},
            {NULL, NULL, NULL, NULL,},
            {NULL, NULL, NULL, NULL,},
            {NULL, NULL, NULL, NULL,},
        };
        systime_t last_now, last_threads;
        //
        Lcd::TextAlign text_align;
        Lcd::Color text_color;
        Lcd::Alpha text_alpha;
        //
        Widget::DataSourceType data_type;
    public:
        WidgetTop(void);
        WidgetTop(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    Lcd::Color bc, Lcd::Alpha ba,
                    Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                    Font *font, Lcd::TextAlign ha, Lcd::Color tc, Lcd::Alpha ta);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_WIDGET_TOP */

#endif /* _WIDGET_TOP_HPP_ */

