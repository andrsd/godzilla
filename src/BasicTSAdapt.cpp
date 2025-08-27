// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/BasicTSAdapt.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
BasicTSAdapt::parameters()
{
    auto params = TimeSteppingAdaptor::parameters();
    return params;
}

BasicTSAdapt::BasicTSAdapt(const Parameters & params) : TimeSteppingAdaptor(params) {}

void
BasicTSAdapt::create()
{
    CALL_STACK_MSG();
    TimeSteppingAdaptor::create();
    set_type(TSADAPTBASIC);
}

} // namespace godzilla
