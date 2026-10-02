# lecture 5 Queuing

QueueHandle_t queue; - declaring queue

This handle is typically declared as a global variable, but in this example queue defined locally in the task.

### QueueHandle_t sensor_data_queue = (QueueHandle_t)pvParameters;
    sensor_data_queue - is the local variable with a type of QueueHandle_t, which is "stored in pvParameters memory address, with a type of (QueueHandle_t)"

### QueueHandle_t sensor_data_queue = xQueueCreate(10, sizeof(uint8_t));
    when crated with xQueueCreate ther required RAM is automatically allocated from FreeRTOS heap.
    10 - maximum number of items the queue can hold at any one time.
    sizeof(uint8_t) - the size, in bytes required to hold each item in the queue.

    Items are queued by copy, not by reference, so this is the number of bytes that will be copied for each queued item. Each item in the queue must be the same size.

### xQueueSend(sensor_data_queue, &sensor_data, portMAX_DELAY);
    sensor_data_queue the handle to the queue on which the item is to be posted.
    &sensor_data - a pointer to the item that is to be placed on the queue
    portMAX_DELAY - specifies how long to wait for the queue to have space available to add the data.  Passing portMAX_DELAY tells the task to wait indefinitely.
         If a task cannot wait indefinitely for space in the queue, the third parameter of xQueueSend() can be set to a specific timeout value. If space becomes available within that timeout period, the data is successfully added to the queue and the function returns pdPASS. If the queue remains full for the entire duration, the function returns pdFAIL, allowing the task to handle the failure appropriately—such as retrying, logging an error, or taking alternate action.
### xQueueReceive(sensor_data_queue, &received_data, portMAX_DELAY)
    sensor_data_queue - is the handle of the queue from which data will be received.
    &received_data - is the memory location where the received dtaa will be stored. 
    The third parameter specifies how long the task should wait for data to become available. Passing portMAX_DELAY causes the task to block indefinitely until data is received 

### xTaskCreate
    (void *)sensor_data_queue - stripping data type.

### Expected output
    Task A (sensor_emulation_task) generates random number and sends it to queue.
    If Task B (screen_emulator_task) recieves new sensor data, it prints in serialmonitor. Expected output:

    Received sensor data: 207
    Received sensor data: 70
    Received sensor data: 41
    Received sensor data: 4
    Received sensor data: 180


### Source:
    https://ece353.engr.wisc.edu/freertos/queues/