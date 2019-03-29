// #include "lis3dh.h"

// // typedef struct {

// //     float ax;   // acceleration on x axis
// //     float ay;   // acceleration on y axis
// //     float az;   // acceleration on z axis

// // } lis3dh_float_data_t;

// // typedef lis3dh_float_data_t lis3dh_float_data_fifo_t[32];


// /** -- platform dependent definitions ------------------------------ */

// // user task stack depth for ESP32
// #define TASK_STACK_DEPTH 2048

// // I2C interface defintions for ESP32 and ESP8266
// #define I2C_BUS       0
// #define I2C_SCL_PIN   17
// #define I2C_SDA_PIN   16
// #define I2C_FREQ      I2C_FREQ_100K


// /* -- user tasks --------------------------------------------------- */

// static lis3dh_sensor_t* sensor;

// /**
//  * Common function used to get sensor data.
//  */
// void read_data ()
// {
//     // #ifdef FIFO_MODE

//     // lis3dh_float_data_fifo_t fifo;

//     // if (lis3dh_new_data (sensor))
//     // {
//     //     uint8_t num = lis3dh_get_float_data_fifo (sensor, fifo);

//     //     printf("%.3f LIS3DH num=%d\n", (double)sdk_system_get_time()*1e-3, num);

//     //     for (int i=0; i < num; i++)
//     //         // max. full scale is +-16 g and best resolution is 1 mg, i.e. 5 digits
//     //         printf("%.3f LIS3DH (xyz)[g] ax=%+7.3f ay=%+7.3f az=%+7.3f\n",
//     //                (double)sdk_system_get_time()*1e-3, 
//     //                fifo[i].ax, fifo[i].ay, fifo[i].az);
//     // }

//     // #else

//     lis3dh_float_data_t  data;

//     if (lis3dh_new_data (sensor) &&
//         lis3dh_get_float_data (sensor, &data))
//         // max. full scale is +-16 g and best resolution is 1 mg, i.e. 5 digits
//         printf("%.3f LIS3DH (xyz)[g] ax=%+7.3f ay=%+7.3f az=%+7.3f\n",
//                (double)sdk_system_get_time()*1e-3, 
//                 data.ax, data.ay, data.az);
        
//     //#endif // FIFO_MODE
// }


// /*
//  * In this example, user task fetches the sensor values every seconds.
//  */

// void user_task_periodic(void *pvParameters)
// {
//     vTaskDelay (100/portTICK_PERIOD_MS);
    
//     while (1)
//     {
//         // read sensor data
//         read_data ();
        
//         // passive waiting until 1 second is over
//         vTaskDelay(100/portTICK_PERIOD_MS);
//     }
// }

// /* -- main program ------------------------------------------------- */

// void user_init(void)
// {
//     // Set UART Parameter.
//     uart_set_baud(0, 115200);
//     // Give the UART some time to settle
//     vTaskDelay(1);

//     /** -- MANDATORY PART -- */

//     // init all I2C bus interfaces at which LIS3DH  sensors are connected
//     i2c_init (I2C_BUS, I2C_SCL_PIN, I2C_SDA_PIN, I2C_FREQ);
    
//     // init the sensor with slave address LIS3DH_I2C_ADDRESS_1 connected to I2C_BUS.
//     sensor = lis3dh_init_sensor (I2C_BUS, LIS3DH_I2C_ADDRESS_1, 0);
    
//     if (sensor)
//     {
        
//         /** -- SENSOR CONFIGURATION PART --- */

//         // #ifdef FIFO_MODE
//         // // clear FIFO and activate FIFO mode if needed
//         // lis3dh_set_fifo_mode (sensor, lis3dh_bypass,  0, lis3dh_int1_signal);
//         // lis3dh_set_fifo_mode (sensor, lis3dh_stream, 10, lis3dh_int1_signal);
//         // #endif

//         // configure HPF and reset the reference by dummy read
//         lis3dh_config_hpf (sensor, lis3dh_hpf_normal, 0, true, true, true, true);
//         lis3dh_get_hpf_ref (sensor);
        
//         // LAST STEP: Finally set scale and mode to start measurements
//         lis3dh_set_scale(sensor, lis3dh_scale_2_g);
//         lis3dh_set_mode (sensor, lis3dh_odr_10, lis3dh_high_res, true, true, true);

//         /** -- TASK CREATION PART --- */

//         // must be done last to avoid concurrency situations with the sensor
//         // configuration part

//         // create a user task that fetches data from sensor periodically
//         xTaskCreate(user_task_periodic, "user_task_periodic", TASK_STACK_DEPTH, NULL, 2, NULL);
//     }
//     else
//         printf("Could not initialize LIS3DH sensor\n");
// }

