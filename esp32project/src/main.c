#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t taskAHandle = NULL;
TaskHandle_t taskBHandle = NULL;


void printFreeHeap(const char* message)
{
  printf(message);
  printf(": ");
  printf("%d", xPortGetFreeHeapSize());
  printf(" bytes");
  printf("\n");
}


void taskA(void *parameter)
{
  while (1)
  {
    printf("Task A is running\n");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}


void taskB(void *parameter)
{
  while (1)
  {
    printf("Task B is running\n");
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void TaskC(void *parameter)
{
  while (1)
  {
    int a = 1;
    int b[100];

    for (int i = 0; i < 100; i++)
    {
      b[i] = a + 1;
    }
    printf("Task C is running\n");
    printf("Stack high water mark: ");
    printf("%d", uxTaskGetStackHighWaterMark(NULL));
    printf("\n");

    printf("Heap befiore malloc (bytes): %d\n", xPortGetFreeHeapSize());
    int *ptr = (int *)pvPortMalloc(1024 * sizeof(int));

    if (ptr == NULL)
    {
      printf("Memory allocation failed (not enough heap space)\n");
      vPortFree(NULL);
    }
    else
    {
      printf("Memory allocated successfully\n");

      for (int i = 0; i < 1024; i++)
      {
        ptr[i] = 3;
      }

    printf("Heap after malloc (bytes): %d\n", xPortGetFreeHeapSize());
    vPortFree(ptr);
    printf("Heap after free (bytes): %d\n", xPortGetFreeHeapSize());

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
}

void app_main(void) {
    printf("Starting FreeRTOS Memory Demo\n");
    printFreeHeap("Free heap at start");

    printf("Creating task A \n");
    
    BaseType_t resultA = xTaskCreatePinnedToCore(
        taskA, 
        "TaskA", 
        8192, 
        NULL,
        1, 
        &taskAHandle, 
        0); // Create task A on core 0

    if (resultA == pdPASS)
    {
        printf("Task A created successfully.\n");
    }
    else
    {
        printf("Task A creation FAILED.\n");
    }

    printFreeHeap("Free heap after Task A");

    vTaskDelay(pdMS_TO_TICKS(1000));

    // Create Task B on core 1
    printf("Creating task B \n");
    BaseType_t resultB = xTaskCreatePinnedToCore(
        taskB, 
        "TaskB", 
        2048, 
        NULL,
        1, 
        &taskBHandle, 
        1); // Create task B on core 1

    if (resultB == pdPASS)
    {
        printf("Task B created successfully.\n");
    }
    else
    {
        printf("Task B creation FAILED.\n");
    }

    printFreeHeap("Free heap after Task B");

    vTaskDelay(pdMS_TO_TICKS(1000));

    // Create Task C on core 0
    printf("Creating task C \n");
    BaseType_t resultC = xTaskCreatePinnedToCore(
        TaskC, 
        "TaskC", 
        2048, 
        NULL,
        1, 
        NULL, 
        0); // Create task C on core 0

    if (resultC == pdPASS)
    {
        printf("Task C created successfully.\n");
    }
    else
    {
        printf("Task C creation FAILED.\n");
    }
    printFreeHeap("Free heap after Task C");

    vTaskDelay(pdMS_TO_TICKS(1000));

}