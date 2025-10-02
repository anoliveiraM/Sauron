#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <optional>
#include <mutex>

struct OnboardingInfo {
    std::string deviceId;
    std::string tenantId;
};

struct NetworkInfo {
    std::string macAddress;
    std::string ipAddress;
    std::string deviceName;
};

class Config {
public:
    static Config& getInstance();
    void updateDeviceInfo(const NetworkInfo& info);
    std::optional<NetworkInfo> getDeviceInfo() const;
    std::optional<OnboardingInfo> getOnboardingInfo() const;
    void fetchDeviceInfo();
    void fetchOnboardingInfo();
    NetworkInfo getDeviceNetworkInfo(const std::string& interface);
    std::string getDeviceHostName(const std::string& interface);



private:
    Config() = default;
    mutable std::mutex mutex_;
    std::optional<NetworkInfo> deviceInfo_;
    std::optional<OnboardingInfo> onboardingInfo_;
};
#endif