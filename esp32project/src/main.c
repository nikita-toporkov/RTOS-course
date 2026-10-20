#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "mpu6050.h"

#define TEST_I2C_PORT I2C_NUM_0 //I2C_NUM_1 alternatively

// blue GPIO 8 SDA
// yellow GPIO 9 SCL

i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,  // Use default clock source
    .i2c_port = TEST_I2C_PORT,          // I2C port number, -1 for auto selecting
    .scl_io_num = 9,                    // GPIO number of I2C SCL signal, pulled-up internally
    .sda_io_num = 8,                    // GPIO number of I2C SDA signal, pulled-up internally
    .glitch_ignore_cnt = 7,             // If the glitch period on the line is less than this value, it can be filtered out, typically value is 7 (unit: I2C module clock cycle) 
    .flags.enable_internal_pullup = true, // Enable internal pullups. Note: This is not strong enough to pullup buses under high-speed frequency. Recommend proper external pull-up if possible
};

i2c_master_bus_handle_t bus_handle; // Handle for the I2C master bus

i2c_device_config_t dev_config = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x68, // I2C device address
    .scl_speed_hz = 100000, // I2C clock speed in Hz
};

i2c_master_dev_handle_t dev_handle;


void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 1 second
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

    
    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    printf("Probing I2C device at address 0x68...\n");

    esp_err_t ret = i2c_master_probe(bus_handle, 0x68, 100000);
    
    if (ret == ESP_OK) {
        printf("Device found at 0x%02X\n", 0x68);
    } else {
        printf("No device at 0x%02X\n", 0x68);
    }

    mpu6050_dev_t mpu;
    ESP_ERROR_CHECK(mpu6050_init(mpu));

    float accel_x;
    float accel_y;
    float accel_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    ESP_ERROR_CHECK(
        mpu6050_get_acceleration(mpu, &accel_x, &accel_y, &accel_z)
    );

    ESP_ERROR_CHECK(
        mpu6050_get_rotation(mpu, &gyro_x, &gyro_y, &gyro_z)
    );

    printf(
    "ACC: X=%f Y=%f Z=%f\n",
        accel_x,
        accel_y,
        accel_z
    );

    printf(
        "GYRO: X=%f Y=%f Z=%f\n",
        gyro_x,
        gyro_y,
        gyro_z
    );  


}
    //i2c_master_bus_add_device(bus_handle, 0x3C, 100000);
