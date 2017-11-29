#pragma once

// /esp32-hal-log.h
#include <esp32-hal-log.h>

struct Logger {
    Logger(const char * tag) : tag(tag) {
    }

    template <typename ...Args> void error(const char * fmt, const Args & ...args) { esp_log_write(ESP_LOG_ERROR, tag, fmt, args...); }
    template <typename ...Args> void warn(const char * fmt, const Args & ...args) { esp_log_write(ESP_LOG_WARN, tag, fmt, args...); }
    template <typename ...Args> void info(const char * fmt, const Args & ...args) { esp_log_write(ESP_LOG_INFO, tag, fmt, args...); }
    template <typename ...Args> void debuf(const char * fmt, const Args & ...args) { esp_log_write(ESP_LOG_DEBUG, tag, fmt, args...); }
        
private:
    const char * tag;
};
