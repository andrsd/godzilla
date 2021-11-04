#pragma once

#include <set>
#include <vector>
#include <ctime>
#include "Object.h"


#define combineNames1(X, Y) X##Y
#define combineNames(X, Y) combineNames1(X, Y)


#define registerObject(classname)                                                    \
    static char combineNames(dummyvar_for_registering_obj_##classname, __LINE__) =   \
        godzilla::Factory::reg<classname>(#classname)

#define registerObjectAlias(classname, alias)                                        \
    static char combineNames(dummyvar_for_registering_obj_##classname, __LINE__) =   \
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
class Factory
{
public:
    Factory() {}

    struct Entry {
        BuildPtr build_ptr;
        ParamsPtr params_ptr;
    };

    /// Register an object
    /// @param class_name Name of the object to register
    template <typename T>
    static char reg(const std::string & class_name)
    {
        Entry entry;
        entry.build_ptr = &buildObj<T>;
        entry.params_ptr = &callValidParams<T>;
        objects[class_name] = entry;
        return '\0';
    }

    /// Get valid parameters for the object
    /// @param class_name Name of the object whose parameter we are requesting
    /// @return Parameters of the object
    static
    InputParameters getValidParams(const std::string & class_name)
    {
        auto it = objects.find(class_name);
        if (it == objects.end()) {
            // FIXME: use godzillaError
            std::cerr << "Unknown object " << class_name << std::endl;
        }

        Entry & entry = it->second;
        InputParameters params = (*entry.params_ptr)();
        return params;
    }

    /// Build an object (must be registered)
    /// @param class_name Type of the object being constructed
    /// @param name Name for the object
    /// @param parameters Parameters this object should have
    /// @return The created object
    template <typename T>
    static
    T *create(const std::string & class_name, const std::string & name, InputParameters & parameters)
    {
        auto it = objects.find(class_name);
        if (it == objects.end()) {
            // FIXME: Use godzillaError
            std::cerr << "Trying to create object of unregistered type '" << class_name <<"'." << std::endl;
            exit(-100);
        }
        else {
            auto entry = it->second;
            T * object = dynamic_cast<T *>(entry.build_ptr(parameters));
            return object;
        }
    }

    static
    bool
    isRegistered(const std::string & class_name)
    {
        auto it = objects.find(class_name);
        return it != objects.end();
    }

protected:
    static std::map<std::string, Entry> objects;
};

} // namespace godzilla
