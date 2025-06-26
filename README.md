# 자동 물류 분류 시스템

실시간 자동 물류 분류 시스템입니다. 컨베이어 벨트를 따라 이동하는 물류의 QR 코드를 인식하여, 지정된 목적지로 자동으로 분류하는 작업을 수행합니다.

## 🛠️ 개발 환경

*   **MCU**: STM32F103C8T6
*   **IDE**: STM32CubeIDE
*   **Frameworks**: STM32Cube HAL, FreeRTOS (CMSIS-OS v2)
*   **Language**: C++

## 💬 통신 프로토콜

시스템은 UART 통신을 통해 QR 리더기(또는 시뮬레이터)와 상호작용합니다.

*   **MCU -> QR 리더기**: QR 코드 인식을 요청합니다.
    *   형식: `<QR_REQUEST,parcel_id>`
    *   예시: `<QR_REQUEST,1>`

*   **QR 리더기 -> MCU**: QR 코드 인식 결과를 응답합니다.
    *   형식: `<QR_OK:parcel_id,destination_id>`
    *   예시: `<QR_OK:1,2>` (1번 물류의 목적지는 2번입니다)
