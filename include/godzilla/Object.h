// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "mpicpp-lite/mpicpp-lite.h"
#include "godzilla/Parameters.h"
#include "godzilla/LoggingInterface.h"
#include "godzilla/String.h"

namespace godzilla {

namespace mpi = mpicpp_lite;

class App;

/// The base class for objects
///
class Object : public LoggingInterface {
public:
    explicit Object(const Parameters & pars);
    virtual ~Object() = default;

    /// Get the type of this object.
    /// @return the name of the type of this object
    String get_type() const;

    /// Get the name of the object
    /// @return The name of the object
    String get_name() const;

    /// Get the App this object is associated with
    App * get_app() const;

    /// Get the MPI comm this object works on
    mpi::Communicator get_comm() const;

    /// Get processor ID (aka MPI rank) this object is running at
    int get_processor_id() const;

    /// Called to construct the object
    virtual void create();

private:
    /// The application owning this object
    App * app;

    /// The type of this object
    const String type;

    /// The name of this object
    const String name;

public:
    /// Method for building Parameters for this class
    static Parameters parameters();
};

} // namespace godzilla
