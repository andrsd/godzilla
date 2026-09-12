// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CoreApp.h"
#include "godzilla/CallStack.h"
#include "godzilla/Parameters.h"
#include "godzilla/Registry.h"
#include "godzilla/Qtr.h"
#include "godzilla/Ref.h"
#include "godzilla/Problem.h"
#include "mpicpp-lite/mpicpp-lite.h"

namespace mpi = mpicpp_lite;

namespace godzilla {

class App : public CoreApp {
public:
    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    App(mpi::Communicator comm, String name);

    /// Build an application object
    ///
    /// @param comm MPI communicator
    /// @param registry Registry with classes that will be used by the application
    /// @param name Name of the application
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments
    App(mpi::Communicator comm, Registry & registry, String name);

    /// Get pointer to the `Problem` class in this application
    ///
    /// @return Get problem this application is representing
    Ref<Problem> get_problem() const;

    /// Get pointer to the `Problem`-derived class in this application
    ///
    /// @return The problem this application is solving
    template <typename T>
    Ref<T>
    get_problem() const
    {
        CALL_STACK_MSG();
        if (this->problem)
            return dynamic_ref_cast<T>(ref(*this->problem));
        else
            throw Exception("Bad cast");
    }

    /// Run the application
    ///
    /// @return Exit code
    virtual int run();

    /// Create parameters for type T
    ///
    /// @tparam C++ object that provides `parameters()`
    /// @return Parameters for class T
    template <typename T>
        requires requires {
            { T::parameters() } -> std::same_as<Parameters>;
        }
    Ref<T>
    make_problem(Parameters & pars, std::source_location loc = std::source_location::current())
    {
        static_assert(IsConstructibleFromParams<T>::value,
                      "T must be constructible from `const Parameters &`");
        expect_matching_type(pars.get<String>("_type"), utils::type_name<T>(), loc);

        auto obj = Qtr<T>::alloc(pars);
        auto problem = obj.get();
        this->problem = std::move(obj);
        return ref(*problem);
    }

protected:
    /// Run the problem build via `build_from_yml`
    void run_problem();

private:
    /// Pointer to `Problem`
    Qtr<Problem> problem;

public:
    static void register_objects(Registry & registry);
};

} // namespace godzilla
