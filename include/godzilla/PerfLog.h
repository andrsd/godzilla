// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/String.h"
#include "petsclog.h"
#include <map>
#include <vector>

#define GODZILLA_PERF_LOG_REGISTER_EVENT(name) perf_log::register_event(name);
#define GODZILLA_PERF_LOG_EVENT(name) perf_log::ScopedEvent __event__(name);

#define GODZILLA_PERF_LOG_REGISTER_STAGE(name) perf_log::register_stage(name);
#define GODZILLA_PERF_LOG_STAGE(name) perf_log::Stage __stage__(name);

namespace godzilla::perf_log {

using EventID = PetscLogEvent;
using StageID = PetscLogStage;
using LogDouble = PetscLogDouble;

/// Initialize performance logging
void init();

bool is_event_registered(const char * name);
bool is_event_registered(String name);

/// Register a logging event
///
/// @param name Event name
/// @return Event ID
EventID register_event(const char * name);
EventID register_event(String name);

/// Get event ID
///
/// @param name Event name
/// @return Event ID
EventID get_event_id(const char * name);
EventID get_event_id(String name);

/// Register a logging stage
///
/// @param name Stage name
/// @return
StageID register_stage(const char * name);
StageID register_stage(String name);

/// Get stage ID
///
/// @param name Stage name
/// @return Stage ID
StageID get_stage_id(const char * name);
StageID get_stage_id(String name);

/// Adds floating point operations to the global counter.
void log_flops(LogDouble n);

/// Performance logging stage
///
class Stage {
public:
    /// Construct a performance logging stage
    ///
    /// @param name Name of the stage. Must be registered in PerfLog class.
    explicit Stage(const char * name);
    explicit Stage(String name);

    /// Construct a performance logging stage from stage ID
    ///
    /// @param id ID of an stage previously registered in PerfLog class.
    explicit Stage(StageID id);

    /// Destroy a stage
    ///
    /// This will finish logging the stage
    virtual ~Stage();

    /// Get ID of this stage
    ///
    /// @return ID of this stage
    StageID get_id() const;

private:
    /// Event ID
    StageID id;
};

class EventInfo {
public:
    EventInfo(EventID event_id, StageID stage_id);

    /// The flag to print info in summary
    bool visible() const;

    /// Get the number of FLOPS
    ///
    /// @return Number of FLOPS
    LogDouble flops() const;

    /// Get total time spent on this event
    ///
    /// @return The total time spent on this event
    LogDouble time() const;

    /// Get number of times this event was called
    ///
    /// @return Number of times this event was called
    int num_calls() const;

    /// The number of messages in this event
    LogDouble num_messages() const;

    /// The total message lengths in this event
    LogDouble messages_length() const;

    /// The number of reductions in this event
    LogDouble num_reductions() const;

private:
    /// Event information collected by PETSc
    PetscEventPerfInfo info;
};

/// Get event information
///
/// @param event_name Event name
/// @param stage_name Stage name
/// @return Event information
EventInfo get_event_info(String event_name, String stage_name = "");

/// Get event information
///
/// @param event_id Event ID (registered via perf_log::register_event)
/// @param stage_id Stage ID (registered via perf_log::register_stage)
/// @return Event information
EventInfo get_event_info(EventID event_id, StageID stage_id = PETSC_DETERMINE);

/// Event for performance logging
///
class Event {
public:
    /// Construct a performance logging event
    ///
    /// @param name Name of the event
    explicit Event(const char * name);
    explicit Event(String name);

    /// Construct a performance logging event from event ID
    ///
    /// @param id ID of a previously registered event
    explicit Event(EventID id);

    /// Destroy an event
    ///
    /// This will finish logging the event
    virtual ~Event() = default;

    /// Log the beginning of the event
    void begin();

    /// Log the end of the event.
    void end();

    /// Get ID of this event
    ///
    /// @return ID of this event
    EventID get_id() const;

    /// Get event name
    String name() const;

    /// Get event info
    EventInfo info() const;

private:
    /// Get event ID from event name
    EventID id_from_name(const char * name);

    /// Event ID
    EventID id;
};

/// Scoped event for performance logging
///
/// Event start at the construction time and ends at the destruction time
class ScopedEvent : public Event {
public:
    /// Construct a scoped performance logging event
    ///
    /// @param name Name of the event
    explicit ScopedEvent(const char * name);
    explicit ScopedEvent(String name);
    virtual ~ScopedEvent();
};

/// Return list of events registered by the application
const std::vector<EventID> & registered_event_ids();

} // namespace godzilla::perf_log
