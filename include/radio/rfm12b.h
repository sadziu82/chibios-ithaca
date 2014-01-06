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
typedef void(* radio_callback_t)(RFM12BDriver *drv,
                                  radio_packet_t *rf_packet,
                                  void *arg);

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
    radio_callback_t idle_cb;
    radio_callback_t recv_done_cb;
    radio_callback_t recv_error_cb;
    radio_callback_t send_done_cb;
    radio_callback_t send_error_cb;
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
    RFM12B_RX_START,
    RFM12B_RX_ACTIVE,
    RFM12B_RX_COMPLETE,
    RFM12B_TX_INIT,
    RFM12B_TX_ACTIVE,
    RFM12B_TX_COMPLETE,
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
typedef struct RFM12BDriver {
    uint8_t txrx_counter;
    uint8_t txrx_buff[sizeof(radio_packet_t) + 12];
    uint16_t txrx_data;
    systime_t task_tmout;
    void *arg;
    //
    RFM12BConfig *config;
    WORKING_AREA(_wa, 256);
    //
    rfm12b_state_t state;
    bool error;
    BinarySemaphore flag;
    lld_lock_t lock;
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
bool rfm12bInit(RFM12BDriver *drv, RFM12BConfig *config, void *arg);

bool rfm12bRecvStart(RFM12BDriver *drv, void *arg);
void rfm12bRecvStartS(RFM12BDriver *drv, void *arg);
bool rfm12bSendStart(RFM12BDriver *drv, radio_packet_t *packet, void *arg);
void rfm12bSendStartS(RFM12BDriver *drv, radio_packet_t *packet, void *arg);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RADIO_RFM12B */

#endif /* _RADIO_RFM12B_H_ */

