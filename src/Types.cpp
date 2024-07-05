// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Types.h"
#include "godzilla/Exception.h"

namespace godzilla {

namespace internal {

ErrorCode
invoke_function_delegate(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    auto * method = static_cast<FunctionDelegate *>(ctx);
    method->invoke(time, x, u);
    return 0;
}

} // namespace internal

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
    default:
        throw InternalError("Unsupported element type");
    }
}

} // namespace godzilla
