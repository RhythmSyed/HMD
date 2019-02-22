#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

/*** E-Paper ***/
    // Image Array
    extern const unsigned char IMAGE_DATA[];
    // Tasks
    void e_paper_task(void *pvParameter);
    // Functions    
    void Epaper_display();
/***************/


/*** Heart Rate Sensor ***/
    // Tasks
    void getBPM_task(void *pvParameter);
    // BPM Callback
    void BPMTimerCallback( TimerHandle_t xTimer );
    // Functions
    void heartRate_ADC_init();
    TimerHandle_t heartRate_timer_init();
    uint32_t heartRate_collect_data(int *down_count, int *up_count, uint32_t *expiredCountBuffer, uint8_t *bufferWriteIndex, TimerHandle_t *bpm_timer);
/*************************/


/*** BLE ***/
    // Tasks
    void bleAdvt_task(void *pvParameters);          // Advertisement Test
    void bleServer_task(void *pvParameters);        // GATT Server
/***********/


/*** Blink ***/
    // Tasks
    void blink_task(void *pvParameter);
/*************/


/*** MPU ***/
    // Tasks
    void MPU_task(void *pvParameter);
    // Functions
    void MPU_init();
    double MPU_collect_data();
/***********/


/*** Mode ***/
    // Tasks
    void ActivityMode_task(void *pvParameters);
    void SleepMode_task(void *pvParameters);
/************/

