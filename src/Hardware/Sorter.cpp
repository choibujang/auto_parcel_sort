#include "Hardware/Sorter.h"
#include <Arduino.h>
// #include "../ParcelSorter.h" // No longer needed, breaking dependency cycle.

Sorter::Sorter(Stepper& stepper)
    : stepper_(stepper),
      moving_(false),
      current_steps_(0),
      target_steps_(0),
      current_destination_(0),
      move_complete_(true) {}

void Sorter::begin(int initial_rpm) {
  stepper_.setSpeed(initial_rpm);
}

bool Sorter::isMoving() const {
  return moving_;
}


int Sorter::getCurrentDestination() const {
  return current_destination_;
}

void Sorter::setTargetDestination(int destination) {
  int new_target_steps = destination * STEPS_PER_DESTINATION;

  // No change if already at the target and not homing
  if (target_steps_ == new_target_steps && current_steps_ == new_target_steps) {
    return;
  }
  
  target_steps_ = new_target_steps;
  current_destination_ = destination;
  moving_ = true;
  move_complete_ = false;
}

bool Sorter::wasMoveComplete() {
  bool result = move_complete_;
  move_complete_ = false;
  return result;
}

void Sorter::setTargetHome() {
  target_steps_ = 0;
  current_destination_ = 0;
  moving_ = (current_steps_ != target_steps_);
}

void Sorter::update() {
  if (!moving_) {
    return;
  }

  if (current_steps_ == target_steps_) {
    move_complete_ = true;
    moving_ = false;
  }
    
  int steps_to_move = target_steps_ - current_steps_;
  int direction = (steps_to_move > 0) ? 1 : -1;

  int steps_this_update = min(abs(steps_to_move), STEPS_PER_UPDATE);

  stepper_.step(steps_this_update * direction);
  current_steps_ += steps_this_update * direction;

  if (current_steps_ == target_steps_) {
    move_complete_ = true;
    moving_ = false;
  }
} 