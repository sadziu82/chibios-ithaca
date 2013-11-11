#include <ithaca.h>

#if ITHACA_USE_IMU || defined(__DOXYGEN__)

#include <math.h>

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
static bool imu_adxl345_read_data(imu_t *imu, imu_sensor_t *sensor) {
    //
    uint8_t data_reg = 0x32, data[6];
    static msg_t status = 0;
    // 
    status = i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr, &data_reg, 1, data, 6, MS2ST(2) );
    if (status != RDY_OK) {
        consoleWarn("%s read data - i2c error == %d\r\n",
                    sensor->desc, i2cGetErrors(imu->i2c_driver));
        return false;
    }
    imu->acc_x = (int16_t)(data[0] | data[1] << 8) * (3.9 / 1000) - 0.03;
    imu->acc_y = (int16_t)(data[2] | data[3] << 8) * (3.9 / 1000) + 0.04;
    imu->acc_z = (int16_t)(data[4] | data[5] << 8) * (3.9 / 1000) + 0.07;
    //
    consoleDebug("%s read data - ok\r\n", sensor->desc);
    // 
    return true;
}

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
        {0x31, 0x2B},
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
    .init = &imu_adxl345_init,
    .read_data = &imu_adxl345_read_data
};

/*
 * @brief   ...
 */
static bool imu_l3g4200d_read_data(imu_t *imu, imu_sensor_t *sensor) {
    //
    uint8_t data_reg = 0x80 | 0x26, data[8];
    static msg_t status = 0;
    // 
    status = i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr, &data_reg, 1, data, 8, MS2ST(2) );
    if (status != RDY_OK) {
        consoleWarn("%s read data - i2c error == %d\r\n",
                    sensor->desc, i2cGetErrors(imu->i2c_driver));
        return false;
    }
    imu->gyro_x = ((int16_t)( data[ 2 ] | data[ 3 ] << 8 ) + 21) * (70.0f / 1000);
    imu->gyro_y = ((int16_t)( data[ 4 ] | data[ 5 ] << 8 ) - 6) * (70.0f / 1000);
    imu->gyro_z = ((int16_t)( data[ 6 ] | data[ 7 ] << 8 ) + 11) * (70.0f / 1000);
    imu->gyro_t = data[ 0 ];
    //
    consoleDebug("%s read data - ok\r\n", sensor->desc);
    // 
    return true;
}


/*
 * @brief   ...
 */
static const imu_sensor_id_t imu_l3g4200d_id = {
    .addr = 0x0F,
    .length = 1,
    .data = {0xD3}
};

/*
 * @brief   ...
 */
static const imu_sensor_init_t imu_l3g4200d_init = {
    .length = 5,
    .data = {
        {0x20, 0x1F},        // power on mode
        {0x21, 0x00},
        {0x22, 0x00},
        {0x23, 0x30},           // 2000 dps
        {0x24, 0x00}
    }
};

/*
 * @brief   ...
 */
static const imu_sensor_t imu_l3g4200d = {
    .desc = "L3G4200D",
    .addr = 0x68,
    .id = &imu_l3g4200d_id,
    .init = &imu_l3g4200d_init,
    .read_data = &imu_l3g4200d_read_data
};

/*
 * @brief   ...
 */
static bool imu_hmc5883l_read_data(imu_t *imu, imu_sensor_t *sensor) {
    //
    uint8_t data_reg = 0x03, data[6];
    static msg_t status = 0;
    float mx, my, mz;
    // 
    status = i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr, &data_reg, 1, data, 6, MS2ST(2));
    if (status != RDY_OK) {
        consoleWarn("%s read data - i2c error == %d\r\n",
                    sensor->desc, i2cGetErrors(imu->i2c_driver));
        return false;
    }
    // 
    mx = (int16_t)(data[1] | data[0] << 8) + 38;
    my = (int16_t)(data[5] | data[4] << 8) + 65;
    mz = (int16_t)(data[3] | data[2] << 8) - 130;
    // 
    imu->mag_x = mx; // (mx - imu->mag_min_x) / (imu->mag_max_x - imu->mag_min_x);
    imu->mag_y = my; // (my - imu->mag_min_y) / (imu->mag_max_y - imu->mag_min_y);
    imu->mag_z = mz; // (mz - imu->mag_min_z) / (imu->mag_max_z - imu->mag_min_z);
    //
    consoleDebug("%s read data - ok\r\n", sensor->desc);
    //consoleWarn("%s - x: %7f; x_min %7f; x_max %7f; y: %7f; y_min %7f; y_max %7f; z: %7f; z_min %7f; z_max %7f;\r\n", sensor->desc, imu->mag_x, imu->mag_min_x, imu->mag_max_x, imu->mag_y, imu->mag_min_y, imu->mag_max_y, imu->mag_z, imu->mag_min_z, imu->mag_max_z);
    // 
    return true;
}

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
        {0x00, 0x78},       // 8 samples per measurement, 75 hz output rate
        {0x01, 0x40},       // +- 8.1 Gauss range 0.88
        {0x02, 0x00}        // continous measurements
    }
};

