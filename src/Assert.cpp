// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Assert.h"
#include "godzilla/String.h"
#include "godzilla/Utils.h"

namespace godzilla {

void
expect_matching_type(String pars_type, String type, std::source_location loc)
{
    expect_true(
        pars_type == type,
        fmt::format("Mismatch in the type of object ({}) and parameters used for construction ({})",
                    utils::demangle(type),
                    utils::demangle(pars_type)),
        loc);
}

} // namespace godzilla
