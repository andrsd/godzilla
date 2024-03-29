// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "petsclog.h"
#include <map>

#ifdef GODZILLA_WITH_PERF_LOG
    #define GODZILLA_PERF_LOG_REGISTER_EVENT(name) PerfLog::register_event(name);
    #define GODZILLA_PERF_LOG_EVENT(name) PerfLog::Event __event__(name);

    #define GODZILLA_PERF_LOG_REGISTER_STAGE(name) PerfLog::register_stage(name);
    #define GODZILLA_PERF_LOG_STAGE(name) PerfLog::Stage __stage__(name);
#else
    #define GODZILLA_PERF_LOG_REGISTER_EVENT(name)
    #define GODZILLA_PERF_LOG_EVENT(name)

    #define GODZILLA_PERF_LOG_REGISTER_STAGE(name)
    #define GODZILLA_PERF_LOG_STAGE(name)
#endif

namespace godzilla {

/// Performance logging
///
class PerfLog {
public:
    /// Initialize performance logging
    static void init();

    static bool is_event_registered(const char * name);
    static bool is_event_registered(const std::string & name);

    /// Register a logging event
    ///
    /// @param name Event name
    /// @return Event ID
    static PetscLogEvent register_event(const char * name);
    static PetscLogEvent register_event(const std::string & name);

    /// Get event ID
    ///
    /// @param name Event name
    /// @return Event ID
    static PetscLogEvent get_event_id(const char * name);
    static PetscLogEvent get_event_id(const std::string & name);

    /// Register a logging stage
    ///
    /// @param name Stage name
    /// @return
    static PetscLogStage register_stage(const char * name);
    static PetscLogStage register_stage(const std::string & name);

    /// Get stage ID
    ///
    /// @param name Stage name
    /// @return Stage ID
    static PetscLogStage get_stage_id(const char * name);
    static PetscLogStage get_stage_id(const std::string & name);

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
        explicit Stage(PetscLogStage id);

        /// Destroy a stage
        ///
        /// This will finish logging the stage
        virtual ~Stage();

        /// Get ID of this stage
        ///
        /// @return ID of this stage
        [[nodiscard]] PetscLogStage get_id() const;

    private:
        /// Event ID
        PetscLogStage id;
    };

    class EventInfo {
    public:
        EventInfo(PetscLogEvent event_id, PetscLogStage stage_id);

        /// Get the number of FLOPS
        ///
        /// @return Number of FLOPS
        [[nodiscard]] PetscLogDouble get_flops() const;

        /// Get total time spent on this event
        ///
        /// @return The total time spent on this event
        [[nodiscard]] PetscLogDouble get_time() const;

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
    static EventInfo get_event_info(const std::string & event_name,
                                    const std::string & stage_name = "");

    /// Get event information
    ///
    /// @param event_id Event ID (registered via PerfLog)
    /// @param stage_id Stage ID (registered via PerfLog)
    /// @return Event information
    static EventInfo get_event_info(PetscLogEvent event_id,
                                    PetscLogStage stage_id = PETSC_DETERMINE);

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
        /// @param id ID of an event previously registered in PerfLog class.
        explicit Event(PetscLogEvent id);

        /// Destroy an event
        ///
        /// This will finish logging the event
        virtual ~Event();

        /// Get ID of this event
        ///
        /// @return ID of this event
        [[nodiscard]] PetscLogEvent get_id() const;

        /// Log number of FLOPS
        void log_flops(PetscLogDouble n);

    private:
        /// Event ID
        PetscLogEvent id;
    };
};

} // namespace godzilla
