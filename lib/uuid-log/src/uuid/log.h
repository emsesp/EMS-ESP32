/*
 * uuid-log - Microcontroller logging framework
 * Copyright 2019  Simon Arlott
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UUID_LOG_H_
#define UUID_LOG_H_

#include <Arduino.h>

#include <cstdarg>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <uuid/common.h>

namespace uuid {

/**
 * Logging framework.
 *
 * Provides a framework for handling log messages. This library is for
 * single threaded applications and cannot be used from an interrupt
 * context.
 *
 * - <a href="https://github.com/nomis/mcu-uuid-log/">Git Repository</a>
 * - <a href="https://mcu-uuid-log.readthedocs.io/">Documentation</a>
 */

// ANSI Colors - added by Proddy
// See https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html
#define COLOR_RESET "\x1B[0m"
#define COLOR_BLACK "\x1B[0;30m"
#define COLOR_RED "\x1B[0;31m"
#define COLOR_GREEN "\x1B[0;32m"
#define COLOR_YELLOW "\x1B[0;33m"
#define COLOR_BLUE "\x1B[0;34m"
#define COLOR_MAGENTA "\x1B[0;35m"
#define COLOR_CYAN "\x1B[0;36m"
#define COLOR_WHITE "\x1B[0;37m"
#define COLOR_BOLD_ON "\x1B[1m"
#define COLOR_BOLD_OFF "\x1B[22m"
#define COLOR_BRIGHT_BLACK "\x1B[0;90m"
#define COLOR_BRIGHT_RED "\x1B[0;91m"
#define COLOR_BRIGHT_GREEN "\x1B[0;92m"
#define COLOR_BRIGHT_YELLOW "\x1B[0;99m"
#define COLOR_BRIGHT_BLUE "\x1B[0;94m"
#define COLOR_BRIGHT_MAGENTA "\x1B[0;95m"
#define COLOR_BRIGHT_CYAN "\x1B[0;96m"
#define COLOR_BRIGHT_WHITE "\x1B[0;97m"
#define COLOR_UNDERLINE "\x1B[4m"
#define COLOR_BRIGHT_RED_BACKGROUND "\x1B[41;1m"

