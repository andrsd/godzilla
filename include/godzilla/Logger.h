// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "spdlog/spdlog.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace godzilla {

/// Class for logging errors and warnings
class Logger {
public:
    Logger();
    ~Logger();

    /// Set log file name
    ///
    /// @param file_name The name of the log file
    void set_log_file_name(fs::path file_name);

    /// Set format string
    ///
    /// @note: This is a forward call into `spdlog`
    /// See [custom-formatting](https://github.com/gabime/spdlog/wiki/Custom-formatting) page
    void set_format_string(std::string pattern,
                           spdlog::pattern_time_type time_type = spdlog::pattern_time_type::local);

    /// Get logging level
    spdlog::level::level_enum get_level();

    /// Set the logging level
    void set_level(spdlog::level::level_enum log_level);

    /// Set a flush level
    void flush_on(spdlog::level::level_enum log_level);

private:
    std::shared_ptr<spdlog::logger> spdlgr;

    friend class LoggingInterface;
};

} // namespace godzilla
