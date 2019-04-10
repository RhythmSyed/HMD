#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"

#include "epaper-29-ws.h"
#include "epaper_fonts.h"
#include <math.h>
#include <inttypes.h>

void ActivityMode_task(void *pvParameters) {

    float upper_threshold = 0.15;
    int step_count = 1;
    float accel_mag = 0;

    display_data.step_count = 0;

    int numReadings = 10;
    float readings[numReadings];
    int readIndex = 0;
    float total = 0;
    float average = 0;

    while(1) {
        if (display_data.current_mode == ACTIVITY_MODE) {

            // moving average algorithm
            total = total - readings[readIndex];
            accel_mag = sqrt(display_data.imu_data.ax*display_data.imu_data.ax + display_data.imu_data.ay*display_data.imu_data.ay + display_data.imu_data.az*display_data.imu_data.az);
            readings[readIndex] = accel_mag;
            total = total + readings[readIndex];
            readIndex = readIndex + 1;

            if (readIndex >= numReadings) {
                readIndex = 0;
            }

            average = total / numReadings;
            
            printf("average: %f\n", average);
            send_BLE(&average, 'A');

            if (average > upper_threshold) {
                printf("THRESHOLD MET!: %f\n", accel_mag);
                display_data.step_count += 1;
            }
        }
        vTaskDelay(100/portTICK_PERIOD_MS);
    }

}

