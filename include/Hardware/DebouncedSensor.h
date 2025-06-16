#ifndef DEBOUNCED_SENSOR_H
#define DEBOUNCED_SENSOR_H

#include <Arduino.h>

// 함수 포인터 타입을 정의하여 의존성 주입에 사용합니다.
// MillisProvider: 현재 시간을 반환하는 함수 타입
// PinReader: 핀의 상태를 읽는 함수 타입
using MillisProvider = unsigned long (*)();
using PinReader = int (*)(uint8_t);

class DebouncedSensor {
 public:
  /**
   * @brief 센서 객체를 생성한다.
   * @param pin 신호를 읽어올 핀
   * @param debounce_ms 신호가 몇 ms 유지되어야 stable하다고 볼 것인지.
   * @param millis_provider 시간을 알려주는 함수 (테스트용)
   * @param pin_reader 핀 상태를 읽는 함수 (테스트용)
   */
  DebouncedSensor(int pin, unsigned long debounce_ms, 
                  MillisProvider millis_provider = ::millis, 
                  PinReader pin_reader = ::digitalRead);

  /**
   * @brief pinMode를 설정하고 last_reading_, confirmed_state_의 첫 기준 상태를 설정한다.
   */
  void begin();

  /**
   * @brief 디바운스를 처리하고 rising edge, falling edge 플래그를 세팅
   */
  void update();

  /**
   * @brief 센서가 LOW에서 HIGH로 바뀌었는지 체크. 한 번만 감지하고 그 뒤에는 플래그를 리셋한다.
   * @return rising_edge_flag_
   */
  bool wasRisingEdge();

  /**
   * @brief 센서가 HIGH에서 LOW로 바뀌었는지 체크. 한 번만 감지하고 그 뒤에는 플래그를 리셋한다.
   * @return falling_edge_flag_
   */
  bool wasFallingEdge();


 private:
  const int pin_;
  const unsigned long debounce_ms_;
  
  // 주입된 함수들을 저장할 멤버 변수
  MillisProvider millis_provider_;
  PinReader pin_reader_;

  int last_reading_;
  int confirmed_state_;
  unsigned long last_change_time_;

  bool rising_edge_flag_;
  bool falling_edge_flag_;
};

#endif // DEBOUNCED_SENSOR_H 