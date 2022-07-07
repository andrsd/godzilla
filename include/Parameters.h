#pragma once

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include "Error.h"

namespace godzilla {

/// Class for user-defined parameters
///
class Parameters {
public:
    Parameters();
    Parameters(const Parameters & p);
    virtual ~Parameters();

protected:
    /// Base class for parameter values
    class Value {
    public:
        virtual ~Value() {}

        /// Return the type of this value as a string
        virtual std::string type() const = 0;

        /// Create a copy of this value
        virtual Value * copy() const = 0;

        /// Is required
        bool required;
        /// Doco string
        std::string doc_string;
        ///
        bool valid;
        /// The set of parameters that will NOT appear in the the dump of the parser tree
        bool is_private;
        ///
        bool set_by_add_param;
    };

    /// Parameter value
    template <typename T>
    class Parameter : public Value {
    public:
        /// @returns A read-only reference to the parameter value.
        const T &
        get() const
        {
            return this->value;
        }

        /// @returns A writable reference to the parameter value.
        T &
        set()
        {
            return this->value;
        }

        inline std::string
        type() const override
        {
            return std::string(typeid(T).name());
        }

        virtual Value *
        copy() const override
        {
            Parameter<T> * copy = new Parameter<T>;
            copy->value = this->value;
            copy->required = this->required;
            copy->doc_string = this->doc_string;
            copy->valid = this->valid;
            copy->is_private = this->is_private;
            copy->set_by_add_param = this->set_by_add_param;
            return copy;
        }

        /// Parameter value
        T value;
    };

public:
    /// Check if parameter exist
    template <typename T>
    bool
    has(const std::string & name) const
    {
        auto it = this->params.find(name);

        if (it != this->params.end())
            if (dynamic_cast<const Parameter<T> *>(it->second) != nullptr)
                return true;

        return false;
    }

    /// Get parameter value
    template <typename T>
    inline const T &
    get(const std::string & name) const
    {
        if (!this->has<T>(name))
            error("No parameter '%s' found.", name);

        auto it = this->params.find(name);
        return dynamic_cast<Parameter<T> *>(it->second)->get();
    }

    /// Set parameter
    template <typename T>
    inline T &
    set(const std::string & name)
    {
        if (!this->has<T>(name))
            this->params[name] = new Parameter<T>;

        this->params[name]->valid = true;
        return dynamic_cast<Parameter<T> *>(this->params[name])->set();
    }

    /// This method adds a parameter and documentation string to the Parameters
    /// object that will be extracted from the input file.  If the parameter is
    /// missing in the input file, and error will be thrown
    template <typename T>
    void add_required_param(const std::string & name, const std::string & doc_string);

    ///@{
    /// These methods add an option parameter and a documentation string to the Parameters
    /// object. The first version of this function takes a default value which is used if the
    /// parameter is not found in the input file. The second method will leave the parameter
    /// uninitialized but can be checked with "is_param_valid" before use.
    template <typename T, typename S>
    void add_param(const std::string & name, const S & value, const std::string & doc_string);
    template <typename T>
    void add_param(const std::string & name, const std::string & doc_string);
    ///@}

    ///@{
    /// These methods add a parameter to the Parameters object which can be retrieved like any
    /// other parameter. This parameter however is not printed in the Input file syntax dump or web
    /// page dump so does not take a documentation string.  The first version of this function takes
    /// an optional default value.
    template <typename T>
    void add_private_param(const std::string & name, const T & value);
    ///@}

    /// Returns a boolean indicating whether the specified parameter is required or not
    bool
    is_param_required(const std::string & name) const
    {
        return this->params.count(name) > 0 && this->params.at(name)->required;
    }

    /// This method returns parameters that have been initialized in one fashion or another,
    /// i.e. The value was supplied as a default argument or read and properly converted from
    /// the input file
    bool
    is_param_valid(const std::string & name) const
    {
        return this->params.count(name) > 0 && this->params.at(name)->valid;
    }

    bool
    is_private(const std::string & name) const
    {
        return this->params.count(name) > 0 && this->params.at(name)->is_private;
    }

    ///
    std::string
    type(const std::string & name) const
    {
        return this->params.at(name)->type();
    }

    ///
    std::string
    get_doc_string(const std::string & name) const
    {
        auto it = this->params.find(name);
        if (it != this->params.end())
            return it->second->doc_string;
        else
            return std::string();
    }

    /// Parameter map iterator.
    typedef std::map<std::string, Parameters::Value *>::iterator iterator;

    /// Constant parameter map iterator.
    typedef std::map<std::string, Parameters::Value *>::const_iterator const_iterator;

    /// Iterator pointing to the beginning of the set of parameters.
    Parameters::iterator
    begin()
    {
        return this->params.begin();
    }

    /// Iterator pointing to the beginning of the set of parameters.
    Parameters::const_iterator
    begin() const
    {
        return this->params.begin();
    }

    /// Iterator pointing to the end of the set of parameters
    Parameters::iterator
    end()
    {
        return this->params.end();
    }

    /// Iterator pointing to the end of the set of parameters
    Parameters::const_iterator
    end() const
    {
        return this->params.end();
    }

    /// Assignment operator
    Parameters & operator=(const Parameters & rhs);

    /// Add `rhs` Parameters into this Parameters object
    Parameters &
    operator+=(const Parameters & rhs)
    {
        for (auto it = rhs.begin(); it != rhs.end(); ++it) {
            auto jt = this->params.find(it->first);
            if (jt != this->params.end())
                delete jt->second;
            this->params[it->first] = it->second->copy();
        }
        return *this;
    }

    void
    clear()
    {
        for (auto & it : this->params)
            delete it.second;
        this->params.clear();
    }

private:
    /// This method is called when adding a Parameter with a default value, can be specialized for
    /// non-matching types.
    template <typename T, typename S>
    void set_param_helper(const std::string & name, T & l_value, const S & r_value);

    /// The actual parameter data. Each Metadata object contains attributes for the corresponding
    /// parameter.
    std::map<std::string, Value *> params;
};

template <typename T>
void
Parameters::add_required_param(const std::string & name, const std::string & doc_string)
{
    if (!this->has<T>(name)) {
        Parameter<T> * param = new Parameter<T>;
        param->required = true;
        param->is_private = false;
        param->doc_string = doc_string;
        param->set_by_add_param = false;
        param->valid = false;
        this->params[name] = param;
    }
}

template <typename T>
void
Parameters::add_param(const std::string & name, const std::string & doc_string)
{
    if (!this->has<T>(name)) {
        Parameter<T> * param = new Parameter<T>;
        param->required = false;
        param->is_private = false;
        param->doc_string = doc_string;
        param->set_by_add_param = false;
        param->valid = false;
        this->params[name] = param;
    }
}

template <typename T, typename S>
void
Parameters::add_param(const std::string & name, const S & value, const std::string & doc_string)
{
    if (!this->has<T>(name)) {
        Parameter<T> * param = new Parameter<T>;
        param->required = false;
        param->value = value;
        param->is_private = false;
        param->doc_string = doc_string;
        param->set_by_add_param = true;
        param->valid = true;
        this->params[name] = param;
    }
}

template <typename T>
void
Parameters::add_private_param(const std::string & name, const T & value)
{
    Parameter<T> * param = new Parameter<T>;
    param->value = value;
    param->required = false;
    param->is_private = true;
    param->set_by_add_param = true;
    param->valid = true;
    this->params[name] = param;
}

} // namespace godzilla
