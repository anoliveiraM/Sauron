#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>
#include <string>
#include <mutex>

class Logger {
public:
    enum Level {
        INFO = 0,
        ERROR = 1,
        DEBUG = 2
    };

    static Logger& getInstance();
    void log(Level level, const std::string& message);
    
    ~Logger();

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logFile_;
    std::mutex logMutex_;
    bool isInitialized_ = false;
};

#endif // LOGGING_H