#include "App/ParcelSorter.h"

ParcelSorter::ParcelSorter(Conveyor& conveyor, Sorter& sorter, DebouncedSensor& sensor1,
                           DebouncedSensor& sensor2, DebouncedSensor& dest_sensor_0,
                           DebouncedSensor& dest_sensor_1, DebouncedSensor& dest_sensor_2,
                           ILogger& logger)
  : conveyor_(conveyor),
    sorter_(sorter),
    sensor1_(sensor1),
    sensor2_(sensor2),
    dest_sensor_0_(dest_sensor_0),
    dest_sensor_1_(dest_sensor_1),
    dest_sensor_2_(dest_sensor_2),
    logger_(logger),
    parcelQueueHandle_(nullptr),
    eventQueueHandle_(nullptr),
    nextParcelId_(1),
    pendingParcelIdQueueHandle_(nullptr) {
  for (int i = 0; i < MAX_PENDING_PARCELS; ++i) {
    pendingParcels_[i].is_active = false;
  }
  const osMessageQueueAttr_t pending_queue_attributes = {
    .name = "pendingParcelIdQueue"
  };
  pendingParcelIdQueueHandle_ = osMessageQueueNew(MAX_PENDING_PARCELS, sizeof(long), &pending_queue_attributes);
}

void ParcelSorter::begin() {
  logger_.log("ParcelSorter: Beginning component initialization...\n");

  conveyor_.begin();
  logger_.log("ParcelSorter: Conveyor initialized.\n");

  sorter_.begin();
  logger_.log("ParcelSorter: Sorter initialized.\n");

  sensor1_.begin();
  sensor2_.begin();
  dest_sensor_0_.begin();
  dest_sensor_1_.begin();
  dest_sensor_2_.begin();
  logger_.log("ParcelSorter: All sensors initialized.\n");

  // Register callbacks to link hardware events to the system's event queue
  sensor1_.onRisingEdge([this]() {
      Event event = {EventType::SENSOR_1_RISING_EDGE};
      osMessageQueuePut(eventQueueHandle_, &event, 0, 0);
  });
  sensor2_.onRisingEdge([this]() {
      Event event = {EventType::SENSOR_2_RISING_EDGE};
      osMessageQueuePut(eventQueueHandle_, &event, 0, 0);
  });
  dest_sensor_0_.onFallingEdge([this]() {
      Event event = {EventType::DEST_0_FALLING_EDGE};
      osMessageQueuePut(eventQueueHandle_, &event, 0, 0);
  });
  dest_sensor_1_.onFallingEdge([this]() {
      Event event = {EventType::DEST_1_FALLING_EDGE};
      osMessageQueuePut(eventQueueHandle_, &event, 0, 0);
  });
  dest_sensor_2_.onFallingEdge([this]() {
      Event event = {EventType::DEST_2_FALLING_EDGE};
      osMessageQueuePut(eventQueueHandle_, &event, 0, 0);
  });
}

void ParcelSorter::update() {
  sensor1_.update();
  sensor2_.update();
  dest_sensor_0_.update();
  dest_sensor_1_.update();
  dest_sensor_2_.update();
  sorter_.update();
}

void ParcelSorter::setQueues(osMessageQueueId_t parcel_queue, osMessageQueueId_t event_queue) {
    parcelQueueHandle_ = parcel_queue;
    eventQueueHandle_ = event_queue;
}

bool ParcelSorter::isWaitingForQrResponse() const {
    return isWaitingForQrResponse_;
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

void ParcelSorter::sendQrRequest(int parcel_id) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "<QR_REQUEST,%d>", parcel_id);
    if (len > 0) {
      // Assuming the QR reader is also connected via a UART or similar
      // and we might want to log this action.
      // For now, this is a placeholder for the actual request sending.
      logger_.log(buffer);
    }
}

int ParcelSorter::findAvailablePendingSlot() {
    for (int i = 0; i < MAX_PENDING_PARCELS; ++i) {
        if (!pendingParcels_[i].is_active) {
            return i;
        }
    }
    return -1; // No slot available
}

int ParcelSorter::findPendingParcel(long parcel_id) {
    for (int i = 0; i < MAX_PENDING_PARCELS; ++i) {
        if (pendingParcels_[i].is_active && pendingParcels_[i].id == parcel_id) {
            return i;
        }
    }
    return -1; // Not found
}

