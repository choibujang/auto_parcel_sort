#include "DebouncedSensor.h"

DebouncedSensor::DebouncedSensor(GPIO_TypeDef* port, uint16_t pin, uint32_t debounce_ms)
    : port_(port),
      pin_(pin),
      debounce_ms_(debounce_ms),
      last_reading_(GPIO_PIN_RESET),
      confirmed_state_(GPIO_PIN_RESET),
      last_change_time_(0) {}

void DebouncedSensor::begin() {
  // Read the initial state of the pin
  confirmed_state_ = HAL_GPIO_ReadPin(port_, pin_);
  last_reading_ = confirmed_state_;
  last_change_time_ = HAL_GetTick();
}

void DebouncedSensor::update() {
  GPIO_PinState current_reading = HAL_GPIO_ReadPin(port_, pin_);

  if (current_reading != last_reading_) {
    last_change_time_ = HAL_GetTick();
  }

  if ((HAL_GetTick() - last_change_time_) > debounce_ms_) {
    if (current_reading != confirmed_state_) {
      confirmed_state_ = current_reading;

      if (confirmed_state_ == GPIO_PIN_SET) {
        if (rising_edge_callback_) {
          rising_edge_callback_();
        }
      } else {
        if (falling_edge_callback_) {
          falling_edge_callback_();
        }
      }
    }
  }

  last_reading_ = current_reading;
}

void DebouncedSensor::onRisingEdge(std::function<void()> callback) {
  rising_edge_callback_ = callback;
}

void DebouncedSensor::onFallingEdge(std::function<void()> callback) {
  falling_edge_callback_ = callback;
}
