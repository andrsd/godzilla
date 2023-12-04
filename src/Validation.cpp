// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
#include "godzilla/CallStack.h"
#include <algorithm>

namespace godzilla {
namespace validation {

bool
in(const std::string & value, const std::vector<std::string> & options)
{
    _F_;
    std::string v = utils::to_lower(value);
    return std::any_of(options.cbegin(), options.cend(), [v](const std::string & o) {
        return v == utils::to_lower(o);
    });
}

} // namespace validation
} // namespace godzilla
