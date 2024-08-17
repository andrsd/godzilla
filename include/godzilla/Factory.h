// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <list>
#include <ctime>
#include "godzilla/Object.h"
#include "godzilla/Registry.h"
#include "godzilla/Exception.h"

namespace godzilla {

class Parameters;

/// Generic factory class for building objects
///
class Factory {
public:
    explicit Factory(const Registry & registry);

    /// Get valid parameters for the object
    ///
    /// @param class_name Name of the object whose parameter we are requesting
    /// @return Parameters of the object
    Parameters *
    get_parameters(const std::string & class_name)
    {
        auto entry = this->registry.get(class_name);
        auto pars = new Parameters((*entry.params_ptr)());
        pars->set<std::string>("_type") = class_name;
        this->params.push_back(pars);
        return pars;
    }

    /// Build an object (must be registered in Registry)
    ///
    /// @param name Name for the object
    /// @param parameters Parameters this object should have
    /// @return The created object
    template <typename T>
    T *
    create(const std::string & name, Parameters & parameters)
    {
        auto class_name = parameters.set<std::string>("_type");
        auto entry = this->registry.get(class_name);
        parameters.set<std::string>("_name") = name;
        T * object = dynamic_cast<T *>(entry.build_ptr(parameters));
        if (object == nullptr)
            throw Exception("Instantiation of object '{}:[{}]' failed.", name, class_name);
        this->objects.push_back(object);
        return object;
    }

    /// Build an object (must be registered in Registry)
    ///
    /// @param name Name for the object
    /// @param parameters Parameters this object should have
    /// @return The created object
    template <typename T>
    T *
    create(const std::string & name, Parameters * parameters)
    {
        return create<T>(name, *parameters);
    }

    /// Check if class is registered
    ///
    /// @param class_name Class name to check
    /// @return `true` if class name is known, `false` otherwise
    [[nodiscard]] bool
    is_registered(const std::string & class_name) const
    {
        return this->registry.exists(class_name);
    }

    /// Destroy all object build by this factory
    void destroy();

private:
    /// Registry supplying information about objects that can be build
    const Registry & registry;
    /// All objects built by this factory
    std::list<Object *> objects;
    /// All Parameters objects built by this factory
    std::list<Parameters *> params;
};

} // namespace godzilla
