#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"

#include "epaper-29-ws.h"
#include "epaper_fonts.h"


void ActivityMode_task(void *pvParameters) {

    int upper_threshold = 30;
    int step_count = 0;
    int gyro_mag = 0;

    while(1) {
        // get MPU data
        gyro_mag = MPU_collect_data();
        
        // calculate step
        if (gyro_mag > upper_threshold) {
            step_count += 1;
        }

        // display current step count
        Epaper_display(step_count, 1);
        
        //send to app
        // if switch to sleep, vtaskSuspend
    

        // BLE_send();
        // display_epaper();

        vTaskDelay(100/portTICK_PERIOD_MS);
    }

}