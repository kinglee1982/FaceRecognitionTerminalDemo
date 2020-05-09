#ifndef LOGGER_H
#define LOGGER_H

#define SZ_LOG_LEVEL_TRACE 0
#define SZ_LOG_LEVEL_DEBUG 1
#define SZ_LOG_LEVEL_INFO 2
#define SZ_LOG_LEVEL_WARN 3
#define SZ_LOG_LEVEL_ERROR 4
#define SZ_LOG_LEVEL_CRITICAL 5
#define SZ_LOG_LEVEL_OFF 6

#define SZ_LOG_LEVEL SZ_LOG_LEVEL_DEBUG

#include <spdlog/spdlog.h>
namespace spd = spdlog;

#define SZ_LOG_TRACE(...) \
  SPDLOG_LOGGER_TRACE(suanzi::Logger::getInstance()->getLogger(), __VA_ARGS__)
#define SZ_LOG_DEBUG(...) \
  SPDLOG_LOGGER_DEBUG(suanzi::Logger::getInstance()->getLogger(), __VA_ARGS__)
#define SZ_LOG_INFO(...) \
  SPDLOG_LOGGER_INFO(suanzi::Logger::getInstance()->getLogger(), __VA_ARGS__)
#define SZ_LOG_WARN(...) \
  SPDLOG_LOGGER_WARN(suanzi::Logger::getInstance()->getLogger(), __VA_ARGS__)
#define SZ_LOG_ERROR(...) \
  SPDLOG_LOGGER_ERROR(suanzi::Logger::getInstance()->getLogger(), __VA_ARGS__)
#define SZ_LOG_CRITICAL(...)                                         \
  SPDLOG_LOGGER_CRITICAL(suanzi::Logger::getInstance()->getLogger(), \
                         __VA_ARGS__)

namespace suanzi {

class Logger {
 public:
  static Logger *getInstance();
  std::shared_ptr<spd::logger> getLogger() { return logger_; }

 private:
  Logger();

 private:
  std::shared_ptr<spd::logger> logger_;
};
}  // namespace suanzi

#endif
