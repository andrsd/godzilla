#include "GYMLFile.h"
#include "App.h"
#include "Factory.h"
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
    app(app)
{
    _F_;
}

void
GYMLFile::parse(const std::string & file_name)
{
    _F_;
    this->root = YAML::LoadFile(file_name);
}

const YAML::Node &
GYMLFile::getYml()
{
    _F_;
    return this->root;
}

void
GYMLFile::build()
{
    _F_;
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
