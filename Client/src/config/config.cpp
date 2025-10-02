#include "config.h"
#include "../consts.h"
#include "../utils/logging.h"

#include <json.hpp>
#include <fstream>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>


// Return instance of singleton
Config& Config::getInstance() {
    static Config instance;
    static bool initialized = false;
    if (!initialized) {
        // Log initialization
        Logger::getInstance().log(Logger::INFO, "Initializing Config singleton!");
        instance.fetchDeviceInfo();
        instance.fetchOnboardingInfo();
        initialized = true;
    }
    return instance;
}

Config::Config() = default;

void Config::updateDeviceInfo(const NetworkInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);
    deviceInfo_ = info; 
}

std::optional<NetworkInfo> Config::getDeviceInfo() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return deviceInfo_;
}

std::optional<OnboardingInfo> Config::getOnboardingInfo() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return onboardingInfo_;
}

void Config::fetchOnboardingInfo() {
    try {
        OnboardingInfo info;

        std::ifstream ifs(ONBOARD_BLOB_PATH);
        if(!ifs.is_open()) {
            Logger::getInstance().log(Logger::ERROR, "Failed to open onboarding blob path: " + ONBOARD_BLOB_PATH);
        }

        // Start reading from blob
        Logger::getInstance().log(Logger::INFO, "Getting onboarding json from file: " + ONBOARD_BLOB_PATH);
        nlohmann::json jreader;
        ifs >> jreader;

        // Read the data into the object
        OnboardingInfo onboardingBlob {
            jreader["TenantId"].get<std::string>(),
            jreader["MachineId"].get<std::string>()
        };

        onboardingInfo_ = onboardingBlob;
    } catch (std::exception& ex) {
        Logger::getInstance().log(Logger::ERROR, "Unexpected error while reading json blob from opt/sauron");
    }
}

NetworkInfo Config::getDeviceNetworkInfo(const std::string& interface) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        Logger::getInstance().log(Logger::ERROR, "Failed to open file descriptor for socket! Returning empty string");
        return NetworkInfo{}; // Returning default constructor in case of error
    }

    NetworkInfo netInfo;

    struct ifreq ifr; 
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);

    if(ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
        close(fd);
        return NetworkInfo{};
    }

    // Start reading MacAddress
    unsigned char* mac = (unsigned char*) ifr.ifr_hwaddr.sa_data;
    char buffer[18];

    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Set netInfo from buffer
    netInfo.macAddress = std::string(buffer);

    // Now grab IpAddress
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);

    if(ioctl(fd , SIOCGIFHWADDR, &ifr) < 0) {
        close(fd);
        return netInfo;
    }

    // Start reading ipAddress
    netInfo.ipAddress = inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);

    // Now get HostName for the device
    char hostNameBuff[256];
    if (gethostname(hostNameBuff, sizeof(hostNameBuff)) < 0 ) {
        Logger::getInstance().log(Logger::ERROR, "Failed to grab hostname");
        return netInfo;
    }

    netInfo.deviceName = hostNameBuff;

    // Close file descriptor to avoid locking file
    close(fd);

    return netInfo;
}

void Config::fetchDeviceInfo() {
    try {
        Logger::getInstance().log(Logger::INFO, "Initiating flow to grab device info");

        // std::string macAddress;
        // std::string ipAddress;
        // std::string deviceName;
        
        // Start with MacAddress


    } catch (std::exception& ex) {
        Logger::getInstance().log(Logger::ERROR, "Unexpected error occurred while getting device info");
    }
}