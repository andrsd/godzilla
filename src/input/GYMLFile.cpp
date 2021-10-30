#include "GYMLFile.h"
#include "InputParameters.h"
#include "GodzillaApp.h"
#include "Factory.h"
#include "GMesh.h"
#include "GProblem.h"
#include "GExecutioner.h"


GYMLFile::GYMLFile(const GodzillaApp & app, Factory & factory) :
    _app(app),
    _factory(factory)
{
}

void
GYMLFile::parse(const std::string & file_name)
{
  _root = YAML::LoadFile(file_name);
}

std::shared_ptr<GExecutioner>
GYMLFile::getExecutioner()
{
    return _executioner;
}

void
GYMLFile::build()
{
    buildMesh();
    buildProblem();
    buildExecutioner();
}

void
GYMLFile::buildMesh()
{
    const std::string class_name = "GMesh";
    InputParameters params = _factory.getValidParams(class_name);
    params.set<const GodzillaApp *>("_gapp") = &_app;
    _mesh = _factory.create<GMesh>(class_name, "mesh", params);
}

void
GYMLFile::buildProblem()
{
    const std::string class_name = "GProblem";
    InputParameters params = _factory.getValidParams(class_name);
    params.set<const GodzillaApp *>("_gapp") = &_app;
    params.set<GMesh *>("_gmesh") = _mesh.get();
    _problem = _factory.create<GProblem>(class_name, "problem", params);
}


void
GYMLFile::buildExecutioner()
{
    const std::string class_name = "GExecutioner";
    InputParameters params = _factory.getValidParams(class_name);
    params.set<const GodzillaApp *>("_gapp") = &_app;
    params.set<GProblem *>("_gproblem") = _problem.get();
    _executioner = _factory.create<GExecutioner>(class_name, "problem", params);
}
