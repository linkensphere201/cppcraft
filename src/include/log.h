#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace cp1craft {
namespace common {
using LoggerPtr = std::shared_ptr<spdlog::logger>;
extern std::shared_ptr<spdlog::logger> g_console;

void init_log_module();

} // namespace common
} // namespace cp1craft

#define log_warn(VAL, ...) \
  if (cp1craft::common::g_console.get()) { cp1craft::common::g_console->warn("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__); }
#define log_error(VAL, ...)  \
  if (cp1craft::common::g_console.get()) { cp1craft::common::g_console->error("[{}:{}] " VAL,__FILE__, __LINE__, ##__VA_ARGS__); }
#define log_info(VAL, ...) \
  if (cp1craft::common::g_console.get()) { cp1craft::common::g_console->info("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__); }
#define log_debug(VAL, ...) \
  if (cp1craft::common::g_console.get()) { cp1craft::common::g_console->debug("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__); }