namespace log {

/**
 * Severity level of log messages.
 *
 * @since 1.0.0
 */
enum Level : int8_t {
    OFF   = -1, /*!< Meta level representing no log messages. @since 1.0.0 */
    EMERG = 0,  /*!< System is unusable. @since 1.0.0 */
    ALERT,      /*!< Action must be taken immediately. @since 1.0.0 */
    CRIT,       /*!< Critical conditions. @since 1.0.0 */
    ERR,        /*!< Error conditions. @since 1.0.0 */
    WARNING,    /*!< Warning conditions. @since 1.0.0 */
    NOTICE,     /*!< Normal but significant conditions. @since 1.0.0 */
    INFO,       /*!< Informational messages. @since 1.0.0 */
    DEBUG,      /*!< Debug-level messages. @since 1.0.0 */
    TRACE,      /*!< Trace messages. @since 1.0.0 */
    ALL,        /*!< Meta level representing all log messages. @since 1.0.0 */
};

/**
 * Facility type of the process logging a message.
 *
 *  @since 1.0.0
 */
enum Facility : uint8_t {
    KERN = 0, /*!< Kernel messages. @since 1.0.0 */
    USER,     /*!< User-level messages. @since 1.0.0 */
    MAIL,     /*!< Mail system. @since 1.0.0 */
    DAEMON,   /*!< System daemons. @since 1.0.0 */
    AUTH,     /*!< Security/authentication messages. @since 1.0.0 */
    SYSLOG,   /*!< Messages generated internally by logger. @since 1.0.0 */
    LPR,      /*!< Line printer subsystem. @since 1.0.0 */
    NEWS,     /*!< Network news subsystem. @since 1.0.0 */
    UUCP,     /*!< UUCP subsystem. @since 1.0.0 */
    CRON,     /*!< Clock daemon. @since 1.0.0 */
    AUTHPRIV, /*!< Security/authentication messages (private). @since 1.0.0 */
    FTP,      /*!< FTP daemon. @since 1.0.0 */
    NTP,      /*!< NTP subsystem. @since 1.0.0 */
    SECURITY, /*!< Log audit. @since 1.0.0 */
    CONSOLE,  /*!< Log alert. @since 1.0.0 */
    CRON2,    /*!< Scheduling daemon. @since 1.0.0 */
    LOCAL0,   /*!< Locally used facility 0. @since 1.0.0 */
    LOCAL1,   /*!< Locally used facility 1. @since 1.0.0 */
    LOCAL2,   /*!< Locally used facility 2. @since 1.0.0 */
    LOCAL3,   /*!< Locally used facility 3. @since 1.0.0 */
    LOCAL4,   /*!< Locally used facility 4. @since 1.0.0 */
    LOCAL5,   /*!< Locally used facility 5. @since 1.0.0 */
    LOCAL6,   /*!< Locally used facility 6. @since 1.0.0 */
    LOCAL7,   /*!< Locally used facility 7. @since 1.0.0 */
};

/**
 * Format a system uptime timestamp as a string.
 *
 * Using the format "d+HH:mm:ss.SSS" with leading zeros for the days.
 *
 * @param[in] timestamp_ms System uptime in milliseconds, see uuid::get_uptime_ms().
 * @param[in] days_width Leading zeros for the days part of the output.
 * @return String with the formatted system uptime.
 * @since 1.0.0
 */
std::string format_timestamp_ms(uint64_t timestamp_ms, unsigned int days_width = 1);

/**
 * Get all log levels.
 *
 * @return A list of all log levels in priority order from
 *         uuid::log::Level::OFF to uuid::log::Level::ALL.
 * @since 2.1.0
 */
std::vector<Level> levels();

/**
 * Format a log level as a single character.
 *
 * Level::EMERG is represented as 'P' because it conflicts with
 * Level::ERR and it used to be the "panic" level.
 *
 * @param[in] level Log level.
 * @return Single character uppercase representation of the log level.
 * @since 1.0.0
 */
char format_level_char(Level level);

/**
 * Format a log level as an uppercase string.
 *
 * @param[in] level Log level.
 * @return Uppercase name of the log level (flash string).
 * @since 1.0.0
 */
const __FlashStringHelper * format_level_uppercase(Level level);

/**
 * Get all log levels as uppercase strings.
 *
 * @return A list of all log levels in priority order from
 *         uuid::log::Level::OFF to uuid::log::Level::ALL
 *         as uppercase strings.
 * @since 2.1.0
 */
std::vector<std::string> levels_uppercase();

/**
 * Parse an uppercase string to a log level.
 *
 * @param[in] name Uppercase name of the log level.
 * @param[out] level Log level.
 * @return True if the named level is valid, otherwise false.
 * @since 2.1.0
 */
bool parse_level_uppercase(const std::string & name, Level & level);

/**
 * Format a log level as a lowercase string.
 *
 * @param[in] level Log level.
 * @return Lowercase name of the log level (flash string).
 * @since 1.0.0
 */
const __FlashStringHelper * format_level_lowercase(Level level);

/**
 * Get all log levels as lowercase strings.
 *
 * @return A list of all log levels in priority order from
 *         uuid::log::Level::OFF to uuid::log::Level::ALL
 *         as lowercase strings.
 * @since 2.1.0
 */
std::vector<std::string> levels_lowercase();

/**
 * Parse a lowercase string to a log level.
 *
 * @param[in] name Lowercase name of the log level.
 * @param[out] level Log level.
 * @return True if the named level is valid, otherwise false.
 * @since 2.1.0
 */
bool parse_level_lowercase(const std::string & name, Level & level);

/**
 * Log message text with timestamp and logger attributes.
 *
 * These will be created when a message is logged and then passed to
 * all registered handlers.
 *
 * @since 1.0.0
 */
struct Message {
    /**
	 * Create a new log message (not directly useful).
	 *
	 * @param[in] uptime_ms System uptime, see uuid::get_uptime_ms().
	 * @param[in] level Severity level of the message.
	 * @param[in] facility Facility type of the process logging the message.
	 * @param[in] name Logger name (flash string).
	 * @param[in] text Log message text.
	 * @since 1.0.0
	 */
    Message(uint64_t uptime_ms, Level level, Facility facility, const __FlashStringHelper * name, const std::string && text);
    ~Message() = default;

