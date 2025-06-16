#include "Hardware/DebouncedSensor.h"

DebouncedSensor::DebouncedSensor(int pin, unsigned long debounce_ms,
                                 MillisProvider millis_provider,
                                 PinReader pin_reader)
    : pin_(pin),
      debounce_ms_(debounce_ms),
      millis_provider_(millis_provider),
      pin_reader_(pin_reader),
      last_reading_(HIGH),
      confirmed_state_(HIGH),
      last_change_time_(0),
      rising_edge_flag_(false),
      falling_edge_flag_(false) {}

void DebouncedSensor::begin() {
    pinMode(pin_, INPUT);
    last_reading_ = pin_reader_(pin_);
    confirmed_state_ = last_reading_;
}

void DebouncedSensor::update() {
    int reading = pin_reader_(pin_);

    if (reading != last_reading_) {
        last_change_time_ = millis_provider_();
    }

    if ((millis_provider_() - last_change_time_) >= debounce_ms_) {
        if (reading != confirmed_state_) {
            confirmed_state_ = reading;
            if (confirmed_state_ == HIGH) {
                rising_edge_flag_ = true;
            } else {
                falling_edge_flag_ = true;
            }
        }
    }
    last_reading_ = reading;
}

bool DebouncedSensor::wasRisingEdge() {
    bool result = rising_edge_flag_;
    rising_edge_flag_ = false;
    return result;
}

bool DebouncedSensor::wasFallingEdge() {
    bool result = falling_edge_flag_;
    falling_edge_flag_ = false;
    return result;
}
