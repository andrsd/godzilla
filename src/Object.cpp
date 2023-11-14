#include "Object.h"
#include "App.h"
#include "CallStack.h"

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
    _app(get_param<App *>("_app")),
    type(get_param<std::string>("_type")),
    name(get_param<std::string>("_name"))
{
    _F_;
}

Object::~Object()
{
    _F_;
}

const std::string &
Object::get_type() const
{
    _F_;
    return this->type;
}

const std::string &
Object::get_name() const
{
    _F_;
    return this->name;
}

const Parameters &
Object::get_parameters() const
{
    _F_;
    return this->pars;
}

bool
Object::is_param_valid(const std::string & par_name) const
{
    _F_;
    return this->pars.is_param_valid(par_name);
}

App *
Object::get_app() const
{
    _F_;
    return this->_app;
}

const mpi::Communicator &
Object::get_comm() const
{
    _F_;
    return this->_app->comm();
}

const mpi::Communicator &
Object::comm() const
{
    _F_;
    return this->_app->comm();
}

int
Object::get_processor_id() const
{
    _F_;
    return this->_app->comm().rank();
}

void
Object::create()
{
    _F_;
}

void
Object::check()
{
    _F_;
}

} // namespace godzilla