    /**
	 * System uptime at the time the message was logged.
	 *
	 * @see uuid::get_uptime_ms()
	 * @since 1.0.0
	 */
    const uint64_t uptime_ms;

    /**
	 * Severity level of the message.
	 *
	 * @since 1.0.0
	 */
    const Level level;

    /**
	 * Facility type of the process that logged the message.
	 *
	 * @since 1.0.0
	 */
    const Facility facility;

    /**
	 * Name of the logger used (flash string).
	 *
	 * @since 1.0.0
	 */
    const __FlashStringHelper * name;

    /**
	 * Formatted log message text.
	 *
	 * Does not include any of the other message attributes, those must
	 * be added by the handler when outputting messages.
	 *
	 * @since 1.0.0
	 */
    const std::string text;
};

/**
 * Logger handler used to process log messages.
 *
 * @since 1.0.0
 */
class Handler {
  public:
    virtual ~Handler() = default;

    /**
	 * Add a new log message.
	 *
	 * This should normally be put in a queue instead of being
	 * processed immediately so that log messages have minimal impact
	 * at the time of use.
	 *
	 * Queues should have a maximum size and discard the oldest message
	 * when full.
	 *
	 * @param[in] message New log message, shared by all handlers.
	 * @since 1.0.0
	 */
    virtual void operator<<(std::shared_ptr<Message> message) = 0;

  protected:
    Handler() = default;
};

/**
 * Logger instance used to make log messages.
 *
 * @since 1.0.0
 */
class Logger {
  public:
/**
	 * This is the maximum length of any log message.
	 *
	 * Determines the size of the buffer used for format string
	 * printing.
	 *
	 * @since 1.0.0
	 */
#if defined(EMSESP_STANDALONE)
    static constexpr size_t MAX_LOG_LENGTH = 500;
#else
    static constexpr size_t MAX_LOG_LENGTH = 255;
#endif

    /**
	 * Create a new logger with the given name and logging facility.
	 *
	 * @param[in] name Logger name (flash string).
	 * @param[in] facility Default logging facility for messages.
	 *
	 * @since 1.0.0
	 */
    Logger(const __FlashStringHelper * name, Facility facility = Facility::LOCAL0);
    ~Logger() = default;

    /**
	 * Register a log handler.
	 *
	 * Call again to change the log level.
	 *
	 * Do not call this function from a static initializer.
	 *
	 * @param[in] handler Handler object that will handle log
	 *                    messages.
	 * @param[in] level Minimum log level that the handler is
	 *                  interested in.
	 * @since 1.0.0
	 */
    static void register_handler(Handler * handler, Level level);

    /**
	 * Unregister a log handler.
	 *
	 * It is safe to call this with a handler that is not registered.
	 *
	 * Do not call this function from a static initializer.
	 *
	 * @param[in] handler Handler object that will no longer handle
	 *                    log messages.
	 * @since 1.0.0
	 */
    static void unregister_handler(Handler * handler);

    /**
	 * Get the current log level of a handler.
	 *
	 * It is safe to call this with a handler that is not registered.
	 *
	 * Do not call this function from a static initializer.
	 *
	 * @param[in] handler Handler object that may handle log
	 *                    messages.
	 * @return The current log level of the specified handler.
	 * @since 1.0.0
	 */
    static Level get_log_level(const Handler * handler);

    /**
	 * Determine if the current log level is enabled by any registered
	 * handlers.
	 *
	 * @return The current minimum global log level across all
	 *         handlers.
	 * @since 1.0.0
	 */
    static inline bool enabled(Level level) {
        return level <= level_;
    }

