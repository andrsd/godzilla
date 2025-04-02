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

} // namespace godzilla
