#ifndef _QOBJECT_HPP_
#define _QOBJECT_HPP_

#if ITHACA_USE_QOBJECT || defined(__DOXYGEN__)

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

//
class QEvent;
class QChildEvent;
class QObject;
class QWidget;

/*
 * @brief   ...
 * @details ...
 */
class QCore {
    public:
        enum class Key: uint8_t {
            JoyUp,
            JoyDown,
            JoyLeft,
            JoyRight,
            JoyEnter,
        };
        enum class Color: uint8_t {
            Black = 0x00,
            Red = 0x01,
            Green = 0x02,
            Blue = 0x03,
            White = 0x04,
        };
        enum class Color565: uint16_t {
            Black = 0x0000,
            Red = 0x00F8,
            Green = 0xE007,
            Blue = 0x1F00,
            White = 0xFFFF,
        };
        enum class Alpha: uint8_t {
            Transparent = 0x00,
            Half = 0x80,
            Solid = 0xFF,
        };
        //
        static const Color565 Color565LookupTable[];
};

/*
 * @brief   ...
 * @details ...
 */
class QEvent {
    public:
        enum Type {
            None,
            LcdBufferFlush,
            ChildAdded,
            ChildRemoved,
            KeyPress,
            KeyRelease,
        };
    protected:
        Type _type;
        bool _accepted;
    public:
        QEvent(Type type);
        ~QEvent();
        Type type(void);
        bool isAccepted(void);
        void setAccepted(bool accepted = true);
};

/*
 * @brief   ...
 * @details ...
 */
class QChildEvent: public QEvent {
    protected:
        QObject *_child;
    public:
        QChildEvent(Type type, QObject *child);
        QObject *child(void);
};

/*
 * @brief   ...
 * @details ...
 */
class QKeyEvent: public QEvent {
    protected:
        QCore::Key _key;
    public:
        QKeyEvent(Type type, QCore::Key key);
        QCore::Key key(void);
};

//
typedef int16_t QCoord;

/*
 * @brief   ...
 * @details ...
 */
class QLcd {
    public:
        enum class Rotation: uint8_t {
            Normal = 0x01,
            Right = 0x02,
            Flip = 0x03,
            Left = 0x04,
        };
    protected:
        //
        Rotation _rotation;
        // geometry, framebuffer emulation
        QCoord _width, _height,
               _raw_width, _raw_height;
        uint32_t _buffer_size;
        uint8_t *_buffer_1, *_buffer_2,
                *_primary_buffer, *_secondary_buffer;
    public:
        QLcd(QCoord width, QCoord height);
        virtual void init(void) { consoleDebug("QLcd::init()\r\n"); };
        virtual void setRotation(Rotation rotation);
        virtual void flush(void);
        virtual inline QCoord width(void) { return this->_width; };
        virtual inline QCoord height(void) { return this->_height; };
        virtual void clear(QCore::Color c);
        virtual void putPixel(QCoord x, QCoord y, QCore::Color c) {};
        inline void drawHLine(QCoord xs, QCoord ys, QCoord l, QCore::Color c);
        inline void drawVLine(QCoord xs, QCoord ys, QCoord l, QCore::Color c);
        void drawCircle(QCoord x, QCoord y, QCoord r, QCore::Color c, QCore::Color fc);
        virtual uint8_t *primaryBuffer(void) { return this->_primary_buffer; };
};

/*
 * @brief   ...
 * @details ...
 */
class QLcdST7735: public QLcd {
    protected:
        // registers
        enum class Register: uint8_t;
        //
        static const QCoord _phy_width = 128,
                            _phy_height = 160;
        // hardware interface
        SPIDriver *_spi_drv;
        SPIConfig *_spi_cfg;
        ioportid_t _cs_port;
        uint16_t _cs_pin;
        ioportid_t _dc_port;
        uint16_t _dc_pin;
        ioportid_t _rst_port;
        uint16_t _rst_pin;
        // hardware related functions
        void assertCS(void);
        void releaseCS(void);
        void assertDC(void) { palClearPad(this->_dc_port, this->_dc_pin); };
        void releaseDC(void) { palSetPad(this->_dc_port, this->_dc_pin); };
        void assertRST(void) { palClearPad(this->_rst_port, this->_rst_pin); };
        void releaseRST(void) { palSetPad(this->_rst_port, this->_rst_pin); };
        //
        static const uint16_t _flush_buffer_size = 4096;
        uint16_t *_flush_buffer;
        //
        ithaca_lock_t _lock;
        //
        static const uint8_t _init_sequence[],
                             _rotation_sequence[],
                             _viewport_sequence[];
        void sendCommand(const uint8_t seq[]);
        // flushing thread
        BinarySemaphore _flush_semaphore;
        static WORKING_AREA(_flush_wa, 256);
        static msg_t flushThread(void *arg);
    public:
        QLcdST7735(SPIDriver *spi_drv, SPIConfig *spi_cfg,
                   ioportid_t cs_port, uint16_t cs_pin,
                   ioportid_t dc_port, uint16_t dc_pin,
                   ioportid_t rst_port, uint16_t rst_pin);
        virtual void init(void);
        virtual void setRotation(Rotation rotation);
        virtual void flush(void);
        virtual void flushBuffer(void);
        virtual inline void putPixel(QCoord x, QCoord y, QCore::Color c);
};

