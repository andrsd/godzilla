// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petsclog.h"
#include <map>

#ifdef GODZILLA_WITH_PERF_LOG
    #define GODZILLA_PERF_LOG_REGISTER_EVENT(name) perf_log::register_event(name);
    #define GODZILLA_PERF_LOG_EVENT(name) perf_log::Event __event__(name);

    #define GODZILLA_PERF_LOG_REGISTER_STAGE(name) perf_log::register_stage(name);
    #define GODZILLA_PERF_LOG_STAGE(name) perf_log::Stage __stage__(name);
#else
    #define GODZILLA_PERF_LOG_REGISTER_EVENT(name)
    #define GODZILLA_PERF_LOG_EVENT(name)

    #define GODZILLA_PERF_LOG_REGISTER_STAGE(name)
    #define GODZILLA_PERF_LOG_STAGE(name)
#endif

namespace godzilla::perf_log {

using EventID = PetscLogEvent;
using StageID = PetscLogStage;
using LogDouble = PetscLogDouble;

/// Initialize performance logging
void init();

bool is_event_registered(const char * name);
bool is_event_registered(const std::string & name);

/// Register a logging event
///
/// @param name Event name
/// @return Event ID
EventID register_event(const char * name);
EventID register_event(const std::string & name);

/// Get event ID
///
/// @param name Event name
/// @return Event ID
EventID get_event_id(const char * name);
EventID get_event_id(const std::string & name);

/// Register a logging stage
///
/// @param name Stage name
/// @return
StageID register_stage(const char * name);
StageID register_stage(const std::string & name);

/// Get stage ID
///
/// @param name Stage name
/// @return Stage ID
StageID get_stage_id(const char * name);
StageID get_stage_id(const std::string & name);

/// Performance logging stage
///
class Stage {
public:
    /// Construct a performance logging stage
    ///
    /// @param name Name of the stage. Must be registered in PerfLog class.
    explicit Stage(const char * name);
    explicit Stage(const std::string & name);

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
    [[nodiscard]] StageID get_id() const;

private:
    /// Event ID
    StageID id;
};

class EventInfo {
public:
    EventInfo(EventID event_id, StageID stage_id);

    /// Get the number of FLOPS
    ///
    /// @return Number of FLOPS
    [[nodiscard]] LogDouble get_flops() const;

    /// Get total time spent on this event
    ///
    /// @return The total time spent on this event
    [[nodiscard]] LogDouble get_time() const;

    /// Get number of times this event was called
    ///
    /// @return Number of times this event was called
    [[nodiscard]] int get_num_calls() const;

private:
    /// Event information collected by PETSc
    PetscEventPerfInfo info;
};

/// Get event information
///
/// @param event_name Event name
/// @param stage_name Stage name
/// @return Event information
EventInfo get_event_info(const std::string & event_name, const std::string & stage_name = "");

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
    /// @param name Name of the event. Must be registered in PerfLog class.
    explicit Event(const char * name);
    explicit Event(const std::string & name);

    /// Construct a performance logging event from event ID
    ///
    /// @param id ID of a previously registered event
    explicit Event(EventID id);

    /// Destroy an event
    ///
    /// This will finish logging the event
    virtual ~Event();

    /// Get ID of this event
    ///
    /// @return ID of this event
    [[nodiscard]] EventID get_id() const;

    /// Log number of FLOPS
    void log_flops(LogDouble n);

private:
    /// Event ID
    EventID id;
};

} // namespace godzilla::perf_log
