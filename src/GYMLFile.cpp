#include "GYMLFile.h"
#include "App.h"
#include "Factory.h"
#include "GGrid.h"
#include "GProblem.h"
#include "GExecutioner.h"
#include "GOutput.h"
#include "CallStack.h"
#include "assert.h"

template<typename T>
std::string type_name()
{
    return typeid(T).name();
}

namespace godzilla {

GYMLFile::GYMLFile(const App & app) :
    GPrintInterface(app),
    app(app),
    grid(nullptr),
    problem(nullptr),
    executioner(nullptr)
{
    _F_;
}

void
GYMLFile::parse(const std::string & file_name)
{
    _F_;
    this->root = YAML::LoadFile(file_name);
}

GGrid *
GYMLFile::getGrid()
{
    _F_;
    return this->grid;
}

GProblem *
GYMLFile::getProblem()
{
    _F_;
    return this->problem;
}

GExecutioner *
GYMLFile::getExecutioner()
{
    _F_;
    return this->executioner;
}

const YAML::Node &
GYMLFile::getYml()
{
    return this->root;
}

void
GYMLFile::build()
{
    _F_;
    buildGrid();
    buildProblem();
    buildExecutioner();
    buildOutputs();
}

void
GYMLFile::buildGrid()
{
    _F_;
    InputParameters params = buildParams(this->root, "grid");
    const std::string & class_name = params.get<std::string>("_type");
    this->grid = Factory::create<GGrid>(class_name, "grid", params);
}

void
GYMLFile::buildProblem()
{
    _F_;
    InputParameters params = buildParams(this->root, "problem");
    const std::string & class_name = params.get<std::string>("_type");
    params.set<GGrid *>("_ggrid") = this->grid;
    this->problem = Factory::create<GProblem>(class_name, "problem", params);
}

void
GYMLFile::buildExecutioner()
{
    _F_;
    InputParameters params = buildParams(this->root, "executioner");
    const std::string & class_name = params.get<std::string>("_type");
    params.set<GProblem *>("_gproblem") = this->problem;
    this->executioner = Factory::create<GExecutioner>(class_name, "problem", params);
}

void
GYMLFile::buildOutputs()
{
    _F_;
    YAML::Node output_root_node = this->root["output"];
    if (!output_root_node)
        return;

    for (const auto & it : output_root_node) {
        YAML::Node output_node = it.first;
        std::string name = output_node.as<std::string>();

        InputParameters params = buildParams(output_root_node, name);
        const std::string & class_name = params.get<std::string>("_type");
        params.set<GProblem *>("_gproblem") = this->problem;
        auto output = Factory::create<GOutput>(class_name, name, params);
        assert(this->executioner != nullptr);
        this->executioner->addOutput(output);
    }
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
    if (!Factory::isRegistered(class_name))
        godzillaError(name, ": Type '", class_name, "' is not a registered object.");

    InputParameters params = Factory::getValidParams(class_name);
    params.set<std::string>("_type") = class_name;
    params.set<std::string>("_name") = name;
    params.set<const App *>("_app") = &this->app;

    for (auto & kv : params) {
        const std::string & param_name = kv.first;
        if (!params.isPrivate(param_name))
            setParameterFromYML(params, node, param_name);
    }

    checkParams(params, name);

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
        else if (param_type == type_name<PetscReal>())
            params.set<PetscReal>(param_name) = val.as<double>();
        else if (param_type == type_name<int>())
            params.set<int>(param_name) = val.as<int>();
        else if (param_type == type_name<unsigned int>())
            params.set<unsigned int>(param_name) = val.as<unsigned int>();
    }
}

void
GYMLFile::checkParams(const InputParameters & params, const std::string & name)
{
    std::ostringstream oss;

    for (const auto & it : params)
    {
        const auto & param_name = it.first;
        if (!params.isParamValid(param_name) && params.isParamRequired(param_name))
            oss << std::endl << "- '" << param_name << "': " << params.getDocString(param_name);
    }

    if (!oss.str().empty())
        godzillaError(name, ": Missing required parameters:", oss.str());
}

}
