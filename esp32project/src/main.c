#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h" // The new component header
#include "esp_err.h"

#define BLINK_GPIO 38

void rgb_blink_task(void *pvParameter) {
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
            led_strip_set_pixel(led_strip, 0, 0, 0, 50);
            
            // Push the color data to the actual LED
            led_strip_refresh(led_strip);
        } else {
            // Turn the LED off
            led_strip_clear(led_strip);
        }

        led_state = !led_state;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    xTaskCreate(
        rgb_blink_task, 
        "rgb_blink_task", 
        4096, // Increased stack size slightly for the LED driver overhead
        NULL, 
        5, 
        NULL
    );
}