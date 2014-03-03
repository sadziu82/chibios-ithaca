#ifndef _LM75AD_H_
#define _LM75AD_H_

#if ITHACA_USE_LM75AD || defined(__DOXYGEN__)

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
typedef enum {
    LM75AD_REGISTER_TEMP = 0x00,
} lm75ad_register_t;

/*
 * @brief   ...
 */
typedef enum {
    LM75AD_UNINIT = 0,
    LM75AD_READY,
    LM75AD_ERROR,
} lm75ad_status_t;
 
/*
 * @brief   ...
 */
typedef struct {
    char *desc;
    I2CDriver *i2c_driver;
    uint8_t i2c_addr;
    lm75ad_status_t status;
    systime_t refresh_time;
    uint16_t refresh_period;
    int8_t temp;
} lm75ad_t;


/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
lm75ad_status_t lm75adInit(lm75ad_t *sensor);
bool lm75adRefreshTemp(lm75ad_t *sensor, varg_t unused);
bool lm75adGetTemperature(lm75ad_t *sensor, int8_t *temp);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_LM75AD */

#endif /* _LM75AD_H_ */