void ParcelSorter::onSensor1RisingEdge() {
  int slot = findAvailablePendingSlot();
  if (slot == -1) {
    logger_.log("Pending parcels array is full. Cannot add new parcel.");
    return;
  }

  long new_id = nextParcelId_++;
  pendingParcels_[slot] = Parcel(new_id, -1);
  pendingParcels_[slot].is_active = true;

  osStatus_t status = osMessageQueuePut(pendingParcelIdQueueHandle_, &new_id, 0, 0);
  if (status != osOK) {
    logger_.log("Failed to queue new pending parcel ID.");
    pendingParcels_[slot].is_active = false; // Rollback
    return;
  }

  sendQrRequest(new_id);
}

void ParcelSorter::onSensor2RisingEdge() {
  long parcel_id_to_sort;
  osStatus_t status = osMessageQueueGet(pendingParcelIdQueueHandle_, &parcel_id_to_sort, 0, 0);

  if (status != osOK) {
    logger_.log("S2 triggered, but pending parcel ID queue is empty.");
    return;
  }
  
  conveyor_.stop();

  int slot = findPendingParcel(parcel_id_to_sort);
  if (slot == -1) {
      char msg[64];
      snprintf(msg, sizeof(msg), "S2 triggered for unknown parcel ID: %ld", parcel_id_to_sort);
      logger_.log(msg);
      conveyor_.run(); // Resume conveyor
      return;
  }

  Parcel& parcel_to_sort = pendingParcels_[slot];

  int destination = parcel_to_sort.destination;

  if (destination == -1) {
    destination = 0; // Default destination
    char msg[64];
    snprintf(msg, sizeof(msg), "Parcel %ld has no QR dest, defaulting to 0.", parcel_to_sort.id);
    logger_.log(msg);
  }

  // Put the parcel into the main conveyor queue for final sorting
  osMessageQueuePut(parcelQueueHandle_, &parcel_to_sort, 0, 0);

  sorter_.setTargetDestination(destination);
}

void ParcelSorter::onSorterMoveComplete() {
  conveyor_.run();
}

void ParcelSorter::onDestinationArrival(int dest_id) {
    Parcel parcel;
    // A destination sensor was triggered, so a parcel *should* be at the front.
    // We get it from the queue directly, no need to peek.
    osStatus_t status = osMessageQueueGet(parcelQueueHandle_, &parcel, NULL, 0);

    if (status == osOK) {
        // We successfully dequeued a parcel. Now, check if it's the correct one.
        if (parcel.destination == dest_id) {
            char msg[64];
            snprintf(msg, sizeof(msg), "Parcel %ld arrived correctly at destination %d.", parcel.id, dest_id);
            logger_.log(msg);
        } else {
            char msg[128];
            snprintf(msg, sizeof(msg), "Mis-sort! Parcel %ld (expected dest %d) arrived at dest %d.",
                     parcel.id, parcel.destination, dest_id);
            logger_.log(msg);
        }

        // Regardless of correct or not, find the parcel in the pending list and deactivate it.
        int slot = findPendingParcel(parcel.id);
        if (slot != -1) {
            pendingParcels_[slot].is_active = false;
        }

    } else {
        // This case (sensor trigger but queue is empty) should ideally not happen in normal operation.
        char msg[64];
        snprintf(msg, sizeof(msg), "Dest sensor %d triggered, but parcel queue was empty.", dest_id);
        logger_.log(msg);
    }
}

void ParcelSorter::onQRResponseReceived(const char* responseData) {
  int parcel_id, destination;
  if (sscanf(responseData, "<QR_OK:%d,%d>", &parcel_id, &destination) == 2) {
      int slot = findPendingParcel(parcel_id);
      if (slot != -1) {
          pendingParcels_[slot].destination = destination;
      } else {
          char msg[64];
          snprintf(msg, sizeof(msg), "QR response for unknown parcel ID: %d", parcel_id);
          logger_.log(msg);
      }
  } else {
      char msg[64];
      snprintf(msg, sizeof(msg), "Malformed QR response: %s", responseData);
      logger_.log(msg);
  }
} 