#ifndef CONFIG_H
#define CONFIG_H

#include <mutex>
#include <optional>

struct DeviceInfo {
    std::string deviceId;
    std::string tenantId;
    std::string macAddress;
    std::string ipAddress;
    std::string deviceName;

    // Default constructor
    DeviceInfo() = default;

    // Constructor with all parameters
    DeviceInfo(const std::string& deviceId, 
               const std::string& tenantId,
               const std::string& macAddress,
               const std::string& ipAddress,
               const std::string& deviceName)
        : deviceId(deviceId), tenantId(tenantId), macAddress(macAddress), 
          ipAddress(ipAddress), deviceName(deviceName) {}

    // Check if all fields are populated
    bool isComplete() {
        return !deviceId.empty() && !tenantId.empty() && 
               !macAddress.empty() && !ipAddress.empty() && !deviceName.empty(); 
    }

    // Clear all device info
    void clear() {
        deviceId.clear();
        tenantId.clear();
        macAddress.clear();
        ipAddress.clear();
        deviceName.clear();
    }

    // Display device info for debugging
    std::string toString() const {
        return "DeviceID: " + deviceId + 
               ", TenantID: " + tenantId +
               ", MAC: " + macAddress +
               ", IP: " + ipAddress +
               ", Name: " + deviceName;
    }


};

struct OnboardingInfo {
    std::string deviceId;
    std::string tenantId;
};

class Config{
public:
    static Config& getInstance();
    bool onboardingStatus = false; // Default to false

    // Delete copy constructor and assignment operator -- Ensure that we don't duplicate instances
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // Onboarding status method
    bool isOnboarded() const;

    // Load onboarding info from onboarding file
    std::optional<OnboardingInfo> getOnboardingInfo();

    // Update all device info
    void updateDeviceInfo(const std::string& deviceId, 
                         const std::string& tenantId,
                         const std::string& macAddress, 
                         const std::string& ipAddress, 
                         const std::string& deviceName);

    // GetDeviceInfo if needed
    std::optional<DeviceInfo> getDeviceInfo() const;

    // Necessity methods to get MacAddress, IpAddress, DeviceName
    std::string getDeviceMacAddress() const;
    std::string getDeviceIpAddress() const;
    std::string getDeviceName() const;

    ~Config() = default;

private:
    Config();

    std::mutex configMutex_;
    std::optional<DeviceInfo> deviceInfo_;
};


#endif