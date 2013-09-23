#ifndef _IMU_H_
#define _IMU_H_

#if ITHACA_USE_IMU || defined(__DOXYGEN__)

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
    uint8_t addr;
    uint8_t length;
    uint8_t data[];
} imu_sensor_id_t;

/*
 * @brief   ...
 */
typedef struct {
    const uint8_t addr;
    const uint8_t value;
} imu_register_t;

/*
 * @brief   ...
 */
typedef struct {
    const uint8_t length;
    const imu_register_t data[];
} imu_sensor_init_t;

/*
 * @brief   ...
 */
typedef struct {
    const uint8_t length;
} imu_sensor_read_t;

/*
 * @brief   ...
 */
typedef struct {
    const char *desc;
    const i2caddr_t addr;
    const imu_sensor_id_t *id;
    const imu_sensor_init_t *init;
    const imu_sensor_init_t *read;
} imu_sensor_t;

/*
 * @brief   ...
 */
typedef struct {
    //
    I2CDriver *i2c_driver;
    I2CConfig i2c_config;
    // 
    i2caddr_t acc_addr;
    i2caddr_t gyro_addr;
    i2caddr_t mag_addr;
    i2caddr_t bar_addr;
    //
    uint32_t roll;
    uint32_t pitch;
    uint32_t yaw;
    //
    uint32_t speed;
    uint32_t heading;
    uint32_t altitude;
    //
    char *desc;
    uint16_t refresh_period;
    // 
    const imu_sensor_t *acc;
    const imu_sensor_t *gyro;
    const imu_sensor_t *mag;
    const imu_sensor_t *bar;
} imu_t;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif
    //
    extern imu_t imu;
    // 
    bool imuInit(imu_t *imu);
    msg_t imuPilot(void *arg);
#ifdef __cplusplus
}
#endif

#endif /* ITHACA_USE_IMU */

#endif /* _IMU_H_ */

