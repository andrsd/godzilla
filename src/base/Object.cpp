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

MPI_Comm
Object::comm()
{
    return MPI_COMM_WORLD;
}

}
