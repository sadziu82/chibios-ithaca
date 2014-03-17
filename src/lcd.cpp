#include <ithaca.hpp>

#if ITHACA_USE_LCD2 || defined(__DOXYGEN__)

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
void Lcd::setRotation(Rotation r) {
    //
    this->rotation = r;
    //
    switch (this->rotation) {
        case Lcd::Rotation::Normal:
        case Lcd::Rotation::Flip:
            this->width = this->raw_width;
            this->height = this->raw_height;
            this->page_width = this->raw_page_width;
            this->page_height = this->raw_page_height;
            break;
        case Lcd::Rotation::Right:
        case Lcd::Rotation::Left:
            this->width = this->raw_height;
            this->height = this->raw_width;
            this->page_width = this->raw_page_height;
            this->page_height = this->raw_page_width;
            break;
    }
}

/*
 * @brief   ...
 * @details ...
 */
bool Lcd::flushPage(void) {
    //
    if (this->page_need_flush == false) {
        return false;
    }
    // swap buffers
    if (this->page_buffer_1 == this->page_buffer_draw) {
        this->page_buffer_draw = this->page_buffer_2;
        this->page_buffer_flush = this->page_buffer_1;
    } else {
        this->page_buffer_draw = this->page_buffer_1;
        this->page_buffer_flush = this->page_buffer_2;
    }
    //
    return true;
};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
Lcd::Lcd(uint16_t width, uint16_t height,
         uint16_t page_width, uint16_t page_height) {
    //
    this->raw_width = this->width = width;
    this->raw_height = this->height = height;
    //
    this->raw_page_width = this->page_width = page_width;
    this->raw_page_height = this->page_height = page_height;
    //
    this->page_size = this->page_width * this->page_height;
    this->raw_page_size = this->page_size * sizeof(Lcd::Color);
};

/*
 * @brief   ...
 * @details ...
 */
void Lcd::init(void) {
    this->page_buffer_1 = (uint16_t *)chCoreAlloc(this->raw_page_size);
    this->page_buffer_2 = (uint16_t *)chCoreAlloc(this->raw_page_size);
    this->page_buffer_draw = this->page_buffer_1;
    this->page_buffer_flush = this->page_buffer_2;
    this->page_need_flush = true;
};

/*
 * @brief   ...
 * @details ...
 */
void Lcd::setMainWindow(Widget *widget) {
    //uint16_t x;
    this->main_window = widget;
    widget->assignLcd(this);
    consoleDebug("setMainWindow()\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
void Lcd::updateScreen(void) {
    //
    if (this->main_window == NULL) {
        return;
    }
    //
    TimeMeasurement screen_update_time;
    tmObjectInit(&screen_update_time);
    tmStartMeasurement(&screen_update_time);
    //
    for (this->page_ys = 0; this->page_ys < this->height;
         this->page_ys += this->page_height) {
        this->page_ye = this->page_ys + this->page_height - 1;
        for (this->page_xs = 0; this->page_xs < this->width;
             this->page_xs += this->page_width) {
            this->page_xe = this->page_xs + this->page_width - 1;
            //consoleDebug("%d %d %d %d\r\n", this->page_xs, this->page_xe,
            //                                this->page_ys, this->page_ye);
            //thisWidgetRedraw(this, root, true);
            // TODO // prepare background
            // TODO for (x = 0; x < this->page_size; x++) {
            // TODO     this->page_buffer_draw[x] = static_cast<uint16_t>(this->bg_color);
            // TODO }
            // by default we will not flush page
            this->page_need_flush = this->main_window->page_flush_needed();
            consoleDebug("page flush: %d\r\n", this->page_need_flush);
            this->main_window->redraw(this->page_need_flush);
            // call virtual flushing method
            this->flushPage();
        }
    }
    //
    tmStopMeasurement(&screen_update_time);
    consoleDebug("screenUpdate() took %d ms\r\n", RTT2MS(screen_update_time.last));
};

/*
 * @brief   ...
 * @details ...
 */
void Lcd::pageChanged(void) {
    this->page_need_flush = true;
}

#endif /* ITHACA_USE_LCD2 */

