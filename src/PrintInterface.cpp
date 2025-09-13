// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PrintInterface.h"
#include "godzilla/CallStack.h"
#include "godzilla/Object.h"
#include "godzilla/App.h"
#include "godzilla/Utils.h"

namespace godzilla {

PrintInterface::TimedEvent::TimedEvent(const PrintInterface * pi,
                                       unsigned int level,
                                       const std::string & event_name,
                                       const std::string & text) :
    pi(pi),
    level(level)
{
    std::string evt_name = fmt::format("{}::{}", this->pi->pi_app->get_name(), event_name);
    if (!perf_log::is_event_registered(evt_name))
        perf_log::register_event(evt_name);
    this->event = Qtr<perf_log::Event>::alloc(evt_name);
    this->event->begin();
    this->start_time = perf_log::get_event_info(evt_name).time();
    if (level <= this->pi->verbosity_level && this->pi->proc_id == 0) {
        fmt::print("{}... ", text);
    }
}

PrintInterface::TimedEvent::~TimedEvent()
{
    this->event->end();
    auto event_id = this->event->get_id();

    if (level <= this->pi->verbosity_level && this->pi->proc_id == 0) {
        auto info = perf_log::get_event_info(event_id);
        fmt::print("done [{}]\n", utils::human_time(info.time() - this->start_time));
    }
}

PrintInterface::PrintInterface(const Object * obj) :
    pi_app(obj->get_app()),
    proc_id(obj->get_processor_id()),
    verbosity_level(obj->get_app()->get_verbosity_level())
{
    CALL_STACK_MSG();
}

PrintInterface::PrintInterface(const App * app) :
    pi_app(app),
    proc_id(app->get_comm().rank()),
    verbosity_level(app->get_verbosity_level())
{
    CALL_STACK_MSG();
}

PrintInterface::PrintInterface(const mpi::Communicator & comm,
                               const App * app,
                               const unsigned int & verbosity_level,
                               std::string prefix) :
    pi_app(app),
    proc_id(comm.rank()),
    verbosity_level(verbosity_level)
{
    CALL_STACK_MSG();
}

} // namespace godzilla
