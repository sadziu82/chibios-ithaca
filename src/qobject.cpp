#include <ithaca.hpp>

#if ITHACA_USE_QOBJECT || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   Color Lookup Table for RGB565.
 * @details This table is used to translate colors from 4bit notation
            into RGB565.
 */
const QCore::Color565 QCore::Color565LookupTable[] = {
    QCore::Color565::Black,
    QCore::Color565::Red,
    QCore::Color565::Green,
    QCore::Color565::Blue,
    QCore::Color565::White,
};

/*
 * @brief   Statup sequence for LCD with ST7735 controller.
 */
const uint8_t QLcdST7735::_init_sequence[] = {
    static_cast<uint8_t>(QLcdST7735::Register::SwReset), 0, 50,
    static_cast<uint8_t>(QLcdST7735::Register::SlpOut), 0, 50,
    static_cast<uint8_t>(QLcdST7735::Register::FrmCtr1), 3, 0x01, 0x2C, 0x2D, 0,
    static_cast<uint8_t>(QLcdST7735::Register::FrmCtr2), 3, 0x01, 0x2C, 0x2D, 0,
    static_cast<uint8_t>(QLcdST7735::Register::FrmCtr3), 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, 0,
    static_cast<uint8_t>(QLcdST7735::Register::InvCtr), 1, 0x07, 0,
    static_cast<uint8_t>(QLcdST7735::Register::PwCtr1), 3, 0xA2, 0x02, 0x84, 0,
    static_cast<uint8_t>(QLcdST7735::Register::PwCtr2), 1, 0xC5, 0,
    static_cast<uint8_t>(QLcdST7735::Register::PwCtr3), 2, 0x0A, 0x00, 0,
    static_cast<uint8_t>(QLcdST7735::Register::PwCtr4), 2, 0x8A, 0x2A, 0,
    static_cast<uint8_t>(QLcdST7735::Register::PwCtr5), 2, 0x8A, 0xEE, 0,
    static_cast<uint8_t>(QLcdST7735::Register::VmCtr1), 1, 0x0E, 0,
    static_cast<uint8_t>(QLcdST7735::Register::InvOff), 0, 0,
    static_cast<uint8_t>(QLcdST7735::Register::ColMod), 1, 0x05, 0,
    static_cast<uint8_t>(QLcdST7735::Register::GmCtrP1), 16,
          0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 
          0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, 
        0,
    static_cast<uint8_t>(QLcdST7735::Register::GmCtrN1), 16,
          0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
          0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
        5,
    //  last
    static_cast<uint8_t>(QLcdST7735::Register::NorOn), 0, 10,
    static_cast<uint8_t>(QLcdST7735::Register::DispOn), 0, 20,
    0,
};

/*
 * @brief   Set rotation sequence for LCD with ST7735 controller.
 */
const uint8_t QLcdST7735::_rotation_sequence[] = {
    static_cast<uint8_t>(QLcdST7735::Register::MadCtl), 1, 0x40, 0,
    0,
};

/*
 * @brief   Set viewport sequence for LCD with ST7735 controller.
 */
