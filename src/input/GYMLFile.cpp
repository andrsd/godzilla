#include "GYMLFile.h"
#include "InputParameters.h"
#include "GodzillaApp.h"
#include "Factory.h"
#include "GMesh.h"
#include "GProblem.h"
#include "GExecutioner.h"

template<typename T>
std::string type_name()
{
    return demangle(typeid(T).name());
}


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
    InputParameters params = buildParams(_root, "mesh");
    const std::string & class_name = params.get<std::string>("_type");
    _mesh = _factory.create<GMesh>(class_name, "mesh", params);
}

void
GYMLFile::buildProblem()
{
    InputParameters params = buildParams(_root, "problem");
    const std::string & class_name = params.get<std::string>("_type");
    params.set<GMesh *>("_gmesh") = _mesh.get();
    _problem = _factory.create<GProblem>(class_name, "problem", params);
}

void
GYMLFile::buildExecutioner()
{
    InputParameters params = buildParams(_root, "executioner");
    const std::string & class_name = params.get<std::string>("_type");
    params.set<GProblem *>("_gproblem") = _problem.get();
    _executioner = _factory.create<GExecutioner>(class_name, "problem", params);
}

InputParameters
GYMLFile::buildParams(const YAML::Node & root, const std::string & name)
{
    YAML::Node node = root[name];
    if (!node)
        godzillaError("Missing '", name, "' block.");

    YAML::Node type = node["type"];
    if (!type)
        godzillaError(name, ": No 'type' specified.");

    const std::string & class_name = type.as<std::string>();
    if (!Registry::isRegisteredObj(class_name))
        godzillaError(name, ": Type '", class_name, "' is not a registered object.");

    InputParameters params = _factory.getValidParams(class_name);
    params.set<std::string>("_type") = class_name;
    params.set<const GodzillaApp *>("_gapp") = &_app;

    for (auto & kv : params) {
        const std::string & param_name = kv.first;
        if (!params.isPrivate(param_name))
            setParameterFromYML(params, node, param_name);
    }

    return params;
}

void
GYMLFile::setParameterFromYML(InputParameters & params, const YAML::Node & node, const std::string & param_name)
{
    YAML::Node val = node[param_name];
    if (val) {
        const std::string & param_type = params.type(param_name);

        if (param_type == type_name<std::string>())
            params.set<std::string>(param_name) = val.as<std::string>();
        else if (param_type == type_name<Real>())
            params.set<Real>(param_name) = val.as<double>();
        else if (param_type == type_name<int>())
            params.set<int>(param_name) = val.as<int>();
        else if (param_type == type_name<unsigned int>())
            params.set<unsigned int>(param_name) = val.as<unsigned int>();
    }
}
