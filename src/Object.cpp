// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/Object.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"

namespace godzilla {

Parameters
Object::parameters()
{
    Parameters params;
    params.add_required_param<App *>("app", "Application we are part of")
        .add_private_param<std::string>("_type", "")
        .add_param<std::string>("name", "Name of the object");
    return params;
}

Object::Object(const Parameters & pars) :
    LoggingInterface(pars.get<App *>("app")->get_logger(), pars.get<std::string>("name", "")),
    app(pars.get<App *>("app")),
    type(pars.get<std::string>("_type")),
    name(pars.get<std::string>("name", ""))
{
    CALL_STACK_MSG();
}

const std::string &
Object::get_type() const
{
    CALL_STACK_MSG();
    return this->type;
}

const std::string &
Object::get_name() const
{
    CALL_STACK_MSG();
    return this->name;
}

App *
Object::get_app() const
{
    CALL_STACK_MSG();
    return this->app;
}

const mpi::Communicator &
Object::get_comm() const
{
    CALL_STACK_MSG();
    return this->app->get_comm();
}

int
Object::get_processor_id() const
{
    CALL_STACK_MSG();
    return this->app->get_comm().rank();
}

void
Object::create()
{
    CALL_STACK_MSG();
}

} // namespace godzilla
