#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "features.h"

#include "epaper-29-ws.h"
#include "epaper_fonts.h"
#include <math.h>

void ActivityMode_task(void *pvParameters) {

    int upper_threshold = 30;
    int step_count = 1;
    uint32_t gyro_mag = 0;

    struct motionTracker MPU_data = {
        .accel_x = 0,
        .accel_y = 0,
        .accel_z = 0,
        .gyro_x = 0,
        .gyro_y = 0,
        .gyro_z = 0
    };

    while(1) {
        // get MPU data
        MPU_collect_data(&MPU_data);
        
        // calculate step
        //accel_mag = sqrt(MPU_data.accel_x*MPU_data.accel_x + MPU_data.accel_y*MPU_data.accel_y + MPU_data.accel_z*MPU_data.accel_z);
	    gyro_mag = sqrt(MPU_data.gyro_x*MPU_data.gyro_x + MPU_data.gyro_y*MPU_data.gyro_y + MPU_data.gyro_z*MPU_data.gyro_z);

        if (gyro_mag > upper_threshold) {
            send_BLE(&gyro_mag, 'G');                       // G for gyro
            Epaper_display(step_count, ACTIVITY_MODE);                  // 1 for activity
            
            step_count += 1;
        }


        vTaskDelay(100/portTICK_PERIOD_MS);
    }

}

