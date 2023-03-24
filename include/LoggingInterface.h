#pragma once

#include "Logger.h"

namespace godzilla {

/// Logging interface to give objects capability to log problems like errors and warnings
///
class LoggingInterface {
public:
    explicit LoggingInterface(Logger * alogger, std::string aprefix = "") :
        logger(alogger),
        prefix(std::move(aprefix))
    {
    }

    /// Log an error
    template <typename... T>
    void
    log_error(fmt::format_string<T...> format, T... args)
    {
        this->logger->error(this->prefix, format, std::forward<T>(args)...);
    }

    /// Log a warning
    template <typename... T>
    void
    log_warning(fmt::format_string<T...> format, T... args)
    {
        this->logger->warning(this->prefix, format, std::forward<T>(args)...);
    }

protected:
    /// Logger object
    Logger * logger;

    /// Prefix for each logger line
    std::string prefix;
};

} // namespace godzilla
