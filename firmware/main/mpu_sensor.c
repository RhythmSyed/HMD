/* 
	Example Attribution: https://github.com/nkolban/esp32-snippets MPU6050 Example Usage
 */

#include <freertos/FreeRTOS.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <math.h>

#include "sdkconfig.h"
#include "features.h"

#define PIN_SDA 16
#define PIN_CLK 17
#define I2C_ADDRESS 0x68 // I2C address of MPU6050

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_PWR_MGMT_1   0x6B


/*
 * The following registers contain the primary data we are interested in
 * 0x3B MPU6050_ACCEL_XOUT_H
 * 0x3C MPU6050_ACCEL_XOUT_L
 * 0x3D MPU6050_ACCEL_YOUT_H
 * 0x3E MPU6050_ACCEL_YOUT_L
 * 0x3F MPU6050_ACCEL_ZOUT_H
 * 0x50 MPU6050_ACCEL_ZOUT_L
 * 0x41 MPU6050_TEMP_OUT_H
 * 0x42 MPU6050_TEMP_OUT_L
 * 0x43 MPU6050_GYRO_XOUT_H
 * 0x44 MPU6050_GYRO_XOUT_L
 * 0x45 MPU6050_GYRO_YOUT_H
 * 0x46 MPU6050_GYRO_YOUT_L
 * 0x47 MPU6050_GYRO_ZOUT_H
 * 0x48 MPU6050_GYRO_ZOUT_L
 */

static char tag[] = "mpu6050";

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

void MPU_init() {
	ESP_LOGD(tag, ">> mpu6050");
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = PIN_SDA;
	conf.scl_io_num = PIN_CLK;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

	i2c_cmd_handle_t cmd;

	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
	i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
	i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, 1);
	i2c_master_write_byte(cmd, 0, 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
}


void MPU_collect_data(struct motionTracker *MPU_data) {
	i2c_cmd_handle_t cmd;

	uint8_t data[14];

	short accel_x_raw;
	short accel_y_raw;
	short accel_z_raw;
    short gyro_x_raw;
	short gyro_y_raw;
	short gyro_z_raw;

    double accel_x;
    double accel_y;
    double accel_z;
    double gyro_x;
    double gyro_y;
    double gyro_z;

	// double accel_mag;
	// double gyro_mag;


	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1));
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
	i2c_cmd_link_delete(cmd);

	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, 1));

	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data,   0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+1, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+2, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+3, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+4, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+5, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+6, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+7, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+8, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+9, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+10, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+11, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+12, 0));
	ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data+13, 1));

	//i2c_master_read(cmd, data, sizeof(data), 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
	i2c_cmd_link_delete(cmd);

	accel_x_raw = (data[0] << 8) | data[1];
	accel_y_raw = (data[2] << 8) | data[3];
	accel_z_raw = (data[4] << 8) | data[5];
	accel_x = accel_x_raw / 16384.0;
	accel_y = accel_y_raw / 16384.0;
	accel_z = accel_z_raw / 16384.0;

	gyro_x_raw = (data[8] << 8) | data[9];
	gyro_y_raw = (data[10] << 8) | data[11];
	gyro_z_raw = (data[12] << 8) | data[13];
	gyro_x = gyro_x_raw / 131.0;
	gyro_y = gyro_y_raw / 131.0;
	gyro_z = gyro_z_raw / 131.0;
	
	// printf("accel: %lf %lf %lf \t", accel_x, accel_y, accel_z);
	// printf("gyro:  %lf %lf %lf \n", gyro_x, gyro_y, gyro_z);

	MPU_data->accel_x = accel_x;
	MPU_data->accel_y = accel_y;
	MPU_data->accel_z = accel_z;
	MPU_data->gyro_x = gyro_x;
	MPU_data->gyro_y = gyro_y;
	MPU_data->gyro_z = gyro_z;

	// accel_mag = sqrt(accel_x*accel_x + accel_y*accel_y + accel_z*accel_z);
	// gyro_mag = sqrt(gyro_x*gyro_x + gyro_y*gyro_y + gyro_z*gyro_z);
	
	return;
	
}