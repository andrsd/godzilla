#include "Object.h"
#include "App.h"

namespace godzilla {

InputParameters
Object::validParams()
{
    InputParameters params = emptyInputParameters();
    params.addPrivateParam<const App *>("_app", nullptr);
    params.addPrivateParam<std::string>("_type");
    params.addPrivateParam<std::string>("_name");
    return params;
}

Object::Object(const InputParameters & parameters) :
    pars(parameters),
    app(*getParam<const App *>("_app")),
    type(getParam<std::string>("_type")),
    name(getParam<std::string>("_name"))
{
    _F_;
    MPI_Comm_rank(comm(), &this->rank);
}

Object::~Object()
{
    _F_;
}

const std::string &
Object::getType() const
{
    _F_;
    return this->type;
}

const std::string &
Object::getName() const
{
    _F_;
    return this->name;
}

const InputParameters &
Object::getParameters() const
{
    _F_;
    return this->pars;
}

bool
Object::isParamValid(const std::string & name) const
{
    _F_;
    return this->pars.isParamValid(name);
}

const App &
Object::getApp() const
{
    _F_;
    return this->app;
}

const MPI_Comm &
Object::comm() const
{
    _F_;
    return this->app.getComm();
}

const int &
Object::processorId() const
{
    _F_;
    return this->rank;
}

void
Object::create()
{
    _F_;
}

} // namespace godzilla
