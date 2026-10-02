#include <stdio.h>
#include <stdlib.h> // Required for atoi()
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "led_strip.h" // The new component header
#include "esp_err.h"

#define BLINK_GPIO 38

QueueHandle_t serial_queue;
uint32_t delay_ms;

void hello_task(void *pvParameter) {
    while (1) {
        printf("Hello world!\n");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void serial_task(void *pvParameter) {
    char rx_buffer[16];
    uint8_t rx_index = 0;

    while (1) {
        // 1. Read exactly one character from the UART buffer
        int c = fgetc(stdin); 

        // 2. Check if a valid character actually arrived
        if (c != EOF) {
            
            // 3. Did the user press Enter? (Checks for both \n and \r)
            if (c == '\n' || c == '\r') {
                if (rx_index > 0) { // Only proceed if we actually typed something
                    
                    rx_buffer[rx_index] = '\0'; // Null-terminate the string
                    printf("\n"); // Drop the terminal cursor to the next line
                    
                    uint32_t parsed_value = (uint32_t)atoi(rx_buffer);
                    
                    if (parsed_value > 0) {
                        printf("Parsed new delay: %lu ms\n", parsed_value);
                        xQueueSend(serial_queue, &parsed_value, portMAX_DELAY);
                    } else {
                        printf("Invalid input.\n");
                    }
                    
                    rx_index = 0; // Reset the buffer index for the next entry
                }
            } 
            // 4. If it's a normal character, store it
            else if (rx_index < sizeof(rx_buffer) - 1) {
                rx_buffer[rx_index] = (char)c;
                rx_index++;
                
                // ECHO: Print the character back to the terminal so you can see it
                putchar(c);
                fflush(stdout); // Force the terminal to draw the character immediately
            }
        }
        
        // 5. Yield slightly to prevent Watchdog Timer crashes if fgetc unblocks
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void rgb_blink_task(void *pvParameter) {

    uint32_t current_delay = 1000; // Default 1 second blink
    uint32_t new_delay = 0;

    led_strip_handle_t led_strip;

    // 1. Configure the LED strip (v3.x API)
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1,
        // The v3 API renamed this property and enum
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .led_model = LED_MODEL_WS2812, 
        };

    // 2. Configure the RMT backend (generates the hardware signal)
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz resolution
        .flags.with_dma = false,
    };
    
    // 3. Initialize the device
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    // ADD THIS: Give the NeoPixel 10ms to wake up
    vTaskDelay(pdMS_TO_TICKS(10)); 

    led_strip_clear(led_strip);
    
    bool led_state = false;

    while (1) {
        if (led_state) {
            // Set pixel 0 to Blue (Red: 0, Green: 0, Blue: 50)
            // Brightness is 0-255. 50 is comfortably bright without blinding you.
            led_strip_set_pixel(led_strip, 0, 0, 0, 25);
            
            // Push the color data to the actual LED
            led_strip_refresh(led_strip);
        } else {
            // Turn the LED off
            led_strip_clear(led_strip);
        }

        led_state = !led_state;
        if (xQueueReceive(serial_queue, &new_delay, pdMS_TO_TICKS(current_delay))==pdTRUE) {
            current_delay = new_delay;
        }
    }
}

void app_main(void) {
    serial_queue = xQueueCreate(1, sizeof(uint32_t)); // Create a queue to hold 10 integers
    delay_ms = 1000; // Set the initial delay to 10 seconds

    xTaskCreate(
        rgb_blink_task, 
        "rgb_blink_task", 
        4096, // Increased stack size slightly for the LED driver overhead
        NULL, 
        5, 
        NULL
    );
    xTaskCreate(
        hello_task, 
        "hello_task", 
        2048, 
        NULL, 
        5, 
        NULL
    );
    xTaskCreate(serial_task,
        "serial_task",
        2048,
        NULL,
        5,
        NULL
    );
}