#include <filesystem>
#include <iostream>
#include <sys/stat.h>


class FileUtils {

public:
    static bool createDirectoryIfNotExists(const std::string& pathToFolder);
    static bool createFileIfNotExists(const std::string& pathToFile, const std::uint16_t filePermissions);
};