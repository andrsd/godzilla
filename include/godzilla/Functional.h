// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/FEProblemInterface.h"
#include "godzilla/CallStack.h"
#include <string>
#include <set>

namespace godzilla {

class FEProblemInterface;

class Functional {
public:
    Functional(FEProblemInterface * fepi, const std::string & region = "");
    virtual ~Functional() = default;

    /// Get value names this functional depends on
    ///
    /// @return List of value names this functional depends on
    const std::set<std::string> & get_dependent_values() const;

    /// Get region where this function is defined
    ///
    /// @return Region name where this functional is defined
    const std::string & get_region() const;

    const std::string
    get_value_name(const std::string & val_name) const
    {
        return this->region.empty() ? val_name : val_name + "@" + this->region;
    }

protected:
    /// Get pointer to FEProblemInterface
    ///
    /// @return Pointer to FEProblemInterface
    FEProblemInterface * get_fe_problem() const;

    /// Get spatial dimension
    ///
    /// @return Spatial dimension
    const Int & get_spatial_dimension() const;

    /// Get physical coordinates
    ///
    /// @return Physical coordinates
    const Point & get_xyz() const;

    /// Get values of a field
    ///
    /// @param field_name The name of the field
    /// @return Reference to a class that contains the field values
    const FieldValue & get_field_value(const std::string & field_name) const;

    /// Get values of a gradient of a field
    ///
    /// @param field_name The name of the field
    /// @return Pointer to array that contains the field gradient values
    const FieldGradient & get_field_gradient(const std::string & field_name) const;

    /// Get values of a time derivative of a field
    ///
    /// @param field_name The name of the field
    /// @return Reference to a class that contains the field time derivative values
    const FieldValue & get_field_dot(const std::string & field_name) const;

    /// Get time at which the function is evaluated
    ///
    /// @return Time at which is the function evaluated
    const Real & get_time() const;

    /// Get value specified by a name
    ///
    /// @tparam T Type of the value
    /// @param name The name of the value
    /// @return Const reference to the value
    template <typename T>
    const T &
    get_value(const std::string & name)
    {
        CALL_STACK_MSG();
        auto val_name = get_value_name(name);
        this->depends_on.insert(val_name);
        return get_fe_problem()->get_value<T>(val_name);
    }

private:
    /// FEProblemInterface this functional is part of
    FEProblemInterface * fepi;
    /// region where this functional is defined
    std::string region;
    /// Value names this functional depends on
    std::set<std::string> depends_on;
};

} // namespace godzilla
