#pragma once

#include <cubic/platform.h>
#include <spdlog/spdlog.h>

namespace cubic {

class CUBIC_API Logger {
 public:
  ~Logger() = default;

  spdlog::logger* GetLogger() const { return mLogger.get(); }

  static Logger* Get();

  void Init();

 private:
  Logger() = default;

  std::shared_ptr<spdlog::logger> mLogger = {};
};

}  // namespace cubic

#define CUB_INFO(...) ::cubic::Logger::Get()->GetLogger()->info(__VA_ARGS__)
#define CUB_WARR(...) ::cubic::Logger::Get()->GetLogger()->warn(__VA_ARGS__)
#define CUB_ERROR(...) ::cubic::Logger::Get()->GetLogger()->error(__VA_ARGS__)
