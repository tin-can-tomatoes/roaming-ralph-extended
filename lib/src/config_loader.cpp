#include <config_loader.h>

#include <fstream>
#include <stdexcept>

static const char* const whitespace = " \t\r\n";

// Read a config file.
// TODO: Need to have this scan a number of directories
// for the best candidate.
// something like:
//  ./configfile > ~/.config/game/configfile > /etc/configfile
ConfigStringMap config_file_read(const char* const path, bool swapkv)
{
  ConfigStringMap map;

  std::ifstream file(path);
  if (!file)
  {
    throw std::runtime_error("Config file " + std::string(path) + " not found");
  }

  std::string line;
  while (std::getline(file, line))
  {
    line.erase(0, line.find_first_not_of(whitespace));
    line.erase(line.find_last_not_of(whitespace) + 1);
    if (line.empty())
    {
      continue;
    }
    std::size_t posn = line.find_first_of(":=");
    if (posn == std::string::npos)
    {
      throw std::runtime_error(
          "Config file " + std::string(path) + " had malformed line"
      );
    }
    std::string key = line.substr(0, posn);
    std::string val = line.substr(posn + 1);
    key.erase(key.find_last_not_of(whitespace) + 1);
    val.erase(0, val.find_first_not_of(whitespace));

    if (key.empty() || val.empty())
    {
      throw std::runtime_error(
          "Config file " + std::string(path) + " had an empty key or value"
      );
    }
    if (swapkv)
    {
      map[val] = key;
    }
    else
    {

      map[key] = val;
    }
  }

  file.close();
  return map;
}
