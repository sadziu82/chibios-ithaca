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
//typedef enum {
//    RadioStateUninit = 0,
//    RadioStatePresent,
//    RadioStateReady,
//    RadioStateIdle,
//    RadioStateRx,
//    RadioStateTx,
//} radio_state_t;

typedef uint8_t radio_state_t;
typedef uint8_t radio_register_address_t;
typedef uint8_t radio_register_bits_t;
typedef uint8_t radio_command_t;
typedef uint8_t rfm22b_device_type_t;
typedef uint8_t rfm22b_sync_words_t;
typedef uint8_t rfm22b_check_header_t;
typedef uint8_t radio_node_address_t;
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
protected:
};

/*
 * @brief   Base display class
 */
class Radio {
public:
    Radio(void);
    void send(RadioPacket *packet);
    radio_state_t getState(void);
    radio_state_t setState(radio_state_t new_state);
    class State {
    public:
        static const radio_state_t Uninit = 0;
        static const radio_state_t Present = 1;
        static const radio_state_t Ready = 2;
    };
    class NodeAddress {
    public:
        static const radio_node_address_t Default = 0;
        static const radio_node_address_t Broadcast = 0;
    };
protected:
    radio_state_t _state;
};

/*
 * @brief   Base display class
 */
class RadioSpi: public Radio {
public:
    RadioSpi(void);
protected:
    bool spiInit(SPIDriver *spi_drv,
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
    bool setModemConfig(rfm22b_modem_config_t *config);
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


// This is the bit in the SPI address that marks it as a write
#define RF22_SPI_WRITE_MASK 0x80

// This is the maximum message length that can be supported by this library. Limited by
// the single message length octet in the header. 
// Yes, 255 is correct even though the FIFO size in the RF22 is only
// 64 octets. We use interrupts to refill the Tx FIFO during transmission and to empty the
// Rx FIFO during reception
// Can be pre-defined to a smaller size (to save SRAM) prior to including this header
#ifndef RF22_MAX_MESSAGE_LEN
//#define RF22_MAX_MESSAGE_LEN 255
#define RF22_MAX_MESSAGE_LEN 50
#endif

// Max number of octets the RF22 Rx and Tx FIFOs can hold
#define RF22_FIFO_SIZE 64

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

// RF22_REG_02_DEVICE_STATUS                    0x02
#define RF22_FFOVL                              0x80
#define RF22_FFUNFL                             0x40
#define RF22_RXFFEM                             0x20
#define RF22_HEADERR                            0x10
#define RF22_FREQERR                            0x08
#define RF22_LOCKDET                            0x04
#define RF22_CPS                                0x03
#define RF22_CPS_IDLE                           0x00
#define RF22_CPS_RX                             0x01
#define RF22_CPS_TX                             0x10

// RF22_REG_03_INTERRUPT_STATUS1                0x03
#define RF22_IFFERROR                           0x80
#define RF22_ITXFFAFULL                         0x40
#define RF22_ITXFFAEM                           0x20
#define RF22_IRXFFAFULL                         0x10
#define RF22_IEXT                               0x08
#define RF22_IPKSENT                            0x04
#define RF22_IPKVALID                           0x02
#define RF22_ICRCERROR                          0x01

// RF22_REG_04_INTERRUPT_STATUS2                0x04
#define RF22_ISWDET                             0x80
#define RF22_IPREAVAL                           0x40
#define RF22_IPREAINVAL                         0x20
#define RF22_IRSSI                              0x10
#define RF22_IWUT                               0x08
#define RF22_ILBD                               0x04
#define RF22_ICHIPRDY                           0x02
#define RF22_IPOR                               0x01

// RF22_REG_05_INTERRUPT_ENABLE1                0x05
#define RF22_ENFFERR                            0x80
#define RF22_ENTXFFAFULL                        0x40
#define RF22_ENTXFFAEM                          0x20
#define RF22_ENRXFFAFULL                        0x10
#define RF22_ENEXT                              0x08
#define RF22_ENPKSENT                           0x04
#define RF22_ENPKVALID                          0x02
#define RF22_ENCRCERROR                         0x01

// RF22_REG_06_INTERRUPT_ENABLE2                0x06
#define RF22_ENSWDET                            0x80
#define RF22_ENPREAVAL                          0x40
#define RF22_ENPREAINVAL                        0x20
#define RF22_ENRSSI                             0x10
#define RF22_ENWUT                              0x08
#define RF22_ENLBDI                             0x04
#define RF22_ENCHIPRDY                          0x02
#define RF22_ENPOR                              0x01

// RF22_REG_07_OPERATING_MODE                   0x07
#define RF22_SWRES                              0x80
#define RF22_ENLBD                              0x40
#define RF22_ENWT                               0x20
#define RF22_X32KSEL                            0x10
#define RF22_TXON                               0x08
#define RF22_RXON                               0x04
#define RF22_PLLON                              0x02
#define RF22_XTON                               0x01

// RF22_REG_08_OPERATING_MODE2                  0x08
#define RF22_ANTDIV                             0xc0
#define RF22_RXMPK                              0x10
#define RF22_AUTOTX                             0x08
#define RF22_ENLDM                              0x04
#define RF22_FFCLRRX                            0x02
#define RF22_FFCLRTX                            0x01

// RF22_REG_0F_ADC_CONFIGURATION                0x0f
#define RF22_ADCSTART                           0x80
#define RF22_ADCDONE                            0x80
#define RF22_ADCSEL                             0x70
#define RF22_ADCSEL_INTERNAL_TEMPERATURE_SENSOR 0x00
#define RF22_ADCSEL_GPIO0_SINGLE_ENDED          0x10
#define RF22_ADCSEL_GPIO1_SINGLE_ENDED          0x20
#define RF22_ADCSEL_GPIO2_SINGLE_ENDED          0x30
#define RF22_ADCSEL_GPIO0_GPIO1_DIFFERENTIAL    0x40
#define RF22_ADCSEL_GPIO1_GPIO2_DIFFERENTIAL    0x50
#define RF22_ADCSEL_GPIO0_GPIO2_DIFFERENTIAL    0x60
#define RF22_ADCSEL_GND                         0x70
#define RF22_ADCREF                             0x0c
#define RF22_ADCREF_BANDGAP_VOLTAGE             0x00
#define RF22_ADCREF_VDD_ON_3                    0x08
#define RF22_ADCREF_VDD_ON_2                    0x0c
#define RF22_ADCGAIN                            0x03

// RF22_REG_10_ADC_SENSOR_AMP_OFFSET            0x10
#define RF22_ADCOFFS                            0x0f

// RF22_REG_12_TEMPERATURE_SENSOR_CALIBRATION   0x12
#define RF22_TSRANGE                            0xc0
#define RF22_TSRANGE_M64_64C                    0x00
#define RF22_TSRANGE_M64_192C                   0x40
#define RF22_TSRANGE_0_128C                     0x80
#define RF22_TSRANGE_M40_216F                   0xc0
#define RF22_ENTSOFFS                           0x20
#define RF22_ENTSTRIM                           0x10
#define RF22_TSTRIM                             0x0f

// RF22_REG_14_WAKEUP_TIMER_PERIOD1             0x14
#define RF22_WTR                                0x3c
#define RF22_WTD                                0x03

// RF22_REG_1D_AFC_LOOP_GEARSHIFT_OVERRIDE      0x1d
#define RF22_AFBCD                              0x80
#define RF22_ENAFC                              0x40
#define RF22_AFCGEARH                           0x38
#define RF22_AFCGEARL                           0x07

// RF22_REG_1E_AFC_TIMING_CONTROL               0x1e
#define RF22_SWAIT_TIMER                        0xc0
#define RF22_SHWAIT                             0x38
#define RF22_ANWAIT                             0x07

// RF22_REG_30_DATA_ACCESS_CONTROL              0x30
#define RF22_ENPACRX                            0x80
#define RF22_MSBFRST                            0x00
#define RF22_LSBFRST                            0x40
#define RF22_CRCHDRS                            0x00
#define RF22_CRCDONLY                           0x20
#define RF22_ENPACTX                            0x08
#define RF22_ENCRC                              0x04
#define RF22_CRC                                0x03
#define RF22_CRC_CCITT                          0x00
#define RF22_CRC_CRC_16_IBM                     0x01
#define RF22_CRC_IEC_16                         0x02
#define RF22_CRC_BIACHEVA                       0x03

// RF22_REG_32_HEADER_CONTROL1                  0x32
#define RF22_BCEN                               0xf0
#define RF22_BCEN_NONE                          0x00
#define RF22_BCEN_HEADER0                       0x10
#define RF22_BCEN_HEADER1                       0x20
#define RF22_BCEN_HEADER2                       0x40
#define RF22_BCEN_HEADER3                       0x80
#define RF22_HDCH                               0x0f
#define RF22_HDCH_NONE                          0x00
#define RF22_HDCH_HEADER0                       0x01
#define RF22_HDCH_HEADER1                       0x02
#define RF22_HDCH_HEADER2                       0x04
#define RF22_HDCH_HEADER3                       0x08

// RF22_REG_33_HEADER_CONTROL2                  0x33
#define RF22_HDLEN                              0x70
#define RF22_HDLEN_0                            0x00
#define RF22_HDLEN_1                            0x10
#define RF22_HDLEN_2                            0x20
#define RF22_HDLEN_3                            0x30
#define RF22_HDLEN_4                            0x40
#define RF22_VARPKLEN                           0x00
#define RF22_FIXPKLEN                           0x08
#define RF22_SYNCLEN                            0x06
#define RF22_SYNCLEN_1                          0x00
#define RF22_SYNCLEN_2                          0x02
#define RF22_SYNCLEN_3                          0x04
#define RF22_SYNCLEN_4                          0x06
#define RF22_PREALEN8                           0x01

// RF22_REG_6D_TX_POWER                         0x6d
#define RF22_TXPOW                              0x07
#define RF22_TXPOW_4X31                         0x08 // Not used in RFM22B
#define RF22_TXPOW_1DBM                         0x00
#define RF22_TXPOW_2DBM                         0x01
#define RF22_TXPOW_5DBM                         0x02
#define RF22_TXPOW_8DBM                         0x03
#define RF22_TXPOW_11DBM                        0x04
#define RF22_TXPOW_14DBM                        0x05
#define RF22_TXPOW_17DBM                        0x06
#define RF22_TXPOW_20DBM                        0x07
// IN RFM23B
#define RF22_TXPOW_LNA_SW                       0x08

// RF22_REG_71_MODULATION_CONTROL2              0x71
#define RF22_TRCLK                              0xc0
#define RF22_TRCLK_NONE                         0x00
#define RF22_TRCLK_GPIO                         0x40
#define RF22_TRCLK_SDO                          0x80
#define RF22_TRCLK_NIRQ                         0xc0
#define RF22_DTMOD                              0x30
#define RF22_DTMOD_DIRECT_GPIO                  0x00
#define RF22_DTMOD_DIRECT_SDI                   0x10
#define RF22_DTMOD_FIFO                         0x20
#define RF22_DTMOD_PN9                          0x30
#define RF22_ENINV                              0x08
#define RF22_FD8                                0x04
#define RF22_MODTYP                             0x30
#define RF22_MODTYP_UNMODULATED                 0x00
#define RF22_MODTYP_OOK                         0x01
#define RF22_MODTYP_FSK                         0x02
#define RF22_MODTYP_GFSK                        0x03

// RF22_REG_75_FREQUENCY_BAND_SELECT            0x75
#define RF22_SBSEL                              0x40
#define RF22_HBSEL                              0x20
#define RF22_FB                                 0x1f

#endif /* _RADIO_H__ */

