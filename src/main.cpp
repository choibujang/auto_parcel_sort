#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Stepper.h>

#include "Hardware/Conveyor.h"
#include "Hardware/DebouncedSensor.h"
#include "Hardware/Sorter.h"
#include "ParcelSorter.h"

// --- Pin Definitions ---
#define SENSOR1_PIN 2
#define SENSOR2_PIN 3
#define DEST_SENSOR_0_PIN 12
#define DEST_SENSOR_1_PIN 13
#define DEST_SENSOR_2_PIN A0 // Pin 14

#define CONVEYOR_PWM_PIN 5
#define CONVEYOR_IN1_PIN 6
#define CONVEYOR_IN2_PIN 7

#define MOTOR_PIN_1 8
#define MOTOR_PIN_2 9
#define MOTOR_PIN_3 10
#define MOTOR_PIN_4 11
#define STEPS_PER_REVOLUTION 2048

#define DEBOUNCE_MS 30

// --- RTOS Handles ---
QueueHandle_t eventQueue;

// --- Global Objects ---
Conveyor conveyor(CONVEYOR_PWM_PIN, CONVEYOR_IN1_PIN, CONVEYOR_IN2_PIN);
Stepper stepper(STEPS_PER_REVOLUTION, MOTOR_PIN_1, MOTOR_PIN_3, MOTOR_PIN_2, MOTOR_PIN_4);
Sorter sorter(stepper);

DebouncedSensor sensor1(SENSOR1_PIN, DEBOUNCE_MS);
DebouncedSensor sensor2(SENSOR2_PIN, DEBOUNCE_MS);
DebouncedSensor destSensor0(DEST_SENSOR_0_PIN, DEBOUNCE_MS);
DebouncedSensor destSensor1(DEST_SENSOR_1_PIN, DEBOUNCE_MS);
DebouncedSensor destSensor2(DEST_SENSOR_2_PIN, DEBOUNCE_MS);

ParcelSorter parcelSorter(conveyor, sorter, sensor1, sensor2, 
                          destSensor0, destSensor1, destSensor2);

// --- Tasks ---

void HardwareUpdateTask(void *pvParameters) {
  const TickType_t xFrequency = pdMS_TO_TICKS(10);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  Event event;

  for (;;) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    
    sensor1.update();
    sensor2.update();
    destSensor0.update();
    destSensor1.update();
    destSensor2.update();
    sorter.update();

    if (sorter.wasMoveComplete()) {
      event.type = EventType::SORTER_MOVE_COMPLETE;
      xQueueSend(eventQueue, &event, 0);
    }

    if (sensor1.wasRisingEdge()) {
      event.type = EventType::SENSOR_1_RISING_EDGE;
      xQueueSend(eventQueue, &event, 0);
    }
    if (sensor2.wasRisingEdge()) {
      event.type = EventType::SENSOR_2_RISING_EDGE;
      xQueueSend(eventQueue, &event, 0);
    }
    if (destSensor0.wasFallingEdge()) {
      event.type = EventType::DEST_0_FALLING_EDGE;
      xQueueSend(eventQueue, &event, 0);
    }
    if (destSensor1.wasFallingEdge()) {
      event.type = EventType::DEST_1_FALLING_EDGE;
      xQueueSend(eventQueue, &event, 0);
    }
    if (destSensor2.wasFallingEdge()) {
      event.type = EventType::DEST_2_FALLING_EDGE;
      xQueueSend(eventQueue, &event, 0);
    }
  }
}

void LogicTask(void *pvParameters) {
  Event receivedEvent;
  for (;;) {
    // Wait indefinitely for an event to arrive
    if (xQueueReceive(eventQueue, &receivedEvent, portMAX_DELAY) == pdPASS) {
      parcelSorter.handleEvent(receivedEvent);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  // Enable the USART Recieve Complete interrupt
  UCSR0B |= (1 << RXCIE0);

  delay(1000); 
  Serial.println("--- Parcel Sorter Initializing (Event-Driven Model) ---");

  // Create the event queue before initializing tasks
  eventQueue = xQueueCreate(10, sizeof(Event)); // Queue for 10 events

  // Initialize hardware
  conveyor.begin();
  sensor1.begin();
  sensor2.begin();
  destSensor0.begin();
  destSensor1.begin();
  destSensor2.begin();
  sorter.begin();
  
  // Initialize system logic
  parcelSorter.begin();

  // Create the tasks
  xTaskCreate(HardwareUpdateTask, "HWUpdate", 192, NULL, 3, NULL);
  xTaskCreate(LogicTask, "Logic", 256, NULL, 2, NULL);
  
  Serial.println("--- System Running ---");
}

void loop() {

}

ISR(USART_RX_vect) {
  static char isrBuffer[128];
  static int isrBufferIndex = 0;

  char inChar = UDR0;

  if (isrBufferIndex < sizeof(isrBuffer) - 1) {
    isrBuffer[isrBufferIndex++] = inChar;
  }

  if (inChar == '>') {
    isrBuffer[isrBufferIndex] = '\0';

    if (String(isrBuffer).startsWith("<QR_")) {
      Event event;
      event.type = EventType::QR_RESPONSE_RECEIVED;
      event.data = String(isrBuffer);
      
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      xQueueSendFromISR(eventQueue, &event, &xHigherPriorityTaskWoken);
      
      // If xHigherPriorityTaskWoken is now true, a context switch should be performed.
      if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
      }
    }
    
    // Reset buffer for the next message
    isrBufferIndex = 0;
  }
}
