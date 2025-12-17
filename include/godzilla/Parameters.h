// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/CallStack.h"
#include "godzilla/Exception.h"
#include "godzilla/Types.h"
#include "godzilla/Assert.h"
#include "godzilla/Qtr.h"
#include "godzilla/Utils.h"
#include <map>

namespace godzilla {

/// Class for user-defined parameters
///
class Parameters {
protected:
    /// Base class for parameter values
    class Value {
    public:
        virtual ~Value() = default;

        /// Return the type of this value as a string
        virtual String type() const = 0;

        /// Create a copy of this value
        virtual Value * copy() const = 0;

        /// Is required
        bool required;
        /// Doco string
        String doc_string;
        ///
        bool valid;
        /// The set of parameters that will NOT appear in the the dump of the parser tree
        bool is_private;
    };

    /// Parameter value
    template <typename T>
    class Parameter : public Value {
    public:
        Parameter() = default;

        /// @returns A read-only reference to the parameter value.
        T
        get() const
        {
            return this->value;
        }

        void
        set(T val)
        {
            this->value = val;
        }

        String
        type() const override
        {
            return utils::demangle(typeid(T).name());
        }

        Value *
        copy() const override
        {
            auto * copy = new Parameter<T>;
            copy->value = this->value;
            copy->required = this->required;
            copy->doc_string = this->doc_string;
            copy->valid = this->valid;
            copy->is_private = this->is_private;
            return copy;
        }

        /// Parameter value
        T value;
    };

public:
    Parameters() = default;
    virtual ~Parameters();

    /// Copy operator
    Parameters(const Parameters & other);

    /// Move operator
    Parameters(Parameters && other) noexcept;

    /// Assignment operator
    Parameters & operator=(const Parameters & other);

    /// Move assignment
    Parameters & operator=(Parameters && other) noexcept;

    /// Check if parameter exist
    template <typename T>
    bool
    has(String name) const
    {
        CALL_STACK_MSG();
        auto it = this->params.find(name);
        if (it == this->params.end())
            return false;
        if (dynamic_cast<const Parameter<T> *>(it->second.get()) == nullptr)
            return false;
        return true;
    }

    /// Get parameter value
    template <typename T>
    inline T
    get(String name) const
    {
        CALL_STACK_MSG();
        if constexpr (IsOptional<T>) {
            using V = typename T::value_type;
            auto it = this->params.find(name);
            if (it == this->params.end())
                return {};

            auto par = it->second.get();
            auto tpar = dynamic_cast<Parameter<V> *>(par);
            if (tpar == nullptr)
                throw Exception("Parameter '{}' has unexpected type ({})", name, par->type());
            if (tpar->valid)
                return tpar->get();
            else
                return {};
        }
        else {
            auto it = this->params.find(name);
            if (it == this->params.end())
                throw Exception("No parameter '{}' found.", name);

            auto par = it->second.get();
            auto tpar = dynamic_cast<Parameter<T> *>(par);
            if (tpar == nullptr)
                throw Exception("Parameter '{}' has unexpected type ({})", name, par->type());
            if (tpar->valid)
                return tpar->get();
            if (tpar->required)
                throw Exception("Required parameter '{}' is not set", name);
            throw Exception("Parameter '{}' is uninitialized", name);
        }
    }

    template <typename T>
    inline T
    get(String name, T default_value) const
    {
        CALL_STACK_MSG();
        auto it = this->params.find(name);
        if (it == this->params.end())
            return default_value;
        auto par = it->second.get();
        auto tpar = dynamic_cast<Parameter<T> *>(par);
        if (tpar->valid)
            return tpar->get();
        else
            return default_value;
    }

    /// Set parameter
    template <typename T>
    inline Parameters &
    set(String name, T value)
    {
        CALL_STACK_MSG();
        if (!this->has<T>(name))
            this->params[name] = Qtr<Parameter<T>>::alloc();

        this->params[name]->valid = true;
        dynamic_cast<Parameter<T> *>(this->params[name].get())->set(value);
        return *this;
    }

