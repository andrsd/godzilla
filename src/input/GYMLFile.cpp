#include "input/GYMLFile.h"
#include "base/GodzillaApp.h"
#include "Factory.h"
#include "mesh/GMesh.h"
#include "problems/GProblem.h"
#include "executioners/GExecutioner.h"
#include "base/CallStack.h"

template<typename T>
std::string type_name()
{
    return demangle(typeid(T).name());
}


GYMLFile::GYMLFile(const GodzillaApp & app, Factory & factory) :
    GPrintInterface(app),
    app(app),
    factory(factory)
{
    _F_;
}

void
GYMLFile::parse(const std::string & file_name)
{
    _F_;
    this->root = YAML::LoadFile(file_name);
}

std::shared_ptr<GMesh>
GYMLFile::getMesh()
{
    _F_;
    return this->mesh;
}

std::shared_ptr<GProblem>
GYMLFile::getProblem()
{
    _F_;
    return this->problem;
}

std::shared_ptr<GExecutioner>
GYMLFile::getExecutioner()
{
    _F_;
    return this->executioner;
}

void
GYMLFile::build()
{
    _F_;
    buildMesh();
    buildProblem();
    buildExecutioner();
}

void
GYMLFile::buildMesh()
{
    _F_;
    InputParameters params = buildParams(root, "mesh");
    const std::string & class_name = params.get<std::string>("_type");
    this->mesh = factory.create<GMesh>(class_name, "mesh", params);
}

void
GYMLFile::buildProblem()
{
    _F_;
    InputParameters params = buildParams(root, "problem");
    const std::string & class_name = params.get<std::string>("_type");
    params.set<GMesh *>("_gmesh") = mesh.get();
    this->problem = factory.create<GProblem>(class_name, "problem", params);
}

void
GYMLFile::buildExecutioner()
{
    _F_;
    InputParameters params = buildParams(root, "executioner");
    const std::string & class_name = params.get<std::string>("_type");
    params.set<GProblem *>("_gproblem") = problem.get();
    this->executioner = factory.create<GExecutioner>(class_name, "problem", params);
}

InputParameters
GYMLFile::buildParams(const YAML::Node & root, const std::string & name)
{
    _F_;
    YAML::Node node = root[name];
    if (!node)
        godzillaError("Missing '", name, "' block.");

    YAML::Node type = node["type"];
    if (!type)
        godzillaError(name, ": No 'type' specified.");

    const std::string & class_name = type.as<std::string>();
    if (!Registry::isRegisteredObj(class_name))
        godzillaError(name, ": Type '", class_name, "' is not a registered object.");

    InputParameters params = factory.getValidParams(class_name);
    params.set<std::string>("_type") = class_name;

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
    _F_;
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
