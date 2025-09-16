#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <fstream>
#include <mutex>

#include "../consts.h"

class Logger {
public:
    enum Level {INFO, ERROR, DEBUG};

    static Logger& getInstance();
    bool isInitialized_ = false;
    void log (Level level, const std::string& message);
    ~Logger();

private:
    Logger();
    std::ofstream logFile_;
    std::mutex logMutex_;
};

#endif