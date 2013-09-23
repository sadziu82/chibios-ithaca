#include <ithaca.h>

#if ITHACA_USE_IMU || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*
 * @brief   ...
 */
static const imu_sensor_id_t imu_adxl345_id = {
    .addr = 0x00,
    .length = 1,
    .data = {0xE5}
};

/*
 * @brief   ...
 */
static const imu_sensor_init_t imu_adxl345_init = {
    .length = 4,
    .data = {
        {0x31, 0x2F},
        {0x2C, 0x08},
        {0x2E, 0x00},
        {0x2D, 0x08}
    }
};

/*
 * @brief   ...
 */
static const imu_sensor_t imu_adxl345 = {
    .desc = "ADXL345",
    .addr = 0x53,
    .id = &imu_adxl345_id,
    .init = &imu_adxl345_init
};

/*
 * @brief   ...
 */
static const imu_sensor_id_t imu_l3gd4200d_id = {
    .addr = 0x0F,
    .length = 1,
    .data = {0xD3}
};

/*
 * @brief   ...
 */
static const imu_sensor_init_t imu_l3gd4200d_init = {
    .length = 5,
    .data = {
        {0x20, 0x0F},        // power on mode
        {0x21, 0x00},
        {0x22, 0x00},
        {0x23, 0x30},
        {0x24, 0x00}
    }
};

/*
 * @brief   ...
 */
static const imu_sensor_t imu_l3gd4200d = {
    .desc = "L3GD4200D",
    .addr = 0x68,
    .id = &imu_l3gd4200d_id,
    .init = &imu_l3gd4200d_init
};

/*
 * @brief   ...
 */
static const imu_sensor_id_t imu_hmc5883l_id = {
    .addr = 0x0A,
    .length = 3,
    .data = {0x48, 0x34, 0x33}
};

/*
 * @brief   ...
 */
static const imu_sensor_init_t imu_hmc5883l_init = {
    .length = 3,
    .data = {
        {0x00, 0x78},
        {0x01, 0x00},
        {0x02, 0x00}
    }
};

/*
 * @brief   ...
 */
static const imu_sensor_t imu_hmc5883l = {
    .desc = "HMC5883L",
    .addr = 0x1E,
    .id = &imu_hmc5883l_id,
    .init = &imu_hmc5883l_init
};

/*
 * @brief   ...
 */
static const imu_sensor_id_t imu_bmp085_id = {
    .addr = 0xD0,
    .length = 2,
    .data = {0x55, 0x01}
};

/*
 * @brief   ...
 */
static const imu_sensor_init_t imu_bmp085_init = {
    .length = 0,
    .data = {
    }
};

/*
 * @brief   ...
 */
static const imu_sensor_t imu_bmp085 = {
    .desc = "BMP085",
    .addr = 0x77,
    .id = &imu_bmp085_id,
    .init = &imu_bmp085_init
};

/*
 * @brief   ...
 */
imu_t imu = {
    // general options
    .desc = "IMU",
    .refresh_period = 50,
    //  io interface
    .i2c_driver = NULL,
    .i2c_config = {OPMODE_I2C, 400000, FAST_DUTY_CYCLE_2},
    .bar_addr = 0x77,
    // sensors
    .acc = &imu_adxl345,
    .gyro = &imu_l3gd4200d,
    .mag = &imu_hmc5883l,
    .bar = &imu_bmp085,
};

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*
 * @brief   ...
 * @details ...
 */
