#pragma once

#include "Logger.h"

namespace godzilla {

/// Logging interface to give objects capability to log problems like errors and warnings
///
class LoggingInterface {
public:
    LoggingInterface(Logger * alogger, std::string aprefix = "") : logger(alogger), prefix(aprefix)
    {
    }

    /// Log an error
    template <typename... Args>
    void
    log_error(const char * s, Args... args)
    {
        this->logger->error(this->prefix, s, std::forward<Args>(args)...);
    }

    /// Log a warning
    template <typename... Args>
    void
    log_warning(const char * s, Args... args)
    {
        this->logger->warning(this->prefix, s, std::forward<Args>(args)...);
    }

protected:
    /// Logger object
    Logger * logger;

    /// Prefix for each logger line
    std::string prefix;
};

} // namespace godzilla
