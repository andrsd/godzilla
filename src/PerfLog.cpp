// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PerfLog.h"
#include "godzilla/Exception.h"

namespace godzilla {

void
PerfLog::init()
{
    PetscLogDefaultBegin();
}

bool
PerfLog::is_event_registered(const char * name)
{
    EventID event_id;
    PetscLogEventGetId(name, &event_id);
    return (event_id != -1);
}

bool
PerfLog::is_event_registered(const std::string & name)
{
    return PerfLog::is_event_registered(name.c_str());
}

PerfLog::EventID
PerfLog::register_event(const char * name)
{
    EventID event_id;
    PetscLogEventGetId(name, &event_id);
    if (event_id == -1) {
        PetscLogEventRegister(name, 0, &event_id);
        return event_id;
    }
    else
        throw Exception("PerfLog event '{}' is already registered.", name);
}

PerfLog::EventID
PerfLog::register_event(const std::string & name)
{
    return PerfLog::register_event(name.c_str());
}

PerfLog::EventID
PerfLog::get_event_id(const char * name)
{
    EventID event_id;
    PetscLogEventGetId(name, &event_id);
    if (event_id != -1)
        return event_id;
    else
        throw Exception("Event '{}' was not registered.", name);
}

PerfLog::EventID
PerfLog::get_event_id(const std::string & name)
{
    return PerfLog::get_event_id(name.c_str());
}

PerfLog::StageID
PerfLog::register_stage(const char * name)
{
    StageID stage_id;
    PetscLogStageGetId(name, &stage_id);
    if (stage_id == -1) {
        PetscLogStageRegister(name, &stage_id);
        return stage_id;
    }
    else
        throw Exception("PerfLog stage '{}' is already registered.", name);
}

PerfLog::StageID
PerfLog::register_stage(const std::string & name)
{
    return PerfLog::register_stage(name.c_str());
}

PerfLog::StageID
PerfLog::get_stage_id(const char * name)
{
    EventID stage_id;
    PetscLogStageGetId(name, &stage_id);
    if (stage_id != -1)
        return stage_id;
    else
        throw Exception("Stage '{}' was not registered.", name);
}

PerfLog::StageID
PerfLog::get_stage_id(const std::string & name)
{
    return PerfLog::get_stage_id(name.c_str());
}

PerfLog::EventInfo
PerfLog::get_event_info(const std::string & event_name, const std::string & stage_name)
{
    EventID event_id = PerfLog::get_event_id(event_name.c_str());
    StageID stage_id =
        stage_name.empty() ? PETSC_DETERMINE : PerfLog::get_stage_id(stage_name.c_str());
    EventInfo info(event_id, stage_id);
    return info;
}

PerfLog::EventInfo
PerfLog::get_event_info(EventID event_id, StageID stage_id)
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

PerfLog::Event::Event(EventID id) : id(id)
{
    PetscLogEventBegin(this->id, 0, 0, 0, 0);
}

PerfLog::Event::~Event()
{
    PetscLogEventEnd(this->id, 0, 0, 0, 0);
}

PerfLog::EventID
PerfLog::Event::get_id() const
{
    return this->id;
}

void
PerfLog::Event::log_flops(LogDouble n)
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

PerfLog::Stage::Stage(EventID id) : id(id)
{
    PetscLogStagePush(this->id);
}

PerfLog::Stage::~Stage()
{
    PetscLogStagePop();
}

PerfLog::StageID
PerfLog::Stage::get_id() const
{
    return this->id;
}

// Event info

PerfLog::EventInfo::EventInfo(EventID event_id, StageID stage_id) : info()
{
    PetscLogEventGetPerfInfo(stage_id, event_id, &this->info);
}

PerfLog::LogDouble
PerfLog::EventInfo::get_flops() const
{
    return this->info.flops;
}

PerfLog::LogDouble
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
