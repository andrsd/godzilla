// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PrintInterface.h"
#include "godzilla/CallStack.h"
#include "godzilla/Object.h"
#include "godzilla/CoreApp.h"
#include "godzilla/PerfLog.h"
#include "godzilla/Terminal.h"
#include "godzilla/Utils.h"
#include <chrono>

namespace godzilla {

namespace {

perf_log::Event
create_event(Ref<const PrintInterface> /* pi */, String app_name, String event_name)
{
    auto name = fmt::format("{}::{}", app_name, event_name);
    if (!perf_log::is_event_registered(name))
        perf_log::register_event(name);
    return perf_log::Event(name);
}

} // namespace

PrintInterface::TimedEvent::TimedEvent(Ref<const PrintInterface> pi,
                                       unsigned int level,
                                       String event_name,
                                       String text) :
    pi_(pi),
    level_(level),
    event_(create_event(pi, pi->pi_app_->get_name(), event_name)),
    text_(text)
{
    this->event_.begin();
    this->start_time_ = this->event_.info().time();
    if (level <= this->pi_->verbosity_level_ && this->pi_->proc_id_ == 0) {
        fmt::print("{}...", text);

        this->running_ = true;
        this->thread_ = std::thread([this] {
            const String frames[] = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
            std::size_t i = 0;
            while (this->running_) {
                fmt::print("\r{}{} {}...", Terminal::erase_line, frames[i++ % 10], this->text_);
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
        });
    }
}

PrintInterface::TimedEvent::~TimedEvent()
{
    this->event_.end();

    this->running_ = false;
    if (this->thread_.joinable())
        this->thread_.join();

    if (level_ <= this->pi_->verbosity_level_ && this->pi_->proc_id_ == 0) {
        auto event_id = this->event_.get_id();
        auto info = perf_log::get_event_info(event_id);
        fmt::print("\r{}{}... took {}\n",
                   Terminal::erase_line,
                   this->text_,
                   utils::human_time(info.time() - this->start_time_));
    }
}

PrintInterface::PrintInterface(const Object * obj) :
    pi_app_(obj->get_app()),
    proc_id_(obj->get_processor_id()),
    verbosity_level_(obj->get_app()->get_verbosity_level())
{
    CALL_STACK_MSG();
}

PrintInterface::PrintInterface(Ref<const CoreApp> app) :
    pi_app_(app),
    proc_id_(app->get_comm().rank()),
    verbosity_level_(app->get_verbosity_level())
{
    CALL_STACK_MSG();
}

PrintInterface::PrintInterface(mpi::Communicator comm,
                               Ref<const CoreApp> app,
                               const unsigned int & verbosity_level,
                               String /* prefix */) :
    pi_app_(app),
    proc_id_(comm.rank()),
    verbosity_level_(verbosity_level)
{
    CALL_STACK_MSG();
}

} // namespace godzilla
