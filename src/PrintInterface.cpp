// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PrintInterface.h"
#include "godzilla/CallStack.h"
#include "godzilla/Object.h"
#include "godzilla/App.h"
#include "godzilla/PerfLog.h"
#include "godzilla/Terminal.h"
#include "godzilla/Utils.h"
#include <chrono>

namespace godzilla {

namespace {

perf_log::Event
create_event(Ref<const PrintInterface> pi, String app_name, String event_name)
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
    pi(pi),
    level(level),
    event(create_event(pi, pi->pi_app->get_name(), event_name)),
    text(text)
{
    this->event.begin();
    this->start_time = this->event.info().time();
    if (level <= this->pi->verbosity_level && this->pi->proc_id == 0) {
        fmt::print("{}...", text);

        this->running = true;
        this->thread = std::thread([this] {
            const String frames[] = { "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
            std::size_t i = 0;
            while (this->running) {
                fmt::print("\r{}{} {}...", Terminal::erase_line, frames[i++ % 10], this->text);
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
        });
    }
}

PrintInterface::TimedEvent::~TimedEvent()
{
    this->event.end();

    this->running = false;
    if (this->thread.joinable())
        this->thread.join();

    if (level <= this->pi->verbosity_level && this->pi->proc_id == 0) {
        auto event_id = this->event.get_id();
        auto info = perf_log::get_event_info(event_id);
        fmt::print("\r{}{}... took {}\n",
                   Terminal::erase_line,
                   this->text,
                   utils::human_time(info.time() - this->start_time));
    }
}

PrintInterface::PrintInterface(const Object * obj) :
    pi_app(obj->get_app()),
    proc_id(obj->get_processor_id()),
    verbosity_level(obj->get_app()->get_verbosity_level())
{
    CALL_STACK_MSG();
}

PrintInterface::PrintInterface(Ref<const App> app) :
    pi_app(app),
    proc_id(app->get_comm().rank()),
    verbosity_level(app->get_verbosity_level())
{
    CALL_STACK_MSG();
}

PrintInterface::PrintInterface(mpi::Communicator comm,
                               Ref<const App> app,
                               const unsigned int & verbosity_level,
                               String prefix) :
    pi_app(app),
    proc_id(comm.rank()),
    verbosity_level(verbosity_level)
{
    CALL_STACK_MSG();
}

} // namespace godzilla
