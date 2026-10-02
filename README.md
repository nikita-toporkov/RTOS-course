# Leture 4 Memory Basics

## Stack, Heap, Statick storage.

    During this leasson I combined both examples into one. 
    Q&A can be found below as well as demo's output. 

### Static storage 

    Static local variable preserves it's value even after they are out of their scope. Lifetime: Till the end of the program.

    difference between static and global variables, ex:

        int global_var = 0; - defined at the top of program
        is globally accessible
        and
        static in static_var = 0; - defined inside block
        is not visible outside module.

### Stack

    Each task maintains its own stack. If a task is created using xTaskCreate() then the memory used as the task's stack is allocated automatically from the FreeRTOS heap, and dimensioned by a parameter passed to the xTaskCreate() API function.
    Stack overflow is a very common cause of application instability.
    
        https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/09-Memory-management/02-Stack-usage-and-stack-overflow-checking#stack-overflow-detection---method-1

### Heap

    he Heap is a (normally) large section of memory that you can acquire buffers out of, and possibly put back when done. FreeRTOS will allocate any object created “dynamically” (not using the create-static version) out of memory in the Heap.

    pvPortMalloc()
    vPortFree()

### Memory leak 

    ...occurs when memory that is no longer used remains allocated.

### functions

    uxTaskGetStackHighWaterMark()

        returns the minimum amount of remaining stack space that was available to the task since the task started executing - that is the amount of stack that remained unused when the task stack was at its greatest (deepest) value. This is what is referred to as the stack 'high water mark'.

    xPortGetFreeHeapSize() 
        
        is a FreeRTOS function which returns the number of free bytes in the (data memory) heap. 

## Q&A
    
    Did the free heap change when you increased the task stack size?

        Free heap has changed accordingly since memory was allocated when task was created.

    What happened to the free heap?

        Its volume has decreasedas some memory was allocated to tasks A, B and C when xTaskCreatePinnedToCore was called.

    Why does a task need stack memory?

        Because CPU needs to store temporary data when task is running. 

    Explain: Why does creating a FreeRTOS task use RAM?

        When creating task developer explicitly tells freeRTOS how much data is allocated specifically to this task. It is the same as giving multiple students some space on a blackboard for them to take notes while they are solving exercise. They can erase some of their writings and put them back. But they only can use space you gave them. When the exercise is completed student goes back to his seat, and memory (desk) is no longer taken.

## DEMOS 

### task A and B stacksize 2048

heap decreased after creating taskA = 2156 bytes
heap decreased after creating taskB = 2156 bytes

    Free heap at start: 390927 bytes
    Creating task A 
    Task A is running
    Task A created successfully.
    Free heap after Task A: 388771 bytes
    Creating task B 
    Task B created successfully.
    Free heap after Task B: Task B is running
    386615 bytes

### task A changed stack size to 4096 

heap decreased after creating taskA = 4204 bytes
heap decreased after creating taskB = 2156 bytes

    Starting FreeRTOS Memory Demo
    Free heap at start: 390927 bytes
    Creating task A 
    Task A is running
    Task A created successfully.
    Free heap after Task A: 386723 bytes
    Creating task B 
    Task B created successfully.
    Free heap after Task B: Task B is running
    384567 bytes

### task A changed stack size to 8192 

heap decreased after creating taskA = 8300 bytes
heap decreased after creating taskB = 2156 bytes

    Starting FreeRTOS Memory Demo
    Free heap at start: 390927 bytes
    Creating task A 
    Task A is running
    Task A created successfully.
    Free heap after Task A: 382627 bytes
    Creating task B 
    Task B created successfully.
    Free heap after Task B: Task B is running
    380471 bytes

### task A, B and C

task A and C standart behaivour (each allocates 2156 bytes from heap).
task C stack is 2048 when taskCreated. Then it outputs HighWaterMark - minimum of unused stack.
Then task C allocates more memory (4096 bytes), because memory is not freed after, it causes Memory Leak and eventually chip runs out of memory.

    Starting FreeRTOS Memory Demo
    Free heap at start: 390927 bytes
    Creating task A 
    Task A is running
    Task A created successfully.
    Free heap after Task A: 388771 bytes
    Creating task B 
    Task B created successfully.
    Free heap after Task B: Task B is running
    386615 bytes
    Task A is running
    Creating task C 
    Task C created successfully.
    Free heap after Task C: 384459 bytes
    Task C is running
    Stack high water mark: 764
    Heap befiore malloc (bytes): 384459
    Memory allocated successfully
    Heap after malloc (bytes): 380359
    Heap after free (bytes): 380359
    I (3775) main_task: Returned from app_main()
    Task B is running
    Task A is running
    Task C is running
    Stack high water mark: 508
    Heap befiore malloc (bytes): 384563
    Memory allocated successfully
    Heap after malloc (bytes): 380463
    Heap after free (bytes): 380463
    Task B is running
    Task A is running
    Task C is running


    Heap befiore malloc (bytes): 15563
    Memory allocation failed (not enough heap space)
    Task C is running
    Stack high water mark: 332
    Heap befiore malloc (bytes): 15563
    Memory allocation failed (not enough heap space)
    Task C is running
    Stack high water mark: 332
    Heap befiore malloc (bytes): 15563
    Memory allocation failed (not enough heap space)
    Task C is running
    Stack high water mark: 332
    Heap befiore malloc (bytes): 15563
    Memory allocation failed (not enough heap space)


### uncomment line 72 //vPortFree(ptr);

Memory Leak issue is fixed by freeng allocated memory.

    Starting FreeRTOS Memory Demo
    Free heap at start: 390927 bytes
    Creating task A 
    Task A is running
    Task A created successfully.
    Free heap after Task A: 388771 bytes
    Creating task B 
    Task B created successfully.
    Free heap after Task B: Task B is running
    386615 bytes
    Task A is running
    Creating task C 
    Task C created successfully.
    Free heap after Task C: 384459 bytes
    Task C is running
    Stack high water mark: 764
    Heap befiore malloc (bytes): 384459
    Memory allocated successfully
    Heap after malloc (bytes): 380359
    Heap after free (bytes): 384459
    I (3775) main_task: Returned from app_main()

### reference

freeRTOS documentation 
https://github.com/ShawnHymel/introduction-to-rtos/blob/main/04-memory-allocation/esp32-freertos-04-demo-stack-overflow/esp32-freertos-04-demo-stack-overflow.ino