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
 * @details ...
 */
struct _imu_t;
struct _imu_sensor_t;

/*
 * @brief   ...
 * @details ...
 */
typedef bool (*imu_sensor_read_func_t)(struct _imu_t *, struct _imu_sensor_t *);

/*
 * @brief   ...
 */
typedef struct _imu_sensor_t {
    const char *desc;
    const i2caddr_t addr;
    const imu_sensor_id_t *id;
    const imu_sensor_init_t *init;
    const imu_sensor_read_func_t read_data;
} imu_sensor_t;

/*
 * @brief   ...
 */
typedef struct _imu_t {
    //
    I2CDriver *i2c_driver;
    I2CConfig i2c_config;
    // 
    i2caddr_t acc_addr;
    i2caddr_t gyro_addr;
    i2caddr_t mag_addr;
    i2caddr_t bar_addr;
    //
    float roll;
    float pitch;
    float yaw;
    float heading;
    //
    float q0, q1, q2, q3;
    float beta;
    //
    float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y, gyro_z, gyro_t;
    float mag_x, mag_y, mag_z;
    float mag_scale_x, mag_scale_y, mag_scale_z;
    float mag_min_x, mag_min_y, mag_min_z;
    float mag_max_x, mag_max_y, mag_max_z;
    //
    uint32_t speed;
    uint32_t altitude;
    //
    char *desc;
    uint16_t refresh_period;
    float sample_period;
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

