/**
 * @file Conveyor.h
 * @brief DC 모터를 작동시키는 클래스
 */
#ifndef CONVEYOR_H
#define CONVEYOR_H

#include "main.h"

class Conveyor {
 public:
  /**
   * @brief DC모터로 움직이는 컨베이어 벨트 객체.
   * @param htim_pwm 타이머 핸들 (for PWM)
   * @param pwm_channel 타이머 채널 (for PWM)
   * @param in1_port 제어신호 핀의 포트
   * @param in1_pin 제어신호 핀
   * @param in2_port 제어신호 핀의 포트
   * @param in2_pin 제어신호 핀
   */
  Conveyor(TIM_HandleTypeDef* htim_pwm, uint32_t pwm_channel, 
           GPIO_TypeDef* in1_port, uint16_t in1_pin, 
           GPIO_TypeDef* in2_port, uint16_t in2_pin);

  /**
   * @brief PWM을 시작하고 모터를 정지 상태로 초기화한다.
   */
  void begin();

  /**
   * @brief 모터를 설정된 속도로 작동시킨다.
   */
  void run();

  /**
   * @brief 모터를 멈춘다.
   */
  void stop();
  
  /**
   * @brief 모터의 속도를 설정한다. (0-100)
   * @param speed
  */
  void setSpeed(uint8_t speed);


  /**
   * @brief 모터가 현재 작동중인지 확인한다.
   * @return is_running_
   */
  bool isRunning() const;

 private:
  TIM_HandleTypeDef* htim_pwm_;
  uint32_t pwm_channel_;
  GPIO_TypeDef* in1_port_;
  uint16_t in1_pin_;
  GPIO_TypeDef* in2_port_;
  uint16_t in2_pin_;
  
  uint8_t speed_percent_;
  bool is_running_;
};

#endif // CONVEYOR_H 