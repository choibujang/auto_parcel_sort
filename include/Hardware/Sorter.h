/**
 * @file Sorter.h
 * @brief Controls the stepper motor for sorting parcels.
 */
#ifndef SORTER_H
#define SORTER_H

#include <Arduino.h>
#include <Stepper.h>

class Sorter {
 public:
  /**
   * @param stepper global Stepper 객체의 레퍼런스
   */
  explicit Sorter(Stepper& stepper);

  /**
   * @brief stepper의 초기 속도를 설정하고 멤버 변수들을 home 상태로 초기화한다.
   * @param initial_rpm 초기 속도
   */
  void begin(int initial_rpm = 30); // Default RPM

  /**
   * @brief 모터가 현재 움직이고 있는지
   * @return moving_
   */
  bool isMoving() const;

  /**
   * @brief steps_remaining_이 0이 될 때까지 매 틱마다 STEPS_PER_UPDATE만큼 움직인다.
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

  bool wasMoveComplete();

  /**
   * @brief sorter의 목적지를 Home으로 변경한다.
   */
  void setTargetHome();

 private:
  Stepper& stepper_;
  bool moving_ = false;
  bool move_complete_ = true;
  int steps_remaining_ = 0;
  int current_steps_; // 물리적 현재 위치
  int target_steps_;  // 물리적 목표 위치
  int current_destination_; // 목표 dest

  static const int STEPS_PER_DESTINATION = 340;
  static const int STEPS_PER_UPDATE = 2;
};

#endif // SORTER_H  