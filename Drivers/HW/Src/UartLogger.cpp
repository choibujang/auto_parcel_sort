#include "UartLogger.h"


UartLogger::UartLogger(UART_HandleTypeDef* huart) : huart_(huart) {}

void UartLogger::log(const char* message) {
    char buffer[128];
    // We add [LOG] prefix and \r\n for terminal newline
    int len = snprintf(buffer, sizeof(buffer), "[LOG] %s\r\n", message);
    if (len > 0) {
        HAL_UART_Transmit(huart_, (uint8_t*)buffer, len, 100);
    }
} 
