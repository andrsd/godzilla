#include "PerfLog.h"
#include "Error.h"

namespace godzilla {

void
PerfLog::init()
{
    PetscLogDefaultBegin();
}

bool
PerfLog::is_event_registered(const char * name)
{
    PetscLogEvent event_id;
    PetscLogEventGetId(name, &event_id);
    return (event_id != -1);
}

bool
PerfLog::is_event_registered(const std::string & name)
{
    return PerfLog::is_event_registered(name.c_str());
}

PetscLogEvent
PerfLog::register_event(const char * name)
{
    PetscLogEvent event_id;
    PetscLogEventGetId(name, &event_id);
    if (event_id == -1) {
        PetscLogEventRegister(name, 0, &event_id);
        return event_id;
    }
    else
        error("PerfLog event '{}' is already registered.", name);
}

PetscLogEvent
PerfLog::register_event(const std::string & name)
{
    return PerfLog::register_event(name.c_str());
}

PetscLogEvent
PerfLog::get_event_id(const char * name)
{
    PetscLogEvent event_id;
    PetscLogEventGetId(name, &event_id);
    if (event_id != -1)
        return event_id;
    else
        error("Event '{}' was not registered.", name);
}

PetscLogEvent
PerfLog::get_event_id(const std::string & name)
{
    return PerfLog::get_event_id(name.c_str());
}

PetscLogStage
PerfLog::register_stage(const char * name)
{
    PetscLogStage stage_id;
    PetscLogStageGetId(name, &stage_id);
    if (stage_id == -1) {
        PetscLogStageRegister(name, &stage_id);
        return stage_id;
    }
    else
        error("PerfLog stage '{}' is already registered.", name);
}

PetscLogStage
PerfLog::register_stage(const std::string & name)
{
    return PerfLog::register_stage(name.c_str());
}

PetscLogStage
PerfLog::get_stage_id(const char * name)
{
    PetscLogEvent stage_id;
    PetscLogStageGetId(name, &stage_id);
    if (stage_id != -1)
        return stage_id;
    else
        error("Stage '{}' was not registered.", name);
}

PetscLogStage
PerfLog::get_stage_id(const std::string & name)
{
    return PerfLog::get_stage_id(name.c_str());
}

PerfLog::EventInfo
PerfLog::get_event_info(const std::string & event_name, const std::string & stage_name)
{
    PetscLogEvent event_id = PerfLog::get_event_id(event_name.c_str());
    PetscLogStage stage_id =
        stage_name.empty() ? PETSC_DETERMINE : PerfLog::get_stage_id(stage_name.c_str());
    EventInfo info(event_id, stage_id);
    return info;
}

PerfLog::EventInfo
PerfLog::get_event_info(PetscLogEvent event_id, PetscLogStage stage_id)
{
    EventInfo info(event_id, stage_id);
    return info;
}

// Event

PerfLog::Event::Event(const char * name) : id(PerfLog::get_event_id(name))
{
    PetscLogEventBegin(this->id, 0, 0, 0, 0);
}

PerfLog::Event::Event(const std::string & name) : id(PerfLog::get_event_id(name.c_str()))
{
    PetscLogEventBegin(this->id, 0, 0, 0, 0);
}

PerfLog::Event::Event(PetscLogEvent id) : id(id)
{
    PetscLogEventBegin(this->id, 0, 0, 0, 0);
}

PerfLog::Event::~Event()
{
    PetscLogEventEnd(this->id, 0, 0, 0, 0);
}

PetscLogEvent
PerfLog::Event::get_id() const
{
    return this->id;
}

void
PerfLog::Event::log_flops(PetscLogDouble n)
{
    PetscLogFlops(n);
}

// Stage

PerfLog::Stage::Stage(const char * name) : id(PerfLog::get_stage_id(name))
{
    PetscLogStagePush(this->id);
}

PerfLog::Stage::Stage(const std::string & name) : id(PerfLog::get_stage_id(name))
{
    PetscLogStagePush(this->id);
}

PerfLog::Stage::Stage(PetscLogEvent id) : id(id)
{
    PetscLogStagePush(this->id);
}

PerfLog::Stage::~Stage()
{
    PetscLogStagePop();
}

PetscLogStage
PerfLog::Stage::get_id() const
{
    return this->id;
}

// Event info

PerfLog::EventInfo::EventInfo(PetscLogEvent event_id, PetscLogStage stage_id) : info()
{
    PetscLogEventGetPerfInfo(stage_id, event_id, &this->info);
}

PetscLogDouble
PerfLog::EventInfo::get_flops() const
{
    return this->info.flops;
}

PetscLogDouble
PerfLog::EventInfo::get_time() const
{
    return this->info.time;
}

int
PerfLog::EventInfo::get_num_calls() const
{
    return this->info.count;
}

} // namespace godzilla
