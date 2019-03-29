#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#define BLE_PROFILE_APP_ID 0

/*** E-Paper ***/
    // Image Array
    extern const unsigned char IMAGE_DATA[];
    extern const unsigned char GIMAGE_RUNNING[];
    extern const unsigned char GIMAGE_HMD[];
    extern const unsigned char GIMAGE_Z[];
    // Tasks
    void e_paper_task(void *pvParameter);
    // Functions    
    void Epaper_display();

    enum Mode_select {
        SLEEP_MODE = 0,
        ACTIVITY_MODE
    };
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
    // Functions
    void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
    void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void char1_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void char1_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void descr1_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void descr1_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void char2_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void char2_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void descr2_read_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void descr2_write_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    void send_BLE(uint32_t *data, char sensor);
/***********/


/*** Blink ***/
    // Tasks
    void blink_task(void *pvParameter);
/*************/


/*** IMU ***/
    // Global Objects
    struct motionTracker {
        double accel_x;
        double accel_y;
        double accel_z;
        double gyro_x;
        double gyro_y;
        double gyro_z;
    };
    // Tasks
    void IMU_task(void *pvParameter);
    // Functions
    void IMU_init();
    void MPU_collect_data(struct motionTracker *MPU_data);
/***********/


/*** Mode ***/
    // Tasks
    void ActivityMode_task(void *pvParameters);
    void SleepMode_task(void *pvParameters);
/************/

