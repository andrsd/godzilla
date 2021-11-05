#include "base/Object.h"


namespace godzilla {

InputParameters
Object::validParams()
{
  InputParameters params = emptyInputParameters();
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
}

const std::string &
Object::getType() const
{
    return this->type;
}

const std::string &
Object::getName() const
{
    return this->name;
}

const InputParameters &
Object::getParameters() const
{
    return this->pars;
}

bool
Object::isParamValid(const std::string & name) const
{
    return this->pars.isParamValid(name);
}

const App &
Object::getApp() const
{
    return this->app;
}

MPI_Comm
Object::comm()
{
    return MPI_COMM_WORLD;
}

}
