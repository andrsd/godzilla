// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Logger.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Logger::Logger()
{
    CALL_STACK_MSG();
}

} // namespace godzilla
