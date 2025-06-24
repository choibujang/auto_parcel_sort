#ifndef ILOGGER_H
#define ILOGGER_H

class ILogger {
public:
    virtual ~ILogger() {}
    virtual void log(const char* message) = 0;
};

#endif // ILOGGER_H 