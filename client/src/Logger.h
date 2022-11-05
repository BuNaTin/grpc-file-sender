#pragma once

#include <spdlog/spdlog.h>

#ifdef NO_LOG

#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGC(...)

#else

#define LOGI(...) spdlog::info(__VA_ARGS__)
#define LOGW(...) spdlog::warn(__VA_ARGS__)
#define LOGE(...) spdlog::error(__VA_ARGS__)
#define LOGC(...) spdlog::critical(__VA_ARGS__)

#endif // DEBUG


// RAII for nice debug
struct LoggerDebug {
    LoggerDebug() : m_cur_level(spdlog::get_level()) {
        spdlog::set_level(spdlog::level::debug);
    }
    ~LoggerDebug() { spdlog::set_level(m_cur_level); }

private:
    spdlog::level::level_enum m_cur_level;
};