    /// This method adds a parameter and documentation string to the Parameters
    /// object that will be extracted from the input file.  If the parameter is
    /// missing in the input file, and error will be thrown
    template <typename T>
    Parameters &
    add_required_param(String name, String doc_string)
    {
        CALL_STACK_MSG();
        if (!this->has<T>(name)) {
            auto param = Qtr<Parameter<T>>::alloc();
            param->required = true;
            param->is_private = false;
            param->doc_string = doc_string;
            param->valid = false;
            this->params[name] = std::move(param);
        }
        return *this;
    }

    ///@{
    /// These methods add an option parameter and a documentation string to the Parameters
    /// object. The first version of this function takes a default value which is used if the
    /// parameter is not found in the input file. The second method will leave the parameter
    /// uninitialized but can be checked with "is_param_valid" before use.
    template <typename T, typename S>
    Parameters &
    add_param(String name, const S & value, String doc_string)
    {
        CALL_STACK_MSG();
        if (!this->has<T>(name)) {
            auto param = Qtr<Parameter<T>>::alloc();
            param->required = false;
            param->value = value;
            param->is_private = false;
            param->doc_string = doc_string;
            param->valid = true;
            this->params[name] = std::move(param);
        }
        return *this;
    }

    template <typename T>
    Parameters &
    add_param(String name, String doc_string)
    {
        CALL_STACK_MSG();
        if (!this->has<T>(name)) {
            auto param = Qtr<Parameter<T>>::alloc();
            param->required = false;
            param->is_private = false;
            param->doc_string = doc_string;
            param->valid = false;
            this->params[name] = std::move(param);
        }
        return *this;
    }
    ///@}

    ///@{
    /// These methods add a parameter to the Parameters object which can be retrieved like any
    /// other parameter. This parameter however is not printed in the Input file syntax dump or web
    /// page dump so does not take a documentation string.  The first version of this function takes
    /// an optional default value.
    template <typename T>
    Parameters &
    add_private_param(String name, const T & value)
    {
        CALL_STACK_MSG();
        auto param = Qtr<Parameter<T>>::alloc();
        param->value = value;
        param->required = false;
        param->is_private = true;
        param->valid = true;
        this->params[name] = std::move(param);
        return *this;
    }

    template <typename T>
    Parameters &
    add_private_param(String name)
    {
        CALL_STACK_MSG();
        expect_true(!this->has<T>(name), "Private parameter '{}' already exists", name);

        auto param = Qtr<Parameter<T>>::alloc();
        param->value = {};
        param->required = true;
        param->is_private = true;
        param->valid = false;
        this->params[name] = std::move(param);
        return *this;
    }

    ///@}

    inline void
    make_param_required(String name)
    {
        CALL_STACK_MSG();
        auto it = this->params.find(name);
        if (it == this->params.end())
            throw Exception("No parameter '{}' found.", name);
        auto par = it->second.get();
        par->required = true;
    }

    /// Returns a boolean indicating whether the specified parameter is required or not
    bool is_param_required(String name) const;

    /// This method returns parameters that have been initialized in one fashion or another,
    /// i.e. The value was supplied as a default argument or read and properly converted from
    /// the input file
    bool is_param_valid(String name) const;

    bool is_param_private(String name) const;

    ///
    String type(String name) const;

    ///
    String get_doc_string(String name) const;

    /// Parameter map iterator.
    using iterator = std::map<String, Qtr<Parameters::Value>>::iterator;

    /// Constant parameter map iterator.
    using const_iterator = std::map<String, Qtr<Parameters::Value>>::const_iterator;

    /// Iterator pointing to the beginning of the set of parameters.
    Parameters::iterator begin();

    /// Iterator pointing to the beginning of the set of parameters.
    Parameters::const_iterator begin() const;

    /// Iterator pointing to the end of the set of parameters
    Parameters::iterator end();

    /// Iterator pointing to the end of the set of parameters
    Parameters::const_iterator end() const;

    /// Add `other` Parameters into this Parameters object
    Parameters & operator+=(const Parameters & other);

    void clear();

private:
    /// The actual parameter data. Each Metadata object contains attributes for the corresponding
    /// parameter.
    std::map<String, Qtr<Value>> params;
};

} // namespace godzilla
