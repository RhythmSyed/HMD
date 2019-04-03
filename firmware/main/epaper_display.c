/* 2.9" Waveshare ePaper Driver Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "esp_log.h"

#include "features.h"
#include "epaper-29-ws.h"
#include "epaper_fonts.h"


#include "driver/gpio.h"
#include "driver/spi_master.h"


// Pin definition of the ePaper module
#define MOSI_PIN     5          // DIN
#define MISO_PIN    -1
#define SCK_PIN     18
#define BUSY_PIN    22
#define DC_PIN      21
#define RST_PIN     23
#define CS_PIN      19

// Color inverse. 1 or 0 = set or reset a bit if set a colored pixel
#define IF_INVERT_COLOR 1


static void epaper_gpio_init(epaper_conf_t * pin)
{
    gpio_pad_select_gpio(pin->reset_pin);
    gpio_set_direction(pin->reset_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin->reset_pin, pin->rst_active_level);
    gpio_pad_select_gpio(pin->dc_pin);
    gpio_set_direction(pin->dc_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin->dc_pin, 1);
    ets_delay_us(10000);
    gpio_set_level(pin->dc_pin, 0);
    gpio_pad_select_gpio(pin->busy_pin);
    gpio_set_direction(pin->busy_pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin->busy_pin, GPIO_PULLUP_ONLY);
}

static epaper_handle_t init_epaper_device(bool partial_update){


    epaper_handle_t device = NULL;

    epaper_conf_t epaper_conf = {
        .busy_pin = BUSY_PIN,
        .cs_pin = CS_PIN,
        .dc_pin = DC_PIN,
        .miso_pin = MISO_PIN,
        .mosi_pin = MOSI_PIN,
        .reset_pin = RST_PIN,
        .sck_pin = SCK_PIN,

        .rst_active_level = 0,
        .busy_active_level = 1,

        .dc_lev_data = 1,
        .dc_lev_cmd = 0,

        .clk_freq_hz = 20 * 1000 * 1000,
        .spi_host = HSPI_HOST,

        .width = EPD_WIDTH,
        .height = EPD_HEIGHT,
        .color_inv = 1,
        .partial_update = 1,
    };

    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,  // MISO not used, we are transferring to the slave only
        .mosi_io_num = epaper_conf.mosi_pin,
        .sclk_io_num = epaper_conf.sck_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // The maximum size sent below covers the case
        // when the whole frame buffer is transferred to the slave
        .max_transfer_sz = EPD_WIDTH * EPD_HEIGHT / 8,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = epaper_conf.clk_freq_hz,
        .mode = 0,  // SPI mode 0
        .spics_io_num = epaper_conf.cs_pin,
        // To Do: clarify what does it mean
        .queue_size = EPAPER_QUE_SIZE_DEFAULT,
        // We are sending only in one direction (to the ePaper slave)
        .flags = (SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE),
        //Specify pre-transfer callback to handle D/C line
        .pre_cb = iot_epaper_pre_transfer_callback,
    }; 

    spi_device_handle_t e_spi = (spi_device_handle_t) calloc(1, sizeof(spi_device_handle_t));
    ret = spi_bus_initialize(epaper_conf.spi_host, &buscfg, 1);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(epaper_conf.spi_host, &devcfg, &e_spi);
    assert(ret == ESP_OK);
    epaper_gpio_init(&epaper_conf);

    device = iot_epaper_create(e_spi, &epaper_conf); //create drive to come out of sleep mode
    return device;
}


void Epaper_display(int mode_data, int mode) {
    char data[25];
    memset(data, 0x00, sizeof(data));
    if (mode == SLEEP_MODE) {
        sprintf(data, "%4d BPM", (uint8_t) mode_data);
    } else if (mode == ACTIVITY_MODE) {
        sprintf(data, "%4d steps", (uint8_t) mode_data);
    }
    

    epaper_handle_t device = NULL;

    epaper_conf_t epaper_conf = {
        .busy_pin = BUSY_PIN,
        .cs_pin = CS_PIN,
        .dc_pin = DC_PIN,
        .miso_pin = MISO_PIN,
        .mosi_pin = MOSI_PIN,
        .reset_pin = RST_PIN,
        .sck_pin = SCK_PIN,

        .rst_active_level = 0,
        .busy_active_level = 1,

        .dc_lev_data = 1,
        .dc_lev_cmd = 0,

        .clk_freq_hz = 20 * 1000 * 1000,
        .spi_host = HSPI_HOST,

        .width = EPD_WIDTH,
        .height = EPD_HEIGHT,
        .color_inv = 1,
        .partial_update = 1,
    };

    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,  // MISO not used, we are transferring to the slave only
        .mosi_io_num = epaper_conf.mosi_pin,
        .sclk_io_num = epaper_conf.sck_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // The maximum size sent below covers the case
        // when the whole frame buffer is transferred to the slave
        .max_transfer_sz = EPD_WIDTH * EPD_HEIGHT / 8,
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = epaper_conf.clk_freq_hz,
        .mode = 0,  // SPI mode 0
        .spics_io_num = epaper_conf.cs_pin,
        // To Do: clarify what does it mean
        .queue_size = EPAPER_QUE_SIZE_DEFAULT,
        // We are sending only in one direction (to the ePaper slave)
        .flags = (SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE),
        //Specify pre-transfer callback to handle D/C line
        .pre_cb = iot_epaper_pre_transfer_callback,
    }; 

    spi_device_handle_t e_spi = (spi_device_handle_t) calloc(1, sizeof(spi_device_handle_t));
    ret = spi_bus_initialize(epaper_conf.spi_host, &buscfg, 1);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(epaper_conf.spi_host, &devcfg, &e_spi);
    assert(ret == ESP_OK);
    epaper_gpio_init(&epaper_conf);

    device = iot_epaper_create(e_spi, &epaper_conf); //create drive to come out of sleep mode
    iot_epaper_set_rotate(device, E_PAPER_ROTATE_270);
    //iot_epaper_display_frame(device, IMAGE_DATA); // display IMAGE_DATA
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
    iot_epaper_clean_paint(device, UNCOLORED); // set screen to white
    iot_epaper_draw_string(device, 0, 90, data, &epaper_font_24, COLORED);

    
    // iot_epaper_draw_string(device, 10, 10, "HMD DEMO ", &epaper_font_24, COLORED); //colored is black
    // iot_epaper_draw_rectangle(device, 5, 5, 195, 40, COLORED);
    iot_epaper_display_frame(device, NULL); // display internal frame buffer
    iot_epaper_delete(device, true);// delete drive to enter into sleep mode

}

static void epaper_draw_activity_mode(epaper_handle_t device, int * position, display_data_t * display_data){
    char steps[sizeof(display_data->imu_data)];

    iot_epaper_draw_string(device, 10, 10, "Activity", &epaper_font_24, COLORED);
    iot_epaper_draw_string(device, 10, 33, "Monitoring", &epaper_font_24, COLORED);
    iot_epaper_draw_rectangle(device, 5, 5, 195, 65, COLORED);

    sprintf(steps, "x=%+7.3f y=%+7.3f z=%+7.3f",
        display_data->imu_data.ax, display_data->imu_data.ay, display_data->imu_data.az);
    iot_epaper_draw_string(device, 10, 90, steps, &epaper_font_12, COLORED);

    iot_epaper_draw_circle(device, 168, 168, 27, COLORED);
    iot_epaper_draw_image(device, 147, 161, GIMAGE_HMD, 45, 16);
    switch((*position)++){
        case 0:
            iot_epaper_draw_image(device, 5, 145, GIMAGE_RUNNING, 50, 50);
            break;
        case 1:
            iot_epaper_draw_image(device, 30, 145, GIMAGE_RUNNING, 50, 50);
            break;
        case 2:
            iot_epaper_draw_image(device, 55, 145, GIMAGE_RUNNING, 50, 50);
            break;
        case 3:
            iot_epaper_draw_image(device, 80, 145, GIMAGE_RUNNING, 50, 50);
            (*position) = 0;
            break;
        default:
            printf("ERROR! \n");
    }

}

void epaper_draw_sleep_mode(epaper_handle_t device, int * position, display_data_t * display_data){
    iot_epaper_draw_string(device, 10, 10, "Sleep", &epaper_font_24, COLORED);
    iot_epaper_draw_string(device, 10, 33, "Monitoring", &epaper_font_24, COLORED);
    iot_epaper_draw_rectangle(device, 5, 5, 195, 65, COLORED);

    iot_epaper_draw_string(device, 10, 70, "BPM: XXX", &epaper_font_24, COLORED);
    iot_epaper_draw_string(device, 10, 90, "Sleep Quality:", &epaper_font_20, COLORED);
    iot_epaper_draw_string(device, 10, 115, "XXXXXXX", &epaper_font_24, COLORED);

    iot_epaper_draw_circle(device, 168, 168, 27, COLORED);
    iot_epaper_draw_image(device, 147, 161, GIMAGE_HMD, 45, 16);
    switch((*position)++){
        case 0:
            iot_epaper_draw_image(device, 5, 145, GIMAGE_Z, 37, 50);
            break;
        case 1:
            
            iot_epaper_draw_image(device, 30, 145, GIMAGE_Z, 37, 50);
            break;
        case 2:
            iot_epaper_draw_image(device, 55, 145, GIMAGE_Z, 37, 50);
            break;
        case 3:
            iot_epaper_draw_image(device, 80, 145, GIMAGE_Z, 37, 50);
            (*position) = 0;
            break;
        default:
            printf("ERROR! \n");
    }

}



void e_paper_task(void *pvParameter)
{  
    epaper_handle_t device = NULL;
    device = init_epaper_device(true);

    int update_count = 0;
    int position = 0;
    display_data_t * display_data = (display_data_t *) pvParameter;
    
    //initially clear screen at startup
    iot_epaper_clean_paint(device, COLORED);
    iot_epaper_display_frame(device, NULL);
    iot_epaper_clean_paint(device, UNCOLORED);
    iot_epaper_display_frame(device, NULL);
    while(1){
        iot_epaper_set_rotate(device, E_PAPER_ROTATE_270); //may need to change
        iot_epaper_clean_paint(device, UNCOLORED); // set screen to white
        // Clear screen every 20 updates
        update_count ++;
        if(update_count == 20){
            iot_epaper_display_frame(device, NULL);
            update_count=0;
        }
        epaper_draw_activity_mode(device, &position, display_data);
        //epaper_draw_sleep_mode(device, &position, display_data);

        iot_epaper_display_frame(device, NULL); // display internal frame buffer

        iot_epaper_sleep(device);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

