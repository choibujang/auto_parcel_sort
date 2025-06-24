/**
 * @file ParcelSorter.h
 * @brief 자동 물류 분류 시스템의 비즈니스 로직 클래스
 */
#ifndef PARCELSORTER_H_
#define PARCELSORTER_H_

#include <stdio.h>
#include <string.h>
#include <functional>

#include "main.h" // HAL, FreeRTOS
#include "Conveyor.h"
#include "Sorter.h"
#include "DebouncedSensor.h"
#include "App/Parcel.h"
#include "App/Event.h"
#include "App/ILogger.h"

// Forward declarations
class Conveyor;
class Sorter;
class DebouncedSensor;

class ParcelSorter {
public:
  static const int MAX_PENDING_PARCELS = 8;
  static const int MAX_CONVEYOR_QUEUE_SIZE = 16;

  ParcelSorter(Conveyor& conveyor, Sorter& sorter, DebouncedSensor& sensor1,
               DebouncedSensor& sensor2, DebouncedSensor& dest_sensor_0,
               DebouncedSensor& dest_sensor_1, DebouncedSensor& dest_sensor_2,
               ILogger& logger); // Inject logger interface

  void begin();
  void update();

  // 이벤트가 일어났을 때 LogicTask에 의해 호출된다
  void handleEvent(const Event& event);

  /**
   * @brief Check if the system is currently waiting for a QR code response.
   * @return True if waiting for a QR response, false otherwise.
   */
  bool isWaitingForQrResponse() const;

  // main.cpp로부터 queue를 받을 때 사용된다
  void setQueues(osMessageQueueId_t parcel_queue, osMessageQueueId_t event_queue);

private:
  // Event handlers
  void onSensor1RisingEdge();
  void onSensor2RisingEdge();
  void onSorterMoveComplete();
  void onDestinationArrival(int destination_id);
  void onQRResponseReceived(const char* responseData);
  void sendQrRequest(int parcel_id);

  // Hardware
  Conveyor& conveyor_;
  Sorter& sorter_;
  DebouncedSensor& sensor1_;
  DebouncedSensor& sensor2_;
  DebouncedSensor& dest_sensor_0_;
  DebouncedSensor& dest_sensor_1_;
  DebouncedSensor& dest_sensor_2_;
  ILogger& logger_; // Use the interface

  osMessageQueueId_t parcelQueueHandle_; // Parcel queue handle
  osMessageQueueId_t eventQueueHandle_;  // Event queue handle
  long nextParcelId_;

  bool isWaitingForQrResponse_;

  // Parcels that are between sensor 1 and sensor 2
  Parcel pendingParcels_[MAX_PENDING_PARCELS];
  // Queue of parcel IDs to maintain order of arrival at sensor 1
  osMessageQueueId_t pendingParcelIdQueueHandle_;

  // Helper methods for managing pending parcels
  int findAvailablePendingSlot();
  int findPendingParcel(long parcel_id);
};

#endif
