#include <./fileUtils.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>

bool FileUtils::createDirectoryIfNotExists(const std::string& directoryPath) {

    try {
        // Check if the directory exists
        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
            return true;
        }

        // Create directory
        return std::filesystem::create_directories(directoryPath);
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Error creating directory: " << directoryPath << ": " << ex.what() << std::endl;
        return false;
    }

}

bool FileUtils::createFileIfNotExists(const std::string& filePath, const std::uint16_t filePermissions) {
    try {
        // Check if file exists
        if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)) {
            return true;
        }

        // Extract folderPath
        std::filesystem::path path(filePath);
        std::string parentDir = path.parent_path().string();

        // Create paret directory if does not exist
        if(!parentDir.empty() && !createDirectoryIfNotExists(parentDir)){
            return false;
        }

        // Create file
        std::ofstream file(filePath);
        if(!file.is_open()) {
            std::cerr << "Failed to create file: " << filePath << std::endl;
            return false;
        }

        // Else we created the file so close handle to it
        file.close();

        // Set the file permissions
        chmod(filePath.c_str(), filePermissions);

        return true;
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << "Unexpected error when creating file: " << filePath  << ": " << ex.what() << std::endl;
    } 
}