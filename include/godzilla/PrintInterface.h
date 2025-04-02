// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <string>
#include "fmt/printf.h"
#include "mpicpp-lite/mpicpp-lite.h"
#include "godzilla/PerfLog.h"
#include "godzilla/Terminal.h"

namespace godzilla {

namespace mpi = mpicpp_lite;

class Object;
class App;

/// Interface for printing on terminal
///
class PrintInterface {
public:
    class TimedEvent {
    public:
        TimedEvent(const PrintInterface * pi,
                   unsigned int level,
                   const std::string & event_name,
                   const std::string & text);
        ~TimedEvent();

        template <typename... T>
        static TimedEvent
        create(const PrintInterface * pi,
               unsigned int level,
               const std::string & event_name,
               fmt::format_string<T...> format,
               T... args)
        {
            std::string text = fmt::format(format, std::forward<T>(args)...);
            return TimedEvent(pi, level, event_name, text);
        }

    private:
        const PrintInterface * pi;
        unsigned int level;
        perf_log::Event * event;
        PetscLogDouble start_time;
    };

public:
    explicit PrintInterface(const Object * obj);
    explicit PrintInterface(const App * app);
    PrintInterface(const mpi::Communicator & comm,
                   const unsigned int & verbosity_level,
                   std::string prefix);

    /// Print a message on a terminal
    ///
    /// @param level Verbosity level. If application verbose level is higher than this number, the
    ///              message will be printed.
    /// @param format String specifying how to interpret the data
    /// @param ... Arguments specifying data to print
    template <typename... T>
    void
    lprintln(unsigned int level, fmt::format_string<T...> format, T... args) const
    {
        if (level <= this->verbosity_level && this->proc_id == 0) {
            fmt::print(stdout, "{}: ", this->prefix);
            fmt::print(stdout, format, std::forward<T>(args)...);
            fmt::print(stdout, "\n");
        }
    }

    /// Convenience version of lprintln with color (the whole line will be colored)
    ///
    /// @param level Verbosity level. If application verbose level is higher than this number, the
    ///             message will be printed.
    /// @param clr Color to use
    /// @param format String specifying how to interpret the data
    /// @param ... Arguments specifying data to print
    template <typename... T>
    void
    lprintln(unsigned int level,
             Terminal::Color clr,
             fmt::format_string<T...> format,
             T... args) const
    {
        if (level <= this->verbosity_level && this->proc_id == 0) {
            fmt::print(stdout, "{}: ", this->prefix);
            fmt::print(stdout, "{}", clr);
            fmt::print(stdout, format, std::forward<T>(args)...);
            fmt::print(stdout, "{}", Terminal::Color::normal);
            fmt::print(stdout, "\n");
        }
    }

private:
    /// Application
    const App * pi_app;
    /// Processor ID
    int proc_id;
    /// Verbosity level
    const unsigned int & verbosity_level;
    /// Prefix to print
    const std::string prefix;
};

#define TIMED_EVENT(level, event_name, ...) \
    auto __timed_event_obj =                \
        PrintInterface::TimedEvent::create(this, level, event_name, __VA_ARGS__)

} // namespace godzilla
