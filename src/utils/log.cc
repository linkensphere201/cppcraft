#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "log.h"

namespace cp1craft {
namespace utils {

std::shared_ptr<spdlog::logger> g_console;
void init_log_module() {
  g_console = spdlog::stdout_color_mt("console");
  spdlog::set_default_logger(g_console);
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [thread %t] %l %v");
}
} // namespace utils
} // namespace cp1craft
