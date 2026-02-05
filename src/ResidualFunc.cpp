// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/ResidualFunc.h"

namespace godzilla {

ResidualFunc::ResidualFunc(Ref<FEProblemInterface> fepi, String region) : Functional(fepi, region)
{
}

} // namespace godzilla
