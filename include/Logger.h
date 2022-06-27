#pragma once

#include <vector>
#include <string>
#include "Terminal.h"
#include "Error.h"
#include "fmt/color.h"

namespace godzilla {

/// Class for logging errors and warnings
class Logger {
public:
    Logger();

    /// Log an error
    template <typename... Args>
    void
    error(const char * s, Args... args)
    {
        error(std::string(""), s, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void
    error(const std::string & prefix, const char * s, Args... args)
    {
        std::string str =
            format_msg(Terminal::Color::red, "[ERROR]", prefix, s, std::forward<Args>(args)...);
        this->entries.push_back(str);
        this->num_errors++;
    }

    /// Log a warning
    template <typename... Args>
    void
    warning(const char * s, Args... args)
    {
        warning(std::string(""), s, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void
    warning(const std::string & prefix, const char * s, Args... args)
    {
        std::string str = format_msg(Terminal::Color::yellow,
                                     "[WARNING]",
                                     prefix,
                                     s,
                                     std::forward<Args>(args)...);
        this->entries.push_back(str);
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
    template <typename... Args>
    std::string
    format_msg(const Terminal::Color & color,
               const char * title,
               const std::string & prefix,
               const char * format,
               Args... args)
    {
        std::string str;
        str += fmt::sprintf("%s%s ", color, title);
        if (prefix.length() > 0)
            str += fmt::sprintf("%s: ", prefix);
        str += fmt::sprintf(format, std::forward<Args>(args)...);
        str += fmt::sprintf("%s", Terminal::Color::normal);
        return str;
    }

    /// List of logged errors/warnings
    std::vector<std::string> entries;
    /// Number of errors
    std::size_t num_errors;
    /// Number of warnings
    std::size_t num_warnings;
};

} // namespace godzilla
