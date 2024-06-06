// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/FunctionDelegate.h"

namespace godzilla::internal {

ErrorCode
invoke_function_method(Int dim, Real time, const Real x[], Int nc, Scalar u[], void * ctx)
{
    auto * method = static_cast<FunctionMethodAbstract *>(ctx);
    return method->invoke(dim, time, x, nc, u);
}

} // namespace godzilla::internal
