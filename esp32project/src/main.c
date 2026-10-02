// Standard C library functionality used for console output and random data.
#include <stdio.h>

// FreeRTOS task and queue APIs used by the emulator tasks.
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// ESP-IDF error definitions.
#include "esp_err.h"


// Generates simulated sensor readings and places them in the shared queue.
void sensor_emulator_task(void *pvParameters) {
    QueueHandle_t sensor_data_queue = (QueueHandle_t)pvParameters;

    // Keep producing sensor readings for the lifetime of the application.
    while (1) {
        uint8_t sensor_data = rand() % 256; // Simulate sensor data (0-255)
        vTaskDelay(pdMS_TO_TICKS(1000)); // Simulate sensor data generation every second
        
        xQueueSend(sensor_data_queue, &sensor_data, portMAX_DELAY); // Send data to queue
    }
}

// Receives sensor readings from the queue and displays them on the console.
void screen_emulator_task(void *pvParameters) {
    QueueHandle_t sensor_data_queue = (QueueHandle_t)pvParameters;
    uint8_t received_data;

    // Wait for and process sensor readings for the lifetime of the application.
    while (1) {
        if (xQueueReceive(sensor_data_queue, &received_data, portMAX_DELAY)==pdTRUE) {
            printf("Received sensor data: %d\n", received_data);
        }
    }
}

// Creates the shared queue and starts the producer and consumer tasks.
void app_main(void) {
    // Store up to ten one-byte sensor readings between the two tasks.
    QueueHandle_t sensor_data_queue = xQueueCreate(10, sizeof(uint8_t));
    
    // Start the task that generates simulated sensor data.
    xTaskCreate(
        sensor_emulator_task,           // Task function
        "Sensor Emulator Task",         // Task name
        4096,                           // Stack size in bytes
        (void *)sensor_data_queue,      // Task parameters (queue handle)
        5,                              // Task priority    
        NULL                            // Task handle (not used)
    );

    // Start the task that reads and displays sensor data.
    xTaskCreate(
        screen_emulator_task, 
        "Screen Emulator Task", 
        4096, 
        (void *)sensor_data_queue, 
        5, 
        NULL
    );
}