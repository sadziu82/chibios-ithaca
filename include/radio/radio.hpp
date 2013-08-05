#ifndef _RADIO_H__
#define _RADIO_H__

/*
 * @brief   Radio library
 */
#include "ch.h"
#include "hal.h"
#include "math.h"
#include "string.h"
#include "chsprintf.h"
#include "console.hpp"

/*
 * @brief   Radio states ...
 */
typedef uint8_t radio_state_t;
typedef uint8_t radio_register_address_t;
typedef uint8_t radio_register_bits_t;
typedef uint8_t radio_command_t;
typedef uint16_t radio_node_address_t;
typedef uint8_t rfm22b_device_type_t;
typedef uint8_t rfm22b_sync_words_t;
typedef uint8_t rfm22b_check_header_t;
typedef uint8_t rfm22b_register_bits_t;
typedef struct {
    uint8_t IfFilterBandwidth;
    uint8_t ClockRecoveryGearshiftOverride;
    uint8_t ClockRecoveryOversamplingRate;
    uint8_t ClockRecoveryOffset2;
    uint8_t ClockRecoveryOffset1;
    uint8_t ClockRecoveryOffset0;
    uint8_t ClockRecoveryTimingLoopGain1;
    uint8_t ClockRecoveryTimingLoopGain0;
    uint8_t OokCounterValue1;
    uint8_t OokCounterValue2;
    uint8_t SlicerPeakHold;
    uint8_t ChargePumpCurrentTrimming;
    uint8_t AgcOverride1;
    uint8_t TxDataRate1;
    uint8_t TxDataRate0;
    uint8_t ModulationControl1;
    uint8_t ModulationControl2;
    uint8_t FrequencyDeviation;
} rfm22b_modem_config_t;

/*
 * @brief   Base display class
 */
class RadioPacket {
public:
    RadioPacket(void);
    uint8_t *data(void);
    uint8_t len(void);
    void inc(void);
    uint8_t data0(void);
protected:
    uint8_t _data[0x20];
    uint8_t _len = 0x20;
};

/*
 * @brief   Base display class
 */
class Radio {
public:
    Radio(void);
    virtual bool send(RadioPacket *packet, systime_t tmout_period) { return false; }; // TODO
    virtual bool recv(RadioPacket *packet, systime_t tmout_period) { return false; }; // TODO
    virtual bool ping(void) { return false; }; // TODO
    radio_state_t getState(void);
    class State {
    public:
        static const radio_state_t Uninit = 0x00;
        static const radio_state_t Present = 0x01;
        static const radio_state_t Ready = 0x02;
        static const radio_state_t Idle = 0x03;
        static const radio_state_t PreSent = 0x04;
        static const radio_state_t Sending = 0x05;
        static const radio_state_t PreRecv = 0x06;
        static const radio_state_t Receiving = 0x07;
        static const radio_state_t Error = 0xFF;
    };
    class NodeAddress {
    public:
        static const radio_node_address_t Default = 0x00;
        static const radio_node_address_t Broadcast = 0xFF;
    };
protected:
    radio_state_t setState(radio_state_t new_state);
    bool lock(void);
    bool unlock(void);
    radio_state_t _state;
    Mutex _state_mtx;
};

/*
 * @brief   Base display class
 */
class RadioSpi: public Radio {
public:
    RadioSpi(void);
protected:
    bool spiInit(SPIDriver *spi_drv,
                 ioportid_t miso_port, uint16_t miso_pin,
                 ioportid_t mosi_port, uint16_t mosi_pin,
                 ioportid_t scl_port, uint16_t scl_pin,
                 ioportid_t cs_port, uint16_t cs_pin);
    bool spiStart(void);
    bool spiStop(void);
    void spiWrite(uint8_t reg, uint8_t tx_data);
    uint8_t spiRead(uint8_t reg);
    void spiBurstWrite(uint8_t reg, const uint8_t *data, uint8_t len);
    uint8_t spiBurstRead(uint8_t reg, uint8_t *data, uint8_t len);
private:
    SPIDriver *_spi_drv;
    SPIConfig _spi_cfg;
    uint16_t spiXfer(uint16_t tx_data);
};

/*
 * @brief   class for displays with spi interface
 */
