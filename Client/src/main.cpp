#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <cstring>

#include "utils/logging.h"
#include "config/config.h" // Assume this will be created later
// #include "metrics/collector.h" // Assume this will be created later
//#include "logs/monitor.h" // Assume this will be created later
// #include "management/executor.h" // Assume this will be created later

// Global shutdown flag
volatile sig_atomic_t shutdown_flag = 0;

void signal_handler(int sig) {
    shutdown_flag = 1;
}

int main (int argc, char* argv[]) {
    // Initialize logging
    Logger& logger = Logger::getInstance();
    if(!logger.isInitialized_) {
        std::cerr << "Logger initialization failed, exiting." << std::endl;
        return 1;
    }

    logger.log(Logger::INFO, "Sauron getting started....");

    // Parse command-line flags
    bool debug_mode = false;
    for (int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "--debug") == 0) {
            debug_mode = true;
            break;
        }
    }

    // Setup signal handlers 
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // If not in debug let's daemonize
    if(!debug_mode) {
        pid_t pid = fork();
        if(pid < 0) {
            std::cerr << "Fork failed, pid: " + pid << std::endl;
            return 1;
        }

        if (pid > 1) {
            return 0; // Parent exists
        }

        setsid(); // Create a new session
        // Close standard file descriptors (optional for now)
        // freopen("/dev/null", "r", stdin);
        // freopen("/dev/null", "w", stdout);
        // freopen("/dev/null", "w", stderr);
    }

    // Initialize config
    Config::getInstance()->load("config.json");

    // Create and start threads
    std::thread metrics_thread([]() {
        MetricsCollector collector
        while(!shutdown_flag) {
            collector.collectAndPush();
            sleep(5); // 5s interval between each time we grab metrics
        }
    });

    // Implement logs thread 
    // Something like bellow this is not important right now!
    // std::thread logs_thread([]() {
    //     LogMonitor monitor;
    //     while (!shutdown_flag) {
    //         monitor.watch();
    //         sleep(30); // 30s poll for <1min latency
    //     }
    // });


    std::thread management_thread([]() {
        ManagementExecutor executor;
        while(!shutdown_flag) {
            executor.listen();
            sleep(15) // Pull CnC command every 5 seconds
        }
    });


    // Main thread waits for shutdown
    while(!shutdown_flag) {
        sleep(1);
    }


    // Join threads on shutdown
    metrics_thread.join();
    // logs_thread.join();
    management_thread.join();
    
    return 0;

}