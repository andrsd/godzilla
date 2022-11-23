#pragma once

#include <set>
#include <list>
#include <ctime>
#include "Object.h"
#include "Error.h"

#define COMBINE_NAMES1(X, Y) X##Y
#define COMBINE_NAMES(X, Y) COMBINE_NAMES1(X, Y)

#define REGISTER_OBJECT(classname)                                                  \
    static char COMBINE_NAMES(dummyvar_for_registering_obj_##classname, __LINE__) = \
        godzilla::Factory::reg<classname>(#classname)

#define REGISTER_OBJECT_ALIAS(classname, alias)                                     \
    static char COMBINE_NAMES(dummyvar_for_registering_obj_##classname, __LINE__) = \
        godzilla::Factory::reg<classname>(#alias)

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

class Parameters;

/// Generic factory class for building objects
///
class Factory {
public:
    struct Entry {
        BuildPtr build_ptr;
        ParamsPtr params_ptr;
    };

    /// Register an object
    /// @param class_name Name of the object to register
    template <typename T>
    static char
    reg(const std::string & class_name)
    {
        Entry entry;
        entry.build_ptr = &build_obj<T>;
        entry.params_ptr = &call_parameters<T>;
        classes[class_name] = entry;
        return '\0';
    }

    /// Get valid parameters for the object
    /// @param class_name Name of the object whose parameter we are requesting
    /// @return Parameters of the object
    static Parameters *
    get_parameters(const std::string & class_name)
    {
        auto it = classes.find(class_name);
        if (it == classes.end())
            error("Getting valid_params for object '%s' failed.  Object is not registred.",
                  class_name);

        Entry & entry = it->second;
        auto * ips = new Parameters((*entry.params_ptr)());
        params.push_back(ips);
        return ips;
    }

    /// Build an object (must be registered)
    /// @param class_name Type of the object being constructed
    /// @param name Name for the object
    /// @param parameters Parameters this object should have
    /// @return The created object
    template <typename T>
    static T *
    create(const std::string & class_name, const std::string & name, Parameters & parameters)
    {
        auto it = classes.find(class_name);
        if (it == classes.end())
            error("Trying to create object of unregistered type '%s'.", class_name);
        else {
            parameters.set<std::string>("_type") = class_name;
            parameters.set<std::string>("_name") = name;

            auto entry = it->second;
            T * object = dynamic_cast<T *>(entry.build_ptr(parameters));
            if (object == nullptr)
                error("Instantiation of object '%s:[%s]' failed.", name, class_name);
            objects.push_back(object);
            return object;
        }
    }

    template <typename T>
    static T *
    create(const std::string & class_name, const std::string & name, Parameters * parameters)
    {
        return create<T>(class_name, name, *parameters);
    }

    static bool
    is_registered(const std::string & class_name)
    {
        auto it = classes.find(class_name);
        return it != classes.end();
    }

    static void destroy();

protected:
    /// All registered classes that this factory can build
    static std::map<std::string, Entry> classes;
    /// All objects built by this factory
    static std::list<Object *> objects;
    /// All Parameters objects built by this factory
    static std::list<Parameters *> params;
};

} // namespace godzilla
