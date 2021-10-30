#include "GYMLFile.h"
#include "InputParameters.h"
#include "GodzillaApp.h"
#include "Factory.h"
#include "GMesh.h"
#include "GProblem.h"
#include "GExecutioner.h"


GYMLFile::GYMLFile(const GodzillaApp & app, Factory & factory) :
    GPrintInterface(app),
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
    if (!_root["mesh"])
        godzillaError("Missing 'mesh' block.");

    YAML::Node mesh_node = _root["mesh"];
    if (!mesh_node["type"])
        godzillaError("No 'type' specified in the 'mesh' block in the input file.");

    const std::string class_name = mesh_node["type"].as<std::string>();

    if (!Registry::isRegisteredObj(class_name))
        godzillaError("Type '", class_name, "' is not a registered object.");

    InputParameters params = _factory.getValidParams(class_name);
    params.set<const GodzillaApp *>("_gapp") = &_app;
    _mesh = _factory.create<GMesh>(class_name, "mesh", params);
}

void
GYMLFile::buildProblem()
{
    if (!_root["problem"])
        godzillaError("Missing 'problem' block.");

    YAML::Node problem_node = _root["problem"];
    if (!problem_node["type"])
        godzillaError("No 'type' specified in the 'problem' block in the input file.");

    const std::string class_name = problem_node["type"].as<std::string>();

    if (!Registry::isRegisteredObj(class_name))
        godzillaError("Type '", class_name, "' is not a registered object.");

    InputParameters params = _factory.getValidParams(class_name);
    params.set<const GodzillaApp *>("_gapp") = &_app;
    params.set<GMesh *>("_gmesh") = _mesh.get();
    _problem = _factory.create<GProblem>(class_name, "problem", params);
}


void
GYMLFile::buildExecutioner()
{
    if (!_root["executioner"])
        godzillaError("Missing 'executioner' block.");

    YAML::Node executioner_node = _root["executioner"];
    if (!executioner_node["type"])
        godzillaError("No 'type' specified in the 'executioner' block in the input file.");

    const std::string class_name = executioner_node["type"].as<std::string>();

    if (!Registry::isRegisteredObj(class_name))
        godzillaError("Type '", class_name, "' is not a registered object.");

    InputParameters params = _factory.getValidParams(class_name);
    params.set<const GodzillaApp *>("_gapp") = &_app;
    params.set<GProblem *>("_gproblem") = _problem.get();
    _executioner = _factory.create<GExecutioner>(class_name, "problem", params);
}
