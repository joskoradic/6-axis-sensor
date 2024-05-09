/*
 * icm_20689.h
 *
 *  Created on: Apr 29, 2022
 *      Author: josko
 */

#ifndef SRC_DEVICE_DRIVERS_ICM_20689_H_
#define SRC_DEVICE_DRIVERS_ICM_20689_H_

#include "../data_types.h"


struct s_icm20689;

typedef struct s_gyro_data{
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
}t_gyro_data;


typedef struct s_accel_data{
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
}t_accel_data;


typedef t_driver_status (*icm20689_init)(struct s_icm20689);
typedef t_driver_status (*icm20689_get_gyro)(t_gyro_data*);
typedef t_driver_status (*icm20689_get_accel)(t_accel_data*);
typedef t_driver_status (*icm20689_get_temperature)(float* temp);
typedef t_driver_status (*icm20689_active)(float* temp);


typedef struct e_icm20689 {
	t_periph_interface interface;
	icm20689_get_gyro get_gyro_data;
	icm20689_get_accel get_accel_data;
	icm20689_get_temperature get_temp;
	icm20689_active is_active;
	t_driver_status status;
}t_icm20689;

t_driver_status icm_20689_init(t_icm20689* icm_module);

#endif /* SRC_DEVICE_DRIVERS_ICM_20689_H_ */

