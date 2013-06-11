#ifndef _SI4421_H__
#define _SI4421_H__

/*
 * @brief   Radio library
 */

#include "ch.h"
#include "hal.h"
#include "raw_usb.hpp"
#include "chsprintf.h"

/*
 * @brief   SPI driver to use for this radio
 */
#define SI4421_SPI          SPID1

/*
 * @brief   Radio spi/rst/irq config
 */
#define SI4421_CE_PORT      GPIOA
#define SI4421_CE_PIN       4
#define SI4421_IRQ_PORT     GPIOC
#define SI4421_IRQ_PIN      13
#define SI4421_RST_PORT     GPIOC
#define SI4421_RST_PIN      14

/*
 * @brief   
 */
#define SI4421_TIMEOUT      100 // 1s timeout



/*
 * @brief   Max radio packet size
 */
#define RADIO_PACKET_LEN    28 // 28 byte packets

/*
 * @brief   Radio packet ...
 */
typedef struct {
    uint8_t src;
    uint8_t dst;
    uint8_t data[ RADIO_PACKET_LEN ];
    uint8_t crc1;
    uint8_t crc2;
} radio_packet_t;

/*
 * @brief   Radio states ...
 */
typedef enum {
    RADIO_STATE_UNINIT = 0,
    RADIO_STATE_READY,
    RADIO_STATE_IDLE,
    RADIO_STATE_RX,
    RADIO_STATE_TX,
} radio_state_t;

/*
 * @brief   RFM12B ...
 */
typedef struct {
    uint32_t rx_packets;
    uint32_t rx_timeouts;
    uint32_t rx_errors;
    uint32_t tx_packets;
    uint32_t tx_timeouts;
    uint32_t tx_errors;
} radio_stats_t;

/*
 * @brief   RFM12B ...
 */
bool_t radioInit( void );
bool radioSend( const radio_packet_t *packet );
bool_t radioRecv( radio_packet_t *packet );
void si4421IRQ( EXTDriver *extp, expchannel_t channel );

//#define HR_REMOTE
//#include "radio_si4421.h"


#endif /* _SI4421_H__ */

