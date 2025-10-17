// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/BasicTSAdapt.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
BasicTSAdapt::parameters()
{
    auto params = TimeSteppingAdaptor::parameters();
    return params;
}

BasicTSAdapt::BasicTSAdapt(const Parameters & pars) : TimeSteppingAdaptor(pars) {}

void
BasicTSAdapt::create()
{
    CALL_STACK_MSG();
    TimeSteppingAdaptor::create();
    set_type(TSADAPTBASIC);
}

} // namespace godzilla
