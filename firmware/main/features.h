#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"


extern const unsigned char IMAGE_DATA[];


void getBPM_task(void *pvParameter);
void BPMTimerCallback( TimerHandle_t xTimer );

void bleAdvt_task(void *pvParameters);

void blink_task(void *pvParameter);

void MPU_task(void *pvParameter);
void e_paper_task(void *pvParameter);

void bleServer_task(void *pvParameters);
