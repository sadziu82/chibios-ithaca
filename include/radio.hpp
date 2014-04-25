#ifndef _RADIO_HPP_
#define _RADIO_HPP_

#if ITHACA_USE_RADIO || defined(__DOXYGEN__)

/*

  Radio Data Frame format

+---------+--------+----------------------------+----------------------------+
| Field   | Lenght | Transmitter                | Copter                     |
+---------+--------+----------------------------+----------------------------+
| id      |      1 | 7-6: 0 - empty, 1 standalone, 2 - master, 3 - slave     |
|         |        | 5-0: group number                                       |
+---------+--------+----------------------------+----------------------------+
| length  |      1 | 7-6: unused                                             |
|         |        | 5-0: stream data length (0 for now, under development)  |
+---------+--------+----------------------------+----------------------------+ R
| control |      2 |    15: abort               |    15: low battery         | C
|         |        |    14: arm/disarm req.     |    14: armed/disarmed      |
|         |        |    13: return home req.    |    13: returning home      | P
|         |        | 12-10: fly mode            | 12-10: current fly mode    | a
|         |        |   9-0: unused for now      |     9: sensors ok          | c
|         |        |                            |     8: gps lock            | k
|         |        |                            |   7-0: battery voltage     | e
+---------+--------+----------------------------+----------------------------+ t
| rc data |      4 | 31-24: roll (right horiz.) | 31-24: roll (in degree)    |
|         |        | 23-16: pitch (right vert.) | 23-16: pitch (in degree)   | o
|         |        |  15-8: yaw (left horiz.)   |  15-0: yaw (in degree)     | n
|         |        |   7-0: thr. (left vert.)   |                            | l
+---------+--------+----------------------------+----------------------------+ y
| ....... | ...... | .......... stream not yet implemented ................. |
+---------+--------+----------------------------+----------------------------+
| crc32   |      4 |                          crc32                          |
+---------+--------+----------------------------+----------------------------+

fly mode:
  000 - easy fly (front on vector between current and start position)
  001 - easy fly with altitude hold
  010 - position hold
  011 - manual
  100 - auto pilot - follow path
  101 - ...
  110 - ...
  111 - ...

 */

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
class RadioPacket {
    public:
        //
        enum class Type: uint8_t {
            Empty = 0x00,
            Standalone = 0x40,
            Master = 0x80,
            Slave = 0xC0,
        };
        //
        enum class RawData: uint8_t {
            MinLength = 0x02,
            MaxLength = 0x30,
            HeadersLength = 0x02,
            TypeGroupIdHeaderOffset = 0x00,
            TypeMask = 0xC0,
            GroupIdMask = 0x3F,
            PayloadLengthHeaderOffset = 0x01,
            LengthMask = 0x3F,
        };
    protected:
        //
        uint8_t *_raw_data;
        uint8_t _raw_trash;
    public:
        //
        RadioPacket(void);
        RadioPacket(uint8_t *raw_data);
        //
        uint8_t rawDataLength(void);
        void setRawDataLength(uint8_t length);
        //
        Type type(void);
        void setType(Type type);
        //
        uint8_t groupId(void);
        void setGroupId(uint8_t group_id);
        //
        void getRawData(uint8_t *raw_data);
        uint32_t getRawCrc32(void);
        void setRawData(uint8_t *raw_data);
        //
        uint8_t &operator[](uint8_t);
};

/*
 * @brief   ...
 * @details ...
 */
class Radio {
    public:
        //
        enum class Status: uint16_t {
            Uninit = 0,
            Idle,
            RxIdle,
            RxActive,
            TxActive,
            Error,
        };
    protected:
    public:
        Radio(void);
        virtual bool init(void);
        virtual bool send(RadioPacket *packet, systime_t tmout);
        virtual bool recv(RadioPacket *packet, systime_t tmout);
};

/*
 * @brief   ...
 * @details ...
 */