/*
 * @brief   ...
 */
enum class QLcdST7735::Register: uint8_t {
    //
    Nop = 0x00,
    SwReset = 0x01,
    RddId = 0x04,
    RddSt = 0x09,
    //
    SlpIn = 0x10,
    SlpOut = 0x11,
    PtlOn = 0x12,
    NorOn = 0x13,
    //
    InvOff = 0x20,
    InvOn = 0x21,
    DispOff = 0x28,
    DispOn = 0x29,
    CaSet = 0x2A,
    RaSet = 0x2B,
    RamWr = 0x2C,
    RamRd = 0x2E,
    //
    PtlAr = 0x30,
    ColMod = 0x3A,
    MadCtl = 0x36,
    //
    FrmCtr1 = 0xB1,
    FrmCtr2 = 0xB2,
    FrmCtr3 = 0xB3,
    InvCtr = 0xB4,
    DisSet5 = 0xB6,
    //
    PwCtr1 = 0xC0,
    PwCtr2 = 0xC1,
    PwCtr3 = 0xC2,
    PwCtr4 = 0xC3,
    PwCtr5 = 0xC4,
    VmCtr1 = 0xC5,
    //
    RdId1 = 0xDA,
    RdId2 = 0xDB,
    RdId3 = 0xDC,
    RdId4 = 0xDD,
    //
    PwCtr6 = 0xFC,
    //
    GmCtrP1 = 0xE0,
    GmCtrN1 = 0xE1,
};

/*
 * @brief   ...
 * @details ...
 */
class QCoreApplication {
    protected:
        static std::list<QEvent *> _event_list;
        static ithaca_lock_t _lock;
        static QLcd *_lcd;
        static QWidget *_main_window;
    public:
        static void init(QLcd *lcd);
        static void postEvent(QObject *object, QEvent *event);
        static void processEvents(void);
        static QLcd *lcd(void);
        static void setMainWindow(QWidget *main_window);
        static QWidget *mainWindow(void);
        static void renderScreen(void);
};

/*
 * @brief   ...
 * @details ...
 */
class QObject {
    protected:
        QObject *_parent;
        QObject **_children;
        char *_object_name;
    public:
        QObject(QObject *parent = 0);
        ~QObject();
        const char *objectName(void);
        void setObjectName(char *name);
        void setParent(QObject *parent);
        virtual void event(QEvent *event);
};

/*
 * @brief   ...
 * @details ...
 */
class QWidget: public QObject {
    protected:
        QCoord _xs, _ys, _xe, _ye, _w, _h;
    public:
        //
        inline QCoord xs(void) { return this->_xs; };
        inline QCoord xe(void) { return this->_xe; };
        inline QCoord ys(void) { return this->_ys; };
        inline QCoord ye(void) { return this->_ye; };
        inline QCoord width(void) { return this->_w; };
        inline QCoord height(void) { return this->_h; };
        //
        QWidget(QWidget *parent, QCoord x, QCoord y, QCoord w, QCoord h);
        QWidget(QCoord x, QCoord y, QCoord w, QCoord h);
        QWidget(QWidget *parent);
        virtual void event(QEvent *event);
        void setGeometry(QCoord x, QCoord y, QCoord w, QCoord h);
};

/*
 * @brief   ...
 * @details ...
 */
class QTabWidget: public QWidget {
    protected:
    public:
        QTabWidget(QWidget *parent, QCore::Color bg);
        QTabWidget(QCore::Color bg);
        virtual void event(QEvent *event);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_QOBJECT */

#endif /* _QOBJECT_HPP_ */

