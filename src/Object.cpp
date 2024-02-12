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
    params.add_private_param<App *>("_app", nullptr);
    params.add_private_param<std::string>("_type", "");
    params.add_private_param<std::string>("_name", "");
    return params;
}

Object::Object(const Parameters & parameters) :
    LoggingInterface(parameters.get<App *>("_app")->get_logger(),
                     parameters.get<std::string>("_name")),
    pars(parameters),
    app(get_param<App *>("_app")),
    type(get_param<std::string>("_type")),
    name(get_param<std::string>("_name"))
{
    CALL_STACK_MSG();
}

Object::~Object()
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

const Parameters &
Object::get_parameters() const
{
    CALL_STACK_MSG();
    return this->pars;
}

bool
Object::is_param_valid(const std::string & par_name) const
{
    CALL_STACK_MSG();
    return this->pars.is_param_valid(par_name);
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
