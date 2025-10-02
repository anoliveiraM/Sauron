#ifndef CONSTS_H
#define CONSTS_H

#include <string>

// Logging constants
const std::string LOG_FOLDER_PATH = "/var/log/sauron";
const std::string LOG_FILE_PATH = "/var/log/sauron/sauron_logs";
const std::uint16_t LOG_FILE_PERMISSIONS = 0644; // rw-r--r--

// Config
const std::string ONBOARD_BLOB_PATH = "/opt/sauron/onboard.json";

#endif