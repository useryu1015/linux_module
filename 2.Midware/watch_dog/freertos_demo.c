#include "FreeRTOS.h"
#include "task.h"

// 定义任务1
void vTask1(void *pvParameters) {
    for (;;) {
        // 任务1的处理逻辑
    }
}

// 定义任务2
void vTask2(void *pvParameters) {
    for (;;) {
        // 任务2的处理逻辑
    }
}

int main(void) {
    // 创建任务1
    xTaskCreate(vTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    
    // 创建任务2
    xTaskCreate(vTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    
    // 启动调度器（将cpu控制权交给rtos处理）
    vTaskStartScheduler();
    
    // 不应该运行到这里
    for (;;);
    
    return 0;
}
