#ifndef _PCA9633_H_
#define _PCA9633_H_

#if ITHACA_USE_PCA9633 || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

#define PCA9633_SEQUENCE_MAX_CHANNELS 4

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
typedef enum {
    PCA9633_CONTROL_NOAI = 0x00,
    PCA9633_CONTROL_AI = 0xA0,
} pca9633_control_t;

/*
 * @brief   ...
 */
typedef enum {
    PCA9633_REGISTER_MODE1 = 0x00,
    PCA9633_REGISTER_MODE2 = 0x01,
    PCA9633_REGISTER_PWM0 = 0x02,
    PCA9633_REGISTER_PWM1 = 0x03,
    PCA9633_REGISTER_PWM2 = 0x04,
    PCA9633_REGISTER_PWM3 = 0x05,
    PCA9633_REGISTER_LEDOUT = 0x08,
} pca9633_register_t;

/*
 * @brief   ...
 */
typedef enum {
    PCA9633_MODE1_AI_DISABLED = 0x00,
} pca9633_mode1_t;

/*
 * @brief   ...
 */
typedef enum {
    PCA9633_MODE2_DMBLINK = 0x20,
    PCA9633_MODE2_INVRT = 0x10,
    PCA9633_MODE2_OCH = 0x08,
    PCA9633_MODE2_OUTDRV_POLE = 0x04,
    PCA9633_MODE2_OUTNE_OFF = 0x00,
    PCA9633_MODE2_OUTNE_ON = 0x01,
    PCA9633_MODE2_OUTNE_HI = 0x02,
} pca9633_mode2_t;

/*
 * @brief   ...
 */
typedef enum {
    PCA9633_LEDOUT_MASK = 0x03,
    PCA9633_LEDOUT_OFF = 0x00,
    PCA9633_LEDOUT_ON = 0x01,
    PCA9633_LEDOUT_PWM = 0x02,
} pca9633_ledout_t;

/*
 * @brief   ...
 */
typedef enum {
    PCA9633_UNINIT = 0,
    PCA9633_READY,
} pca9633_status_t;
 
/*
 * @brief   ...
 */
typedef enum {
    PCA9633_MODE_NORMAL = 0,
    PCA9633_MODE_INVERTED,
} pca9633_mode_t;
 
/*
 * @brief   ...
 */
typedef struct {
    char *desc;
    I2CDriver *i2c_driver;
    uint8_t i2c_addr;
    pca9633_status_t status;
    pca9633_mode_t mode;
} pca9633_t;

/*
 * @brief   ...
 */
typedef enum {
    PCA9633_CHANNEL_UNINIT = 0,
    PCA9633_CHANNEL_READY,
} pca9633_channel_status_t;
 
/*
 * @brief   ...
 */
typedef enum {
    PCA9633_CHANNEL_RED = 0,
    PCA9633_CHANNEL_GREEN = 1,
    PCA9633_CHANNEL_BLUE = 2,
    PCA9633_CHANNEL_AMBER = 3,
} pca9633_channel_type_t;
 
/*
 * @brief   ...
 */
typedef enum {
    PCA9633_CHANNEL_MODE_SWITCH = 0,
    PCA9633_CHANNEL_MODE_PWM,
} pca9633_channel_mode_t;
 
/*
 * @brief   ...
 */
typedef enum {
    PCA9633_SWITCH_OFF = 0,
    PCA9633_SWITCH_ON,
} pca9633_switch_state_t;
 
/*
 * @brief   ...
 */
typedef struct {
    pca9633_t *pca;
    pca9633_channel_status_t status;
    pca9633_channel_type_t channel;
    pca9633_channel_mode_t mode;
    uint16_t fader;
    uint16_t period;
    uint8_t prev;
    uint8_t cur;
    uint8_t next;
    systime_t change_start;
    systime_t fade_start;
} pca9633_channel_t;

/*
 * @brief   ...
 */
typedef struct pca9633_sequence_data_t pca9633_sequence_data_t;

/*
 * @brief   ...
 */
struct pca9633_sequence_data_t {
    pca9633_sequence_data_t *next;
    uint8_t data[];
};

/*
 * @brief   ...
 */
typedef struct {
    uint8_t channel_number;
    pca9633_channel_t *channel[PCA9633_SEQUENCE_MAX_CHANNELS];
    systime_t timestamp;
    uint16_t position;
    uint16_t period;
    pca9633_sequence_data_t *cur_seq;
    uint16_t sleep_timeout;
    pca9633_sequence_data_t *sleep_seq;
    systime_t sleep_start;
    ithaca_lock_t lock;
} pca9633_sequence_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
pca9633_status_t pca9633Init(pca9633_t *pca);
pca9633_channel_status_t pca9633ChannelInit(pca9633_channel_t *channel);
pca9633_channel_status_t pca9633ChannelPwmSet(pca9633_channel_t *channel,
                                              uint32_t pwm);
pca9633_channel_status_t pca9633ChannelPwmFade(pca9633_channel_t *channel,
                                               uint32_t unused);
pca9633_channel_status_t pca9633ChannelSwitchSet(pca9633_channel_t *channel,
                                                 uint32_t state);
pca9633_channel_status_t pca9633ChannelUpdate(pca9633_channel_t *channel,
                                              varg_t unused);
void pca9633SequenceInit(pca9633_sequence_t *sequence);
void pca9633SequenceUpdate(pca9633_sequence_t *sequence, varg_t unused);
bool pca9633SequenceSet(pca9633_sequence_t *sequence,
                        pca9633_sequence_data_t *seq);
bool pca9633SequenceActivateSleep(pca9633_sequence_t *sequence);
    ////bool pca9633SleepReset(pca9633_t *pca, varg_t unused);
    ////bool pca9633SleepActivate(pca9633_t *pca, varg_t unused);
    ////color_t pca9633AdvanceSequence(pca9633_t *pca);
    ////bool pca9633Play(pca9633_t *pca);
    ////bool pca9633SequenceSet(pca9633_t *pca, const color_t *sequence);
    ////bool pca9633SequenceLengthSet(pca9633_t *pca, uint32_t length);
    ////msg_t pca9633Player(void *arg);
    ////bool pca9633SetRed(pca9633_t *pca, uint32_t red);
    ////bool pca9633SetGreen(pca9633_t *pca, uint32_t green);
    ////bool pca9633SetBlue(pca9633_t *pca, uint32_t blue);
    ////bool pca9633SetAmber(pca9633_t *pca, uint32_t amber);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_PCA9633 */

#endif /* _PCA9633_H_ */

