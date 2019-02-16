#include "freertos/timers.h"

void get_BPM(void *pvParameter);
void BPMTimerCallback( TimerHandle_t xTimer );

void bleAdvtTask(void *pvParameters);
void e_paper_task(void *pvParameter);