    /**
	 * Log a message at level Level::EMERG.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void emerg(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::EMERG.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void emerg(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::ALERT.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void alert(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::ALERT.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void alert(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::CRIT.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void crit(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::CRIT.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void crit(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::ERR.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void err(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::ERR.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void err(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::WARNING.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void warning(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::WARNING.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void warning(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::NOTICE.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void notice(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::NOTICE.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void notice(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::INFO.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void info(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::INFO.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 */
    void info(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::DEBUG.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void debug(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::DEBUG.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void debug(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message at level Level::TRACE.
	 *
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void trace(const char * format, ...) const /* __attribute__((format (printf, 2, 3))) */;
    /**
	 * Log a message at level Level::TRACE.
	 *
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void trace(const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 2, 3))) */;

    /**
	 * Log a message with a custom facility.
	 *
	 * @param[in] level Severity level of the message.
	 * @param[in] facility Facility type of the process logging the message.
	 * @param[in] format Format string.
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void log(Level level, Facility facility, const char * format, ...) const /* __attribute__((format (printf, 3, 4))) */;
    /**
	 * Log a message with a custom facility.
	 *
	 * @param[in] level Severity level of the message.
	 * @param[in] facility Facility type of the process logging the message.
	 * @param[in] format Format string (flash string).
	 * @param[in] ... Format string arguments.
	 * @since 1.0.0
	 */
    void log(Level level, Facility facility, const __FlashStringHelper * format, ...) const /* __attribute__((format (printf, 4, 5))) */;

  private:
    /**
	 * Refresh the minimum global log level across all handlers.
	 *
	 * @since 1.0.0
	 */
    static void refresh_log_level();

    /**
	 * Log a message at the specified level.
	 *
	 * @param[in] level Severity level of the message.
	 * @param[in] format Format string.
	 * @param[in] ap Variable arguments pointer for format string.
	 * @since 1.0.0
	 */
    void vlog(Level level, const char * format, va_list ap) const;
    /**
	 * Log a message at the specified level.
	 *
	 * @param[in] level Severity level of the message.
	 * @param[in] format Format string (flash string).
	 * @param[in] ap Variable arguments pointer for format string.
	 * @since 1.0.0
	 */
    void vlog(Level level, const __FlashStringHelper * format, va_list ap) const;

    /**
	 * Log a message at the specified level and facility.
	 *
	 * @param[in] level Severity level of the message.
	 * @param[in] facility Facility type of the process logging the message.
	 * @param[in] format Format string.
	 * @param[in] ap Variable arguments pointer for format string.
	 * @since 1.0.0
	 */
    void vlog(Level level, Facility facility, const char * format, va_list ap) const;
    /**
	 * Log a message at the specified level and facility.
	 *
	 * @param[in] level Severity level of the message.
	 * @param[in] facility Facility type of the process logging the message.
	 * @param[in] format Format string (flash string).
	 * @param[in] ap Variable arguments pointer for format string.
	 * @since 1.0.0
	 */
    void vlog(Level level, Facility facility, const __FlashStringHelper * format, va_list ap) const;

    /**
	 * Dispatch a log message to all handlers that are registered to
	 * handle messages of the specified level.
	 *
	 * Automatically sets the timestamp of the message to the current
	 * system uptime.
	 *
	 * @param[in] level Severity level of the message.
	 * @param[in] facility Facility type of the process logging the message.
	 * @param[in] text Log message text.
	 * @since 1.0.0
	 */
    void dispatch(Level level, Facility facility, std::vector<char> & text) const;

    static std::map<Handler *, Level> handlers_; /*!< Registered log handlers. @since 1.0.0 */
    static Level                      level_;    /*!< Minimum global log level across all handlers. @since 1.0.0 */

    const __FlashStringHelper * name_;     /*!< Logger name (flash string). @since 1.0.0 */
    const Facility              facility_; /*!< Default logging facility for messages. @since 1.0.0 */
};

} // namespace log

} // namespace uuid

#endif
