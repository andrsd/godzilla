#include "GYMLFile.h"
#include "App.h"
#include "Factory.h"
#include "Grid.h"
#include "Problem.h"
#include "Function.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "FEProblemInterface.h"
#include "Output.h"
#include "CallStack.h"
#include "assert.h"

template <typename T>
std::string
type_name()
{
    return typeid(T).name();
}

namespace godzilla {

GYMLFile::GYMLFile(const App & app) :
    PrintInterface(app),
    LoggingInterface(const_cast<Logger &>(app.getLogger())),
    app(app),
    grid(nullptr),
    problem(nullptr)
{
    _F_;
}

void
GYMLFile::parse(const std::string & file_name)
{
    _F_;
    this->root = YAML::LoadFile(file_name);
}

Grid *
GYMLFile::getGrid()
{
    _F_;
    return this->grid;
}

Problem *
GYMLFile::getProblem()
{
    _F_;
    return this->problem;
}

const YAML::Node &
GYMLFile::getYml()
{
    _F_;
    return this->root;
}

const std::vector<Function *> &
GYMLFile::getFunctions()
{
    _F_;
    return this->functions;
}

void
GYMLFile::build()
{
    _F_;
    buildFunctions();
    buildGrid();
    buildProblem();
    buildInitialConditions();
    buildBoundaryConditions();
    buildOutputs();
}

void
GYMLFile::buildFunctions()
{
    _F_;
    YAML::Node funcs_node = this->root["functions"];
    if (!funcs_node)
        return;

    for (const auto & it : funcs_node) {
        YAML::Node fn_node = it.first;
        std::string name = fn_node.as<std::string>();

        InputParameters params = buildParams(funcs_node, name);
        const std::string & class_name = params.get<std::string>("_type");
        auto fn = Factory::create<Function>(class_name, name, params);
        this->functions.push_back(fn);
    }
}

void
GYMLFile::buildGrid()
{
    _F_;
    InputParameters & params = buildParams(this->root, "grid");
    const std::string & class_name = params.get<std::string>("_type");
    this->grid = Factory::create<Grid>(class_name, "grid", params);
}

void
GYMLFile::buildProblem()
{
    _F_;
    InputParameters & params = buildParams(this->root, "problem");
    const std::string & class_name = params.get<std::string>("_type");
    params.set<Grid *>("_grid") = this->grid;
    this->problem = Factory::create<Problem>(class_name, "problem", params);
}

void
GYMLFile::buildInitialConditions()
{
    _F_;
    YAML::Node ics_root_node = this->root["ics"];
    if (!ics_root_node)
        return;

    FEProblemInterface * fepface = dynamic_cast<FEProblemInterface *>(this->problem);
    if (fepface == nullptr)
        logError("Supplied problem type '",
                 this->problem->getType(),
                 "' does not support initial conditions.");
    else {
        for (const auto & it : ics_root_node) {
            YAML::Node ic_node = it.first;
            std::string name = ic_node.as<std::string>();

            InputParameters & params = buildParams(ics_root_node, name);
            const std::string & class_name = params.get<std::string>("_type");
            auto ic = Factory::create<InitialCondition>(class_name, name, params);
            fepface->addInitialCondition(ic);
        }
    }
}

void
GYMLFile::buildBoundaryConditions()
{
    _F_;
    YAML::Node bcs_root_node = this->root["bcs"];
    if (!bcs_root_node)
        return;

    FEProblemInterface * fepface = dynamic_cast<FEProblemInterface *>(this->problem);
    if (fepface == nullptr)
        logError("Supplied problem type '",
                 this->problem->getType(),
                 "' does not support boundary conditions.");
    else {
        for (const auto & it : bcs_root_node) {
            YAML::Node bc_node = it.first;
            std::string name = bc_node.as<std::string>();

            InputParameters & params = buildParams(bcs_root_node, name);
            const std::string & class_name = params.get<std::string>("_type");
            auto bc = Factory::create<BoundaryCondition>(class_name, name, params);
            fepface->addBoundaryCondition(bc);
        }
    }
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

        InputParameters & params = buildParams(output_root_node, name);
        const std::string & class_name = params.get<std::string>("_type");
        params.set<Problem *>("_problem") = this->problem;
        auto output = Factory::create<Output>(class_name, name, params);
        assert(this->problem != nullptr);
        this->problem->addOutput(output);
    }
}

InputParameters &
GYMLFile::buildParams(const YAML::Node & root, const std::string & name)
{
    _F_;
    YAML::Node node = root[name];
    if (!node)
        error("Missing '", name, "' block.");

    YAML::Node type = node["type"];
    if (!type)
        error(name, ": No 'type' specified.");

    const std::string & class_name = type.as<std::string>();
    if (!Factory::isRegistered(class_name))
        error(name, ": Type '", class_name, "' is not a registered object.");

    InputParameters & params = Factory::getValidParams(class_name);
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
GYMLFile::setParameterFromYML(InputParameters & params,
                              const YAML::Node & node,
                              const std::string & param_name)
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
        // vector values
        else if (param_type == type_name<std::vector<PetscReal>>())
            params.set<std::vector<PetscReal>>(param_name) =
                readVectorValue<double>(param_name, val);
        else if (param_type == type_name<std::vector<int>>())
            params.set<std::vector<int>>(param_name) = readVectorValue<int>(param_name, val);
        else if (param_type == type_name<std::vector<std::string>>())
            params.set<std::vector<std::string>>(param_name) =
                readVectorValue<std::string>(param_name, val);
    }
}

template <typename T>
std::vector<T>
GYMLFile::readVectorValue(const std::string & param_name, const YAML::Node & val_node)
{
    _F_;
    std::vector<T> vec;
    if (val_node.IsScalar())
        vec.push_back(val_node.as<T>());
    else if (val_node.IsSequence())
        vec = val_node.as<std::vector<T>>();
    else
        logError("Parameter '",
                 param_name,
                 "' must be either a single value or a vector of values.");

    return vec;
}

void
GYMLFile::checkParams(const InputParameters & params, const std::string & name)
{
    _F_;
    std::ostringstream oss;

    for (const auto & it : params) {
        const auto & param_name = it.first;
        if (!params.isParamValid(param_name) && params.isParamRequired(param_name))
            oss << std::endl << "- '" << param_name << "': " << params.getDocString(param_name);
    }

    if (!oss.str().empty())
        logError(name, ": Missing required parameters:", oss.str());
}

} // namespace godzilla
