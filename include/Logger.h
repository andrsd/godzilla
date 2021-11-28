#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "Terminal.h"

namespace godzilla {

/// Class for loggin errors and warnings
class Logger {
public:
    Logger();

    /// Log an error
    template <typename... Args>
    void
    error(Args &&... args)
    {
        std::ostringstream oss;
        streamAll(oss, COLOR_RED, "error: ", COLOR_DEFAULT, std::forward<Args>(args)...);
        this->entries.push_back(oss.str());
        this->num_errors++;
    }

    /// Log a warning
    template <typename... Args>
    void
    warning(Args &&... args)
    {
        std::ostringstream oss;
        streamAll(oss, COLOR_MAGENTA, "warning: ", COLOR_DEFAULT, std::forward<Args>(args)...);
        this->entries.push_back(oss.str());
        this->num_warnings++;
    }

    /// Get the number of logged errors/warnings
    ///
    /// @return Number of logger errors/warnings
    std::size_t getNumEntries() const;

    /// Get the number of errors
    ///
    /// @return Number of errors
    std::size_t getNumErrors() const;

    /// Get the number of warnings
    ///
    /// @return Number of warnings
    std::size_t getNumWarnings() const;

    /// Print logged errors and warnings
    void print() const;

protected:
    void
    streamAll(std::ostringstream & ss)
    {
    }

    template <typename T, typename... Args>
    void
    streamAll(std::ostringstream & ss, T && val, Args &&... args)
    {
        ss << val;
        streamAll(ss, std::forward<Args>(args)...);
    }

    /// List of logged errors/warnings
    std::vector<std::string> entries;
    /// Number of errors
    std::size_t num_errors;
    /// Number of warnings
    std::size_t num_warnings;
};

} // namespace godzilla
