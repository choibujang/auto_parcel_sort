#include "ParcelSorter.h"
#include <Arduino.h>

ParcelSorter::ParcelSorter(Conveyor& conveyor, Sorter& sorter, DebouncedSensor& sensor1,
                           DebouncedSensor& sensor2, DebouncedSensor& dest_sensor_0,
                           DebouncedSensor& dest_sensor_1, DebouncedSensor& dest_sensor_2)
    : conveyor_(conveyor),
      sorter_(sorter),
      sensor1_(sensor1),
      sensor2_(sensor2),
      dest_sensor_0_(dest_sensor_0),
      dest_sensor_1_(dest_sensor_1),
      dest_sensor_2_(dest_sensor_2),
      nextParcelId_(1) {}

void ParcelSorter::begin() {
  conveyor_.run();
}

void ParcelSorter::handleEvent(const Event& event) {
  switch (event.type) {
    case EventType::SENSOR_1_RISING_EDGE:
      onSensor1RisingEdge();
      break;
    case EventType::SENSOR_2_RISING_EDGE:
      onSensor2RisingEdge();
      break;
    case EventType::SORTER_MOVE_COMPLETE:
      onSorterMoveComplete();
      break;
    case EventType::DEST_0_FALLING_EDGE:
      onDestinationArrival(0);
      break;
    case EventType::DEST_1_FALLING_EDGE:
      onDestinationArrival(1);
      break;
    case EventType::DEST_2_FALLING_EDGE:
      onDestinationArrival(2);
      break;
    case EventType::QR_RESPONSE_RECEIVED:
      onQRResponseReceived(event.data);
      break;
  }
}

void ParcelSorter::logError(const String& message) {
    Serial.print("[ERROR_LOG] ");
    Serial.println(message);
}

// Event: A new parcel is detected at the entrance.
// Action: Add parcel to pipeline and request its QR info.
void ParcelSorter::onSensor1RisingEdge() {
  if (parcelQueue_.isFull()) {
    logError("Queue is full. Cannot add new parcel.");
    return;
  }
  Parcel new_parcel(nextParcelId_++);
  parcelQueue_.enqueue(new_parcel);
  Serial.print("<QR_REQUEST,");
  Serial.print(new_parcel.id);
  Serial.println(">");
}

// Event: A parcel has reached the sorting gate.
// Action: Stop the conveyor and command the sorter to the correct destination.
void ParcelSorter::onSensor2RisingEdge() {
  if (parcelQueue_.isEmpty()) {
    logError("S2 triggered, but parcel queue is empty.");
    return;
  }
  
  conveyor_.stop();

  Parcel& frontParcel = parcelQueue_.front();
  int destination = frontParcel.destination;

  if (destination == -1) {
    destination = 0; // Default destination for unknown parcels
    String msg = "Parcel " + String(frontParcel.id) + " has no destination, defaulting to 0.";
    logError(msg);
  }

  sorter_.setTargetDestination(destination);
}

// Event: The sorter has finished its move.
// Action: Restart the conveyor belt.
void ParcelSorter::onSorterMoveComplete() {
  conveyor_.run();
}

// Event: A parcel has been detected at a destination chute.
// Action: Check for correctness and remove the parcel from the pipeline.
void ParcelSorter::onDestinationArrival(int destination_id) {
    if (parcelQueue_.isEmpty()) {
        logError("Destination sensor " + String(destination_id) + " triggered, but queue is empty.");
        return;
    }

    Parcel front_parcel;
    parcelQueue_.peek(front_parcel);

    int expected_dest = (front_parcel.destination == -1) ? 0 : front_parcel.destination;

    if (expected_dest != destination_id) {
        String msg = "Mis-sort! Parcel " + String(front_parcel.id) + 
                     " (expected dest: " + String(expected_dest) + 
                     ") arrived at dest " + String(destination_id);
        logError(msg);
    }
    
    // Dequeue the parcel regardless of whether it was a mis-sort or not,
    // to keep the system moving.
    Parcel dequeued_parcel;
    parcelQueue_.dequeue(dequeued_parcel);
}


// Event: An asynchronous QR response has been received.
// Action: Find the corresponding parcel in the queue and update its data.
void ParcelSorter::onQRResponseReceived(const String& responseData) {
  int id_end_idx = responseData.indexOf(',');
  if (id_end_idx == -1) id_end_idx = responseData.length();
  
  int parcel_id = responseData.substring(responseData.indexOf(':') + 1, id_end_idx).toInt();

  if (!responseData.startsWith("QR_OK")) {
      logError("Malformed QR response: " + responseData);
      return;
  }

  int destination = responseData.substring(id_end_idx + 1).toInt();
  bool parcel_found = false;
  // Iterate from the rear for efficiency
  for (int i = parcelQueue_.getSize() - 1; i >= 0; --i) {
    if (parcelQueue_.get(i).id == parcel_id) {
      parcelQueue_.get(i).destination = destination;
      parcel_found = true;
      break; 
    }
  }

  if (!parcel_found) {
    logError("QR response for unknown parcel ID: " + String(parcel_id));
  }
} 