#ifndef _RADIO_RFM12B_H__
#define _RADIO_RFM12B_H__

/*
 * @brief   Radio library
 */
#include "ch.h"
#include "hal.h"
#include "math.h"
#include "string.h"
#include "chsprintf.h"

/*
 * @brief   Radio library
 */
#include "console.hpp"
#include "radio.hpp"

/*
 * @brief   Radio states ...
 */
typedef uint16_t rfm12b_register_bits_t;

/*
 * @brief   class for displays with spi interface
 */
class Rfm12B: public Radio {
public:
    bool init(SPIDriver *spi_drv,
              ioportid_t miso_port, uint16_t miso_pin,
              ioportid_t mosi_port, uint16_t mosi_pin,
              ioportid_t scl_port, uint16_t scl_pin,
              ioportid_t cs_port, uint16_t cs_pin,
              ioportid_t nirq_port, uint16_t nirq_pin,
              ioportid_t rst_port, uint16_t rst_pin,
              systime_t tmout = 50);
    bool send(RadioPacket *packet, systime_t tmout_period);
    bool recv(RadioPacket *packet, systime_t tmout_period);
protected:
    void assertReset(void);
    void releaseReset(void);
    uint16_t xfer(const uint16_t cmd);
    void write(const uint16_t reg, const uint16_t data);
    uint16_t status(void);
    void sleepMode(void);
    void idleMode(void);
    void rxMode(void);
    void txMode(void);
    void resetTxFifio(void);
    void resetRxFifio(void);
    bool reset(systime_t tmout);
    bool configure(systime_t tmout);
    class Register {
    public:
        static const uint16_t Status = 0x0000;
        static const uint16_t Configuration = 0x8000;
        static const uint16_t PowerManagement = 0x8200;
        static const uint16_t Frequency = 0xA000;
        static const uint16_t DataRate = 0xC600;
        static const uint16_t ReceiverControl = 0x9000;
        static const uint16_t DataFilter = 0xC200;
        static const uint16_t FifoResetMode = 0xCA00;
        static const uint16_t SynchronPattern = 0xCE00;
        static const uint16_t AFC = 0xC400;
        static const uint16_t TxConfig = 0x9800;
        static const uint16_t PLL = 0xCC00;
        static const uint16_t WakeUpTimer = 0xE000;
        static const uint16_t LowDutyCycle = 0xC800;
        static const uint16_t TxWrite = 0xB800;
        static const uint16_t RxRead = 0xB000;
        static const uint16_t LowBatteryDetector = 0xC000;
    };
    class Status {
    public:
        static const rfm12b_register_bits_t PowerOnReset = 0x4000;
        static const rfm12b_register_bits_t LowBattery = 0x0400;
        static const rfm12b_register_bits_t Rssi = 0x0100;
    };
    class PowerManagement {
    public:
        static const rfm12b_register_bits_t DisableClockOutput = 0x0001;
        static const rfm12b_register_bits_t EnableLowBatteryDetector = 0x0004;
        static const rfm12b_register_bits_t EnableCrystalOscillator = 0x0008;
        static const rfm12b_register_bits_t EnableSynthesizer = 0x0010;
        static const rfm12b_register_bits_t EnableTransmitter = 0x0020;
        static const rfm12b_register_bits_t EnableBaseBandBlock = 0x0040;
        static const rfm12b_register_bits_t EnableReceiver = 0x0080;
    };
    class LowBattery {
    public:
        static const rfm12b_register_bits_t v22 = 0x0000;
        static const rfm12b_register_bits_t v37 = 0x000F;
    };
private:
    ioportid_t _nirq_port;
    uint16_t _nirq_pin;
    ioportid_t _rst_port;
    uint16_t _rst_pin;
    SPIDriver *_spi_drv;
    SPIConfig _spi_cfg;
    uint16_t spiXfer(uint16_t tx_data);
};

#endif /* _RADIO_RFM12B_H__ */

