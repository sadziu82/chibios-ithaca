#ifndef _RADIO_PPM_H_
#define _RADIO_PPM_H_

#if ITHACA_USE_RADIO_PPM || defined(__DOXYGEN__)

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
typedef struct PPMDriver PPMDriver;

/*
 * @brief   ...
 * @details ...
 */
typedef void(* ppm_callback_t)(PPMDriver *drv,
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
    ppm_callback_t idle_cb;
    ppm_callback_t recv_done_cb;
    ppm_callback_t recv_error_cb;
    ppm_callback_t send_done_cb;
    ppm_callback_t send_error_cb;
} PPMConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    PPM_UNINIT = 0,
    PPM_STOP,
    PPM_IDLE,
    PPM_RX_INIT,
    PPM_RX_START,
    PPM_RX_ACTIVE,
    PPM_RX_COMPLETE,
    PPM_TX_INIT,
    PPM_TX_ACTIVE,
    PPM_TX_COMPLETE,
} ppm_state_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // combined settings
    PPM_FREQUENCY_876_63 = 0x0CFF,       // 876.63 MHz
} ppm_frequency_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    // combined settings
    PPM_DATA_RATE_BR256000 = 0x01,       // ~256000 bps
    PPM_DATA_RATE_BR115200 = 0x02,       // ~115200 bps
    PPM_DATA_RATE_BR57600 = 0x05,        // ~57600 bps
    PPM_DATA_RATE_BR49200 = 0x06,        // ~49200 bps
    PPM_DATA_RATE_BR38400 = 0x08,        // ~38400 bps
    PPM_DATA_RATE_BR19200 = 0x11,        // ~19200 bps
    PPM_DATA_RATE_BR9600 = 0x23,         // ~9600 bps
    PPM_DATA_RATE_BR4800 = 0x47,         // ~4800 bps
    PPM_DATA_RATE_BR2400 = 0x91,         // ~2400 bps
    PPM_DATA_RATE_BR1200 = 0x9E,         // ~1200 bps
} ppm_data_rate_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct PPMDriver {
    uint8_t txrx_counter;
    uint8_t txrx_buff[sizeof(radio_packet_t) + 12];
    uint16_t txrx_data;
    systime_t task_tmout;
    void *arg;
    //
    PPMConfig *config;
    WORKING_AREA(_wa, 256);
    //
    ppm_state_t state;
    bool error;
    BinarySemaphore flag;
    lld_lock_t lock;
    // ext channel config
    EXTChannelConfig nirq_cfg;
} PPMDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

//
extern PPMDriver PPMD1;

#ifdef __cplusplus
extern "C" {
#endif
bool ppmInit(PPMDriver *drv, PPMConfig *config, void *arg);
bool ppmRecvStart(PPMDriver *drv, void *arg);
void ppmRecvStartS(PPMDriver *drv, void *arg);
bool ppmSendStart(PPMDriver *drv, radio_packet_t *packet, void *arg);
void ppmSendStartS(PPMDriver *drv, radio_packet_t *packet, void *arg);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RADIO_PPM */

#endif /* _RADIO_PPM_H_ */

