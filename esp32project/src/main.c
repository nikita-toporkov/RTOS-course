#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"

#define BLINK_GPIO 38


void sensor_emulator_task(void *pvParameters) {
    QueueHandle_t sensor_data_queue = (QueueHandle_t)pvParameters;
    while (1) {
        uint8_t sensor_data = rand() % 256; // Simulate sensor data (0-255)
        vTaskDelay(pdMS_TO_TICKS(1000)); // Simulate sensor data generation every second
        
        xQueueSend(sensor_data_queue, &sensor_data, portMAX_DELAY); // Send data to queue
    }
}

void screen_emulator_task(void *pvParameters) {
    QueueHandle_t sensor_data_queue = (QueueHandle_t)pvParameters;
    uint8_t received_data;

    while (1) {
        if (xQueueReceive(sensor_data_queue, &received_data, portMAX_DELAY)) {
            printf("Received sensor data: %d\n", received_data);
        }
    }
}

void app_main(void) {
    QueueHandle_t sensor_data_queue = xQueueCreate(10, sizeof(uint8_t));
    
    xTaskCreate(
        sensor_emulator_task, 
        "Sensor Emulator Task", 
        4096, // Increased stack size slightly for the LED driver overhead
        sensor_data_queue, 
        5, 
        NULL
    );

    xTaskCreate(
        screen_emulator_task, 
        "Screen Emulator Task", 
        4096, // Increased stack size slightly for the LED driver overhead
        sensor_data_queue, 
        5, 
        NULL
    );
}