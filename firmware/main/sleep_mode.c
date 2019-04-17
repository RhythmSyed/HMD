#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"
#include "driver/gpio.h"
#define BUFFER_LENGTH 10

#include <math.h>
#include <string.h>


void SleepMode_task(void *pvParameters) {
    float bpm_float;

    // Accelerometer parameters
    float upper_threshold = 0.15;
    int step_count = 1;
    float accel_mag = 0;

    display_data.step_count = 0;

    int numReadings = 10;
    float readings[numReadings];
    int readIndex = 0;
    float total = 0;
    float average_accel = 0;

    int data_toggle = 0;

    while(1) {
        if (display_data.current_mode == SLEEP_MODE) {

            // moving average algorithm
            total = total - readings[readIndex];
            accel_mag = sqrt(display_data.imu_data.ax*display_data.imu_data.ax + display_data.imu_data.ay*display_data.imu_data.ay + display_data.imu_data.az*display_data.imu_data.az);
            readings[readIndex] = accel_mag;
            total = total + readings[readIndex];
            readIndex = readIndex + 1;

            if (readIndex >= numReadings) {
                readIndex = 0;
            }

            average_accel = total / numReadings;


            if (data_toggle == 0) {
                bpm_float = (float) display_data.hr_bpm_data;
                send_BLE(&bpm_float, 'H');                    // H for heart rate
                data_toggle = 1;
            } else if (data_toggle == 1) {
               send_BLE(&average_accel, 'A');                // A for accel
               data_toggle = 0;
            }

        }
        vTaskDelay(100/portTICK_PERIOD_MS);

    }

}