// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Object.h"
#include "godzilla/CoreApp.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
Object::parameters()
{
    Parameters params;
    params.add_required_param<LateRef<CoreApp>>("app", "Application we are part of")
        .add_private_param<String>("_type", "")
        .add_param<mpi::Communicator>("comm", "MPI communicator")
        .add_param<String>("name", "Name of the object");
    return params;
}

Object::Object(const Parameters & pars) :
    LoggingInterface(pars.get<Ref<CoreApp>>("app")->get_logger()),
    app_(pars.get<Ref<CoreApp>>("app")),
    comm_(pars.get<mpi::Communicator>("comm", this->app_->get_comm())),
    type_(pars.get<String>("_type")),
    name_(pars.get<String>("name", ""))
{
    CALL_STACK_MSG();
}

String
Object::get_type() const
{
    CALL_STACK_MSG();
    return this->type_;
}

String
Object::get_name() const
{
    CALL_STACK_MSG();
    return this->name_;
}

Ref<CoreApp>
Object::get_app() const
{
    CALL_STACK_MSG();
    return this->app_;
}

mpi::Communicator
Object::get_comm() const
{
    CALL_STACK_MSG();
    return this->comm_;
}

int
Object::get_processor_id() const
{
    CALL_STACK_MSG();
    return this->comm_.rank();
}

void
Object::create()
{
    CALL_STACK_MSG();
}

} // namespace godzilla
