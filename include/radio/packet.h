#ifndef _RADIO_PACKET_H_
#define _RADIO_PACKET_H_

#if ITHACA_USE_RADIO_PACKET || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#ifndef RADIO_PACKET_DATA_SIZE
#define RADIO_PACKET_DATA_SIZE 0x10
#endif /* RADIO_PACKET_DATA_SIZE */

#if RADIO_PACKET_DATA_SIZE % 4 != 0
    #error RADIO_PACKET_DATA_SIZE must be multiple of 4
#endif /* RADIO_PACKET_DATA_SIZE % 4 != 0 */

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
#if RADIO_PACKET_DATA_SIZE > 0
    //
    uint8_t data[RADIO_PACKET_DATA_SIZE];
#endif /* RADIO_PACKET_DATA_SIZE */
} radio_packet_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_RADIO_PACKET */

#endif /* _RADIO_PACKET_H_ */

