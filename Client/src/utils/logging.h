#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>

// Using producer consumer pattern so we don't lock other threads for logging purposes
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
    std::ofstream logFile_;
    bool isInitialized_ = false;
    std::thread writerThread_;
    std::pair<Level, std::string> logMessage;

    // Async components of logger
    std::queue<std::pair<Level, std::string>> logQueue_;
    std::mutex queueMutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{true};

    void writerLoop(); //Backgroud thread function
};

#endif // LOGGING_H