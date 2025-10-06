#include "logging.h"
#include "../consts.h"
#include "./fileUtils.h"

#include <filesystem>
#include <iostream>
#include <sys/stat.h>
#include <ctime>
#include <thread>


Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    try {
        if(!FileUtils::createDirectoryIfNotExists(LOG_FOLDER_PATH)) {
            throw std::runtime_error("Failed to create logs dir: " + LOG_FOLDER_PATH);
        }

        if (!FileUtils::createFileIfNotExists(LOG_FILE_PATH, LOG_FILE_PERMISSIONS)) {
            throw std::runtime_error("Failed to create log file: " + LOG_FILE_PATH);
        }

        logFile_.open(LOG_FILE_PATH, std::ios::app);
        if (!logFile_.is_open()) {
            throw std::runtime_error("Failed to open log file: " + LOG_FILE_PATH);
        }

        std::time_t current_time = std::time(nullptr);
        logFile_ << "=== Logger initialized at: " << std::asctime(std::localtime(&current_time));
        logFile_.flush();
        isInitialized_ = true;

        // Start background thread writer
        writerThread_ = std::thread(&Logger::writerLoop, this);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        isInitialized_ = false;
    }
}

void Logger::log(Level level, const std::string& message) {
    if(!isInitialized_) {
        std::cerr << "Logging failed because instance thread has not been initialized!" << std::endl;
        return;
    }

    // Non blocking just push to logging queue
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        logQueue_.emplace(level, message);
    }
    cv_.notify_one(); // wake up writter thread

}

void Logger::writerLoop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        cv_.wait(lock, [this] {return !logQueue_.empty() || !running_;});

        while (!logQueue_.empty()) {
            logMessage = logQueue_.front();
            logQueue_.pop();
            lock.unlock(); // release lock while doing I/O

            // Format and write
            std::time_t currentTime = std::time(nullptr);
            struct tm* timeinfo = localtime(&currentTime);

            std::string levelStr = (logMessage.first == ERROR) ? "ERROR" : (logMessage.first == DEBUG) ? "DEBUG" : "INFO";
            char timestamp[20];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

            logFile_ << "[" << timestamp << "]" << levelStr << ": " << logMessage.second << std::endl;

            if(logMessage.first == ERROR) {
                logFile_.flush();
            }

            lock.lock(); // Reaquire lock for next iteration
        }
    }
}

Logger::~Logger() {
    running_ = false;
    cv_.notify_one();
    if(writerThread_.joinable()) {
        writerThread_.join();
    }
    if (logFile_.is_open()) {
        logFile_.close();
    }
}