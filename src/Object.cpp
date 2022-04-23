#include "Object.h"
#include "App.h"

namespace godzilla {

InputParameters
Object::validParams()
{
    InputParameters params = empty_input_parameters();
    params.add_private_param<const App *>("_app", nullptr);
    params.add_private_param<std::string>("_type");
    params.add_private_param<std::string>("_name");
    return params;
}

Object::Object(const InputParameters & parameters) :
    LoggingInterface(const_cast<Logger &>(parameters.get<const App *>("_app")->get_logger()),
                     parameters.get<std::string>("_name")),
    pars(parameters),
    app(*get_param<const App *>("_app")),
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

const InputParameters &
Object::get_parameters() const
{
    _F_;
    return this->pars;
}

bool
Object::is_param_valid(const std::string & name) const
{
    _F_;
    return this->pars.is_param_valid(name);
}

const App &
Object::get_app() const
{
    _F_;
    return this->app;
}

const MPI_Comm &
Object::comm() const
{
    _F_;
    return this->app.get_comm();
}

const PetscMPIInt &
Object::processor_id() const
{
    _F_;
    return this->app.get_comm_rank();
}

const PetscMPIInt &
Object::comm_size() const
{
    _F_;
    return this->app.get_comm_size();
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
