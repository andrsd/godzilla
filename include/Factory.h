#pragma once

#include <set>
#include <list>
#include <ctime>
#include "Object.h"
#include "PrintInterface.h"

#define combineNames1(X, Y) X##Y
#define combineNames(X, Y) combineNames1(X, Y)

#define registerObject(classname)                                                  \
    static char combineNames(dummyvar_for_registering_obj_##classname, __LINE__) = \
        godzilla::Factory::reg<classname>(#classname)

#define registerObjectAlias(classname, alias)                                      \
    static char combineNames(dummyvar_for_registering_obj_##classname, __LINE__) = \
        godzilla::Factory::reg<classname>(#alias)

namespace godzilla {

using ObjectPtr = Object *;

using ParamsPtr = InputParameters (*)();

using BuildPtr = ObjectPtr (*)(const InputParameters & parameters);

template <typename T>
ObjectPtr
buildObj(const InputParameters & parameters)
{
    return new T(parameters);
}

template <typename T>
auto
callValidParams() -> decltype(T::validParams(), emptyInputParameters())
{
    return T::validParams();
}

class InputParameters;

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
        entry.build_ptr = &buildObj<T>;
        entry.params_ptr = &callValidParams<T>;
        classes[class_name] = entry;
        return '\0';
    }

    /// Get valid parameters for the object
    /// @param class_name Name of the object whose parameter we are requesting
    /// @return Parameters of the object
    static InputParameters &
    getValidParams(const std::string & class_name)
    {
        auto it = classes.find(class_name);
        if (it == classes.end())
            error("Getting validParams for object '",
                  class_name,
                  "' failed.  Object is not registred.");

        Entry & entry = it->second;
        InputParameters * ips = new InputParameters((*entry.params_ptr)());
        params.push_back(ips);
        return *ips;
    }

    /// Build an object (must be registered)
    /// @param class_name Type of the object being constructed
    /// @param name Name for the object
    /// @param parameters Parameters this object should have
    /// @return The created object
    template <typename T>
    static T *
    create(const std::string & class_name, const std::string & name, InputParameters & parameters)
    {
        auto it = classes.find(class_name);
        if (it == classes.end())
            error("Trying to create object of unregistered type '", class_name, "'.");
        else {
            parameters.set<std::string>("_type") = class_name;
            parameters.set<std::string>("_name") = name;

            auto entry = it->second;
            T * object = dynamic_cast<T *>(entry.build_ptr(parameters));
            if (object == nullptr)
                error("Instantiation of object '", name, ":[", class_name, "]' failed.");
            objects.push_back(object);
            return object;
        }
    }

    static bool
    isRegistered(const std::string & class_name)
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
    /// All InputParameters objects built by this factory
    static std::list<InputParameters *> params;
};

} // namespace godzilla
