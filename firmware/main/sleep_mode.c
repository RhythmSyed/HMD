#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"
#define BUFFER_LENGTH 10


void SleepMode_task(void *pvParameters) {

    // Heart Rate timer parameters
    int down_count = 0;
    int up_count = 0;
    uint32_t expiredCountBuffer[BUFFER_LENGTH];
    uint8_t bufferWriteIndex = 0;
    TimerHandle_t bpm_timer = heartRate_timer_init();
    uint32_t BPM = 0;

    while(1) {

        BPM = heartRate_collect_data(&down_count, &up_count, expiredCountBuffer, &bufferWriteIndex, &bpm_timer);
       
        if (BPM != -1) {
            send_BLE(&BPM, 'H');                    // H for heart rate
            Epaper_display((int) BPM, 0);           // 0 for sleep mode
        }
        

    }

}