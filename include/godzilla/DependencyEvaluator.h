// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <map>
#include "godzilla/Error.h"
#include "godzilla/CallStack.h"
#include "godzilla/DependencyGraph.h"

namespace godzilla {

class Functional;
class ValueFunctional;
class Parameters;

/// Evaluator of functionals with defined dependencies
///
class DependencyEvaluator {
    /// Base class for values provided by functionals
    class ValueBase {
    public:
        ValueBase() : declared(false) {}
        virtual ~ValueBase() = default;

        /// Mark this value as declared
        void
        set_declared()
        {
            this->declared = true;
        }

        bool
        is_declared() const
        {
            return this->declared;
        }

    private:
        bool declared;
    };

    /// Class for concrete values provided by functionals
    template <typename T>
    class Value : public ValueBase {
    public:
        /// Get a read reference to the value
        const T &
        get() const
        {
            return this->value;
        }

        /// Get a write reference to the value
        T &
        set()
        {
            set_declared();
            return this->value;
        }

    private:
        T value;
    };

public:
    DependencyEvaluator();
    virtual ~DependencyEvaluator();

    /// Get all functionals
    ///
    /// @return List of all functionals
    const std::map<std::string, const ValueFunctional *> & get_functionals() const;

    /// Create a functional
    ///
    /// @tparam Fn Type of the functional to create
    /// @param name Name of the functional
    /// @param params Parameters needed to build the functional
    template <typename Fn>
    void create_functional(const std::string & name, const Parameters & params);

    /// Get a reference to a functional using its name
    ///
    /// @param name The name of the functional
    /// @return Reference to the functional
    virtual const ValueFunctional & get_functional(const std::string & name) const;

    /// Declare a value with a name
    ///
    /// @tparam T Type of the value
    /// @param val_name Value name
    /// @return Reference to the actual value
    template <typename T>
    T & declare_value(const std::string & val_name);

    /// Get value using its name
    ///
    /// @tparam T Type of the requested value
    /// @param val_name Value name
    /// @return Const reference to the actual value
    template <typename T>
    const T & get_value(const std::string & val_name);

    std::map<std::string, const ValueFunctional *> get_suppliers() const;

    DependencyGraph<const Functional *>
    build_dependecy_graph(const std::map<std::string, const ValueFunctional *> & suppliers);

private:
    /// All created functionals
    std::map<std::string, const ValueFunctional *> functionals;
    /// Values computed by functionals
    std::map<std::string, const ValueBase *> values;
};

template <typename T>
T &
DependencyEvaluator::declare_value(const std::string & val_name)
{
    CALL_STACK_MSG();
    auto it = this->values.find(val_name);
    if (it == this->values.end()) {
        auto * new_val = new Value<T>();
        this->values[val_name] = new_val;
        return new_val->set();
    }
    else {
        if (it->second->is_declared())
            error("Trying to declare an already existing value '{}'.", val_name);
        else {
            auto val = dynamic_cast<Value<T> *>(const_cast<ValueBase *>(it->second));
            return val->set();
        }
    }
}

template <typename T>
const T &
DependencyEvaluator::get_value(const std::string & val_name)
{
    CALL_STACK_MSG();
    auto it = this->values.find(val_name);
    if (it != this->values.end()) {
        auto val = dynamic_cast<const Value<T> *>(it->second);
        return val->get();
    }
    else {
        auto * new_val = new Value<T>();
        this->values[val_name] = new_val;
        return new_val->get();
    }
}

template <typename Fn>
void
DependencyEvaluator::create_functional(const std::string & name, const Parameters & params)
{
    CALL_STACK_MSG();
    const auto & it = this->functionals.find(name);
    if (it == this->functionals.end()) {
        auto * fnl = new Fn(params);
        this->functionals[name] = fnl;
    }
    else
        error("Functional with name '{}' already exists.", name);
}

} // namespace godzilla
