#include "freertos/FreeRTOS.h"

// Heart Rate sensor
#include <driver/adc.h>
#include "freertos/timers.h"
#include "features.h"

// fixes watchdog timer issue
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#define BUFFER_LENGTH 10


void BPMTimerCallback( TimerHandle_t xTimer ){
    uint32_t expiredCount;
    /* The number of times this timer has expired is saved as the timer's ID. Obtain the count. */
    expiredCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
    /* Increment the count, and then test to see if the timer has expired maxExpiredCount yet. */
    expiredCount++;
    vTimerSetTimerID( xTimer, ( void * ) expiredCount );
}

void heartRate_ADC_init() {
    /* Initialize and start the heart rate adc */
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);
}


TimerHandle_t heartRate_timer_init() {
    /* Create timer to count for BPM*/
    TimerHandle_t bpm_timer;
    bpm_timer = xTimerCreate( "Timer", pdMS_TO_TICKS( 10 ), pdTRUE, ( void * ) 0, BPMTimerCallback );
    if( bpm_timer == NULL ){
        /* The timer was not created. */
    }
    else{
            /* Start the timer. No block time is specified. Even if one were specified, it would be ignored because the RTOS scheduler has not yet been started. */
            if( xTimerStart( bpm_timer, 0 ) != pdPASS ){
                /* The timer could not be set into the Active state. */
            }
    }

    return bpm_timer;
}


int heartRate_collect_data(int *down_count, int *up_count, uint32_t *expiredCountBuffer, uint8_t *bufferWriteIndex, TimerHandle_t *bpm_timer) {
    int bpm = 0;
    int hrt_bt_adc_val = 0;    
    int expiredCount = 0;
    
    /* fixes watchdog timer issue */
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;

    /* The value from the adc for Heart Beat sensor */
    hrt_bt_adc_val = adc1_get_raw(ADC1_CHANNEL_0);
    // printf("down_count: %d\n", *down_count);
    // printf("up_count: %d\n", *up_count);
    printf("hrt_bt_adc_val: %d\n", hrt_bt_adc_val);

    /* Thresholds to calculate the if there was a heart beat or not */
    if( hrt_bt_adc_val < 1683){
        *down_count = *down_count + 1;
    }
    if(*down_count > 10 && hrt_bt_adc_val > 1882){
        *up_count = *up_count + 1;
    }
    if( *up_count > 20 && hrt_bt_adc_val < 1753){
        
        *down_count = 0;
        *up_count = 0;
        
        /* The number of times this timer has expired is saved as the timer's ID. Obtain the count. */
        expiredCount = ( uint32_t ) pvTimerGetTimerID(*bpm_timer);
        /* Write new expiredCount to buffer */
        expiredCountBuffer[ *bufferWriteIndex ] = expiredCount;
        *bufferWriteIndex = *bufferWriteIndex + 1;
        if (*bufferWriteIndex == BUFFER_LENGTH){
            *bufferWriteIndex = 0;
        }

        /* Average expire count in buffer */
        for(uint8_t index = 0; index <BUFFER_LENGTH; index++){
            expiredCount += expiredCountBuffer[ index ];
        }
        expiredCount = expiredCount / BUFFER_LENGTH;
        
        /* Expires every 1/100 of a second. Calc BPM*/
        bpm =  (100 * 60 * 1) / expiredCount;

        //configPRINTF( ( "BEAT! ADC Val: %d  BPM: %d \r\n", hrt_bt_adc_val, bpm ) );
        //printf("BEAT! ADC Val: %d  BPM: %d \r\n", hrt_bt_adc_val, bpm);
        printf("BEAT! ADC Val: %d  BPM: %d\n", hrt_bt_adc_val, bpm);
        /* Reset timer */
        xTimerReset(*bpm_timer, 0);
        vTimerSetTimerID(*bpm_timer, 0);

        return bpm;
    }
    
    return -1;
}



void BPM_task(void *pvParameter) {
    /* Create timer to count for BPM*/
    TimerHandle_t bmp_timer;
    bmp_timer = xTimerCreate( "Timer", pdMS_TO_TICKS( 10 ), pdTRUE, ( void * ) 0, BPMTimerCallback );
    if( bmp_timer == NULL ){
        /* The timer was not created. */
    }
    else{
        /* Start the timer. No block time is specified. Even if one were specified, it would be ignored because the RTOS scheduler has not yet been started. */
        if( xTimerStart( bmp_timer, 0 ) != pdPASS ){
            /* The timer could not be set into the Active state. */
        }
    }

    /* Initialize and start the heart rate adc */
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);

    int down_count = 0;
    int up_count = 0;
    int hrt_bt_adc_val = 0;
    int expiredCount = 0;
    #define BUFFER_LENGTH 15
    #define FILTER_AMOUNT 15
    int rawLPFilter[ FILTER_AMOUNT ];
    int lpFilterWriteIndex = 0;
    int expiredCountBuffer[ BUFFER_LENGTH ];
    int bufferWriteIndex = 0;
    int bpm = 0;
    while (1) {

        
        /* fixes watchdog timer issue */
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;

        /* The value from the adc for Heart Beat sensor */
        hrt_bt_adc_val = adc1_get_raw(ADC1_CHANNEL_0);

        /* Average Raw values to software LP filter */
        rawLPFilter[ lpFilterWriteIndex++ ] = hrt_bt_adc_val;
        if (lpFilterWriteIndex == FILTER_AMOUNT){
            lpFilterWriteIndex = 0;
        }
        /* Average raw in buffer */
        for(uint8_t index = 0; index < FILTER_AMOUNT; index++){
            hrt_bt_adc_val += rawLPFilter[ index ];
        }
        hrt_bt_adc_val = hrt_bt_adc_val / FILTER_AMOUNT;

        /* Thresholds to calculate the if there was a heart beat or not */
        if( hrt_bt_adc_val < 1683){
            down_count++;
        }
        if(down_count > 2 && hrt_bt_adc_val > 1882){
            up_count++;
        }
        if( up_count > 4 && hrt_bt_adc_val < 1753){
            
            down_count = 0;
            up_count = 0;
            
            /* The number of times this timer has expired is saved as the timer's ID. Obtain the count. */
            expiredCount = ( uint32_t ) pvTimerGetTimerID(bmp_timer);
            /* Write new expiredCount to buffer */
            expiredCountBuffer[ bufferWriteIndex ] = expiredCount;
            bufferWriteIndex ++;
            if (bufferWriteIndex == BUFFER_LENGTH){
                bufferWriteIndex = 0;
            }
            /* Average expire count in buffer */
            for(uint8_t index = 0; index <BUFFER_LENGTH;index ++){
                expiredCount += expiredCountBuffer[ index ];
            }
            expiredCount = expiredCount / BUFFER_LENGTH;
            /* Expires every 1/100 of a second. Calc BPM*/
            bpm =  (100 * 60 * 1) / expiredCount;
            display_data.hr_bpm_data = bpm;

            //configPRINTF( ( "BEAT! ADC Val: %d  BPM: %d \r\n", hrt_bt_adc_val, bpm ) );
            //printf("BEAT! ADC Val: %d  BPM: %d \r\n", hrt_bt_adc_val, bpm);
            printf("BEAT! ADC Val: %d  BPM: %d\n", hrt_bt_adc_val, bpm);
            /* Reset timer */
            xTimerReset(bmp_timer, 0);
            vTimerSetTimerID( bmp_timer,0);
        }
        //need to find vefuse for chip
        //should hover around 1735
        //if heart beat goes up .5V, then goes up 525 points

        //wrist, low 1611, target thresh 1753
        //high 1913, target thresh 1812

    }
}


