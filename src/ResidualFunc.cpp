// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ResidualFunc.h"

namespace godzilla {

AbstractResidualFunctional::AbstractResidualFunctional(FEProblemInterface * fepi,
                                                       const std::string & region) :
    Functional(fepi, region)
{
}

} // namespace godzilla
