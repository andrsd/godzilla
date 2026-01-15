// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Logger.h"
#include "godzilla/CallStack.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace godzilla {

Logger::Logger()
{
    CALL_STACK_MSG();
    this->spdlgr = spdlog::null_logger_mt("file_logger");
}

Logger::~Logger()
{
    spdlog::drop("file_logger");
}

void
Logger::set_log_file_name(fs::path file_name)
{
    CALL_STACK_MSG();
    spdlog::drop("file_logger");
    this->spdlgr = spdlog::basic_logger_mt("file_logger", file_name, true);
    this->spdlgr->set_pattern("[%Y %b %d %H:%M:%S.%e] [%l] %v");
}

void
Logger::set_format_string(std::string pattern, spdlog::pattern_time_type time_type)
{
    CALL_STACK_MSG();
    this->spdlgr->set_pattern(pattern, time_type);
}

spdlog::level::level_enum
Logger::get_level()
{
    return this->spdlgr->level();
}

void
Logger::set_level(spdlog::level::level_enum log_level)
{
    this->spdlgr->set_level(log_level);
}

void
Logger::flush_on(spdlog::level::level_enum log_level)
{
    this->spdlgr->flush_on(log_level);
}

} // namespace godzilla
