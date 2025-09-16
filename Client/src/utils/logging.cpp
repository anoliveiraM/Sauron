#include "logging.h"
#include "../consts.h"
#include "./fileUtils.h"

#include <filesystem>
#include <iostream>
#include <sys/stat.h>
#include <ctime>

// Return instance of logger
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// Open log file
Logger::Logger() {
    try {

        // Create log dir if not exist
        if (!FileUtils::createDirectoryIfNotExists(LOG_FOLDER_PATH)) {
            throw std::runtime_error("Failed to create logs dir: " + LOG_FOLDER_PATH);
        } 

        // Create log file if it does not exist
        if (!FileUtils::createFileIfNotExists(LOG_FILE_PATH, LOG_FILE_PERMISSIONS)) {
            throw std::runtime_error("Failed to create log file: " + LOG_FILE_PATH);
        }

        // Now open the log file in append mode
        logFile_.open(LOG_FILE_PATH, std::ios::app);
        if (!logFile_.is_open()) {
            throw std::runtime_error("Failed to open log file: " + LOG_FILE_PATH);
        }

        std::time_t current_time = std::time(nullptr);

        logFile_ << "=== Logger initialized at: " << std::asctime(std::localtime(&current_time));
        logFile_.flush();
        isInitialized_ = true;

    } catch ( const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        // Allow partial construction but mark as uninitialized
        isInitialized_ = false;
    }
}

void Logger::log(Level level, const std::string& message) {
    if(!isInitialized_ || !logFile_.is_open()) {
        std::cerr << "Logging failed: File not opened" << std::endl;
        return;
    }

    // Lock to ensure we don't deadlock and have have to threads grabbing the same instance of logger
    // Right now we implement like this, if this causes delays in execution we will need to move to async logging
    std::lock_guard<std::mutex> lock(logMutex_);

    // Get current time
    std::time_t currentTime = std::time(nullptr);
    struct tm* timeinfo = localtime(&currentTime);

    // Get logging level
    std::string levelStr = "INFO";
    switch (level)
    {
    case 0:
        levelStr = "INFO";
        break;
    case 1:
        levelStr = "ERROR";
        break;
    case 2: 
        levelStr = "DEBUG";
        break;
    default:
        break;
    }

    // Create timestamp
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Add entry to log file
    logFile_ << "[" << timestamp << "]" << levelStr << ": " << message << std::endl;

    // Flush on errors for immidiate visibility
    if (level == ERROR) {
        logFile_.flush();
    }

}

// Deconstructor to remove handle on log file to avoid handle leaks
Logger::~Logger() {
    if(logFile_.is_open()) {
        logFile_.close();
    }
}