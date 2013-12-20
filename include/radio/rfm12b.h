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
 * @brief   ...
 * @details ...
 */
typedef msg_t(* radio_packet_callback_t)(radio_packet_t *rf_packet);
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
    uint8_t group_id;
    uint16_t base_freq;
    uint16_t freq_step;
    uint8_t data_rate;
    //
    uint16_t recv_timeout;
    uint8_t freq_chan;
    int8_t priority;
    //
    radio_packet_callback_t idle_cb;
    radio_packet_callback_t recv_done_cb;
    radio_packet_callback_t recv_error_cb;
    radio_packet_callback_t send_done_cb;
    radio_packet_callback_t send_error_cb;
} RFM12BConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    RFM12B_UNINIT = 0,
    RFM12B_STOP,
    RFM12B_IDLE,
    RFM12B_RX_INIT,
    RFM12B_RX_WAIT,
    RFM12B_RX_ACTIVE,
    RFM12B_RX_COMPLETE,
    RFM12B_RX_ERROR,
    RFM12B_TX_INIT,
    RFM12B_TX_ACTIVE,
    RFM12B_TX_COMPLETE,
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
    // combined settings
    RFM12B_DATA_RATE_BR256000 = 0x01,       // ~256000 bps
    RFM12B_DATA_RATE_BR115200 = 0x02,       // ~115200 bps
    RFM12B_DATA_RATE_BR57600 = 0x05,        // ~57600 bps
    RFM12B_DATA_RATE_BR49200 = 0x06,        // ~49200 bps
    RFM12B_DATA_RATE_BR38400 = 0x08,        // ~38400 bps
    RFM12B_DATA_RATE_BR19200 = 0x11,        // ~19200 bps
    RFM12B_DATA_RATE_BR9600 = 0x23,         // ~9600 bps
    RFM12B_DATA_RATE_BR4800 = 0x47,         // ~4800 bps
    RFM12B_DATA_RATE_BR2400 = 0x91,         // ~2400 bps
    RFM12B_DATA_RATE_BR1200 = 0x9E,         // ~1200 bps
} rfm12b_data_rate_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    uint8_t _buff_cnt;
    uint8_t buffer[sizeof(radio_packet_t) + 12];
    uint16_t txrx_data;
    systime_t _tmout;
    //
    RFM12BConfig *_cfg;
    WORKING_AREA(_wa, 512);
    //
    rfm12b_state_t _state;
    Semaphore _sem;
    // ext channel config
    EXTChannelConfig nirq_cfg;
} RFM12BDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

//
extern RFM12BDriver RFM12BD1;

#ifdef __cplusplus
extern "C" {
#endif
//
bool rfm12b_lld_init(RFM12BDriver *drv, RFM12BConfig *config);
bool rfm12b_lld_send(RFM12BDriver *drv, radio_packet_t *packet);
bool rfm12b_lld_recv(RFM12BDriver *drv, radio_packet_t *packet);

bool rfm12bSendStart(RFM12BDriver *drv);
bool rfm12bRecvStart(RFM12BDriver *drv);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RADIO_RFM12B */

#endif /* _RADIO_RFM12B_H_ */

