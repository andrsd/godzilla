#pragma once

#include <map>
#include "Object.h"

namespace godzilla {

using ObjectPtr = Object *;

using ParamsPtr = Parameters (*)();

using BuildPtr = ObjectPtr (*)(const Parameters & parameters);

template <typename T>
ObjectPtr
build_obj(const Parameters & parameters)
{
    return new T(parameters);
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
    /// Get the instance of registry
    ///
    /// @return Instance of the Registry class
    static Registry & instance();

    /// Register an object
    ///
    /// @param class_name Name of the object to register
    template <typename T>
    char
    reg(const std::string & class_name)
    {
        Entry entry = { &build_obj<T>, &call_parameters<T> };
        this->classes[class_name] = entry;
        return '\0';
    }

    /// Check if class is registered
    ///
    /// @param class_name Class name to check
    /// @return `true` is class is registered, `false` otherwise
    bool
    exists(const std::string & class_name) const
    {
        auto it = this->classes.find(class_name);
        return it != this->classes.end();
    }

    /// Find registry entry for a given class
    ///
    /// @param class_name Name of the class
    /// @return Registry entry
    const Entry &
    get(const std::string & class_name)
    {
        auto it = this->classes.find(class_name);
        if (it == this->classes.end())
            error("Class '{}' is not registered.", class_name);
        return it->second;
    }

private:
    Registry() = default;

private:
    /// All registered classes that we can build
    std::map<std::string, Entry> classes;
};

} // namespace godzilla