/*
 * @brief   ...
 */
static const imu_sensor_t imu_hmc5883l = {
    .desc = "HMC5883L",
    .addr = 0x1E,
    .id = &imu_hmc5883l_id,
    .init = &imu_hmc5883l_init,
    .read_data = &imu_hmc5883l_read_data
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
    .init = &imu_bmp085_init,
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
    .gyro = &imu_l3g4200d,
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
    if (imuSensorInit(imu, imu->acc) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // gyroscope
    if (imuSensorInit(imu, imu->gyro) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // magnetometer
    if (imuSensorInit(imu, imu->mag) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // barometer
    if (imuSensorInit(imu, imu->bar) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // 
    i2cReleaseBus(imu->i2c_driver); 
    consoleInfo("init - status: %d, timing: %d\r\n", status, chTimeNow() - start);
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
    if (sensor->read_data != NULL) {
        //
        if (sensor->read_data(imu, (struct _imu_sensor_t *)sensor) == false) {
            return false;
        }
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
    if (imuSensorRefreshData(imu, imu->acc) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // gyroscope
    if (imuSensorRefreshData(imu, imu->gyro) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // magnetometer
    if (imuSensorRefreshData(imu, imu->mag) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // barometer
    if (imuSensorRefreshData(imu, imu->bar) == false) {
        i2cStop(imu->i2c_driver);
        i2cStart(imu->i2c_driver, &imu->i2c_config);
        status = false;
    }
    // 
    i2cReleaseBus(imu->i2c_driver); 
    //
    consoleDebug("refresh data - status: %d, timing: %d\r\n",
                status, chTimeNow() - start);
    // 
    return status;
}

/*
 * @brief   IMU processing thread
 * @details IMU processing thread
 */
static float radian = 57.296f;

/*
 * @brief   IMU processing thread
 * @details http://en.wikipedia.org/wiki/Fast_inverse_square_root
 */
float invSqrt(float x) {
    volatile float halfx = 0.5f * x;
    volatile float y = x;
    volatile long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/*
 * @brief   IMU processing thread
 * @details IMU processing thread
 */
bool imuMadgwickAHRSupdateIMU(imu_t *imu) {
    float recipNorm;
    float s0, s1, s2, s3;
    float gx, gy, gz, ax, ay, az;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;
    // 
    ax = imu->acc_x;
    ay = imu->acc_y;
    az = imu->acc_z;
    //
    gx = imu->gyro_x / radian;
    gy = imu->gyro_y / radian;
    gz = imu->gyro_z / radian;
    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-imu->q1 * gx - imu->q2 * gy - imu->q3 * gz);
    qDot2 = 0.5f * (imu->q0 * gx + imu->q2 * gz - imu->q3 * gy);
    qDot3 = 0.5f * (imu->q0 * gy - imu->q1 * gz + imu->q3 * gx);
    qDot4 = 0.5f * (imu->q0 * gz + imu->q1 * gy - imu->q2 * gx);
    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;   
        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * imu->q0;
        _2q1 = 2.0f * imu->q1;
        _2q2 = 2.0f * imu->q2;
        _2q3 = 2.0f * imu->q3;
        _4q0 = 4.0f * imu->q0;
        _4q1 = 4.0f * imu->q1;
        _4q2 = 4.0f * imu->q2;
        _8q1 = 8.0f * imu->q1;
        _8q2 = 8.0f * imu->q2;
        q0q0 = imu->q0 * imu->q0;
        q1q1 = imu->q1 * imu->q1;
        q2q2 = imu->q2 * imu->q2;
        q3q3 = imu->q3 * imu->q3;
        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * imu->q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * imu->q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * imu->q3 - _2q1 * ax + 4.0f * q2q2 * imu->q3 - _2q2 * ay;
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;
        // Apply feedback step
        qDot1 -= imu->beta * s0;
        qDot2 -= imu->beta * s1;
        qDot3 -= imu->beta * s2;
        qDot4 -= imu->beta * s3;
    }
    // Integrate rate of change of quaternion to yield quaternion
    imu->q0 += qDot1 * imu->sample_period;
    imu->q1 += qDot2 * imu->sample_period;
    imu->q2 += qDot3 * imu->sample_period;
    imu->q3 += qDot4 * imu->sample_period;
    // Normalise quaternion
    recipNorm = invSqrt(imu->q0 * imu->q0 + imu->q1 * imu->q1 + imu->q2 * imu->q2 + imu->q3 * imu->q3);
    imu->q0 *= recipNorm;
    imu->q1 *= recipNorm;
    imu->q2 *= recipNorm;
    imu->q3 *= recipNorm;
    //
    // imu->roll = atan2f( 2 * imu->q1 * imu->q2 - 2 * imu->q0 * imu->q3, 2 * imu->q0*imu->q0 + 2 * imu->q1 * imu->q1 - 1 );
    // imu->pitch = atanf( gx / sqrtf( gy * gy + gz * gz ) );
    // imu->yaw = atanf( gy / sqrtf( gx * gx + gz * gz ) );
    //
    return true;
}

/*
 * @brief   IMU processing thread
 * @details IMU processing thread
 */
void imuMadgwickAHRSupdate(imu_t *imu) {
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
    float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
    float gx, gy, gz, ax, ay, az, mx, my, mz;
    //
    gx = imu->gyro_x / radian;
    gy = imu->gyro_y / radian;
    gz = imu->gyro_z / radian;
    // 
    ax = imu->acc_x;
    ay = imu->acc_y;
    az = imu->acc_z;
    // 
    mx = imu->mag_x;
    my = imu->mag_y;
    mz = imu->mag_z;
    // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
    if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
        imuMadgwickAHRSupdateIMU(imu);
        return;
    }
    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-imu->q1 * gx - imu->q2 * gy - imu->q3 * gz);
    qDot2 = 0.5f * (imu->q0 * gx + imu->q2 * gz - imu->q3 * gy);
    qDot3 = 0.5f * (imu->q0 * gy - imu->q1 * gz + imu->q3 * gx);
    qDot4 = 0.5f * (imu->q0 * gz + imu->q1 * gy - imu->q2 * gx);
    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;   
        // Normalise magnetometer measurement
        recipNorm = invSqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;
        // Auxiliary variables to avoid repeated arithmetic
        _2q0mx = 2.0f * imu->q0 * mx;
        _2q0my = 2.0f * imu->q0 * my;
        _2q0mz = 2.0f * imu->q0 * mz;
        _2q1mx = 2.0f * imu->q1 * mx;
        _2q0 = 2.0f * imu->q0;
        _2q1 = 2.0f * imu->q1;
        _2q2 = 2.0f * imu->q2;
        _2q3 = 2.0f * imu->q3;
        _2q0q2 = 2.0f * imu->q0 * imu->q2;
        _2q2q3 = 2.0f * imu->q2 * imu->q3;
        q0q0 = imu->q0 * imu->q0;
        q0q1 = imu->q0 * imu->q1;
        q0q2 = imu->q0 * imu->q2;
        q0q3 = imu->q0 * imu->q3;
        q1q1 = imu->q1 * imu->q1;
        q1q2 = imu->q1 * imu->q2;
        q1q3 = imu->q1 * imu->q3;
        q2q2 = imu->q2 * imu->q2;
        q2q3 = imu->q2 * imu->q3;
        q3q3 = imu->q3 * imu->q3;
        // Reference direction of Earth's magnetic field
        hx = mx * q0q0 - _2q0my * imu->q3 + _2q0mz * imu->q2 + mx * q1q1 + _2q1 * my * imu->q2 + _2q1 * mz * imu->q3 - mx * q2q2 - mx * q3q3;
        hy = _2q0mx * imu->q3 + my * q0q0 - _2q0mz * imu->q1 + _2q1mx * imu->q2 - my * q1q1 + my * q2q2 + _2q2 * mz * imu->q3 - my * q3q3;
        _2bx = sqrt(hx * hx + hy * hy);
        _2bz = -_2q0mx * imu->q2 + _2q0my * imu->q1 + mz * q0q0 + _2q1mx * imu->q3 - mz * q1q1 + _2q2 * my * imu->q3 - mz * q2q2 + mz * q3q3;
        _4bx = 2.0f * _2bx;
        _4bz = 2.0f * _2bz;
        // Gradient decent algorithm corrective step
        s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * imu->q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * imu->q3 + _2bz * imu->q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * imu->q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * imu->q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * imu->q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * imu->q2 + _2bz * imu->q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * imu->q3 - _4bz * imu->q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * imu->q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * imu->q2 - _2bz * imu->q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * imu->q1 + _2bz * imu->q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * imu->q0 - _4bz * imu->q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * imu->q3 + _2bz * imu->q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * imu->q0 + _2bz * imu->q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * imu->q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;
        // Apply feedback step
        qDot1 -= imu->beta * s0;
        qDot2 -= imu->beta * s1;
        qDot3 -= imu->beta * s2;
        qDot4 -= imu->beta * s3;
    }
    // Integrate rate of change of quaternion to yield quaternion
    imu->q0 += qDot1 * imu->sample_period;
    imu->q1 += qDot2 * imu->sample_period;
    imu->q2 += qDot3 * imu->sample_period;
    imu->q3 += qDot4 * imu->sample_period;
    // Normalise quaternion
    recipNorm = invSqrt(imu->q0 * imu->q0 + imu->q1 * imu->q1 + imu->q2 * imu->q2 + imu->q3 * imu->q3);
    imu->q0 *= recipNorm;
    imu->q1 *= recipNorm;
    imu->q2 *= recipNorm;
    imu->q3 *= recipNorm;
}

/*
 * @brief   IMU processing thread
 * @details IMU processing thread
 */
void imuGetYawPitchRoll(imu_t *imu) {
  float gx, gy, gz; // estimated gravity direction
  
  gx = 2 * (imu->q1*imu->q3 - imu->q0*imu->q2);
  gy = 2 * (imu->q0*imu->q1 + imu->q2*imu->q3);
  gz = imu->q0*imu->q0 - imu->q1*imu->q1 - imu->q2*imu->q2 + imu->q3*imu->q3;
  
  imu->yaw = atan2(2 * imu->q1 * imu->q2 - 2 * imu->q0 * imu->q3, 2 * imu->q0*imu->q0 + 2 * imu->q1 * imu->q1 - 1) * radian;
  imu->pitch = atan(gx / sqrt(gy*gy + gz*gz))  * radian;
  imu->roll = atan(gy / sqrt(gx*gx + gz*gz))  * radian;
}

/*
 * @brief   ...
 */
static bool imu_hmc5883l_calibrate(imu_t *imu, const imu_sensor_t *sensor) {
    //
    uint8_t selftest_bias1[] = {0x00, 0x72},
            selftest_bias2[] = {0x00, 0x71},
            selftest_scale[] = {0x01, 0x20},
            selftest_single[] = {0x02, 0x01};
    float mx1, my1, mz1;
    float mx2, my2, mz2;
    //
    uint8_t init_1[] = {0x00, 0x78},
            init_2[] = {0x01, 0x60},
            init_3[] = {0x02, 0x00};
    //
    uint8_t data_addr = 0x03, data1[6], data2[6];
    systime_t calibration_end;
    msg_t status = 0;
    // 
    systime_t wakeup_time;
    // put sensor in self-test "+"
    chThdSleepMilliseconds(50);
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       selftest_bias1, 2, NULL, 0, MS2ST(2));
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       selftest_scale, 2, NULL, 0, MS2ST(2));
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       selftest_single, 2, NULL, 0, MS2ST(2));
    // get data after self-test
    chThdSleepMilliseconds(75);
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       &data_addr, 1, data1, 6, MS2ST(2));
    // put sensor in self-test "-"
    chThdSleepMilliseconds(50);
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       selftest_bias2, 2, NULL, 0, MS2ST(2));
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       selftest_scale, 2, NULL, 0, MS2ST(2));
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       selftest_single, 2, NULL, 0, MS2ST(2));
    // get data after self-test
    chThdSleepMilliseconds(75);
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       &data_addr, 1, data2, 6, MS2ST(2));
    // init in normal mode
    chThdSleepMilliseconds(50);
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       init_1, 2, NULL, 0, MS2ST(2));
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       init_2, 2, NULL, 0, MS2ST(2));
    status |= i2cMasterTransmitTimeout(imu->i2c_driver, sensor->addr,
                                       init_3, 2, NULL, 0, MS2ST(2));
    //
    imu->mag_min_x = (int16_t)(data1[1] | data1[0] << 8);
    imu->mag_min_z = (int16_t)(data1[3] | data1[2] << 8);
    imu->mag_min_y = (int16_t)(data1[5] | data1[4] << 8);
    // 
    imu->mag_max_x = (int16_t)(data2[1] | data2[0] << 8);
    imu->mag_max_z = (int16_t)(data2[3] | data2[2] << 8);
    imu->mag_max_y = (int16_t)(data2[5] | data2[4] << 8);
    // check for communication error5
    if (status != RDY_OK) {
        consoleWarn("%s calibration: i2c error == %d\r\n",
                    sensor->desc, i2cGetErrors(imu->i2c_driver));
        return false;
    }
    // 
    consoleInfo("%s calibration: ok\r\n", sensor->desc);
    // 
    return true;
}

