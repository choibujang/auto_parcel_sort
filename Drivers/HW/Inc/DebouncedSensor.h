#ifndef DEBOUNCED_SENSOR_H
#define DEBOUNCED_SENSOR_H

#include "main.h"
#include <functional> // For std::function

class DebouncedSensor {
 public:
  /**
   * @brief 센서 객체를 생성한다.
   * @param port 신호를 읽어올 핀의 포트
   * @param pin 신호를 읽어올 핀
   * @param debounce_ms 신호가 몇 ms 유지되어야 stable하다고 볼 것인지.
   */
  DebouncedSensor(GPIO_TypeDef* port, uint16_t pin, uint32_t debounce_ms);

  /**
   * @brief 핀의 초기 상태를 읽는다.
   */
  void begin();

  /**
   * @brief 디바운스를 처리하고 등록된 엣지 콜백을 호출한다.
   */
  void update();

  /**
   * @brief Rising edge (LOW->HIGH)가 감지되었을 때 호출될 함수를 등록한다.
   * @param callback 등록할 콜백 함수
   */
  void onRisingEdge(std::function<void()> callback);

  /**
   * @brief Falling edge (HIGH->LOW)가 감지되었을 때 호출될 함수를 등록한다.
   * @param callback 등록할 콜백 함수
   */
  void onFallingEdge(std::function<void()> callback);

 private:
  GPIO_TypeDef* port_;
  const uint16_t pin_;
  const uint32_t debounce_ms_;
  
  GPIO_PinState last_reading_;
  GPIO_PinState confirmed_state_;
  uint32_t last_change_time_;

  std::function<void()> rising_edge_callback_;
  std::function<void()> falling_edge_callback_;
};

#endif