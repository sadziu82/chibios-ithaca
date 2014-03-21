#include <ithaca.hpp>

#if ITHACA_USE_WIDGET_TOP || defined(__DOXYGEN__)

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

/*
 * @brief   ...
 * @details ...
 */
void WidgetTop::self_redraw(bool force_redraw) {
    //
    if ((this->need_redraw || force_redraw) == false) {
        return;
    }
    //
    WidgetFrame::self_redraw(force_redraw);
    //
    this->drawText(this->xs + 2, this->ys,
                   this->font, "CPU:", Lcd::TextAlign::Left,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 62, this->ys,
                   this->font, this->cpu_pct, Lcd::TextAlign::Right,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 72, this->ys,
                   this->font, "BATT:", Lcd::TextAlign::Left,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 157, this->ys,
                   this->font, this->batt_volt, Lcd::TextAlign::Right,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 2, this->ys + 12,
                   this->font, "MEM:", Lcd::TextAlign::Left,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 85, this->ys + 12,
                   this->font, this->free_mem, Lcd::TextAlign::Right,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 95, this->ys + 12,
                   this->font, "UP:", Lcd::TextAlign::Left,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 157, this->ys + 12,
                   this->font, this->uptime, Lcd::TextAlign::Right,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Transparent);
    //
    this->drawText(this->xs + 1, this->ys + 24,
                   this->font, "NAME", Lcd::TextAlign::Left,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Solid);
    //
    this->drawText(this->xs + 48, this->ys + 24,
                   this->font, "PR", Lcd::TextAlign::Left,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Solid);
    //
    this->drawText(this->xs + 115, this->ys + 24,
                   this->font, "TIME", Lcd::TextAlign::Right,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Solid);
    //
    this->drawText(this->xs + 157, this->ys + 24,
                   this->font, "CPU%", Lcd::TextAlign::Right,
                   this->text_color, this->text_alpha,
                   this->bg_color, Lcd::Alpha::Solid);
    //
    char *text = NULL;
    uint16_t y = this->ys + 36;
    uint8_t i;
    //Thread *tp = chRegFirstThread();
    for (i = 0; i < 6; i++) {
        if (y > this->ye) {
            break;
        }
        //
        this->drawText(this->xs, y,
                       this->font, this->stats[i][0], Lcd::TextAlign::Left,
                       this->text_color, this->text_alpha,
                       this->bg_color, Lcd::Alpha::Solid);
        //
        this->drawText(this->xs + 65, y,
                       this->font, this->stats[i][1], Lcd::TextAlign::Right,
                       this->text_color, this->text_alpha,
                       this->bg_color, Lcd::Alpha::Solid);
        //
        this->drawText(this->xs + 115, y,
                       this->font, this->stats[i][2], Lcd::TextAlign::Right,
                       this->text_color, this->text_alpha,
                       this->bg_color, Lcd::Alpha::Solid);
        //
        this->drawText(this->xs + 157, y,
                       this->font, this->stats[i][3], Lcd::TextAlign::Right,
                       this->text_color, this->text_alpha,
                       this->bg_color, Lcd::Alpha::Solid);
        y += 12;
    }
    //
    if (text != NULL) {
        chHeapFree(text);
        text = NULL;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void WidgetTop::update_data(void) {
    //
    systime_t threads_time = 0;
    //
    uint8_t i, j;
    Thread *tp = chRegFirstThread();
    //
    for (i = 0; i < 6; i++) {
        if (tp != NULL) {
            if (strcmp(tp->p_name, "idle") != 0) {
                //
                threads_time += tp->p_time;
            }
            this->stats[i][0] = asprintf(this->stats[i][0], "%s", tp->p_name);
            this->stats[i][1] = asprintf(this->stats[i][1], "%d", tp->p_prio);
            if (tp->p_time < 99999) {
                this->stats[i][2] = asprintf(this->stats[i][2], "%d", tp->p_time);
            } else {
                this->stats[i][2] = asprintf(this->stats[i][2], "%dk", tp->p_time / 1000);
            }
            this->stats[i][3] = asprintf(this->stats[i][3], "%d", ((100 * tp->p_time) / chTimeNow()));
            //
            tp = chRegNextThread(tp);
        } else {
            for (j = 0; j < 4; j++) {
                if (this->stats[i][j] != NULL) {
                    chHeapFree(this->stats[i][j]);
                    this->stats[i][j] = NULL;
                }
            }
        }
    }
    //
    this->free_mem = asprintf(this->free_mem, "%fkB", chCoreStatus()/1024.0);
    this->uptime = asprintf(this->uptime, "%.2d:%.2d", chTimeNow() / 60000, (chTimeNow() / 1000) % 60);
    this->batt_volt = asprintf(this->batt_volt, "%fV", 13.74);
    this->cpu_pct = asprintf(this->cpu_pct, "%d%%", ((100 * (threads_time - this->last_threads) / (chTimeNow() - this->last_now))));
    //
    this->last_threads = threads_time;
    this->last_now = chTimeNow();
    //
    this->need_redraw = true;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
WidgetTop::WidgetTop(void) {
    // TODO add initializers for local data
    //this->setFont();
}

/*
 * @brief   ...
 * @details ...
 */
WidgetTop::WidgetTop(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                     Lcd::Color bc, Lcd::Alpha ba,
                     Lcd::Color fc, Lcd::Alpha fa, Lcd::LineStyle fs,
                     Font *font, Lcd::TextAlign ha, Lcd::Color tc, Lcd::Alpha ta) :
                     WidgetFrame(x, y, w, h, bc, ba, fc, fa, fs) {
    //
    consoleDebug("WidgetTop(%d, %d, %d, %d\r\n", x, y, w, h);
    this->font = font;
    this->text_align = ha;
    this->text_color = tc;
    this->text_alpha = ta;
}

#endif /* ITHACA_USE_WIDGET_TOP */

