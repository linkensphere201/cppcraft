#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace cp1craft {
namespace utils {
using LoggerPtr = std::shared_ptr<spdlog::logger>;
extern std::shared_ptr<spdlog::logger> g_console;

void init_log_module();

} // namespace utils
} // namespace cp1craft

#define log_warn(VAL, ...) \
  if (cp1craft::utils::g_console.get()) { cp1craft::utils::g_console->warn("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__); }
#define log_error(VAL, ...)  \
  if (cp1craft::utils::g_console.get()) { cp1craft::utils::g_console->error("[{}:{}] " VAL,__FILE__, __LINE__, ##__VA_ARGS__); }
#define log_info(VAL, ...) \
  if (cp1craft::utils::g_console.get()) { cp1craft::utils::g_console->info("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__); }
#define log_debug(VAL, ...) \
  if (cp1craft::utils::g_console.get()) { cp1craft::utils::g_console->debug("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__); }

