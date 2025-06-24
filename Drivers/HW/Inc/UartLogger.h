#ifndef UART_LOGGER_H
#define UART_LOGGER_H

#include <stdio.h>
#include <string.h>
#include "App/ILogger.h"
#include "main.h"

class UartLogger : public ILogger {
public:
    UartLogger(UART_HandleTypeDef* huart);
    void log(const char* message) override;

private:
    UART_HandleTypeDef* huart_;
};

#endif // UART_LOGGER_H 