// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Error.h"
#include "godzilla/Logger.h"
#include "godzilla/Ref.h"
#include "fmt/core.h"

namespace godzilla {

/// Logging interface to give objects capability to report problems like errors and warnings
///
class LoggingInterface {
public:
    explicit LoggingInterface(Ref<Logger> logger) : log_(logger) {}

    template <typename... ARGS>
    void
    info(fmt::format_string<ARGS...> fmt, ARGS &&... args) const
    {
        this->log_->spdlgr_->info(fmt, std::forward<ARGS>(args)...);
    }

    template <typename... ARGS>
    void
    warning(fmt::format_string<ARGS...> fmt, ARGS &&... args) const
    {
        internal::warning(fmt, std::forward<ARGS>(args)...);
        this->log_->spdlgr_->warn(fmt, std::forward<ARGS>(args)...);
    }

    template <typename... ARGS>
    void
    error(fmt::format_string<ARGS...> fmt, ARGS &&... args) const
    {
        this->log_->spdlgr_->error(fmt, std::forward<ARGS>(args)...);
        internal::error(fmt, std::forward<ARGS>(args)...);
    }

    template <typename... ARGS>
    void
    critical(fmt::format_string<ARGS...> fmt, ARGS &&... args) const
    {
        this->log_->spdlgr_->critical(fmt, std::forward<ARGS>(args)...);
        internal::error(fmt, std::forward<ARGS>(args)...);
    }

    template <typename... ARGS>
    void
    debug(fmt::format_string<ARGS...> fmt, ARGS &&... args) const
    {
        this->log_->spdlgr_->debug(fmt, std::forward<ARGS>(args)...);
    }

    template <typename... ARGS>
    void
    trace(fmt::format_string<ARGS...> fmt, ARGS &&... args) const
    {
        this->log_->spdlgr_->trace(fmt, std::forward<ARGS>(args)...);
    }

private:
    Ref<Logger> log_;
};

} // namespace godzilla
