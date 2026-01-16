// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/details/periodic_worker.h"
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

    /// Start/Restart a periodic flusher thread
    template <typename Rep, typename Period>
    void
    flush_every(std::chrono::duration<Rep, Period> interval)
    {
        std::lock_guard<std::mutex> lock(this->flusher_mutex);
        auto clbk = [this]() {
            this->spdlgr->flush();
        };
        this->periodic_flusher =
            spdlog::details::make_unique<spdlog::details::periodic_worker>(clbk, interval);
    }

private:
    std::shared_ptr<spdlog::logger> spdlgr;
    std::mutex flusher_mutex;
    std::unique_ptr<spdlog::details::periodic_worker> periodic_flusher;

    friend class LoggingInterface;
};

} // namespace godzilla
