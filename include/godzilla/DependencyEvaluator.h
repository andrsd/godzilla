// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <map>
#include "godzilla/Exception.h"
#include "godzilla/CallStack.h"
#include "godzilla/DependencyGraph.h"
#include "godzilla/String.h"
#include "godzilla/Assert.h"

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
    DependencyEvaluator() = default;
    virtual ~DependencyEvaluator();

    /// Get all functionals
    ///
    /// @return List of all functionals
    const std::map<String, const ValueFunctional *> & get_functionals() const;

    /// Create a functional
    ///
    /// @tparam Fn Type of the functional to create
    /// @param name Name of the functional
    /// @param params Parameters needed to build the functional
    template <typename Fn>
    void create_functional(String name, const Parameters & pars);

    /// Get a reference to a functional using its name
    ///
    /// @param name The name of the functional
    /// @return Reference to the functional
    const ValueFunctional & get_functional(String name) const;

    /// Declare a value with a name
    ///
    /// @tparam T Type of the value
    /// @param val_name Value name
    /// @return Reference to the actual value
    template <typename T>
    T & declare_value(String val_name);

    /// Get value using its name
    ///
    /// @tparam T Type of the requested value
    /// @param val_name Value name
    /// @return Const reference to the actual value
    template <typename T>
    const T & get_value(String val_name);

    std::map<String, const ValueFunctional *> get_suppliers() const;

    DependencyGraph<const Functional *>
    build_dependecy_graph(const std::map<String, const ValueFunctional *> & suppliers);

private:
    /// All created functionals
    std::map<String, const ValueFunctional *> functionals;
    /// Values computed by functionals
    std::map<String, const ValueBase *> values;
};

template <typename T>
T &
DependencyEvaluator::declare_value(String val_name)
{
    CALL_STACK_MSG();
    auto it = this->values.find(val_name);
    if (it == this->values.end()) {
        auto * new_val = new Value<T>();
        this->values[val_name] = new_val;
        return new_val->set();
    }
    else {
        expect_true(!it->second->is_declared(),
                    "Trying to declare an already existing value '{}'.",
                    val_name);
        auto val = dynamic_cast<Value<T> *>(const_cast<ValueBase *>(it->second));
        return val->set();
    }
}

template <typename T>
const T &
DependencyEvaluator::get_value(String val_name)
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
DependencyEvaluator::create_functional(String name, const Parameters & pars)
{
    CALL_STACK_MSG();
    const auto & it = this->functionals.find(name);
    expect_true(it == this->functionals.end(), "Functional with name '{}' already exists.", name);
    auto * fnl = new Fn(pars);
    this->functionals[name] = fnl;
}

} // namespace godzilla
