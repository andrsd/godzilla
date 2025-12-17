// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <map>
#include "godzilla/Object.h"
#include "godzilla/Exception.h"
#include "godzilla/String.h"

#define REGISTER_OBJECT(registry, classname) registry.add<classname>(#classname)

namespace godzilla {

using ObjectPtr = Object *;

using ParamsPtr = Parameters (*)();

using BuildPtr = ObjectPtr (*)(const Parameters & pars);

template <typename T>
ObjectPtr
build_obj(const Parameters & pars)
{
    return new T(pars);
}

template <typename T>
auto
call_parameters() -> decltype(T::parameters(), Parameters())
{
    return T::parameters();
}

/// Registry that maintains mapping of how to build object based on their C++ names
class Registry {
private:
    /// Describes how to build an Object
    struct Entry {
        /// Function pointer that builds the object
        BuildPtr build_ptr;
        /// Function pointer that builds Parameters associated with the class
        ParamsPtr params_ptr;
    };

public:
    struct ObjectDescription {
        struct Parameter {
            /// Parameter name
            String name;
            /// Parameter type
            String type;
            /// Parameter description
            String description;
            /// Is parameter required
            bool required;
        };

        /// Object name
        String name;
        /// Object parameters
        std::vector<Parameter> parameters;
    };

    /// Register an object
    ///
    /// @param class_name Name of the object to register
    template <typename T>
    void
    add(String class_name)
    {
        Entry entry = { &build_obj<T>, &call_parameters<T> };
        this->classes[class_name] = entry;
    }

    /// Check if class is registered
    ///
    /// @param class_name Class name to check
    /// @return `true` is class is registered, `false` otherwise
    bool exists(String class_name) const;

    /// Find registry entry for a given class
    ///
    /// @param class_name Name of the class
    /// @return Registry entry
    const Entry & get(String class_name) const;

    /// Get object description
    std::vector<ObjectDescription> get_object_description() const;

public:
    Registry() = default;

private:
    /// All registered classes that we can build
    std::map<String, Entry> classes;
};

} // namespace godzilla
