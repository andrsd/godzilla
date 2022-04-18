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
        stream_all(oss,
                  Terminal::Color::red,
                  "error: ",
                  Terminal::Color::normal,
                  std::forward<Args>(args)...);
        this->entries.push_back(oss.str());
        this->num_errors++;
    }

    /// Log a warning
    template <typename... Args>
    void
    warning(Args &&... args)
    {
        std::ostringstream oss;
        stream_all(oss,
                  Terminal::Color::magenta,
                  "warning: ",
                  Terminal::Color::normal,
                  std::forward<Args>(args)...);
        this->entries.push_back(oss.str());
        this->num_warnings++;
    }

    /// Get the number of logged errors/warnings
    ///
    /// @return Number of logger errors/warnings
    std::size_t get_num_entries() const;

    /// Get the number of errors
    ///
    /// @return Number of errors
    std::size_t get_num_errors() const;

    /// Get the number of warnings
    ///
    /// @return Number of warnings
    std::size_t get_num_warnings() const;

    /// Print logged errors and warnings
    void print() const;

protected:
    void
    stream_all(std::ostringstream & ss)
    {
    }

    template <typename T, typename... Args>
    void
    stream_all(std::ostringstream & ss, T && val, Args &&... args)
    {
        ss << val;
        stream_all(ss, std::forward<Args>(args)...);
    }

    /// List of logged errors/warnings
    std::vector<std::string> entries;
    /// Number of errors
    std::size_t num_errors;
    /// Number of warnings
    std::size_t num_warnings;
};

} // namespace godzilla
