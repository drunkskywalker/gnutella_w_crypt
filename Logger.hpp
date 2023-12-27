#include <fstream>
#include <iostream>
#include <string>

class Logger {
  std::string logFilePath;
  std::fstream logFile;

 public:
  Logger() : logFilePath("./log.txt"), logFile() {}
  Logger(std::string logFilePath) : logFilePath(logFilePath), logFile() {}
  bool init();
  bool log(std::string message, std::string type);
  bool logEvent(std::string event);
  bool logError(std::string error);

  ~Logger() {
    if (logFile.is_open()) {
      logFile.close();
    }
  }
};