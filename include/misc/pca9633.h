#ifndef _PCA9633_H_
#define _PCA9633_H_

#if ITHACA_USE_PCA9633 || defined(__DOXYGEN__)

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
 */
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t amber;
} color_t;
 
/*
 * @brief   ...
 */
typedef struct {
    //
    char *desc;
    I2CDriver *i2c_driver;
    uint8_t i2c_addr;
    uint16_t sleep_timeout;
    uint16_t fader_timeout;
    float step;
    uint16_t play_delay;
    //
    bool active;
    uint8_t sequence_length;
    color_t *sequence;
    //
    uint8_t seq_i;
    uint8_t seq_j;
    color_t prev_color;
    //
    systime_t sleep_start;
    systime_t sleep_end;
} pca9633_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
    bool pca9633Init(pca9633_t *pca);
    bool pca9633SleepReset(pca9633_t *pca, varg_t unused);
    bool pca9633SleepActivate(pca9633_t *pca, varg_t unused);
    color_t pca9633AdvanceSequence(pca9633_t *pca);
    bool pca9633Play(pca9633_t *pca);
    bool pca9633SequenceSet(pca9633_t *pca, const color_t *sequence);
    bool pca9633SequenceLengthSet(pca9633_t *pca, uint32_t length);
    msg_t pca9633Player(void *arg);
    bool pca9633SetRed(pca9633_t *pca, uint32_t red);
    bool pca9633SetGreen(pca9633_t *pca, uint32_t green);
    bool pca9633SetBlue(pca9633_t *pca, uint32_t blue);
    bool pca9633SetAmber(pca9633_t *pca, uint32_t amber);
    #ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_PCA9633 */

#endif /* _PCA9633_H_ */