static bool imuSensorCheck(imu_t *imu, const imu_sensor_t *sensor) {
    // 
    uint8_t data[3], len, i;
    msg_t status;
    // stm32f1xx can read only one byte - must read zero or more than one
    if (sensor->id->length == 1) {
        len = 2;
    } else {
        len = sensor->id->length;
    }
    data[0] = data[1] = data[2] = 0;
    status = i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                      &sensor->id->addr, 1, data, len,
                                      MS2ST(2));
    if (status != RDY_OK) {
        consoleWarn("%s check - i2c error == %d\r\n",
                    sensor->desc, i2cGetErrors(imu->i2c_driver));
        return false;
    }
    // 
    for (i = 0; i < sensor->id->length; i++) {
        if (data[i] != sensor->id->data[i]) {
            consoleWarn("%s check - chip id mis-match: %d == %d\r\n",
                        sensor->desc, data[i], sensor->id->data[i]);
            return false;
        }
    }
    //
    consoleDebug("%s check - ok\r\n", sensor->desc);
    //
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
static bool imuSensorInit(imu_t *imu, const imu_sensor_t *sensor) {
    // 
    uint8_t i, data[2];
    msg_t status;
    // 
    if (imuSensorCheck(imu, sensor) == false) {
        return false;
    }
    //
    for (i = 0; i < sensor->init->length; i++) {
        // 
        data[0] = sensor->init->data[i].addr;
        data[1] = sensor->init->data[i].value;
        status =  i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                           data, 2, NULL, 0, MS2ST(2));
        if (status != RDY_OK) {
            consoleError("%s init - i2c error = %d\r\n", status,
                         sensor->desc, i2cGetErrors(imu->i2c_driver));
            return false;
        }
        i++;
    }
    //
    consoleDebug("%s init - ok\r\n", sensor->desc);
    // 
    return true;
}

/*
 * @brief   ...
 */
bool imuInit(imu_t *imu) {
    //
    bool status;
    systime_t start;
    // 
    start = chTimeNow();
    status = true;
    i2cAcquireBus(imu->i2c_driver); 
    // accelerometer
    if (!imuSensorInit(imu, imu->acc) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // gyroscope
    if (!imuSensorInit(imu, imu->gyro)) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // magnetometer
    if (!imuSensorInit(imu, imu->mag)) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // barometer
    if (!imuSensorInit(imu, imu->bar)) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // 
    i2cReleaseBus(imu->i2c_driver); 
    consoleInfo("%s init timing: %d\r\n", imu->desc, chTimeNow() - start);
    return status;
}

/*
 * @brief   ...
 * @details ...
 */
bool imuSensorRefreshData(imu_t *imu, const imu_sensor_t *sensor) {
    // 
    if (imuSensorCheck(imu, sensor) == false) {
        return false;
    }
    //
    consoleDebug("%s refresh data - ok\r\n", sensor->desc);
    // 
    return true;
}

/*
 * @brief   ...
 * @details ...
 */
bool imuRefreshData(imu_t *imu) {
    //
    bool status;
    systime_t start;
    // 
    start = chTimeNow();
    status = true;
    i2cAcquireBus(imu->i2c_driver); 
    // accelerometer
    if (!imuSensorRefreshData(imu, imu->acc) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // gyroscope
    if (!imuSensorRefreshData(imu, imu->gyro)) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // magnetometer
    if (!imuSensorRefreshData(imu, imu->mag)) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // barometer
    if (!imuSensorRefreshData(imu, imu->bar)) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // 
    i2cReleaseBus(imu->i2c_driver); 
    // data calculation
    chThdSleepMilliseconds(imu->refresh_period / 2);
    //
    consoleInfo("%s refresh data timing: %d\r\n", imu->desc, chTimeNow() - start);
    // 
    return status;
}

/*
 * @brief   IMU processing thread
 * @details IMU processing thread
 */
msg_t imuPilot(void *arg) {
    //
    systime_t wakeup_time;
    imu_t *imu;
    //
    imu = (imu_t *)arg; 
    chRegSetThreadName(imu->desc);
    //
    while (true) {
        // initialize imu board
        while (!imuInit(imu)) {
            consoleError("initialization failed, retrying\r\n");
            chThdSleepMilliseconds(imu->refresh_period);
        }
        // play!
        wakeup_time = chTimeNow();
        while (imuRefreshData(imu)) {
            //consoleWrite("play failed, retrying\r\n");
            consoleDebug("refreshing imu data\r\n");
            wakeup_time += MS2ST(imu->refresh_period);
            if (wakeup_time > chTimeNow()) {
                chThdSleepUntil(wakeup_time);
            } else {
                consoleError("loop time exceeded\r\n");
                wakeup_time = chTimeNow();
            }
        }
    }
    //
    return 0;
}

#endif /* ITHACA_USE_IMU */

