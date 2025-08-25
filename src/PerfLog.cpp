// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/PerfLog.h"
#include "godzilla/Exception.h"
#include "godzilla/Types.h"

namespace godzilla::perf_log {

const Int INVALID_EVENT_ID = -1;
const Int INVALID_STAGE_ID = -1;

void
init()
{
    PetscLogDefaultBegin();
}

bool
is_event_registered(const char * name)
{
    EventID event_id;
    PetscLogEventGetId(name, &event_id);
    return (event_id != INVALID_EVENT_ID);
}

bool
is_event_registered(const std::string & name)
{
    return is_event_registered(name.c_str());
}

EventID
register_event(const char * name)
{
    EventID event_id;
    PetscLogEventGetId(name, &event_id);
    if (event_id == INVALID_EVENT_ID) {
        PetscLogEventRegister(name, 0, &event_id);
        return event_id;
    }
    else
        throw Exception("PerfLog event '{}' is already registered.", name);
}

EventID
register_event(const std::string & name)
{
    return register_event(name.c_str());
}

EventID
get_event_id(const char * name)
{
    EventID event_id;
    PetscLogEventGetId(name, &event_id);
    if (event_id != INVALID_EVENT_ID)
        return event_id;
    else
        throw Exception("Event '{}' was not registered.", name);
}

EventID
get_event_id(const std::string & name)
{
    return get_event_id(name.c_str());
}

StageID
register_stage(const char * name)
{
    StageID stage_id;
    PetscLogStageGetId(name, &stage_id);
    if (stage_id == INVALID_STAGE_ID) {
        PetscLogStageRegister(name, &stage_id);
        return stage_id;
    }
    else
        throw Exception("PerfLog stage '{}' is already registered.", name);
}

StageID
register_stage(const std::string & name)
{
    return register_stage(name.c_str());
}

StageID
get_stage_id(const char * name)
{
    EventID stage_id;
    PetscLogStageGetId(name, &stage_id);
    if (stage_id != INVALID_STAGE_ID)
        return stage_id;
    else
        throw Exception("Stage '{}' was not registered.", name);
}

StageID
get_stage_id(const std::string & name)
{
    return get_stage_id(name.c_str());
}

void
log_flops(LogDouble n)
{
    PetscLogFlops(n);
}

EventInfo
get_event_info(const std::string & event_name, const std::string & stage_name)
{
    EventID event_id = get_event_id(event_name.c_str());
    StageID stage_id = stage_name.empty() ? PETSC_DETERMINE : get_stage_id(stage_name.c_str());
    EventInfo info(event_id, stage_id);
    return info;
}

EventInfo
get_event_info(EventID event_id, StageID stage_id)
{
    EventInfo info(event_id, stage_id);
    return info;
}

// Event

Event::Event(const char * name) : id(id_from_name(name)) {}

Event::Event(const std::string & name) : id(id_from_name(name.c_str())) {}

Event::Event(EventID id) : id(id) {}

Event::~Event() {}

void
Event::begin()
{
    PetscLogEventBegin(this->id, 0, 0, 0, 0);
}

void
Event::end()
{
    PetscLogEventEnd(this->id, 0, 0, 0, 0);
}

EventID
Event::get_id() const
{
    return this->id;
}

EventID
Event::id_from_name(const char * name)
{
    if (is_event_registered(name))
        return get_event_id(name);
    else
        return register_event(name);
}

// Stage

Stage::Stage(const char * name) : id(get_stage_id(name))
{
    PetscLogStagePush(this->id);
}

Stage::Stage(const std::string & name) : id(get_stage_id(name))
{
    PetscLogStagePush(this->id);
}

Stage::Stage(EventID id) : id(id)
{
    PetscLogStagePush(this->id);
}

Stage::~Stage()
{
    PetscLogStagePop();
}

StageID
Stage::get_id() const
{
    return this->id;
}

// Event info

EventInfo::EventInfo(EventID event_id, StageID stage_id) : info()
{
    PetscLogEventGetPerfInfo(stage_id, event_id, &this->info);
}

LogDouble
EventInfo::flops() const
{
    return this->info.flops;
}

LogDouble
EventInfo::time() const
{
    return this->info.time;
}

int
EventInfo::num_calls() const
{
    return this->info.count;
}

// ScopedEvent

ScopedEvent::ScopedEvent(const char * name) : Event(name)
{
    begin();
}

ScopedEvent::ScopedEvent(const std::string & name) : Event(name)
{
    begin();
}

ScopedEvent::~ScopedEvent()
{
    end();
}

} // namespace godzilla::perf_log
