#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"

// This is not working. May be a timer issue?


void SleepMode_task(void *pvParameters) {

    // Heart Rate timer parameters
    int down_count = 0;
    int up_count = 0;
    int hrt_bt_adc_val = 0;
    uint32_t expiredCount = 0;
    uint8_t bufferWriteIndex = 0;
    uint32_t bpm = 0;
    TimerHandle_t bpm_timer;

    bpm_timer = heartRate_timer_init();

    while(1) {
        bpm = heartRate_collect_data(down_count, up_count, hrt_bt_adc_val, expiredCount, bufferWriteIndex, bpm_timer);
        Epaper_display((int) bpm);

    }

}