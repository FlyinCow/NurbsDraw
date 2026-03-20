#pragma once

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <string>

// Simple logging system with levels
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

class Logger {
public:
    static LogLevel level;
    
    static void set_level(LogLevel lvl) {
        level = lvl;
    }
    
    static void debug(const char* fmt, ...) {
        if (level <= LogLevel::DEBUG) {
            va_list args;
            va_start(args, fmt);
            log("DEBUG", fmt, args);
            va_end(args);
        }
    }
    
    static void info(const char* fmt, ...) {
        if (level <= LogLevel::INFO) {
            va_list args;
            va_start(args, fmt);
            log("INFO", fmt, args);
            va_end(args);
        }
    }
    
    static void warning(const char* fmt, ...) {
        if (level <= LogLevel::WARNING) {
            va_list args;
            va_start(args, fmt);
            log("WARNING", fmt, args);
            va_end(args);
        }
    }
    
    static void error(const char* fmt, ...) {
        if (level <= LogLevel::ERROR) {
            va_list args;
            va_start(args, fmt);
            log("ERROR", fmt, args);
            va_end(args);
        }
    }
    
private:
    static void log(const char* level_str, const char* fmt, va_list args) {
        time_t now = time(nullptr);
        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localtime(&now));
        
        printf("[%s] [%s] ", timestamp, level_str);
        vprintf(fmt, args);
        printf("\n");
    }
};

LogLevel Logger::level = LogLevel::INFO;

// Convenience macros
#define LOG_DEBUG(...) Logger::debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::error(__VA_ARGS__)
