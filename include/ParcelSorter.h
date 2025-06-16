/**
 * @file ParcelSorter.h
 * @brief Orchestrates the parcel sorting process using the Gatekeeper model with an event-driven architecture.
 */
#ifndef PARCEL_SORTER_H
#define PARCEL_SORTER_H


#include "Hardware/Conveyor.h"
#include "Hardware/Sorter.h"
#include "Hardware/DebouncedSensor.h"
#include "MyQueue.h"
#include "Parcel.h"

// Defines the types of events that can be sent between tasks
enum class EventType {
  SENSOR_1_RISING_EDGE,
  SENSOR_2_RISING_EDGE,
  DEST_0_FALLING_EDGE,
  DEST_1_FALLING_EDGE,
  DEST_2_FALLING_EDGE,
  QR_RESPONSE_RECEIVED,
  SORTER_MOVE_COMPLETE
};

// The data structure for events passed in the queue
struct Event {
  EventType type;
  String data; // For QR responses
  // Add other fields as needed for more complex events
};

class ParcelSorter {
public:
  static const int MAX_PARCELS_IN_FLIGHT = 5;

  ParcelSorter(Conveyor& conveyor, Sorter& sorter, DebouncedSensor& sensor1,
               DebouncedSensor& sensor2, DebouncedSensor& dest_sensor_0,
               DebouncedSensor& dest_sensor_1, DebouncedSensor& dest_sensor_2);

  void begin();

  // Event handlers, called by the LogicTask when an event is received
  void handleEvent(const Event& event);

private:
  void logError(const String& message);

  // Event-specific handlers
  void onSensor1RisingEdge();
  void onSensor2RisingEdge();
  void onDest0FallingEdge();
  void onDest1FallingEdge();
  void onDest2FallingEdge();
  void onDestinationArrival(int destination_id);
  void onQRResponseReceived(const String& responseData);
  void onSorterMoveComplete();

  // Hardware references
  Conveyor& conveyor_;
  Sorter& sorter_;
  DebouncedSensor& sensor1_;
  DebouncedSensor& sensor2_;
  DebouncedSensor& dest_sensor_0_;
  DebouncedSensor& dest_sensor_1_;
  DebouncedSensor& dest_sensor_2_;

  // RTOS handles - REMOVED
  // QueueHandle_t event_queue_;

  // Parcel pipeline management
  MyQueue<Parcel, MAX_PARCELS_IN_FLIGHT> parcelQueue_;
  int nextParcelId_;

  // No longer needed - state is managed by the event sequence.
  // bool parcel_waiting_at_gate_;
};

#endif // PARCEL_SORTER_H