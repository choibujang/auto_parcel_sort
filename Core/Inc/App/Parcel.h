#ifndef PARCEL_H
#define PARCEL_H

#include <stdint.h>

struct Parcel {
  long id;
  int destination;
  bool is_active; // To mark if the slot in the pending array is used

  Parcel(long p_id = -1, int dest = -1) : id(p_id), destination(dest), is_active(false) {}
};

#endif