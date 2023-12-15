// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Godzilla.h"
#include "godzilla/BasicTSAdapt.h"
#include "godzilla/CallStack.h"

namespace godzilla {

REGISTER_OBJECT(BasicTSAdapt);

Parameters
BasicTSAdapt::parameters()
{
    Parameters params = TimeSteppingAdaptor::parameters();
    return params;
}

BasicTSAdapt::BasicTSAdapt(const Parameters & params) : TimeSteppingAdaptor(params) {}

void
BasicTSAdapt::create()
{
    _F_;
    TimeSteppingAdaptor::create();
    set_type(TSADAPTBASIC);
}

} // namespace godzilla
