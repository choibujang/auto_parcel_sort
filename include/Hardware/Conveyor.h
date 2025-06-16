/**
 * @file Conveyor.h
 * @brief DC 모터를 작동시키는 클래스
 */
#ifndef CONVEYOR_H
#define CONVEYOR_H

#include <Arduino.h>

class Conveyor {
 public:
  /**
   * @brief DC모터로 움직이는 컨베이어 벨트 객체.
   * @param speed_pin PWM 핀
   * @param in1_pin 제어신호 핀
   * @param in2_pin 제어신호 핀
   */
  Conveyor(int speed_pin, int in1_pin, int in2_pin);

  /**
   * @brief 핀들을 initialize하고 모터를 full speed로 작동시킨다.
   */
  void begin();

  /**
   * @brief 모터를 half speed로 작동시킨다.
   */
  void run();

  /**
   * @brief 모터를 멈춘다.
   */
  void stop();

  /**
   * @brief 모터가 현재 작동중인지 확인한다.
   * @return is_running_
   */
  bool isRunning() const;

 private:
  const int speed_pin_;
  const int in1_pin_;
  const int in2_pin_;
  bool is_running_;
};

#endif // CONVEYOR_H 