#include <ithaca.h>

#if ITHACA_USE_RADIO_LINK || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define STREAM_BUFFER_SIZE 0x100

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 */
static uint8_t input_buffer[STREAM_BUFFER_SIZE];
static uint8_t output_buffer[STREAM_BUFFER_SIZE];

/*
 * @brief   ...
 */
static INPUTQUEUE_DECL(input_queue, input_buffer, STREAM_BUFFER_SIZE, NULL, NULL);
static OUTPUTQUEUE_DECL(output_queue, output_buffer, STREAM_BUFFER_SIZE, NULL, NULL);

/*
 * @brief   ...
 */
typedef struct {
    //
    uint16_t control;
    uint8_t stick_lv;
    uint8_t stick_lh;
    uint8_t stick_rv;
    uint8_t stick_rh;
} rc_master_data_t;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool rlStore(void) {
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
bool rlStart(void) {
    chIQInit(&input_queue, input_buffer, STREAM_BUFFER_SIZE, NULL, NULL);
    chIQInit(&output_queue, output_buffer, STREAM_BUFFER_SIZE, NULL, NULL);
}

#endif /* ITHACA_USE_RADIO_LINK */

