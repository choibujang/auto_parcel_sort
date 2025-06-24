#include "Sorter.h"

// DWT_Init and delay_us are in main.cpp, we need access to the counter
// and the clock frequency.
extern "C" uint32_t HAL_RCC_GetHCLKFreq(void);

Sorter::Sorter(GPIO_TypeDef* in1_port, uint16_t in1_pin,
               GPIO_TypeDef* in2_port, uint16_t in2_pin,
               GPIO_TypeDef* in3_port, uint16_t in3_pin,
               GPIO_TypeDef* in4_port, uint16_t in4_pin)
    : in1_port_(in1_port), in1_pin_(in1_pin),
      in2_port_(in2_port), in2_pin_(in2_pin),
      in3_port_(in3_port), in3_pin_(in3_pin),
      in4_port_(in4_port), in4_pin_(in4_pin) {}

void Sorter::begin() {
  moving_ = false;
  steps_remaining_ = 0;
  current_steps_ = 0;
  target_steps_ = 0;
  current_destination_ = 0;
  last_step_time_ = 0;
  setSpeed(30); // Default RPM
}

void Sorter::setSpeed(int rpm) {
    if (rpm > 0) {
        step_delay_us_ = 60000000L / (STEPS_PER_REVOLUTION * rpm);
    }
}

bool Sorter::isMoving() const {
  return moving_;
}

int Sorter::getCurrentDestination() const {
  return current_destination_;
}

void Sorter::setTargetDestination(int destination) {
    target_steps_ = destination * STEPS_PER_DESTINATION;
    steps_remaining_ = abs(target_steps_ - current_steps_);
    current_destination_ = destination;
    
    if(steps_remaining_ > 0) {
        moving_ = true;
    }
}

void Sorter::setTargetHome() {
    setTargetDestination(0);
}

void Sorter::update() {
    if (steps_remaining_ == 0) {
        if (moving_) {
            moving_ = false;
        }
        return;
    }

    moving_ = true;

    uint32_t now_us = DWT->CYCCNT / (HAL_RCC_GetHCLKFreq() / 1000000);
    if (now_us - last_step_time_ < step_delay_us_) {
        return;
    }
    
    last_step_time_ = now_us;
    
    int direction = (target_steps_ > current_steps_) ? 1 : -1;

    int step_index = (current_steps_ + direction) % 8;
    if (step_index < 0) {
      step_index += 8;
    }
    stepMotor(step_index);
    
    current_steps_ += direction;
    steps_remaining_--;
}

void Sorter::stepMotor(int this_step) {
    switch (this_step) {
      case 0: // 1000
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_RESET);
        break;
      case 1: // 1100
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_RESET);
        break;
      case 2: // 0100
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_RESET);
        break;
      case 3: // 0110
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_RESET);
        break;
      case 4: // 0010
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_RESET);
        break;
      case 5: // 0011
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_SET);
        break;
      case 6: // 0001
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_SET);
        break;
      case 7: // 1001
        HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_SET);
        HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in3_port_, in3_pin_, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(in4_port_, in4_pin_, GPIO_PIN_SET);
        break;
    }
} 
