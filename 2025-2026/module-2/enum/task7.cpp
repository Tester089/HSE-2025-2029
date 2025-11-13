#include <iostream>
#include <string>
#include <vector>


enum class LogLevel {
  DEBUG=0,
  INFO=1,
  WARNING=2,
  ERROR=3,
  CRITICAL=4
};

enum class LogCategory {
  NETWORK=1,
  DATABASE=1<<1,
  UI=1<<2,
  SECURITY=1<<3,
  PERFORMANCE=1<<4
};


struct LogEntry {
  LogLevel level;
  LogCategory category;
  std::string message;
};

LogCategory operator & (LogCategory a, LogCategory b) {
  return static_cast<LogCategory>(static_cast<int>(a)&static_cast<int>(b));
}

LogCategory operator | (LogCategory a, LogCategory b) {
  return static_cast<LogCategory>(static_cast<int>(a)|static_cast<int>(b));
}

LogLevel operator < (LogLevel a, LogLevel b) {
  return static_cast<LogLevel>(static_cast<int>(a)<static_cast<int>(b));
}

bool filterLogs(LogEntry &piece_of_info, LogLevel level, LogCategory category) {

  if (static_cast<bool>(piece_of_info.category & category) == 0) return false;

  if (static_cast<bool>(piece_of_info.level < level)) return false;

  return true;
}

void printLog(LogEntry &log) {
  switch (log.level) {
    case LogLevel::DEBUG :
      std::cout << "[DEBUG] ";
      break;
    case LogLevel::INFO :
      std::cout << "[INFO] ";
      break;
    case LogLevel::WARNING :
      std::cout << "[WARNING] ";
      break;
    case LogLevel::ERROR :
      std::cout << "[ERROR] ";
      break;
    case LogLevel::CRITICAL :
      std::cout << "[CRITICAL] ";
      break;
  }

  switch (log.category) {
    case LogCategory::DATABASE :
      std::cout << "<DATABASE>\t";
      break;
    case LogCategory::NETWORK :
      std::cout << "<NETWORK>\t";
      break;
    case LogCategory::UI :
      std::cout << "<UI>\t";
      break;
    case LogCategory::SECURITY :
      std::cout << "<SECURITY>\t";
      break;
    case LogCategory::PERFORMANCE :
      std::cout << "<PERFORMANCE>\t";
      break;
  }
  std::cout << log.message << "\n";
}

int main() {

  LogEntry log1 = {LogLevel::INFO, LogCategory::NETWORK, "Соединение установлено"};
  LogEntry log2 = {LogLevel::ERROR, LogCategory::DATABASE, "Не удалось выполнить запрос"};
  LogEntry log3 = {LogLevel::DEBUG, LogCategory::UI, "Кнопка нажата"};
  LogEntry log4 = {LogLevel::CRITICAL, LogCategory::SECURITY, "Попытка несанкционированного доступа"};

  LogLevel minLevel = LogLevel::WARNING;
  LogCategory activeCategories = LogCategory::DATABASE | LogCategory::SECURITY;


  if (filterLogs(log1, minLevel, activeCategories)) printLog(log1);
  if (filterLogs(log2, minLevel, activeCategories)) printLog(log2);
  if (filterLogs(log3, minLevel, activeCategories)) printLog(log3);
  if (filterLogs(log4, minLevel, activeCategories)) printLog(log4);


}