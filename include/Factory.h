#pragma once

#include <list>
#include <ctime>
#include "Object.h"
#include "Registry.h"
#include "Error.h"

#define COMBINE_NAMES1(X, Y) X##Y
#define COMBINE_NAMES(X, Y) COMBINE_NAMES1(X, Y)

#define REGISTER_OBJECT(classname)                                                     \
    static char COMBINE_NAMES(dummyvar_for_registering_obj_##classname, __COUNTER__) = \
        godzilla::Registry::instance().reg<classname>(#classname)

#define REGISTER_OBJECT_ALIAS(classname, alias)                                        \
    static char COMBINE_NAMES(dummyvar_for_registering_obj_##classname, __COUNTER__) = \
        godzilla::Registry::instance().reg<classname>(#alias)

namespace godzilla {

class Parameters;

/// Generic factory class for building objects
///
class Factory {
public:
    /// Get valid parameters for the object
    ///
    /// @param class_name Name of the object whose parameter we are requesting
    /// @return Parameters of the object
    Parameters *
    get_parameters(const std::string & class_name)
    {
        auto registry = Registry::instance();
        auto entry = registry.get(class_name);
        auto pars = new Parameters((*entry.params_ptr)());
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
    create(const std::string & class_name, const std::string & name, Parameters & parameters)
    {
        auto registry = Registry::instance();
        auto entry = registry.get(class_name);
        parameters.set<std::string>("_type") = class_name;
        parameters.set<std::string>("_name") = name;
        T * object = dynamic_cast<T *>(entry.build_ptr(parameters));
        if (object == nullptr)
            error("Instantiation of object '{}:[{}]' failed.", name, class_name);
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
    create(const std::string & class_name, const std::string & name, Parameters * parameters)
    {
        return create<T>(class_name, name, *parameters);
    }

    /// Check if class is registered
    ///
    /// @param class_name Class name to check
    /// @return `true` if class name is known, `false` otherwise
    bool
    is_registered(const std::string & class_name) const
    {
        return Registry::instance().exists(class_name);
    }

    /// Destroy all object build by this factory
    void destroy();

protected:
    /// All objects built by this factory
    std::list<Object *> objects;
    /// All Parameters objects built by this factory
    std::list<Parameters *> params;
};

} // namespace godzilla
