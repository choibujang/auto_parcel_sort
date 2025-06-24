#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>


#include "App/Event.h"
#include "App/ILogger.h"
#include "App/Parcel.h"
#include "App/ParcelSorter.h"
#include "Sorter.h"
#include "UartLogger.h"
#include "Conveyor.h"
#include "DebouncedSensor.h"

/*
+-----------------------------------------------------------------------------+
| Static Global Objects that are needed across the file                       |
+-----------------------------------------------------------------------------+
*/
osMessageQueueId_t eventQueueHandle;
osMessageQueueId_t parcelQueueHandle;
osThreadId_t hardwareUpdateTaskHandle;
osThreadId_t logicTaskHandle;

#define UART_RX_BUFFER_SIZE 64
uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];

static Conveyor conveyor(&htim2, TIM_CHANNEL_1, CONVEYOR_IN1_GPIO_Port, CONVEYOR_IN1_Pin, CONVEYOR_IN2_GPIO_Port, CONVEYOR_IN2_Pin);
static Sorter sorter(SORTER_IN1_GPIO_Port, SORTER_IN1_Pin, SORTER_IN2_GPIO_Port, SORTER_IN2_Pin, SORTER_IN3_GPIO_Port, SORTER_IN3_Pin, SORTER_IN4_GPIO_Port, SORTER_IN4_Pin);
static UartLogger logger(&huart2);

static DebouncedSensor sensor1(SENSOR1_GPIO_Port, SENSOR1_Pin, 30);
static DebouncedSensor sensor2(SENSOR2_GPIO_Port, SENSOR2_Pin, 30);
static DebouncedSensor dest_sensor_0(DEST0_GPIO_Port, DEST0_Pin, 30);
static DebouncedSensor dest_sensor_1(DEST1_GPIO_Port, DEST1_Pin, 30);
static DebouncedSensor dest_sensor_2(DEST2_GPIO_Port, DEST2_Pin, 30);

static ParcelSorter parcelSorter(conveyor, sorter, sensor1, sensor2, dest_sensor_0, dest_sensor_1, dest_sensor_2, logger);

/*
+-----------------------------------------------------------------------------+
| Application Entry Point                                                     |
+-----------------------------------------------------------------------------+
*/
void Application_main();

void HardwareUpdateTask(void *argument);
void LogicTask(void *argument);
void DWT_Init(void);

extern "C" void Application_main() {
    logger.log("Application_main: System starting...\n");
    DWT_Init();

    /* Create Queues */
    logger.log("Application_main: Creating queues...\n");
    const osMessageQueueAttr_t event_queue_attributes = { .name = "eventQueue" };
    eventQueueHandle = osMessageQueueNew(32, sizeof(Event), &event_queue_attributes);

    const osMessageQueueAttr_t parcel_queue_attributes = { .name = "parcelQueue" };
    parcelQueueHandle = osMessageQueueNew(ParcelSorter::MAX_CONVEYOR_QUEUE_SIZE, sizeof(Parcel), &parcel_queue_attributes);
    logger.log("Application_main: Queues created.\n");

    /*
    +-----------------------------------------------------------------------------+
    | System Initialization                                                       |
    +-----------------------------------------------------------------------------+
    */
    // setQueues must be called before begin() so that begin() can register callbacks with a valid queue handle.
    logger.log("Application_main: Initializing ParcelSorter...\n");
    parcelSorter.setQueues(parcelQueueHandle, eventQueueHandle);
    parcelSorter.begin();
    logger.log("Application_main: ParcelSorter initialized.\n");

    /* Create Threads */
    logger.log("Application_main: Creating tasks...\n");
    const osThreadAttr_t hw_task_attributes = {
      .name = "HardwareUpdate", .stack_size = 512 * 4, .priority = (osPriority_t) osPriorityHigh,
    };
    hardwareUpdateTaskHandle = osThreadNew(HardwareUpdateTask, NULL, &hw_task_attributes);

    const osThreadAttr_t logic_task_attributes = {
      .name = "LogicTask", .stack_size = 1024 * 4, .priority = (osPriority_t) osPriorityNormal,
    };
    logicTaskHandle = osThreadNew(LogicTask, NULL, &logic_task_attributes);
    logger.log("Application_main: Tasks created.\n");

    // UART DMA Reception
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart_rx_buffer, UART_RX_BUFFER_SIZE);
}

/*
+-----------------------------------------------------------------------------+
| RTOS Tasks                                                                  |
+-----------------------------------------------------------------------------+
*/
void HardwareUpdateTask(void *argument) {
  uint32_t last_wake_time = osKernelGetTickCount();
  const uint32_t period_ms = 10;
  uint32_t loop_count = 0;

  for(;;) {
    // Tell the orchestrator to update all its components
    parcelSorter.update();
    
    // Log every 500ms (10ms * 50 loops) to avoid flooding the console
    if (++loop_count >= 50) {
        logger.log("HardwareUpdateTask is running...\n");
        loop_count = 0;
    }

    last_wake_time += period_ms;
    osDelayUntil(last_wake_time);
  }
}

void LogicTask(void *argument) {
  Event event;
  char log_msg[64];
  for(;;) {
    osStatus_t status = osMessageQueueGet(eventQueueHandle, &event, NULL, osWaitForever);
    if (status == osOK) {
	  snprintf(log_msg, sizeof(log_msg), "LogicTask: Handling event type %d\n", static_cast<int>(event.type));
	  logger.log(log_msg);
      parcelSorter.handleEvent(event);
    }
  }
}

/*
+-----------------------------------------------------------------------------+
| HAL Callback                                                                |
+-----------------------------------------------------------------------------+
*/
extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART2) {
        Event event;
        event.type = EventType::QR_RESPONSE_RECEIVED;
        uint16_t len = (Size < MAX_EVENT_DATA_SIZE) ? Size : (MAX_EVENT_DATA_SIZE - 1);
        memcpy(event.data, uart_rx_buffer, len);
        event.data[len] = '\0';
        
        osMessageQueuePut(eventQueueHandle, &event, 0, 0);

        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart_rx_buffer, UART_RX_BUFFER_SIZE);
    }
}

void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
