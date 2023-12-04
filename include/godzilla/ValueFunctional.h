// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/Types.h"
#include "godzilla/Functional.h"
#include "godzilla/FEProblemInterface.h"

namespace godzilla {

class ValueFunctional : public Functional {
public:
    ValueFunctional(FEProblemInterface * fepi, const std::string & region = "");

    /// Get value names provided by this functional
    ///
    /// @return List of value names provided by this functional
    const std::set<std::string> & get_provided_values() const;

    /// Evaluate this functional
    virtual void evaluate() const = 0;

protected:
    template <typename T>
    T &
    declare_value(const std::string & name)
    {
        _F_;
        auto val_name = get_value_name(name);
        this->provides.insert(val_name);
        return this->evalr->declare_value<T>(val_name);
    }

private:
    /// Dependency evaluator
    DependencyEvaluator * evalr;
    /// Values this object provides (i.e what was announced via `declare_value`)
    std::set<std::string> provides;
};

} // namespace godzilla
