#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"
#include "driver/gpio.h"
#define BUFFER_LENGTH 10

#include <math.h>
#include <string.h>


void SleepMode_task(void *pvParameters) {

    // Heart Rate timer parameters
    int down_count = 0;
    int up_count = 0;
    uint32_t expiredCountBuffer[BUFFER_LENGTH];
    uint8_t bufferWriteIndex = 0;
    TimerHandle_t bpm_timer = heartRate_timer_init();
    int BPM = 0;
    float bpm_float;
    gpio_pad_select_gpio(GPIO_NUM_13);
    gpio_set_direction(GPIO_NUM_13, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_13, 1);

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

    // Artificial Heart Rate
    int HR_min = 35;
    int HR_max = 84;
    int Artificial_HeartRate = (HR_min + HR_max) / 2;

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

            if (average_accel > 0.1) {
                if (Artificial_HeartRate > HR_max) {
                    Artificial_HeartRate = (HR_min + HR_max) / 2;
                } else if (Artificial_HeartRate < HR_min) {
                    Artificial_HeartRate = (HR_min + HR_max) / 2;
                }
                
                Artificial_HeartRate += 1;
            }

            //BPM = heartRate_collect_data(&down_count, &up_count, expiredCountBuffer, &bufferWriteIndex, &bpm_timer);
            //printf("BPM: %d\n", BPM);

            if (data_toggle == 0) {
                // if (BPM == -1) {
                //     bpm_float = -1 * (float) BPM;
                // }
                // display_data.hr_bpm_data = bpm_float;

                bpm_float = (float) Artificial_HeartRate;
                display_data.hr_bpm_data = bpm_float;
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