const uint8_t QLcdST7735::_viewport_sequence[] = {
    static_cast<uint8_t>(QLcdST7735::Register::RaSet), 4, 0x00, 0x00, 0x00, 0x9F, 0,
    static_cast<uint8_t>(QLcdST7735::Register::CaSet), 4, 0x00, 0x00, 0x00, 0x7F, 0,
    static_cast<uint8_t>(QLcdST7735::Register::RamWr), 0, 0,
    0,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   Acquire SPI bus and assert ChipSelect signal for ST7735 controller.
 */
void QLcdST7735::assertCS(void) {
    spiAcquireBus(this->_spi_drv);
    palClearPad(this->_cs_port, this->_cs_pin);
};

/*
 * @brief   Release ChipSelect signal from ST7735 controller and release SPI bus.
 */
void QLcdST7735::releaseCS(void) {
    palSetPad(this->_cs_port, this->_cs_pin);
    spiReleaseBus(this->_spi_drv);
};

/*
 * @brief   Send commands sequence to ST7735 controller.
 * @param[in] seq   pointer to commands sequence
 */
void QLcdST7735::sendCommand(const uint8_t seq[]) {
    //
    uint16_t i = 0, argc = 0, delay = 0;
    consoleDebug("QLcdST7735::setRotation() send command\r\n");
    // process sequence
    do {
        // send command
        this->assertDC();
        // assert chip select
        this->assertCS();
        // send command
        spiSend(this->_spi_drv, 1, &seq[i++]);
        // send arguments if any
        argc = seq[i++];
        if (argc > 0) {
            this->releaseDC();
            spiSend(this->_spi_drv, argc, &seq[i]);
            i += argc;
        }
        // wwait if necessary
        delay = seq[i++];
        if (delay > 0) {
            //consoleDebug("delay: %d\r\n", delay);
            chThdSleepMilliseconds(delay);
        }
        // reselese chip select
        this->releaseCS();
    } while (seq[i] != 0);
};

/*
 * @brief   Working Area for QLcdST7735::flushThread thread()
 */
WORKING_AREA(QLcdST7735::_flush_wa, 512);

/*
 * @brief   This thread will send framebuffer content to LCD
 * @details This is a static method. It will call @flushBuffer() method
            on object from parameter in a separate thread.
 * @param[in] arg   pointer to the QLcdST7735 object
 */
msg_t QLcdST7735::flushThread(void *arg) {
    //
    chRegSetThreadName("QLcdST7735");
    QLcdST7735 *qlcd = (QLcdST7735 *)arg;
    //
    while (true) {
        qlcd->flushBuffer();
    }
    //
    return RDY_OK;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   Constructor for QEvent class.
 * @param[in] type  QEvent type
 */
QEvent::QEvent(Type type) {
    //
    this->_type = type;
    this->_accepted = false;
    //
    consoleDebug("QEvent(0x%.8lx, %d) created\r\n", this, this->_type);
}

/*
 * @brief   Destructor for QEvent class.
 */
QEvent::~QEvent() {
    //
    consoleDebug("QEvent(0x%.8lx, %d) destroyed\r\n", this, this->_type);
}

/*
 * @brief   Public method to return QEvent type.
 */
QEvent::Type QEvent::type(void) {
    //
    return this->_type;
}

/*
 * @brief   Public method to check if QEvent has been accepted already.
 */
bool QEvent::isAccepted(void) {
    //
    return this->_accepted;
}

/*
 * @brief   Public method to allow "accepting" QEvent.
 */
void QEvent::setAccepted(bool accepted) {
    //
    this->_accepted = accepted;
}

/*
 * @brief   ...
 * @details ...
 */
QChildEvent::QChildEvent(Type type, QObject *child): QEvent(type) {
    //
    this->_child = child;
    consoleDebug("QChildEvent(%d) created\r\n", this->_child);
}

/*
 * @brief   ...
 * @details ...
 */
QObject *QChildEvent::child(void) {
    //
    return this->_child;
}

/*
 * @brief   ...
 * @details ...
 */
QKeyEvent::QKeyEvent(Type type, QCore::Key key): QEvent(type) {
    //
    this->_key = key;
    consoleDebug("QKeyEvent(%d) created\r\n", this->_key);
}

/*
 * @brief   ...
 * @details ...
 */
QCore::Key QKeyEvent::key(void) {
    //
    return this->_key;
}

/*
 * @brief   ...
 * @details ...
 */
QLcd::QLcd(QCoord width, QCoord height) {
    //
    this->_raw_width = this->_width = width;
    this->_raw_height = this->_height = height;
    this->_buffer_size = width * height / 2;
    //
    this->_buffer_1 = (uint8_t *)chCoreAlloc(this->_buffer_size);
    this->_buffer_2 = (uint8_t *)chCoreAlloc(this->_buffer_size);
    //
    this->_primary_buffer = this->_buffer_1;
    this->_secondary_buffer = this->_buffer_2;
    //
    this->_rotation = Rotation::Normal;
    //
    consoleDebug("QLcd()\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
void QLcd::setRotation(Rotation rotation) {
    //
    this->_rotation = rotation;
    //
    switch (rotation) {
        case QLcd::Rotation::Normal:
        case QLcd::Rotation::Flip:
            this->_width = this->_raw_width;
            this->_height = this->_raw_height;
            break;
        case QLcd::Rotation::Right:
        case QLcd::Rotation::Left:
            this->_width = this->_raw_height;
            this->_height = this->_raw_width;
            break;
    }
}

/*
 * @brief   ...
 * @details ...
 */
void QLcd::flush(void) {
    uint8_t *tmp = this->_primary_buffer;
    this->_primary_buffer = this->_secondary_buffer;
    this->_secondary_buffer = tmp;
}

/*
 * @brief   ...
 * @details ...
 */
void QLcd::clear(QCore::Color c) {
    //
    memset(this->_primary_buffer,
           static_cast<uint8_t>(c) | (static_cast<uint8_t>(c) << 4),
           this->_buffer_size);
}

/*
 * @brief   ...
 * @details ...
 */
QLcdST7735::QLcdST7735(SPIDriver *spi_drv, SPIConfig *spi_cfg,
                       ioportid_t cs_port, uint16_t cs_pin,
                       ioportid_t dc_port, uint16_t dc_pin,
                       ioportid_t rst_port, uint16_t rst_pin): QLcd(QLcdST7735::_phy_width, QLcdST7735::_phy_height) {
    //
    this->_spi_drv = spi_drv;
    this->_spi_cfg = spi_cfg;
    this->_cs_port = cs_port;
    this->_cs_pin = cs_pin;
    this->_dc_port = dc_port;
    this->_dc_pin = dc_pin;
    this->_rst_port = rst_port;
    this->_rst_pin = rst_pin;
    //
    this->_flush_buffer = (uint16_t *)chCoreAlloc(this->_flush_buffer_size * 2);
    //
    this->_lock.name = "QLcdST7735";
    this->_lock.flag = false;
}

/*
 * @brief   ...
 * @details ...
 */
void QLcdST7735::init(void) {
    // initialize base class
    QLcd::init();
    // spi validation
    if (this->_spi_drv == NULL) {
        consoleInfo("QLcdST7735::init() called with _spi_drv == NULL\r\n");
        return;
    }
    // io ports
    palSetPadMode(this->_cs_port, this->_cs_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(this->_dc_port, this->_dc_pin, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(this->_rst_port, this->_rst_pin, PAL_MODE_OUTPUT_PUSHPULL);
    // reset chip
    palClearPad(this->_rst_port, this->_rst_pin);
    chThdSleepMilliseconds(5);
    palSetPad(this->_rst_port, this->_rst_pin);
    // send power on sequence
    this->sendCommand(this->_init_sequence);
    // set default rotation
    this->setRotation(Rotation::Normal);
    //
    chBSemInit(&this->_flush_semaphore, false);
    chThdCreateStatic(QLcdST7735::_flush_wa, sizeof(QLcdST7735::_flush_wa),
                      chThdSelf()->p_prio, QLcdST7735::flushThread, this);
    //
    consoleDebug("QLcdST7735::init()\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
void QLcdST7735::setRotation(Rotation rotation) {
    //
    QLcd::setRotation(rotation);
    // prepare sequence
    uint8_t seq[sizeof(QLcdST7735::_rotation_sequence)];
    memcpy(seq, QLcdST7735::_rotation_sequence, sizeof(QLcdST7735::_rotation_sequence));
    //
    switch (rotation) {
        case QLcd::Rotation::Normal:
            seq[2] = 0x00;
            break;
        case QLcd::Rotation::Right:
            seq[2] = 0x60;
            break;
        case QLcd::Rotation::Flip:
            seq[2] = 0xC0;
            break;
        case QLcd::Rotation::Left:
            seq[2] = 0xA0;
            break;
    }
    //
    this->sendCommand(seq);
    //
    consoleDebug("QLcdST7735::setRotation(%d, %d, %d)\r\n", rotation, this->width(), this->height());
}

/*
 * @brief   ...
 * @details ...
 */
void QLcdST7735::flush(void) {
    QLcd::flush();
    chBSemSignal(&this->_flush_semaphore);
}

/*
 * @brief   ...
 * @details ...
 */
void QLcdST7735::flushBuffer(void) {
    if (chBSemWait(&this->_flush_semaphore) == RDY_OK) {
        // prepare sequence
        uint8_t seq[sizeof(QLcdST7735::_viewport_sequence)];
        memcpy(seq, QLcdST7735::_viewport_sequence, sizeof(QLcdST7735::_viewport_sequence));
        // set correct viewport
        seq[3] = 0;
        seq[5] = this->height() - 1;
        seq[10] = 0;
        seq[12] = this->width() - 1;
        this->sendCommand(seq);
        // assert chip select, data will be sent
        this->releaseDC();
        this->assertCS();
        // send data using hardware spi
        uint32_t idx = 0, i;
        uint8_t cidx;
        //
        TimeMeasurement total_flush_time;
        tmObjectInit(&total_flush_time);
        tmStartMeasurement(&total_flush_time);
        //
        TimeMeasurement flush_time;
        tmObjectInit(&flush_time);
        // translate into RGR565 and send to LCD
        while (idx < this->_buffer_size) {
            //
            tmStartMeasurement(&flush_time);
            for (i = 0; i < this->_flush_buffer_size;) {
                cidx = this->_primary_buffer[idx];
                this->_flush_buffer[i++] = static_cast<uint16_t>(QCore::Color565LookupTable[cidx & 0x0F]);
                cidx >>= 4;
                this->_flush_buffer[i++] = static_cast<uint16_t>(QCore::Color565LookupTable[cidx & 0x0F]);
                idx++;
            }
            tmStopMeasurement(&flush_time);
            consoleDevel("flushBuffer() prepare took %d us\r\n", RTT2US(flush_time.last));
            //
            tmStartMeasurement(&flush_time);
            spiSend(this->_spi_drv, this->_flush_buffer_size * 2, this->_flush_buffer);
            tmStopMeasurement(&flush_time);
            consoleDevel("flushBuffer() spiSend took %d ms\r\n", RTT2MS(flush_time.last));
        }
        // release spi
        this->releaseCS();
        tmStopMeasurement(&total_flush_time);
        consoleDebug("flushBuffer() took %d ms\r\n", RTT2MS(total_flush_time.last));
    }
}

/*
 * @brief   ...
 * @details ...
 */
inline void QLcdST7735::putPixel(QCoord x, QCoord y, QCore::Color c) {
    //
    uint32_t idx;
    //idx = (y * QCoreApplication::lcd()->width() + x) / 2;
    idx = ((y << 7) + x) / 2;
    if (x % 2 == 0) {
        this->_primary_buffer[idx] &= 0xF0;
        this->_primary_buffer[idx] |= static_cast<uint8_t>(c);
    } else {
        this->_primary_buffer[idx] &= 0x0F;
        this->_primary_buffer[idx] |= (static_cast<uint8_t>(c) << 4);
    }
    //
    consoleDevel("QLcdSt7735::putPixel()\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
inline void QLcd::drawHLine(QCoord xs, QCoord ys, QCoord l, QCore::Color c) {
    //
    QCoord xe = xs + l, i;
    uint32_t idx;
    uint8_t *buffer = QCoreApplication::lcd()->primaryBuffer();
    //
    idx = ((ys *  QCoreApplication::lcd()->width()) + xs) / 2;
    for (i = xs; i < xe; i++) {
        if (i % 2 == 0) {
            buffer[idx] &= 0xF0;
            buffer[idx] |= static_cast<uint8_t>(c);
        } else {
            buffer[idx] &= 0x0F;
            buffer[idx] |= (static_cast<uint8_t>(c) << 4);
            idx++;
        }
    }
}

/*
 * @brief   ...
 * @details ...
 */
inline void QLcd::drawVLine(QCoord xs, QCoord ys, QCoord l, QCore::Color c) {
    //
    QCoord ye = ys + l, i,
           width_2 = QCoreApplication::lcd()->width() / 2;
    uint32_t idx;
    uint8_t *buffer = QCoreApplication::lcd()->primaryBuffer();
    //
    idx = ((ys * QCoreApplication::lcd()->width()) + xs) / 2;
    if (xs % 2 == 0) {
        for (i = ys; i < ye; i++) {
            buffer[idx] &= 0xF0;
            buffer[idx] |= static_cast<uint8_t>(c);
            idx += width_2;
        }
    } else {
        for (i = ys; i < ye; i++) {
            buffer[idx] &= 0x0F;
            buffer[idx] |= (static_cast<uint8_t>(c) << 4);
            idx += width_2;
        }
    }
}

/*
 * @brief   ...
 * @details ...
 */
void QLcd::drawCircle(QCoord x, QCoord y, QCoord r, QCore::Color c, QCore::Color fc) {
    //
    QCoord r_2 = r / 2;
    //
    int i = r, j = 0;
    int radiusError = 1 - i;
 
    while (i >= j) {
        this->drawHLine(-i + x, j + y, i * 2, c);
        this->drawHLine(-i + x, -j + y, i * 2, c);
        this->drawHLine(-j + x, -i + y, j * 2, c);
        this->drawHLine(-j + x, i + y, j * 2, c);
        j++;
        if (radiusError < 0) {
            radiusError += 2 * j + 1;
        } else {
            i--;
            radiusError+= 2 * (j - i + 1);
        }
    }
}
//
std::list<QEvent *> QCoreApplication::_event_list;
ithaca_lock_t QCoreApplication::_lock = {
    .name = "QCoreApplication",
    .flag = false,
};
QLcd *QCoreApplication::_lcd = NULL;
QWidget *QCoreApplication::_main_window = NULL;

/*
 * @brief   ...
 * @details ...
 */
void QCoreApplication::init(QLcd *lcd) {
    //
    QCoreApplication::_lcd = lcd;
    QCoreApplication::_lcd->init();
    consoleDebug("QCoreApplication::init()\r\n");
}

/*
 * @brief   ...
 * @details ...
 */
void QCoreApplication::postEvent(QObject *object, QEvent *event) {
    // XXX need to pass this also
    (void)object;
    //
    if (ithacaLockTimeout(&QCoreApplication::_lock, MS2ST(10)) == true) {
        QCoreApplication::_event_list.push_back(event);
        ithacaUnlock(&QCoreApplication::_lock);
        consoleDebug("QCoreApplication::postEvent(%.8lx, %d) ok\r\n", event, event->type());
    } else {
        consoleDebug("QCoreApplication::postEvent(%.8lx, %d) failed\r\n", event, event->type());
    }
}

/*
 * @brief   ...
 * @details ...
 */
void QCoreApplication::processEvents(void) {
    //
    QEvent *event;
    //
    std::list<QEvent *>::iterator iter;
    for (iter = QCoreApplication::_event_list.begin();
         iter != QCoreApplication::_event_list.end(); ) {
        // try to acquire lock
        if (ithacaLockTimeout(&QCoreApplication::_lock, MS2ST(10)) == false) {
            consoleDebug("QCoreApplication::processEvents() unable to acquire lock\r\n");
            return;
        }
        // save pointer to event
        event = *iter;
        // XXX free event memory, remove from list and release lock
        iter = QCoreApplication::_event_list.erase(iter);
        ithacaUnlock(&QCoreApplication::_lock);
        //
        consoleDebug("QCoreApplication::processEvents(%.8lx, %d)\r\n", event, event->type());
        delete event;
    }
}

/*
 * @brief   ...
 * @details ...
 */
QLcd *QCoreApplication::lcd(void) {
    return QCoreApplication::_lcd;
}

/*
 * @brief   ...
 * @details ...
 * @note    This might be called only after init()
 */
void QCoreApplication::setMainWindow(QWidget *main_window) {
    QCoreApplication::_main_window = main_window;
    main_window->setGeometry(0, 0, QCoreApplication::lcd()->width(), QCoreApplication::lcd()->height());
}

/*
 * @brief   ...
 * @details ...
 */
QWidget *QCoreApplication::mainWindow(void) {
    return QCoreApplication::_main_window;
}

/*
 * @brief   ...
 * @details ...
 */
void QCoreApplication::renderScreen(void) {
    static uint8_t x, y, c;
    // this saves about 6ms on execution time
    QCoord width = QCoreApplication::lcd()->width(),
           height = QCoreApplication::lcd()->height();
    uint8_t *buffer = QCoreApplication::lcd()->primaryBuffer();
    //
    TimeMeasurement render_time;
    tmObjectInit(&render_time);
    //
    tmStartMeasurement(&render_time);
        if (++c >= 5) {
            c = 0;
        }
    QCoreApplication::lcd()->clear(QCore::Color::Black);
    for (y = 0; y < height; y++) {
        if (++c >= 5) {
            c = 0;
        }
        QCoreApplication::lcd()->drawHLine(10, y, 140, static_cast<QCore::Color>(c));
            ////////////////////////////////////////////////////////////
    }
    for (x = 0; x < 10; x++) {
        if (++c >= 5) { c = 0; }
        QCoreApplication::lcd()->drawVLine(x, 10, 108, static_cast<QCore::Color>(c));
        QCoreApplication::lcd()->drawVLine(x + 150, 10, 108, static_cast<QCore::Color>(c));
            ////////////////////////////////////////////////////////////
    }
    if (++c >= 5) { c = 0; }
    QCoreApplication::lcd()->drawCircle(80, 64, 32, static_cast<QCore::Color>(c), static_cast<QCore::Color>(c));
    if (++c >= 5) { c = 0; }
    QCoreApplication::lcd()->drawCircle(40, 32, 16, static_cast<QCore::Color>(c), static_cast<QCore::Color>(c));
    if (++c >= 5) { c = 0; }
    QCoreApplication::lcd()->drawCircle(120, 32, 16, static_cast<QCore::Color>(c), static_cast<QCore::Color>(c));
    if (++c >= 5) { c = 0; }
    QCoreApplication::lcd()->drawCircle(120, 96, 16, static_cast<QCore::Color>(c), static_cast<QCore::Color>(c));
    if (++c >= 5) { c = 0; }
    QCoreApplication::lcd()->drawCircle(40, 96, 16, static_cast<QCore::Color>(c), static_cast<QCore::Color>(c));
    tmStopMeasurement(&render_time);
    consoleDebug("renderScreen() took %d ms\r\n", RTT2MS(render_time.last));
    // flush buffer
    QCoreApplication::lcd()->flush();
}

/*
 * @brief   ...
 * @details ...
 */
QObject::QObject(QObject *parent) {
    //
    this->_parent = NULL;
    this->_children = NULL;
    this->_object_name = NULL;
    //
    this->setParent(parent);
}

/*
 * @brief   ...
 * @details ...
 */
QObject::~QObject() {
    // free memory for object name
    if (this->_object_name != NULL) {
        chHeapFree(this->_object_name);
        this->_object_name = NULL;
    }
}

/*
 * @brief   ...
 * @details ...
 */
const char *QObject::objectName(void) {
    // return pointer to current object name
    return this->_object_name;
}

/*
 * @brief   ...
 * @details ...
 */
void QObject::setObjectName(char *name) {
    // free memory of old name if set
    if (this->_object_name != NULL) {
        chHeapFree(this->_object_name);
        this->_object_name = NULL;
    }
    // set name if not null
    if (name != NULL) {
        // allocate memory for name
        this->_object_name = (char *)chHeapAlloc(NULL, strlen(name) + 1);
        // verify that memory has been allocated
        if (this->_object_name != NULL) {
            strcpy(this->_object_name, name);
        }
    }
    consoleDebug("QObject name set to %s\r\n", this->_object_name);
}

/*
 * @brief   ...
 * @details ...
 */
void QObject::setParent(QObject *parent) {
    // remove from old parent
    if (this->_parent != NULL) {
        QCoreApplication::postEvent(this->_parent,
                                    new QChildEvent(QEvent::ChildRemoved, this));
        this->_parent = NULL;
    }
    //
    QCoreApplication::postEvent(parent,
                                new QChildEvent(QEvent::ChildAdded, this));
    this->_parent = parent;
    //
    consoleDebug("%s: setParent()\r\n", this->objectName());
}

/*
 * @brief   ...
 * @details ...
 */
void QObject::event(QEvent *event) {
    consoleDebug("QObject::event(%d)\r\n", event->type());
}

/*
 * @brief   ...
 * @details ...
 */
QWidget::QWidget(QWidget *parent, QCoord x, QCoord y, QCoord w, QCoord h): QObject(parent) {
    this->setGeometry(x, y, w, h);
}

/*
 * @brief   ...
 * @details ...
 */
QWidget::QWidget(QCoord x, QCoord y, QCoord w, QCoord h): QWidget(NULL, x, y, w, h) {
}

/*
 * @brief   ...
 * @details ...
 */
QWidget::QWidget(QWidget *parent): QWidget(parent, 0, 0, 1, 1) {
}

/*
 * @brief   ...
 * @details ...
 */
void QWidget::event(QEvent *event) {
    consoleDebug("QWidget::event(%d)\r\n", event->type());
}

/*
 * @brief   ...
 * @details ...
 */
void QWidget::setGeometry(QCoord x, QCoord y, QCoord w, QCoord h) {
    this->_xs = x;
    this->_ys = y;
    this->_xe = x + w - 1;
    this->_ye = y + h - 1;
    this->_w = w;
    this->_h = h;
}

/*
 * @brief   ...
 * @details ...
 */
QTabWidget::QTabWidget(QWidget *parent, QCore::Color bg): QWidget(parent) {
}

/*
 * @brief   ...
 * @details ...
 */
QTabWidget::QTabWidget(QCore::Color bg): QTabWidget(NULL, bg) {
}

/*
 * @brief   ...
 * @details ...
 */
void QTabWidget::event(QEvent *event) {
    consoleDebug("QTabWidget::event(%d)\r\n", event->type());
}

#endif /* ITHACA_USE_QOBJECT */

