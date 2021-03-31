#pragma once

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

namespace cp1craft {
namespace utils {
using LoggerPtr = std::shared_ptr<spdlog::logger>;
extern std::shared_ptr<spdlog::logger> g_console;

void init_log_module();

} // namespace utils
} // namespace cp1craft

#define do_log(LLVL, LOGGER, VAL, ...)                                         \
  if (LOGGER) {                                                                \
    LOGGER->LLVL("[{}:{}] " VAL, __FILE__, __LINE__, ##__VA_ARGS__);           \
  }

#define log_warn(VAL, ...)                                                     \
  do_log(warn, cp1craft::utils::g_console.get(), VAL, ##__VA_ARGS__);

#define log_error(VAL, ...)                                                    \
  do_log(error, cp1craft::utils::g_console.get(), VAL, ##__VA_ARGS__);

#define log_info(VAL, ...)                                                     \
  do_log(info, cp1craft::utils::g_console.get(), VAL, ##__VA_ARGS__);

#define log_debug(VAL, ...)                                                    \
  do_log(debug, cp1craft::utils::g_console.get(), VAL, ##__VA_ARGS__);

