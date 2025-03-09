#pragma once

#include <map>
#include <string>

typedef std::map<std::string, std::string> ConfigStringMap;

ConfigStringMap config_file_read(const char* const path, bool swapkv = false);
