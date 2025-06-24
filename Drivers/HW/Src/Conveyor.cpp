#include "Conveyor.h"

Conveyor::Conveyor(TIM_HandleTypeDef* htim_pwm, uint32_t pwm_channel, 
                   GPIO_TypeDef* in1_port, uint16_t in1_pin, 
                   GPIO_TypeDef* in2_port, uint16_t in2_pin)
    : htim_pwm_(htim_pwm),
      pwm_channel_(pwm_channel),
      in1_port_(in1_port),
      in1_pin_(in1_pin),
      in2_port_(in2_port),
      in2_pin_(in2_pin),
      speed_percent_(50),
      is_running_(false) {}

void Conveyor::begin() {
  HAL_TIM_PWM_Start(htim_pwm_, pwm_channel_);
  stop();
}

void Conveyor::run() {
  uint32_t max_duty = htim_pwm_->Instance->ARR;
  __HAL_TIM_SET_COMPARE(htim_pwm_, pwm_channel_, (max_duty * speed_percent_) / 100);

  HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_SET);
  HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_RESET);
  
  is_running_ = true;
}

void Conveyor::stop() {
  HAL_GPIO_WritePin(in1_port_, in1_pin_, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(in2_port_, in2_pin_, GPIO_PIN_RESET);
  
  is_running_ = false;
}

void Conveyor::setSpeed(uint8_t speed) {
  if (speed > 100) {
    speed_percent_ = 100;
  } else {
    speed_percent_ = speed;
  }
  
  if (is_running_) {
    uint32_t max_duty = htim_pwm_->Instance->ARR;
    __HAL_TIM_SET_COMPARE(htim_pwm_, pwm_channel_, (max_duty * speed_percent_) / 100);
  }
}

bool Conveyor::isRunning() const {
  return is_running_;
} 
