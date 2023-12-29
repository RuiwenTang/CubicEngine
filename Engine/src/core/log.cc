#include <cubic/core/log.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <vector>

namespace cubic {

Logger* Logger::Get() {
  static Logger* g_instance = nullptr;
  if (g_instance == nullptr) {
    g_instance = new Logger();

    g_instance->Init();
  }

  return g_instance;
}

void Logger::Init() {
  std::vector<spdlog::sink_ptr> log_sinks{};

  log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

  log_sinks[0]->set_pattern("%^[%T] %n: %v%$");

  mLogger = std::make_shared<spdlog::logger>("[CUBIC]", log_sinks.begin(), log_sinks.end());
  mLogger->set_level(spdlog::level::trace);
  mLogger->flush_on(spdlog::level::trace);

  spdlog::register_logger(mLogger);
}

}  // namespace cubic
