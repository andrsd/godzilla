#include "PrintInterface.h"
#include "CallStack.h"
#include "Object.h"
#include "App.h"
#include "Utils.h"
#include "mpi/Communicator.h"

namespace godzilla {

PrintInterface::TimedEvent::TimedEvent(const PrintInterface * pi,
                                       unsigned int level,
                                       const std::string & event_name,
                                       const std::string & text) :
    pi(pi),
    level(level)
{
    std::string evt_name = fmt::format("{}::{}", this->pi->pi_app->get_name(), event_name);
    if (!PerfLog::is_event_registered(evt_name))
        PerfLog::register_event(evt_name);
    this->event = new PerfLog::Event(evt_name);
    this->start_time = PerfLog::get_event_info(evt_name).get_time();
    if (level <= this->pi->verbosity_level && this->pi->proc_id == 0) {
        fmt::print("{}... ", text);
    }
}

PrintInterface::TimedEvent::~TimedEvent()
{
    auto event_id = this->event->get_id();
    delete this->event;

    if (level <= this->pi->verbosity_level && this->pi->proc_id == 0) {
        auto info = PerfLog::get_event_info(event_id);
        fmt::print("done [{}]\n", utils::human_time(info.get_time() - this->start_time));
    }
}

PrintInterface::PrintInterface(const Object * obj) :
    pi_app(obj->get_app()),
    proc_id(obj->get_processor_id()),
    verbosity_level(obj->get_app()->get_verbosity_level()),
    prefix(obj->get_name())
{
    _F_;
}

PrintInterface::PrintInterface(const App * app) :
    pi_app(app),
    proc_id(app->get_comm().rank()),
    verbosity_level(app->get_verbosity_level()),
    prefix(app->get_name())
{
    _F_;
}

PrintInterface::PrintInterface(const mpi::Communicator & comm,
                               const unsigned int & verbosity_level,
                               std::string prefix) :
    proc_id(comm.rank()),
    verbosity_level(verbosity_level),
    prefix(std::move(prefix))
{
}

} // namespace godzilla
