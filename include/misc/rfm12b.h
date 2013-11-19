#ifndef _RFM12B_H_
#define _RFM12B_H_

#if ITHACA_USE_RFM12B || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#ifndef RADIO_PACKET_SIZE
#define RADIO_PACKET_SIZE 0x20
#endif

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
typedef struct {
    //
    uint8_t dst;
    uint8_t src;
    //
    uint8_t data[RADIO_PACKET_SIZE - 2];
} radio_packet_t;

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
    // irq pin
    ioportid_t nirq_port;
    uint16_t nirq_pin;
    // rst pin
    ioportid_t rst_port;
    uint16_t rst_pin;
    // 
    uint8_t group_id;
    uint16_t frequency;
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
    RFM12B_IDLE_RX,
    RFM12B_ACTIVE_RX,
    RFM12B_ACTIVE_TX,
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
    // rfm12b state
    rfm12b_state_t state;
    // rfm12b configuration
    RFM12BConfig *config;
} RFM12BDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

//
extern RFM12BDriver RFM12B;

#ifdef __cplusplus
extern "C" {
#endif
//
bool rfm12b_lld_init(RFM12BConfig *config);
bool rfm12b_lld_send(RFM12BDriver *drv, radio_packet_t *packet);
bool rfm12b_lld_recv(RFM12BDriver *drv, radio_packet_t *packet);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RFM12B */

#endif /* _RFM12B_H_ */

