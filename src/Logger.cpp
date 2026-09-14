// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Logger.h"
#include "godzilla/CallStack.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace godzilla {

Logger::Logger() : logger_name_("file_logger")
{
    CALL_STACK_MSG();
    this->spdlgr_ = spdlog::null_logger_mt(this->logger_name_);
}

Logger::Logger(String name) : logger_name_(fmt::format("file_logger:{}", name))
{
    CALL_STACK_MSG();
    this->spdlgr_ = spdlog::null_logger_mt(logger_name_);
}

Logger::~Logger()
{
    spdlog::drop(this->logger_name_);
}

void
Logger::set_log_file_name(fs::path file_name)
{
    CALL_STACK_MSG();
    spdlog::drop(this->logger_name_);
    this->spdlgr_ = spdlog::basic_logger_mt(this->logger_name_, file_name, true);
    this->spdlgr_->set_pattern("[%Y %b %d %H:%M:%S.%e] [%l] %v");
}

void
Logger::set_format_string(std::string pattern, spdlog::pattern_time_type time_type)
{
    CALL_STACK_MSG();
    this->spdlgr_->set_pattern(pattern, time_type);
}

spdlog::level::level_enum
Logger::get_level()
{
    return this->spdlgr_->level();
}

void
Logger::set_level(spdlog::level::level_enum log_level)
{
    this->spdlgr_->set_level(log_level);
}

void
Logger::flush_on(spdlog::level::level_enum log_level)
{
    this->spdlgr_->flush_on(log_level);
}

} // namespace godzilla
