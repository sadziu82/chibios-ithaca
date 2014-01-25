#include <ithaca.h>

#if ITHACA_USE_LM75AD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*
 * @brief   ...
 */
uint8_t LM75AD_GET_TEMP[] = {LM75AD_REGISTER_TEMP};

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 */
lm75ad_status_t lm75adInit(lm75ad_t *sensor) {
    sensor->status = LM75AD_READY;
    sensor->refresh_time = 0;
    sensor->temp = 0;
    //
    return sensor->status;
}

/*
 * @brief   ...
 */
bool lm75adRefreshTemp(lm75ad_t *sensor, varg_t unused) {
    //
    msg_t status = RDY_OK;
    uint8_t data[2];
    int16_t temp;
    (void)unused;
    //
    consoleDebug("lm75adRefreshTemp start\r\n");
    if (sensor->refresh_time < chTimeNow()) {
        sensor->refresh_time = chTimeNow() + MS2ST(sensor->refresh_period);
        i2cAcquireBus(sensor->i2c_driver);
        status = i2cMasterTransmitTimeout(sensor->i2c_driver,
                                          sensor->i2c_addr,
                                          LM75AD_GET_TEMP, 1,
                                          data, 2, MS2ST(3));
        if (status == RDY_OK) {
            consoleDebug("lm75adRefreshTemp refresh ok\r\n");
            temp = (int16_t)((data[0]<<8) + (data[1]));
            sensor->temp = temp / 256;
            sensor->status = LM75AD_READY;
        } else {
            consoleDebug("lm75adRefreshTemp refresh fail\r\n");
            i2cStop(sensor->i2c_driver);
            i2cStart(sensor->i2c_driver, sensor->i2c_driver->config);
            sensor->status = LM75AD_ERROR;
        }
        i2cReleaseBus(sensor->i2c_driver);
    }
    //
    if (sensor->status == LM75AD_READY) {
        consoleDebug("lm75adRefreshTemp end: %d\r\n", sensor->temp);
    } else {
        consoleDebug("lm75adRefreshTemp failed\r\n");
    }
    //
    return sensor->status;
}

/*
 * @brief   ...
 */
bool lm75adGetTemperature(lm75ad_t *sensor, int8_t *temp) {
    //
    *temp = sensor->temp;
    return true;
}

#endif /* ITHACA_USE_LM75AD */

