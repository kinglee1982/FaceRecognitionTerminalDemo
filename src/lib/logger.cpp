#include "logger.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace suanzi;

Logger *Logger::getInstance() {
  static Logger self;
  return &self;
}

Logger::Logger() {
  auto level = spd::level::level_enum(SZ_LOG_LEVEL_DEBUG);

  auto consoleLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleLogger->set_level(level);
  auto rotatingLogger = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      "logs/quface.log", 1048576 * 10, 5);
  rotatingLogger->set_level(level);

  spd::details::registry::instance().flush_on(level);
  spd::details::registry::instance().flush_every(std::chrono::seconds(5));

  spdlog::sinks_init_list sinks = {consoleLogger, rotatingLogger};
  logger_ = std::make_shared<spdlog::logger>("quface", sinks);
  logger_->set_level(level);
}
