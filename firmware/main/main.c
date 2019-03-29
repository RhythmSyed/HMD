#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_main.h"
#include "sdkconfig.h"
#include "features.h"
#include "driver/gpio.h"
#define TAG "HMD_MAIN"



void BLE_init() {
    esp_err_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGI(TAG, "Bluetooth controller release classic bt memory failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed\n", __func__);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed\n", __func__);
        return;
    }
}


void app_main() {

    esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );

    // // initializations
    // BLE_init();
    //MPU_init();
    IMU_init();
    //heartRate_ADC_init();
    // // gpio_pad_select_gpio(GPIO_NUM_14);
    // // gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);

    // // msg received callbacks
    // esp_ble_gatts_register_callback(gatts_event_handler);
    // esp_ble_gap_register_callback(gap_event_handler);
    // esp_ble_gatts_app_register(BLE_PROFILE_APP_ID);

    // if (gpio_get_level(GPIO_NUM_14)) {
    //     xTaskCreate(&ActivityMode_task, "SleepMode_task", 4096, NULL, 5, NULL);
    //} else {
    //     xTaskCreate(&SleepMode_task, "SleepMode_task", 4096, NULL, 5, NULL);
    // }


    // Main tasks
    //xTaskCreate(&getBPM_task, "getBPM_task", 4096, NULL, 5, NULL);
    //xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    //xTaskCreate(&MPU_task, "MPU_task", 4096, NULL, 5, NULL);
    xTaskCreate(&IMU_task, "IMU_task", 4096, NULL, 2, NULL);
    /* Task used to test the epaper display*/
    //xTaskCreate(&e_paper_task, "test_epaper_task", 4 * 1024, NULL, 5, NULL);

    //xTaskCreate(&ActivityMode_task, "ActivityMode_task", 4096, NULL, 5, NULL);
    //xTaskCreate(&SleepMode_task, "SleepMode_task", 4096, NULL, 5, NULL);

}
