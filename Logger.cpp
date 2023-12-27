#include "Logger.hpp"

bool Logger::init() {
  logFile.open(logFilePath, std::ios::out | std::ios::app);
  if (!logFile.is_open()) {
    return false;
  }
  return true;
}

bool Logger::log(std::string message, std::string type) {
  try {
    if (!logFile.is_open()) {
      return false;
    }
    int timestamp = time(NULL);
    logFile << "[" << type << "] "
            << "[" << timestamp << "] " << message << std::endl;
    return true;
  }
  catch (std::exception e) {
    std::cerr << "Exception in logging: " << e.what() << std::endl;
    return false;
  }
}

bool Logger::logEvent(std::string event) {
  return log(event, "EVENT");
}

bool Logger::logError(std::string error) {
  return log(error, "ERROR");
}