/*
 * @brief   IMU processing thread
 * @details IMU processing thread
 */
void imuGetHeading(imu_t *imu) {
    float Head_X;
    float Head_Y;
    float cos_roll;
    float sin_roll;
    float cos_pitch;
    float sin_pitch;
    float mx, my, mz;
    float norm;
    //
    mx = imu->mag_x; my = imu->mag_y; mz = imu->mag_z;
    norm = invSqrt(mx * mx + my * my + mz * mz);
    mx *= norm; my *= norm; mz *= norm;
    //
    cos_roll = cos(imu->pitch/radian);
    sin_roll = sin(imu->pitch/radian);
    cos_pitch = cos(imu->roll/radian);
    sin_pitch = sin(imu->roll/radian);
  
    // Tilt compensated Magnetic field X component:
    Head_X = mx * cos_pitch + my * sin_roll * sin_pitch + mz * cos_roll * sin_pitch;
    // Tilt compensated Magnetic field Y component:
    Head_Y = my * cos_roll - mz * sin_roll;
    // Magnetic imu->heading
    imu->heading = atan2(-Head_Y,Head_X)*radian;
    
    //imu->heading = atan2(mx, my)*radian;
  
  //// // Declination correction (if supplied)
  //// if( MAGNETIC_DECLINATION != 0 ) 
  //// {
  ////     imu->heading = imu->heading + ToRad(MAGNETIC_DECLINATION);
  ////     if (imu->heading > M_PI)    // Angle normalization (-180 deg, 180 deg)
  ////         imu->heading -= (2.0 * M_PI);
  ////     else if (imu->heading < -M_PI)
  ////         imu->heading += (2.0 * M_PI);
  //// }
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
    imu->sample_period = imu->refresh_period / 1000.0f;
    //
    imu->q0 = 1;
    imu->q1 = imu->q2 = imu->q3 = 0;
    imu->beta = 0.2f;
    //
    while (true) {
        // initialize imu board
        while (!imuInit(imu)) {
            consoleError("initialization failed, retrying\r\n");
            chThdSleepMilliseconds(imu->refresh_period);
        }
        /// // 
        /// imu_l3g4200d_calibrate(imu, imu->gyro);
        /// while (1) {
        ///     chThdSleepMilliseconds(10);
        /// }
        /// 
        imu_hmc5883l_calibrate(imu, imu->mag);
        // play!
        wakeup_time = chTimeNow();
        while (imuRefreshData(imu) != false) {
            //consoleWrite("play failed, retrying\r\n");
            //consoleWarn("acc data - x: %f, y: %f, z: %f \r\n",
            //             imu->acc_x, imu->acc_y, imu->acc_z);
            //consoleWarn("gyro data - x: %f, y: %f, z: %f t: %f\r\n",
            //             imu->gyro_x, imu->gyro_y, imu->gyro_z, imu->gyro_t);
            //consoleWarn("mag data - x: %f, y: %f, z: %f \r\n",
            //            imu->mag_x, imu->mag_y, imu->mag_z);
            // 
            imuMadgwickAHRSupdate(imu);
            //imuMadgwickAHRSupdateIMU(imu);
            //
            imuGetYawPitchRoll(imu);
            //imuGetHeading(imu);
            // consoleWarn("AHRS - q0: %5f, q1: %5f, q2: %5f, q3: %5f \r\n",
            //             imu->q0, imu->q1, imu->q2, imu->q3);
            /// //
            /// imu->roll += imu->gyro_x * sample_period;
            /// imu->pitch += imu->gyro_y * sample_period;
            /// imu->yaw += imu->gyro_z * sample_period;
            consoleWarn("roll: %5f, pitch: %5f, yaw: %5f, heading: %5f\r\n",
                        imu->roll, imu->pitch, imu->yaw, imu->heading);
            // FIXME data calculation
            //chThdSleepMilliseconds(imu->refresh_period / 2);
            //
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