class Rfm22B: public RadioSpi {
public:
    bool init(SPIDriver *spi_drv,
              ioportid_t miso_port, uint16_t miso_pin,
              ioportid_t mosi_port, uint16_t mosi_pin,
              ioportid_t scl_port, uint16_t scl_pin,
              ioportid_t cs_port, uint16_t cs_pin,
              ioportid_t nirq_port, uint16_t nirq_pin,
              ioportid_t sdn_port, uint16_t sdn_pin,
              systime_t tmout = 50);
    bool reset(systime_t tmout);
    bool configure(systime_t tmout);
    bool setShutdownMode(void);
    bool setIdleMode(void);
    bool setRxMode(void);
    bool setTxMode(void);
    bool setFrequency(float centre, float afcPullInRange);
    bool setModemConfig(const rfm22b_modem_config_t *config);
    void resetTxFifio(void);
    void resetRxFifio(void);
    bool send(RadioPacket *packet, systime_t tmout_period);
    bool recv(RadioPacket *packet, systime_t tmout_period);
    class Register {
    public:
        static const radio_register_address_t DeviceType = 0x00;
        static const radio_register_address_t VersionCode = 0x01;
        static const radio_register_address_t DeviceStatus = 0x02;
        static const radio_register_address_t InterruptStatus1 = 0x03;
        static const radio_register_address_t InterruptStatus2 = 0x04;
        static const radio_register_address_t InterruptEnable1 = 0x05;
        static const radio_register_address_t InterruptEnable2 = 0x06;
        static const radio_register_address_t OperatingMode1 = 0x07;
        static const radio_register_address_t OperatingMode2 = 0x08;
        static const radio_register_address_t OscillatorLoadCapacitance = 0x09;
        static const radio_register_address_t UcOutputClock = 0x0A;
        static const radio_register_address_t GpioConfiguration0 = 0x0B;
        static const radio_register_address_t GpioConfiguration1 = 0x0C;
        static const radio_register_address_t GpioConfiguration2 = 0x0D;
        static const radio_register_address_t IoPortConfiguration = 0x0E;
        static const radio_register_address_t AdcConfiguration = 0x0F;
        static const radio_register_address_t AdcSensorAmpOffset = 0x10;
        static const radio_register_address_t AdcValue = 0x11;
        static const radio_register_address_t TemperatureSensorCalibration = 0x12;
        static const radio_register_address_t TemperatureValueOffset = 0x13;
        static const radio_register_address_t WakeupTimerPeriod1 = 0x14;
        static const radio_register_address_t WakeupTimerPeriod2 = 0x15;
        static const radio_register_address_t WakeupTimerPeriod3 = 0x16;
        static const radio_register_address_t WakeupTimerValue1 = 0x17;
        static const radio_register_address_t WakeupTimerValue2 = 0x18;
        static const radio_register_address_t LdcModeDuration = 0x19;
        static const radio_register_address_t LowBatteryDetectorThreshold = 0x1A;
        static const radio_register_address_t BatteryVoltageLevel = 0x1B;
        static const radio_register_address_t IfFilterBandwidth = 0x1C;
        static const radio_register_address_t AfcLoopGearshiftOverride = 0x1D;
        static const radio_register_address_t AfcTimingControl = 0x1E;
        static const radio_register_address_t ClockRecoveryGearshiftOverride = 0x1F;
        static const radio_register_address_t ClockRecoveryOversamplingRate = 0x20;
        static const radio_register_address_t ClockRecoveryOffset2 = 0x21;
        static const radio_register_address_t ClockRecoveryOffset1 = 0x22;
        static const radio_register_address_t ClockRecoveryOffset0 = 0x23;
        static const radio_register_address_t ClockRecoveryTimingLoopGain1 = 0x24;
        static const radio_register_address_t ClockRecoveryTimingLoopGain0 = 0x25;
        static const radio_register_address_t Rssi = 0x26;
        static const radio_register_address_t RssiThreshold = 0x27;
        static const radio_register_address_t AntennaDiversity1 = 0x28;
        static const radio_register_address_t AntennaDiversity2 = 0x29;
        static const radio_register_address_t AfcLimiter = 0x2A;
        static const radio_register_address_t AfcCorrectionRead = 0x2B;
        static const radio_register_address_t OokCounterValue1 = 0x2C;
        static const radio_register_address_t OokCounterValue2 = 0x2D;
        static const radio_register_address_t SlicerPeakHold = 0x2E;
        static const radio_register_address_t DataAccessControl = 0x30;
        static const radio_register_address_t EzmacStatus = 0x31;
        static const radio_register_address_t HeaderControl1 = 0x32;
        static const radio_register_address_t HeaderControl2 = 0x33;
        static const radio_register_address_t PreambleLength = 0x34;
        static const radio_register_address_t PreambleDetectionControl1 = 0x35;
        static const radio_register_address_t SyncWord3 = 0x36;
        static const radio_register_address_t SyncWord2 = 0x37;
        static const radio_register_address_t SyncWord1 = 0x38;
        static const radio_register_address_t SyncWord0 = 0x39;
        static const radio_register_address_t TransmitHeader3 = 0x3A;
        static const radio_register_address_t TransmitHeader2 = 0x3B;
        static const radio_register_address_t TransmitHeader1 = 0x3C;
        static const radio_register_address_t TransmitHeader0 = 0x3D;
        static const radio_register_address_t PacketLength = 0x3E;
        static const radio_register_address_t CheckHeader3 = 0x3F;
        static const radio_register_address_t CheckHeader2 = 0x40;
        static const radio_register_address_t CheckHeader1 = 0x41;
        static const radio_register_address_t CheckHeader0 = 0x42;
        static const radio_register_address_t HeaderEnable3 = 0x43;
        static const radio_register_address_t HeaderEnable2 = 0x44;
        static const radio_register_address_t HeaderEnable1 = 0x45;
        static const radio_register_address_t HeaderEnable0 = 0x46;
        static const radio_register_address_t ReceivedHeader3 = 0x47;
        static const radio_register_address_t ReceivedHeader2 = 0x48;
        static const radio_register_address_t ReceivedHeader1 = 0x49;
        static const radio_register_address_t ReceivedHeader0 = 0x4A;
        static const radio_register_address_t ReceivedPacketLength = 0x4B;
        static const radio_register_address_t AnalogTestBusSelect = 0x50;
        static const radio_register_address_t DigitalTestBusSelect = 0x51;
        static const radio_register_address_t TxRampControl = 0x52;
        static const radio_register_address_t PllTuneTime = 0x53;
        static const radio_register_address_t CalibrationControl = 0x55;
        static const radio_register_address_t ModemTest = 0x56;
        static const radio_register_address_t ChargePumpTest = 0x57;
        static const radio_register_address_t ChargePumpCurrentTrimming = 0x58;
        static const radio_register_address_t DividerCurrentTrimming = 0x59;
        static const radio_register_address_t VcoCurrentTrimming = 0x5A;
        static const radio_register_address_t VcoCalibration = 0x5B;
        static const radio_register_address_t SynthesizerTest = 0x5C;
        static const radio_register_address_t BlockEnableOverride1 = 0x5D;
        static const radio_register_address_t BlockEnableOverride2 = 0x5E;
        static const radio_register_address_t BlockEnableOverride3 = 0x5F;
        static const radio_register_address_t ChannelFilterCoefficientAddress = 0x60;
        static const radio_register_address_t ChannelFilterCoefficientValue = 0x61;
        static const radio_register_address_t CrystalOscillatorPorControl = 0x62;
        static const radio_register_address_t RcOscillatorCoarseCalibration = 0x63;
        static const radio_register_address_t RcOscillatorFineCalibration = 0x64;
        static const radio_register_address_t LdoControlOverride = 0x65;
        static const radio_register_address_t LdoLevelSettings = 0x66;
        static const radio_register_address_t DeltaSigmaAdcTuning1 = 0x67;
        static const radio_register_address_t DeltaSigmaAdcTuning2 = 0x68;
        static const radio_register_address_t AgcOverride1 = 0x69;
        static const radio_register_address_t AgcOverride2 = 0x6A;
        static const radio_register_address_t GfskFirFilterCoefficientAddress = 0x6B;
        static const radio_register_address_t GfskFirFilterCoefficientValue = 0x6C;
        static const radio_register_address_t TxPower = 0x6D;
        static const radio_register_address_t TxDataRate1 = 0x6E;
        static const radio_register_address_t TxDataRate0 = 0x6F;
        static const radio_register_address_t ModulationControl1 = 0x70;
        static const radio_register_address_t ModulationControl2 = 0x71;
        static const radio_register_address_t FrequencyDeviation = 0x72;
        static const radio_register_address_t FrequencyOffset1 = 0x73;
        static const radio_register_address_t FrequencyOffset2 = 0x74;
        static const radio_register_address_t FrequencyBandSelect = 0x75;
        static const radio_register_address_t NominalCarrierFrequency1 = 0x76;
        static const radio_register_address_t NominalCarrierFrequency0 = 0x77;
        static const radio_register_address_t FrequencyHoppingChannelSelect = 0x79;
        static const radio_register_address_t FrequencyHoppingStepSize = 0x7A;
        static const radio_register_address_t TxFifoControl1 = 0x7C;
        static const radio_register_address_t TxFifoControl2 = 0x7D;
        static const radio_register_address_t RxFifoControl = 0x7E;
        static const radio_register_address_t FifoAccess = 0x7F;
    };
    class Command {
    public:
        static const radio_command_t SwReset = 0x80;
    };
    class DeviceType {
    public:
        static const rfm22b_device_type_t Tx = 0x07;
        static const rfm22b_device_type_t RxTx = 0x08;
    };
    class SyncWords {
    public:
        static const rfm22b_sync_words_t Default[];
    };
    class ModemConfig {
    public:
        static const rfm22b_modem_config_t FSK_Rb2Fd5;
        static const rfm22b_modem_config_t FSK_Rb2_4Fd36;
        static const rfm22b_modem_config_t FSK_Rb4_8Fd45;
        static const rfm22b_modem_config_t FSK_Rb9_6Fd45;
        static const rfm22b_modem_config_t FSK_Rb19_2Fd9_6;
        static const rfm22b_modem_config_t FSK_Rb38_4Fd19_6;
        static const rfm22b_modem_config_t FSK_Rb57_6Fd28_8;
        static const rfm22b_modem_config_t FSK_Rb125Fd125;
    };
    class CheckHeader {
    public:
        static const rfm22b_check_header_t Default = 0x00;
    };
    class DeviceStatus {
    public:
        static const rfm22b_register_bits_t FifoOverflow = 0x80;
        static const rfm22b_register_bits_t FifoUnderflow = 0x40;
        static const rfm22b_register_bits_t RxFifoEmpty = 0x20;
        static const rfm22b_register_bits_t HeaderError = 0x10;
        static const rfm22b_register_bits_t FrequencyError = 0x08;
        static const rfm22b_register_bits_t LockDetect = 0x04;
        static const rfm22b_register_bits_t Cps = 0x03;
        static const rfm22b_register_bits_t CpsIdle = 0x00;
        static const rfm22b_register_bits_t CpsRx = 0x01;
        static const rfm22b_register_bits_t CpsTx = 0x10;
    };
    class InterruptStatus1 {
    public:
        static const rfm22b_register_bits_t FifoError = 0x80;
        static const rfm22b_register_bits_t TxFifoAlmostFull = 0x40;
        static const rfm22b_register_bits_t TxFifoAlmostEmpty = 0x20;
        static const rfm22b_register_bits_t RxFifoAlmostFull = 0x10;
        static const rfm22b_register_bits_t External = 0x08;
        static const rfm22b_register_bits_t PacketSent = 0x04;
        static const rfm22b_register_bits_t PacketValid = 0x02;
        static const rfm22b_register_bits_t CrcError = 0x01;
    };
    class InterruptStatus2 {
    public:
        static const rfm22b_register_bits_t SWDET = 0x80;
        static const rfm22b_register_bits_t PreambleValid = 0x40;
        static const rfm22b_register_bits_t PreambleInvalid = 0x20;
        static const rfm22b_register_bits_t RSSI = 0x10;
        static const rfm22b_register_bits_t WakeupTimer = 0x08;
        static const rfm22b_register_bits_t LowBatteryDetected = 0x04;
        static const rfm22b_register_bits_t ChipReady = 0x02;
        static const rfm22b_register_bits_t PowerOnReset = 0x01;
    };
    class InterruptEnable1 {
    public:
        static const rfm22b_register_bits_t FifoError = 0x80;
        static const rfm22b_register_bits_t TxFifoAlmostFull = 0x40;
        static const rfm22b_register_bits_t TxFifoAlmostEmpty = 0x20;
        static const rfm22b_register_bits_t RxFifoAlmostFull = 0x10;
        static const rfm22b_register_bits_t External = 0x08;
        static const rfm22b_register_bits_t PacketSent = 0x04;
        static const rfm22b_register_bits_t PacketValid = 0x02;
        static const rfm22b_register_bits_t CrcError = 0x01;
    };
    class InterruptEnable2 {
    public:
        static const rfm22b_register_bits_t SWDET = 0x80;
        static const rfm22b_register_bits_t PreambleValid = 0x40;
        static const rfm22b_register_bits_t PreambleInvalid = 0x20;
        static const rfm22b_register_bits_t RSSI = 0x10;
        static const rfm22b_register_bits_t WakeupTimer = 0x08;
        static const rfm22b_register_bits_t LowBatteryDetected = 0x04;
        static const rfm22b_register_bits_t ChipReady = 0x02;
        static const rfm22b_register_bits_t PowerOnReset = 0x01;
    };
    class OperatingMode1 {
    public:
        static const rfm22b_register_bits_t SoftwareReset = 0x80;
        static const rfm22b_register_bits_t EnableLowBatteryDetector = 0x40;
        static const rfm22b_register_bits_t EnableWakeupTimer = 0x20;
        static const rfm22b_register_bits_t X32KSEL = 0x10;
        static const rfm22b_register_bits_t TxOn = 0x08;
        static const rfm22b_register_bits_t RxOn = 0x04;
        static const rfm22b_register_bits_t PllOn = 0x02;
        static const rfm22b_register_bits_t XtalOn = 0x01;
    };
    class OperatingMode2 {
    public:
        static const rfm22b_register_bits_t AntennaDiversity = 0xC0;
        static const rfm22b_register_bits_t RxMultiPacket = 0x10;
        static const rfm22b_register_bits_t AutoTx = 0x08;
        static const rfm22b_register_bits_t ENLDM = 0x04;
        static const rfm22b_register_bits_t FifoClearRx = 0x02;
        static const rfm22b_register_bits_t FifoClearTx = 0x01;
    };
    class AdcConfiguration {
    public:
        static const rfm22b_register_bits_t Start = 0x80;
        static const rfm22b_register_bits_t Done = 0x80;
        static const rfm22b_register_bits_t SEL = 0x70;
        static const rfm22b_register_bits_t SEL_INTERNAL_TEMPERATURE_SENSOR = 0x00;
        static const rfm22b_register_bits_t SEL_GPIO0_SINGLE_ENDED = 0x10;
        static const rfm22b_register_bits_t SEL_GPIO1_SINGLE_ENDED = 0x20;
        static const rfm22b_register_bits_t SEL_GPIO2_SINGLE_ENDED = 0x30;
        static const rfm22b_register_bits_t SEL_GPIO0_GPIO1_DIFFERENTIAL = 0x40;
        static const rfm22b_register_bits_t SEL_GPIO1_GPIO2_DIFFERENTIAL = 0x50;
        static const rfm22b_register_bits_t SEL_GPIO0_GPIO2_DIFFERENTIAL = 0x60;
        static const rfm22b_register_bits_t SEL_GND = 0x70;
        static const rfm22b_register_bits_t REF = 0x0c;
        static const rfm22b_register_bits_t REF_BANDGAP_VOLTAGE = 0x00;
        static const rfm22b_register_bits_t REF_VDD_ON_3 = 0x08;
        static const rfm22b_register_bits_t REF_VDD_ON_2 = 0x0c;
        static const rfm22b_register_bits_t GAIN = 0x03;
    };
    class AdcSensorAmpOffset {
    public:
        static const rfm22b_register_bits_t Offset = 0x0F;
    };
    class TemperatureSensorCalibration {
    public:
        static const rfm22b_register_bits_t TSRANGE = 0xC0;
        static const rfm22b_register_bits_t TSRANGE_M64_64C = 0x00;
        static const rfm22b_register_bits_t TSRANGE_M64_192C = 0x40;
        static const rfm22b_register_bits_t TSRANGE_0_128C = 0x80;
        static const rfm22b_register_bits_t TSRANGE_M40_216F = 0xC0;
        static const rfm22b_register_bits_t ENTSOFFS = 0x20;
        static const rfm22b_register_bits_t ENTSTRIM = 0x10;
        static const rfm22b_register_bits_t TSTRIM = 0x0F;
    };
    class WakeupTimerPeriod {
    public:
        static const rfm22b_register_bits_t WTR = 0x3C;
        static const rfm22b_register_bits_t WTD = 0x03;
    };
    class AfcLoopGearshiftOverride {
    public:
        static const rfm22b_register_bits_t AFBCD = 0x80;
        static const rfm22b_register_bits_t ENAFC = 0x40;
        static const rfm22b_register_bits_t AFCGEARH = 0x38;
        static const rfm22b_register_bits_t AFCGEARL = 0x07;
    };
    class AfcTimingControl {
    public:
        static const rfm22b_register_bits_t SWAIT_TIMER = 0xc0;
        static const rfm22b_register_bits_t SHWAIT = 0x38;
        static const rfm22b_register_bits_t ANWAIT = 0x07;
    };
    class DataAccessControl {
    public:
        static const rfm22b_register_bits_t ENPACRX = 0x80;
        static const rfm22b_register_bits_t MSBFRST = 0x00;
        static const rfm22b_register_bits_t LSBFRST = 0x40;
        static const rfm22b_register_bits_t CRCHDRS = 0x00;
        static const rfm22b_register_bits_t CRCDONLY = 0x20;
        static const rfm22b_register_bits_t ENPACTX = 0x08;
        static const rfm22b_register_bits_t ENCRC = 0x04;
        static const rfm22b_register_bits_t Crc = 0x03;
        static const rfm22b_register_bits_t CrcCcitt = 0x00;
        static const rfm22b_register_bits_t Crc16Ibm = 0x01;
        static const rfm22b_register_bits_t CrcIec16 = 0x02;
        static const rfm22b_register_bits_t CrcBiacheva = 0x03;
    };
    class HeaderControl1 {
    public:
        static const rfm22b_register_bits_t BCEN = 0xf0;
        static const rfm22b_register_bits_t BCEN_NONE = 0x00;
        static const rfm22b_register_bits_t BCEN_HEADER0 = 0x10;
        static const rfm22b_register_bits_t BCEN_HEADER1 = 0x20;
        static const rfm22b_register_bits_t BCEN_HEADER2 = 0x40;
        static const rfm22b_register_bits_t BCEN_HEADER3 = 0x80;
        static const rfm22b_register_bits_t HDCH = 0x0f;
        static const rfm22b_register_bits_t HDCH_NONE = 0x00;
        static const rfm22b_register_bits_t HDCH_HEADER0 = 0x01;
        static const rfm22b_register_bits_t HDCH_HEADER1 = 0x02;
        static const rfm22b_register_bits_t HDCH_HEADER2 = 0x04;
        static const rfm22b_register_bits_t HDCH_HEADER3 = 0x08;
    };
    class HeaderControl2 {
    public:
        static const rfm22b_register_bits_t HDLEN = 0x70;
        static const rfm22b_register_bits_t HDLEN_0 = 0x00;
        static const rfm22b_register_bits_t HDLEN_1 = 0x10;
        static const rfm22b_register_bits_t HDLEN_2 = 0x20;
        static const rfm22b_register_bits_t HDLEN_3 = 0x30;
        static const rfm22b_register_bits_t HDLEN_4 = 0x40;
        static const rfm22b_register_bits_t VARPKLEN = 0x00;
        static const rfm22b_register_bits_t FIXPKLEN = 0x08;
        static const rfm22b_register_bits_t SYNCLEN = 0x06;
        static const rfm22b_register_bits_t SYNCLEN_1 = 0x00;
        static const rfm22b_register_bits_t SYNCLEN_2 = 0x02;
        static const rfm22b_register_bits_t SYNCLEN_3 = 0x04;
        static const rfm22b_register_bits_t SYNCLEN_4 = 0x06;
        static const rfm22b_register_bits_t PREALEN8 = 0x01;
    };
    class TxPower {
    public:
        static const rfm22b_register_bits_t TXPOW = 0x07;
        static const rfm22b_register_bits_t TXPOW_4X31 = 0x08; // Not used in RFM22B
        static const rfm22b_register_bits_t TXPOW_1DBM = 0x00;
        static const rfm22b_register_bits_t TXPOW_2DBM = 0x01;
        static const rfm22b_register_bits_t TXPOW_5DBM = 0x02;
        static const rfm22b_register_bits_t TXPOW_8DBM = 0x03;
        static const rfm22b_register_bits_t TXPOW_11DBM = 0x04;
        static const rfm22b_register_bits_t TXPOW_14DBM = 0x05;
        static const rfm22b_register_bits_t TXPOW_17DBM = 0x06;
        static const rfm22b_register_bits_t TXPOW_20DBM = 0x07;
        // static const rfm22b_register_bits_t TXPOW_LNA_SW = 0x08;
    };
    class ModulationControl2 {
    public:
        static const rfm22b_register_bits_t TRCLK = 0xc0;
        static const rfm22b_register_bits_t TRCLK_NONE = 0x00;
        static const rfm22b_register_bits_t TRCLK_GPIO = 0x40;
        static const rfm22b_register_bits_t TRCLK_SDO = 0x80;
        static const rfm22b_register_bits_t TRCLK_NIRQ = 0xc0;
        static const rfm22b_register_bits_t DTMOD = 0x30;
        static const rfm22b_register_bits_t DTMOD_DIRECT_GPIO = 0x00;
        static const rfm22b_register_bits_t DTMOD_DIRECT_SDI = 0x10;
        static const rfm22b_register_bits_t DTMOD_FIFO = 0x20;
        static const rfm22b_register_bits_t DTMOD_PN9 = 0x30;
        static const rfm22b_register_bits_t ENINV = 0x08;
        static const rfm22b_register_bits_t FD8 = 0x04;
        static const rfm22b_register_bits_t MODTYP = 0x30;
        static const rfm22b_register_bits_t MODTYP_UNMODULATED = 0x00;
        static const rfm22b_register_bits_t MODTYP_OOK = 0x01;
        static const rfm22b_register_bits_t MODTYP_FSK = 0x02;
        static const rfm22b_register_bits_t MODTYP_GFSK = 0x03;
    };
    class FrequencyBandSelect {
    public:
        static const rfm22b_register_bits_t SBSEL = 0x40;
        static const rfm22b_register_bits_t HBSEL = 0x20;
        static const rfm22b_register_bits_t FB = 0x1f;
    };
protected:
    void assert_sdn( void );
    void release_sdn( void );
    void shutdown_mode(void);
    void idle_mode(void);
    bool check_module(void);
private:
    ioportid_t _nirq_port;
    uint16_t _nirq_pin;
    ioportid_t _sdn_port;
    uint16_t _sdn_pin;
    bool ioInit(ioportid_t nirq_port, uint16_t nirq_pin,
                 ioportid_t sdn_port, uint16_t sdn_pin);
};

/*
 * @brief   RFM12B ...
 */
void radio_test( void );



// Keep track of the mode the RF22 is in
#define RF22_MODE_IDLE         0
#define RF22_MODE_RX           1
#define RF22_MODE_TX           2

// These values we set for FIFO thresholds (4, 55) are actually the same as the POR values
#define RF22_TXFFAEM_THRESHOLD 4
#define RF22_RXFFAFULL_THRESHOLD 55

// This is the default node address,
#define RF22_DEFAULT_NODE_ADDRESS 0

// This address in the TO addreess signifies a broadcast
#define RF22_BROADCAST_ADDRESS 0xff

// Number of registers to be passed to setModemConfig()
#define RF22_NUM_MODEM_CONFIG_REGS 18

// Register names

// These register masks etc are named wherever possible
// corresponding to the bit and field names in the RF-22 Manual
// RF22_REG_00_DEVICE_TYPE                      0x00
#define RF22_DEVICE_TYPE_RX_TRX                 0x08
#define RF22_DEVICE_TYPE_TX                     0x07


#endif /* _RADIO_H__ */

