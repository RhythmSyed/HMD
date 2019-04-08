#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"

#include "epaper-29-ws.h"
#include "epaper_fonts.h"
#include <math.h>
#include <inttypes.h>

void ActivityMode_task(void *pvParameters) {

    float upper_threshold = 0.5;
    int step_count = 1;
    float accel_mag = 0;

    display_data.step_count = 0;

    while(1) {
        if (display_data.current_mode == ACTIVITY_MODE) {
            // get IMU data
            //IMU_recordData(display_data);
            
            // calculate step
            accel_mag = sqrt(display_data.imu_data.ax*display_data.imu_data.ax + display_data.imu_data.ay*display_data.imu_data.ay + display_data.imu_data.az*display_data.imu_data.az);
            printf("accel_mag: %f\n", accel_mag);
            if (accel_mag > upper_threshold) {
                printf("THRESHOLD MET!: %f\n", accel_mag);
                send_BLE(&accel_mag, 'A');                       // A for accel
                display_data.step_count += 1;
            }
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }

}

