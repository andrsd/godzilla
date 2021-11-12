#pragma once

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include "PrintInterface.h"

namespace godzilla {

/// Class for user-defined parameters
///
class InputParameters {
protected:
    /// Base class for parameter values
    class Value {
    public:
        virtual std::string type() const = 0;

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
        type() const
        {
            return std::string(typeid(T).name());
        }

        /// Parameter value
        T value;
    };

public:
    InputParameters(const InputParameters & rhs);

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
            error("No parameter '", name, "' found.");

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

    /// This method adds a parameter and documentation string to the InputParameters
    /// object that will be extracted from the input file.  If the parameter is
    /// missing in the input file, and error will be thrown
    template <typename T>
    void addRequiredParam(const std::string & name, const std::string & doc_string);

    ///@{
    /// These methods add an option parameter and a documentation string to the InputParameters
    /// object. The first version of this function takes a default value which is used if the
    /// parameter is not found in the input file. The second method will leave the parameter
    /// uninitialized but can be checked with "isParamValid" before use.
    template <typename T, typename S>
    void addParam(const std::string & name, const S & value, const std::string & doc_string);
    template <typename T>
    void addParam(const std::string & name, const std::string & doc_string);
    ///@}

    ///@{
    /// These methods add a parameter to the InputParameters object which can be retrieved like any
    /// other parameter. This parameter however is not printed in the Input file syntax dump or web
    /// page dump so does not take a documentation string.  The first version of this function takes
    /// an optional default value.
    template <typename T>
    void addPrivateParam(const std::string & name, const T & value);
    template <typename T>
    void addPrivateParam(const std::string & name);
    ///@}

    /// Returns a boolean indicating whether the specified parameter is required or not
    bool
    isParamRequired(const std::string & name) const
    {
        return this->params.count(name) > 0 && this->params.at(name)->required;
    }

    /// This method returns parameters that have been initialized in one fashion or another,
    /// i.e. The value was supplied as a default argument or read and properly converted from
    /// the input file
    bool
    isParamValid(const std::string & name) const
    {
        return this->params.count(name) > 0 && this->params.at(name)->valid;
    }

    bool
    isPrivate(const std::string & name) const
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
    getDocString(const std::string & name) const
    {
        auto it = this->params.find(name);
        if (it != this->params.end())
            return it->second->doc_string;
        else
            return std::string();
    }

    /// Parameter map iterator.
    typedef std::map<std::string, InputParameters::Value *>::iterator iterator;

    /// Constant parameter map iterator.
    typedef std::map<std::string, InputParameters::Value *>::const_iterator const_iterator;

    /// Iterator pointing to the beginning of the set of parameters.
    InputParameters::iterator
    begin()
    {
        return this->params.begin();
    }

    /// Iterator pointing to the beginning of the set of parameters.
    InputParameters::const_iterator
    begin() const
    {
        return this->params.begin();
    }

    /// Iterator pointing to the end of the set of parameters
    InputParameters::iterator
    end()
    {
        return this->params.end();
    }

    /// Iterator pointing to the end of the set of parameters
    InputParameters::const_iterator
    end() const
    {
        return this->params.end();
    }

private:
    /// Private constructor so that InputParameters can only be created in certain places.
    InputParameters();

    /// This method is called when adding a Parameter with a default value, can be specialized for
    /// non-matching types.
    template <typename T, typename S>
    void setParamHelper(const std::string & name, T & l_value, const S & r_value);

    /// The actual parameter data. Each Metadata object contains attributes for the corresponding
    /// parameter.
    std::map<std::string, Value *> params;

    // These are the only objects allowed to _create_ InputParameters
    friend InputParameters emptyInputParameters();
};

template <typename T>
void
InputParameters::addRequiredParam(const std::string & name, const std::string & doc_string)
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
InputParameters::addParam(const std::string & name, const std::string & doc_string)
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
InputParameters::addParam(const std::string & name, const S & value, const std::string & doc_string)
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
InputParameters::addPrivateParam(const std::string & name)
{
    if (!this->has<T>(name)) {
        Parameter<T> * param = new Parameter<T>;
        param->required = false;
        param->is_private = true;
        param->set_by_add_param = false;
        param->valid = false;
        this->params[name] = param;
    }
}

template <typename T>
void
InputParameters::addPrivateParam(const std::string & name, const T & value)
{
    Parameter<T> * param = new Parameter<T>;
    param->value = value;
    param->required = false;
    param->is_private = true;
    param->set_by_add_param = true;
    param->valid = true;
    this->params[name] = param;
}

InputParameters emptyInputParameters();

} // namespace godzilla
