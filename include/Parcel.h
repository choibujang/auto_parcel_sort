#ifndef PARCEL_H
#define PARCEL_H

class Parcel {
public:
  int id;
  int destination;

  // Default constructor for creating placeholder objects
  Parcel(int newId = 0) : id(newId), destination(-1) {} // -1 means destination not yet known
};

#endif // PARCEL_H 