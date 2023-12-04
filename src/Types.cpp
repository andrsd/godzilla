// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Types.h"

namespace godzilla {

std::string
get_element_type_str(const ElementType & type)
{
    switch (type) {
    case EDGE2:
        return "EDGE2";
    case TRI3:
        return "TRI3";
    case QUAD4:
        return "QUAD4";
    case TET4:
        return "TET4";
    case HEX8:
        return "HEX8";
    }
}

} // namespace godzilla
