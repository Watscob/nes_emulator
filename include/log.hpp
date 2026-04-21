#ifndef LOG_HPP
#define LOG_HPP

#include <format>
#include <print>
#include <string_view>

class Logger
{
  public:
    enum class LogLevel
    {
        QUIET,
        ERROR,
        INFO,
        DEBUG
    };

    void set_level(LogLevel level) { level_ = level; }

    template <typename... Args>
    void print_log(LogLevel level, std::format_string<Args...> fmt, Args&&... args)
    {
        if (level > level_)
            return;

        std::print("{}[{}] ", level_to_color(level), level_to_string(level));
        std::print(fmt, std::forward<Args>(args)...);
        std::println("{}", COLOR_RESET);
    }

    static Logger& get()
    {
        static Logger s_logger;
        return s_logger;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

  private:
    LogLevel level_;

    static constexpr std::string_view COLOR_RESET = "\033[0m";
    static constexpr std::string_view COLOR_RED = "\033[31m";
    static constexpr std::string_view COLOR_GREEN = "\033[32m";
    static constexpr std::string_view COLOR_GREY = "\033[90m";

    Logger()
        : level_(LogLevel::QUIET)
    {
    }

    std::string_view level_to_string(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::ERROR:
            return "ERROR   ";
        case LogLevel::INFO:
            return "INFO    ";
        case LogLevel::DEBUG:
            return "DEBUG   ";
        default:
            return "UNKNOWN ";
        }
    }

    std::string_view level_to_color(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::ERROR:
            return COLOR_RED;
        case LogLevel::INFO:
            return COLOR_GREEN;
        case LogLevel::DEBUG:
            return COLOR_GREY;
        default:
            return COLOR_RESET;
        }
    }
};

inline void set_log_level(Logger::LogLevel level)
{
    Logger::get().set_level(level);
}

template <typename... Args>
inline void log_error(std::format_string<Args...> fmt, Args&&... args)
{
    Logger::get().print_log(Logger::LogLevel::ERROR, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void log_info(std::format_string<Args...> fmt, Args&&... args)
{
    Logger::get().print_log(Logger::LogLevel::INFO, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void log_debug(std::format_string<Args...> fmt, Args&&... args)
{
    Logger::get().print_log(Logger::LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
}

#endif /* LOG_HPP */
