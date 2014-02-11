#ifndef _RADIO_RFM12B_H_
#define _RADIO_RFM12B_H_

#if ITHACA_USE_RADIO_RFM12B || defined(__DOXYGEN__)

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
 * @brief   Just forward declaration.
 */
typedef struct RFM12BDriver RFM12BDriver;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    // spi driver
    SPIDriver *spi_drv;
    // spi cs/ce
    ioportid_t cs_port;
    uint16_t cs_pin;
    // ext driver
    EXTDriver *ext_drv;
    uint32_t ext_mode;
    // irq pin
    ioportid_t nirq_port;
    uint16_t nirq_pin;
    // rst pin
    ioportid_t rst_port;
    uint16_t rst_pin;
    // 
    uint8_t syncword;
    uint8_t preamble_length;
    uint8_t postamble_length;
    uint16_t base_freq;
    uint16_t freq_step;
    uint8_t data_rate;
} RFM12BConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    RFM12B_UNINIT = 0,
    RFM12B_STOP,
    RFM12B_IDLE,
    //
    RFM12B_RX_START,
    RFM12B_RX_WAIT,
    RFM12B_RX_DATA,
    RFM12B_RX_CRC,
    RFM12B_RX_POSTAMBLE,
    RFM12B_RX_COMPLETE,
    //
    RFM12B_TX_START,
    RFM12B_TX_PREAMBLE,
    RFM12B_TX_SYNC1,
    RFM12B_TX_SYNC2,
    RFM12B_TX_DATA,
    RFM12B_TX_CRC,
    RFM12B_TX_POSTAMBLE,
    RFM12B_TX_COMPLETE,
    //
    RFM12B_RX_ERROR,
    RFM12B_TX_ERROR,
} rfm12b_state_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // combined settings
    RFM12B_FREQUENCY_876_63 = 0x0CFF,       // 876.63 MHz
} rfm12b_frequency_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    RFM12B_DATA_RATE_MAX = 0x01,        // ~172.4 kbps
    RFM12B_DATA_RATE_115200 = 0x02,        // ~115.2 kbps
    RFM12B_DATA_RATE_57600 = 0x05,        // ~57.6 kbps
    RFM12B_DATA_RATE_38400 = 0x08,        // ~38.4 kbps
    RFM12B_DATA_RATE_19200 = 0x11,        // ~19.2 kbps
    RFM12B_DATA_RATE_9600 = 0x24,        // ~9.6 kbps
    RFM12B_DATA_RATE_4800 = 0x47,        // 4.8 kbps
} rfm12b_data_rate_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct RFM12BDriver {
    rfm12b_state_t state;
    uint8_t txrx_counter;
    uint16_t txrx_data;
    SPIConfig spi_cfg;
    EXTChannelConfig nirq_cfg;
    ithaca_lock_t lock;
    bool spi_xfer_last;
    uint32_t crc_self;
    uint32_t crc_recv;
} RFM12BDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
bool rfm12b_lld_init(RadioDriver *radio);
void rfm12b_lld_idle(RadioDriver *radio);
bool rfm12b_lld_is_error(RadioDriver *radio);
void rfm12b_lld_receive_start(RadioDriver *radio);
bool rfm12b_lld_receive_is_completed(RadioDriver *radio);
void rfm12b_lld_send_start(RadioDriver *radio);
bool rfm12b_lld_send_is_completed(RadioDriver *radio);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RADIO_RFM12B */

#endif /* _RADIO_RFM12B_H_ */

