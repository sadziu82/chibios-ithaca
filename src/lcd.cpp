#include <ithaca.hpp>

#if ITHACA_USE_LCD || defined(__DOXYGEN__)

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
    // update data in all widgets
    this->main_window->updateData();
    // redraw widgets if necessary
    for (this->page_ys = 0; this->page_ys < this->height;
         this->page_ys += this->page_height) {
        //
        this->page_ye = this->page_ys + this->page_height - 1;
        //
        for (this->page_xs = 0; this->page_xs < this->width;
             this->page_xs += this->page_width) {
            //
            this->page_xe = this->page_xs + this->page_width - 1;
            // by default we will not flush page
            this->page_need_flush = this->main_window->page_flush_needed();
            //consoleDebug("%d, %d, %d\r\n", this->page_xs, this->page_ys, this->page_need_flush);
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
void Lcd::clearPage(Color c) {
    //
    uint16_t i;
    //
    for (i = 0; i < this->page_size; i++) {
        this->page_buffer_draw[i] = static_cast<uint16_t>(c);
    }
}

/*
 * @brief   ...
 * @details ...
 */
void Lcd::drawHLine(uint16_t x, uint16_t y, uint16_t l, Color c, Alpha a) {
    // FIXME add alpha handling
    (void)a;
    //
    uint16_t xmin = MAX(this->page_xs, x),
             xmax = MIN(this->page_xe, x + l);
    //
    if ((x + l < this->page_xs) || (x > this->page_xe) ||
        (y < this->page_ys) || (y > this->page_ye)) {
        return;
    }
    //
    xmin &= 0x1F;
    xmax &= 0x1F;
    //
    uint16_t idx;
    idx = ((y - this->page_ys) * this->page_width) + xmin;
    //
    for (x = xmin; x <= xmax; x++) {
        this->page_buffer_draw[idx++] = static_cast<uint16_t>(c);
    }
}

/*
 * @brief   ...
 * @details ...
 */
void Lcd::drawVLine(uint16_t x, uint16_t y, uint16_t l, Color c, Alpha a) {
    // FIXME add alpha handling
    (void)a;
    //
    uint16_t ymin = MAX(this->page_ys, y),
             ymax = MIN(this->page_ye, y + l);
    //
    if ((x < this->page_xs) || (x > this->page_xe) ||
        (y + l < this->page_ys) || (y > this->page_ye)) {
        return;
    }
    //
    ymin &= 0x1F;
    ymax &= 0x1F;
    //
    uint16_t idx;
    idx = (ymin * this->page_width) + (x & 0x1F);
    //
    for (y = ymin; y <= ymax; y++) {
        this->page_buffer_draw[idx] = static_cast<uint16_t>(c);
        idx += this->page_width;
    }
}

#endif /* ITHACA_USE_LCD */