class RadioRFM12B: public Radio {
    public:
        //
        enum class DataRate: uint16_t {
            bMAX = 0x01,        // ~172.4 kbps
            b115200 = 0x02,        // ~115.2 kbps
            b57600 = 0x05,        // ~57.6 kbps
            b38400 = 0x08,        // ~38.4 kbps
            b19200 = 0x11,        // ~19.2 kbps
            b9600 = 0x24,        // ~9.6 kbps
            b4800 = 0x47,        // 4.8 kbps
        };
        //
        enum class Frequency: uint16_t {
            f868 = 0x0CFF,
        };
    protected:
        //
        enum class Register: uint16_t {
            Status = 0x0000,
            Configuration = 0x8000,
            PowerManagement = 0x8200,
            Frequency = 0xA000,
            DataRate = 0xC600,
            ReceiverControl = 0x9000,
            DataFilter = 0xC200,
            FifoResetMode = 0xCA00,
            SynchronPattern = 0xCE00,
            Afc = 0xC400,
            TxConfig = 0x9800,
            Pll = 0xCC00,
            WakeupTimer = 0xE000,
            LowDutyCycle = 0xC800,
            TxFifoWrite = 0xB800,
            RxFifoRead = 0xB000,
            LowBatteryDetector = 0xC000,
        };
        //
        enum class Status: uint16_t {
            TxReady = 0x8000,               // in tx mode
            RxReady = 0x8000,               // in rx mode
            PowerOnReset = 0x4000,
            TxUnderrun = 0x2000,            // in tx mode
            RxOverflow = 0x2000,            // in rx mode
            Wakeup = 0x1000,
            ExtInt = 0x0800,
            LowBattery = 0x0400,
            FifoEmpty = 0x0200,
            Rssi = 0x0100,                  // in rx mode
            Ats = 0x0100,                   // in tx mode
        };
        //
        enum class Configuration: uint16_t {
            // Single Bits
            InternalDataRegister = 0x0080,
            FifoMode = 0x0040,
            Freq868 = 0x0020,
            Cap125pF = 0x0008,
            // preset modes
            Band868Idle = Freq868 | Cap125pF,
            Band868Rx = Freq868 | Cap125pF | FifoMode,
            Band868Tx = Freq868 | Cap125pF | InternalDataRegister,
        };
        //
        enum class PowerManagement: uint16_t {
            // single bits
            DisableClockOutput = 0x0001,
            EnableLowBatteryDetector = 0x0004,
            EnableCrystalOscillator = 0x0008,
            EnableSynthesizer = 0x0010,
            EnableTransmitter = 0x0020,
            EnableBaseBandBlock = 0x0040,
            EnableReceiver = 0x0080,
            // preset modes
            IdleMode = EnableBaseBandBlock | EnableSynthesizer |
                       EnableCrystalOscillator | DisableClockOutput,
            TxMode = EnableBaseBandBlock | EnableSynthesizer |
                     EnableCrystalOscillator | DisableClockOutput |
                     EnableTransmitter,
            RxMode = EnableBaseBandBlock | EnableSynthesizer |
                     EnableCrystalOscillator | DisableClockOutput |
                     EnableReceiver,
        };
        //
        enum class FifoReset: uint16_t {
            Clear = 0x0081,
            Set = 0x0083,
        };
        //
        enum class LowBattery: uint16_t {
            // low voltage detection presets
            V22 = 0x0000,
            V37 = 0x000F,
        };
    protected:
        // busy flag
        ithaca_lock_t _busy;
        // spi driver
        SPIDriver *_spi_drv;
        // spi cs/ce
        ioportid_t _cs_port;
        uint16_t _cs_pin;
        // ext driver
        EXTDriver *_ext_drv;
        uint32_t _ext_ch_mode;
        EXTChannelConfig _ext_ch_cfg;
        // irq pin
        ioportid_t _nirq_port;
        uint16_t _nirq_pin;
        // rst pin
        ioportid_t _rst_port;
        uint16_t _rst_pin;
        // 
        uint8_t _syncword;
        uint8_t _preamble_length;
        uint8_t _postamble_length;
        Frequency _base_freq;
        // TODO
        //uint16_t _freq_step;
        DataRate _data_rate;
        // hardware related functions
        void _assertCS(void);
        void _releaseCS(void);
        void _assertRST(void) { palClearPad(this->_rst_port, this->_rst_pin); };
        void _releaseRST(void) { palSetPad(this->_rst_port, this->_rst_pin); };
        //
        uint16_t _xfer(const uint16_t cmd);
        void _write(Register reg, const uint16_t data);
        uint8_t _read(void);
        uint16_t _status(void);
        //
        Semaphore _radio_semaphore;
        //static WORKING_AREA(_thread_wa, 512);
        //static msg_t radioThread(void *arg);
        Thread *_thread_ptr;
        static void nIRQ(EXTDriver *extp, expchannel_t channel);
        void wakeup(void);
        TimeMeasurement tm;
    public:
        //
        RadioRFM12B(EXTDriver *ext_drv, uint32_t ext_ch_mode,
                    SPIDriver *spi_drv,
                    ioportid_t cs_port, uint16_t cs_pin,
                    ioportid_t nirq_port, uint16_t nirq_pin,
                    ioportid_t rst_port, uint16_t rst_pin);
        virtual bool init(void);
        bool configure(uint8_t sw, Frequency bf, DataRate dr);
        virtual bool send(RadioPacket *packet, systime_t tmout);
        virtual bool recv(RadioPacket *packet, systime_t tmout);
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#endif /* ITHACA_USE_RADIO */

#endif /* _RADIO_HPP_ */

