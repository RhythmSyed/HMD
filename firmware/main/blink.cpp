#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "features.h"
#include "freertos/timers.h"

void blink_task(void *pvParameter)
{

    gpio_pad_select_gpio(GPIO_NUM_5);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);

    while(1) {
        gpio_set_level(GPIO_NUM_5, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_5, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}