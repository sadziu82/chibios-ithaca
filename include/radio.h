#ifndef _RADIO_H_
#define _RADIO_H_

#if ITHACA_USE_RADIO || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#ifndef RADIO_PACKET_DATA_SIZE
#define RADIO_PACKET_DATA_SIZE 0x10
#endif /* RADIO_PACKET_DATA_SIZE */

#if RADIO_PACKET_DATA_SIZE % 4 != 0
#error RADIO_PACKET_DATA_SIZE must be multiple of 4
#endif /* RADIO_PACKET_DATA_SIZE % 4 != 0 */

#if RADIO_PACKET_DATA_SIZE < 4 || RADIO_PACKET_DATA_SIZE > 60
#error RADIO_PACKET_DATA_SIZE must be 4 <= x <= 60
#endif /* RADIO_PACKET_DATA_SIZE < 4 || RADIO_PACKET_DATA_SIZE > 48 */

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
    uint8_t data[RADIO_PACKET_DATA_SIZE];
} radio_packet_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct RadioDriver RadioDriver;

/*
 * @brief   ...
 * @details ...
 */
typedef void(* radio_callback_t)(RadioDriver *radio, radio_packet_t *packet);

/*
 * @brief   ...
 * @details ...
 */
#include <radio/ppm.h>
#include <radio/rfm12b.h>
#include <radio/rfm22b.h>
#include <radio/mesh.h>
#include <radio/rc.h>

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
    RADIO_UNINIT = 0,
    RADIO_STOP,
    RADIO_IDLE,
    RADIO_RX,
    RADIO_TX,
    RADIO_ERROR,
} radio_state_t;

/*
 * @brief   ...
 * @details ...
 */
typedef enum {
#if ITHACA_USE_RADIO_PPM
    RADIO_LLD_TYPE_PPM,
#endif
#if ITHACA_USE_RADIO_RFM12B
    RADIO_LLD_TYPE_RFM12B,
#endif
#if ITHACA_USE_RADIO_RFM22B
    RADIO_LLD_TYPE_RFM22B,
#endif
} radio_lld_type_t;

/*
 * @brief   ...
 * @details ...
 */
typedef union {
#if ITHACA_USE_RADIO_PPM
    PPMConfig *ppm;
#endif
#if ITHACA_USE_RADIO_RFM12B
    RFM12BConfig *rfm12b;
#endif
#if ITHACA_USE_RADIO_RFM22B
    RFM22BConfig *rfm22b;
#endif
} radio_lld_config_t;

/*
 * @brief   ...
 * @details ...
 */
typedef union {
#if ITHACA_USE_RADIO_PPM
    PPMDriver ppm;
#endif
#if ITHACA_USE_RADIO_RFM12B
    RFM12BDriver rfm12b;
#endif
#if ITHACA_USE_RADIO_RFM22B
    RFM22BDriver rfm22b;
#endif
} radio_lld_driver_t;

/*
 * @brief   ...
 * @details ...
 */
typedef struct {
    //
    char *name;
    uint16_t timeout;
    //
    radio_lld_type_t lld_type;
    radio_lld_config_t lld_config;
    //
    radio_callback_t idle_cb;
    radio_callback_t recv_done_cb;
    radio_callback_t recv_error_cb;
    radio_callback_t send_done_cb;
    radio_callback_t send_error_cb;
} RadioConfig;

/*
 * @brief   ...
 * @details ...
 */
typedef struct RadioDriver {
    RadioConfig *config;
    systime_t timeout;
    radio_lld_driver_t lld_driver;
    radio_packet_t packet;
    radio_state_t state;
    ithaca_lock_t lock;
    WORKING_AREA(radio_thread_working_area, 256);
    Thread *tp;
    void *user_arg;
    TimeMeasurement tm;
} RadioDriver;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
void radioInit(RadioDriver *radio, RadioConfig *config, void *arg);
bool radioIdleI(RadioDriver *radio, void *arg);
bool radioRecvStartI(RadioDriver *radio, void *arg);
bool radioRecvStart(RadioDriver *radio, void *arg);
bool radioSendStartI(RadioDriver *radio, radio_packet_t *packet, void *arg);
bool radioSendStart(RadioDriver *radio, radio_packet_t *packet, void *arg);
bool radioIdle(RadioDriver *radio, void *arg);
//bool radioRecv(RadioDriver *radio, radio_packet_t *packet, void *arg);
//bool radioSend(RadioDriver *radio, radio_packet_t *packet, void *arg);
//void radioSetTimeout(RadioDriver *radio, systime_t timeout);
//void radioSetFrequency(RadioDriver *radio, radio_frequency_t frequency);
//void radioSetDataRate(RadioDriver *radio, radio_data_rate_t data_rate);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RADIO */

#endif /* _RADIO_H_ */

