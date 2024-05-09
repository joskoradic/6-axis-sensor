/*
 * icm_20689.c
 *
 *  Created on: Apr 29, 2022
 *      Author: josko Radic
 */

#ifndef SRC_DEVICE_DRIVERS_ICM_20689_C_
#define SRC_DEVICE_DRIVERS_ICM_20689_C_

#include "icm_20689.h"
#include "stm32l0xx_hal.h"
#include "../handlers.h"
#include "../io.h"


// TEMP
const uint8_t    TEMP_OUT_H        = 0x41;
const uint8_t    TEMP_OUT_L        = 0x42;

// ACCEL
const uint8_t    ACCEL_XOUT_H         = 0x3B;
const uint8_t    ACCEL_XOUT_L         = 0x3C;
const uint8_t    ACCEL_YOUT_H         = 0x3D;
const uint8_t    ACCEL_YOUT_L         = 0x3E;
const uint8_t    ACCEL_ZOUT_H         = 0x3F;
const uint8_t    ACCEL_ZOUT_L         = 0x40;


// GYRO
const uint8_t    GYRO_XOUT_H          = 0x43;
const uint8_t    GYRO_XOUT_L          = 0x44;
const uint8_t    GYRO_YOUT_H          = 0x45;
const uint8_t    GYRO_YOUT_L          = 0x46;
const uint8_t    GYRO_ZOUT_H          = 0x47;
const uint8_t    GYRO_ZOUT_L          = 0x48;


const uint8_t PWR_MGMT_2              = 0x6C;

static const float TEMPERATURE_SENSITIVITY = 326.8f; // LSB/C
static const float TEMPERATURE_OFFSET = 20.f; // C


const uint8_t WHO_AM_I_ADDRES               = 0x75;
const uint8_t WHO_AM_VALUE                  = 0x98;



static HAL_StatusTypeDef get_axis(void* interface, int16_t* data, const uint8_t reg ) ;
static t_driver_status get_accelerometar_data(t_accel_data* data);
static t_driver_status get_gyroscope_data(t_gyro_data* data);
static t_driver_status get_temperature(float* temp);
static t_driver_status is_active(void* interface);

static void* g_interface = NULL;


static HAL_StatusTypeDef get_axis(void* interface, int16_t* data, const uint8_t reg ) {
	HAL_StatusTypeDef result = HAL_ERROR;
	uint8_t buffer[2] = {0};

	result = spi_read_reg((SPI_HandleTypeDef*)interface, reg, &buffer[0]); //MSB
	if(result == HAL_OK) {
		result = spi_read_reg(interface, reg+1, &buffer[1]); //LSB
		if(result != HAL_OK) {
			return result;
		}
	}
	else {
		return result;
	}

	*data = buffer[0];
	*data <<= 8;
    *data |= buffer[1];

    return result;
}




static t_driver_status get_accelerometar_data(t_accel_data* data) {
	if(get_axis(g_interface, &data->accel_x,ACCEL_XOUT_H) != HAL_OK){
		return STATUS_ERROR;
	}
	if(get_axis(g_interface, &data->accel_y,ACCEL_YOUT_H) != HAL_OK){
		return STATUS_ERROR;
	}
	if(get_axis(g_interface, &data->accel_z,ACCEL_ZOUT_H) != HAL_OK){
		return STATUS_ERROR;
	}
	return STATUS_OK;
}




static t_driver_status get_gyroscope_data(t_gyro_data* data) {
	if(get_axis(g_interface, &data->gyro_x,GYRO_XOUT_H) != HAL_OK){
		return STATUS_ERROR;
	}
	if(get_axis(g_interface, &data->gyro_y,GYRO_YOUT_H) != HAL_OK){
		return STATUS_ERROR;
	}
	if(get_axis(g_interface, &data->gyro_z,GYRO_ZOUT_H) != HAL_OK){
		return STATUS_ERROR;
	}
	return STATUS_OK;
}

static t_driver_status get_temperature(float* temp) {
	t_driver_status status =  STATUS_ERROR;
	int16_t data = INT16_MAX;
	float temperature = 0.00;
	if((get_axis(g_interface,&data, TEMP_OUT_H) == HAL_OK) && (data != INT16_MAX)) {
		temperature = (float)data;
		temperature = (temperature / TEMPERATURE_SENSITIVITY) + TEMPERATURE_OFFSET;
		//temperature = (temperature - 25.00/) + 25.00;
		*temp = temperature;
		status = STATUS_OK;
	}
	else {
		status = STATUS_INVALID;
	}
	return status;
}

static t_driver_status is_active(void* interface) {

	HAL_StatusTypeDef result = HAL_ERROR;
	t_driver_status status =  STATUS_ERROR;
	uint8_t data = 0;

	result = spi_read_reg((SPI_HandleTypeDef*)interface,WHO_AM_I_ADDRES,&data);

	if(result == HAL_OK) {
		if(data == WHO_AM_VALUE) {
			status = STATUS_OK;
		}
		else {
			status = STATUS_ERROR;
		}
	}
	else {
		status = STATUS_ERROR;
	}

	return status;
}


t_driver_status icm_20689_config() {
	uint8_t status = 0;
	  spi_write_reg(get_interface_handle(INTERFACE_SPI1), 0x6b, 0x01 ); // PWR_MGMT_1
	  spi_read_reg(get_interface_handle(INTERFACE_SPI1), 0x6b, &status ); // PWR_MGMT_1
	  spi_read_reg(get_interface_handle(INTERFACE_SPI1), 0x6c, &status); // PWR_MGMT_2
      return STATUS_INVALID;
}


t_driver_status icm_20689_init(t_icm20689* icm_module) {
	t_driver_status status = STATUS_ERROR;

	icm_module->interface.interface_type = INTERFACE_SPI1;
	g_interface = get_interface_handle(INTERFACE_SPI1);
	if(g_interface != NULL) {

		icm_module->get_accel_data = get_accelerometar_data;
		icm_module->get_gyro_data= get_gyroscope_data;
		icm_module->get_temp = get_temperature;
		icm_module->is_active = is_active;

		if(is_active(g_interface) == STATUS_OK) {
			icm_module->status = STATUS_INITIALIZED;
			status = icm_module->status;
			icm_20689_config();
		}
		else {
			icm_module->status = STATUS_ERROR;
			status = icm_module->status;
		}
	}
	else {
		icm_module->status = STATUS_INTERFACE_ERR;
		status = icm_module->status;
	}
	return status;
}




#endif /* SRC_DEVICE_DRIVERS_ICM_20689_C_ */

