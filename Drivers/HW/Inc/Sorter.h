/**
 * @file Sorter.h
 * @brief Controls the stepper motor for sorting parcels.
 */
#ifndef SORTER_H
#define SORTER_H

#include <stdlib.h>
#include "main.h"

class Sorter {
 public:
  /**
   * @param in1_port, in1_pin, ... : Stepper driver input pins
   */
  Sorter(GPIO_TypeDef* in1_port, uint16_t in1_pin,
         GPIO_TypeDef* in2_port, uint16_t in2_pin,
         GPIO_TypeDef* in3_port, uint16_t in3_pin,
         GPIO_TypeDef* in4_port, uint16_t in4_pin);

  /**
   * @brief 멤버 변수들을 home 상태로 초기화한다.
   */
  void begin();
  
  /**
   * @brief Sets the speed in RPM. This calculates the required delay between steps.
   * @param rpm
   */
  void setSpeed(int rpm);

  /**
   * @brief 모터가 현재 움직이고 있는지
   * @return moving_
   */
  bool isMoving() const;

  /**
   * @brief Moves the motor by one step. This should be called periodically.
   */
  void update();


  /**
   * @brief 현재 목적지가 어디인지 반환한다.
   * @return current_destination_
   */
  int getCurrentDestination() const;

  /**
   * @brief sorter의 목적지를 설정한다.
   */
  void setTargetDestination(int destination);

  /**
   * @brief sorter의 목적지를 Home으로 변경한다.
   */
  void setTargetHome();

 private:
  void stepMotor(int this_step);

  // Pins for the stepper motor driver (e.g., ULN2003)
  GPIO_TypeDef* in1_port_;
  uint16_t      in1_pin_;
  GPIO_TypeDef* in2_port_;
  uint16_t      in2_pin_;
  GPIO_TypeDef* in3_port_;
  uint16_t      in3_pin_;
  GPIO_TypeDef* in4_port_;
  uint16_t      in4_pin_;

  bool moving_ = false;
  long steps_remaining_ = 0;
  int current_steps_; // 물리적 현재 위치
  int target_steps_;  // 물리적 목표 위치
  int current_destination_; // 목표 dest
  
  uint32_t step_delay_us_; // Delay between steps in microseconds, calculated from RPM
  uint32_t last_step_time_; // Last step time in microseconds

  static const int STEPS_PER_REVOLUTION = 2048; // Common for 28BYJ-48
  static const int STEPS_PER_DESTINATION = 340;
};

#endif // SORTER_H  
