#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#define MAX_EVENT_DATA_SIZE 32

enum class EventType {
  SENSOR_1_RISING_EDGE,
  SENSOR_2_RISING_EDGE,
  DEST_0_FALLING_EDGE,
  DEST_1_FALLING_EDGE,
  DEST_2_FALLING_EDGE,
  QR_RESPONSE_RECEIVED,
  SORTER_MOVE_COMPLETE
};

struct Event {
  EventType type;
  char data[MAX_EVENT_DATA_SIZE]; 
};

#endif