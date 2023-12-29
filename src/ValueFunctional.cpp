// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ValueFunctional.h"
#include "godzilla/CallStack.h"

namespace godzilla {

ValueFunctional::ValueFunctional(FEProblemInterface * fepi, const std::string & region) :
    Functional(fepi, region),
    evalr(fepi)
{
}

const std::set<std::string> &
ValueFunctional::get_provided_values() const
{
    CALL_STACK_MSG();
    return this->provides;
}

} // namespace godzilla
