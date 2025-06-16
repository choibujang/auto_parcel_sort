#include "Parcel.h"

// This file is intentionally left empty as the Parcel class is fully
// defined in the header file for simplicity.

Parcel::Parcel(int id)
    : id_(id),
      destination_(-1), // -1 indicates no destination assigned yet
      status_(ParcelStatus::AWAITING_QR),
      creation_time_(millis()) {}

int Parcel::getId() const {
  return id_;
}

int Parcel::getDestination() const {
  return destination_;
}

ParcelStatus Parcel::getStatus() const {
  return status_;
}

unsigned long Parcel::getCreationTime() const {
  return creation_time_;
}

void Parcel::setDestination(int destination) {
  destination_ = destination;
}

void Parcel::setStatus(ParcelStatus status) {
  status_ = status;
} 