#include "InputFile.h"
#include "App.h"
#include "Factory.h"
#include "Mesh.h"
#include "Problem.h"
#include "Output.h"
#include "CallStack.h"
#include "Validation.h"
#include "Utils.h"
#include "assert.h"
#include "fmt/format.h"
#include "yaml-cpp/node/iterator.h"

namespace godzilla {

InputFile::InputFile(const App * app) :
    PrintInterface(app),
    LoggingInterface(const_cast<App *>(app)->get_logger()),
    app(app),
    mesh(nullptr),
    problem(nullptr)
{
    _F_;
}

InputFile::~InputFile() {}

bool
InputFile::parse(const std::string & file_name)
{
    _F_;
    lprintf(9, "Parsing input file '%s'", file_name);
    try {
        this->root = YAML::LoadFile(file_name);
        return true;
    }
    catch (YAML::ParserException & e) {
        log_error("Failed to parse the input file: %s", e.what() + 10);
        return false;
    }
}

Mesh *
InputFile::get_mesh()
{
    _F_;
    return this->mesh;
}

Problem *
InputFile::get_problem()
{
    _F_;
    return this->problem;
}

void
InputFile::create()
{
    _F_;
    lprintf(9, "Creating objects");
    for (auto & obj : this->objects)
        obj->create();
}

void
InputFile::check()
{
    _F_;
    for (auto & obj : this->objects)
        obj->check();
}

void
InputFile::add_object(Object * obj)
{
    _F_;
    if (obj != nullptr) {
        // only add objects with valid parameters
        if (this->valid_param_object_names.count(obj->get_name()) == 1)
            this->objects.push_back(obj);
    }
}

void
InputFile::build_mesh()
{
    _F_;
    lprintf(9, "- mesh");
    Parameters * params = build_params(this->root, "mesh");
    const std::string & class_name = params->get<std::string>("_type");
    this->mesh = Factory::create<Mesh>(class_name, "mesh", params);
    add_object(this->mesh);
}

void
InputFile::build_problem()
{
    _F_;
    lprintf(9, "- problem");
    Parameters * params = build_params(this->root, "problem");
    const std::string & class_name = params->get<std::string>("_type");
    params->set<const Mesh *>("_mesh") = this->mesh;
    this->problem = Factory::create<Problem>(class_name, "problem", params);
    add_object(this->problem);
}

void
InputFile::build_outputs()
{
    _F_;
    YAML::Node output_root_node = this->root["output"];
    if (!output_root_node)
        return;

    lprintf(9, "- outputs");

    for (const auto & it : output_root_node) {
        YAML::Node output_node = it.first;
        std::string name = output_node.as<std::string>();

        Parameters * params = build_params(output_root_node, name);
        const std::string & class_name = params->get<std::string>("_type");
        params->set<const Problem *>("_problem") = this->problem;
        auto output = Factory::create<Output>(class_name, name, params);
        assert(this->problem != nullptr);
        this->problem->add_output(output);
    }
}

Parameters *
InputFile::build_params(const YAML::Node & root, const std::string & name)
{
    _F_;
    YAML::Node node = root[name];
    if (!node)
        error("Missing '%s' block.", name);

    std::set<std::string> unused_param_names;
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it)
            unused_param_names.insert(it->first.as<std::string>());
    }

    YAML::Node type = node["type"];
    if (!type)
        error("%s: No 'type' specified.", name);
    const std::string & class_name = type.as<std::string>();
    if (!Factory::is_registered(class_name))
        error("%s: Type '%s' is not a registered object.", name, class_name);
    unused_param_names.erase("type");

    Parameters * params = Factory::get_parameters(class_name);
    params->set<std::string>("_type") = class_name;
    params->set<std::string>("_name") = name;
    params->set<const App *>("_app") = this->app;

    for (auto & kv : *params) {
        const std::string & param_name = kv.first;
        if (!params->is_private(param_name)) {
            set_parameter_from_yml(params, node, param_name);
            unused_param_names.erase(param_name);
        }
    }

    check_params(params, name, unused_param_names);

    return params;
}

void
InputFile::set_parameter_from_yml(Parameters * params,
                                  const YAML::Node & node,
                                  const std::string & param_name)
{
    _F_;
    YAML::Node val = node[param_name];
    if (val) {
        const std::string & param_type = params->type(param_name);

        if (param_type == utils::type_name<std::string>())
            params->set<std::string>(param_name) = val.as<std::string>();
        else if (param_type == utils::type_name<PetscReal>())
            params->set<PetscReal>(param_name) = val.as<double>();
        else if (param_type == utils::type_name<PetscInt>())
            params->set<PetscInt>(param_name) = val.as<PetscInt>();
        else if (param_type == utils::type_name<int>())
            params->set<int>(param_name) = val.as<int>();
        else if (param_type == utils::type_name<unsigned int>())
            params->set<unsigned int>(param_name) = val.as<unsigned int>();
        // vector values
        else if (param_type == utils::type_name<std::vector<PetscReal>>())
            params->set<std::vector<PetscReal>>(param_name) =
                read_vector_value<double>(param_name, val);
        else if (param_type == utils::type_name<std::vector<PetscInt>>())
            params->set<std::vector<PetscInt>>(param_name) =
                read_vector_value<PetscInt>(param_name, val);
        else if (param_type == utils::type_name<std::vector<int>>())
            params->set<std::vector<int>>(param_name) = read_vector_value<int>(param_name, val);
        else if (param_type == utils::type_name<std::vector<std::string>>())
            params->set<std::vector<std::string>>(param_name) =
                read_vector_value<std::string>(param_name, val);
        // maps
        else if (param_type == utils::type_name<std::map<std::string, std::vector<std::string>>>())
            params->set<std::map<std::string, std::vector<std::string>>>(param_name) =
                read_map_value<std::string, std::vector<std::string>>(param_name, val);
        else if (param_type == utils::type_name<std::map<std::string, PetscReal>>())
            params->set<std::map<std::string, PetscReal>>(param_name) =
                read_map_value<std::string, PetscReal>(param_name, val);
        // bools
        else if (param_type == utils::type_name<bool>())
            params->set<bool>(param_name) = read_bool_value(param_name, val);
        else
            set_app_defined_param(params, param_name, param_type, val);
    }
}

void
InputFile::set_app_defined_param(Parameters * params,
                                 const std::string & name,
                                 const std::string & type,
                                 const YAML::Node & val)
{
}

bool
InputFile::read_bool_value(const std::string & param_name, const YAML::Node & val_node)
{
    _F_;
    bool val;
    if (val_node.IsScalar()) {
        std::string str = utils::to_lower(val_node.as<std::string>());
        if (validation::in(str, { "on", "true", "yes" }))
            val = true;
        else if (validation::in(str, { "off", "false", "no" }))
            val = false;
        else
            log_error("Parameter '%s' must be either 'on', 'off', 'true', 'false', 'yes' or 'no'.",
                      param_name);
    }
    else
        log_error("Parameter '%s' must be either 'on', 'off', 'true', 'false', 'yes' or 'no'.",
                  param_name);

    return val;
}

template <typename T>
std::vector<T>
InputFile::read_vector_value(const std::string & param_name, const YAML::Node & val_node)
{
    _F_;
    std::vector<T> vec;
    if (val_node.IsScalar())
        vec.push_back(val_node.as<T>());
    else if (val_node.IsSequence())
        vec = val_node.as<std::vector<T>>();
    else
        log_error("Parameter '%s' must be either a single value or a vector of values.",
                  param_name);

    return vec;
}

template <typename K, typename V>
std::map<K, V>
InputFile::read_map_value(const std::string & param_name, const YAML::Node & val_node)
{
    _F_;
    std::map<K, V> map;
    if (val_node.IsMap()) {
        for (YAML::const_iterator it = val_node.begin(); it != val_node.end(); ++it) {
            K key = it->first.as<K>();
            V val = it->second.as<V>();
            map.insert(std::pair<K, V>(key, val));
        }
    }
    else
        log_error("Parameter '%s' must be a map.", param_name);

    return map;
}

void
InputFile::check_params(const Parameters * params,
                        const std::string & name,
                        std::set<std::string> & unused_param_names)
{
    _F_;
    std::string missing_pars;

    for (const auto & it : *params) {
        const auto & param_name = it.first;
        if (!params->is_param_valid(param_name) && params->is_param_required(param_name))
            missing_pars +=
                fmt::sprintf("\n- '%s': %s", param_name, params->get_doc_string(param_name));
    }

    if (!missing_pars.empty())
        log_error("%s: Missing required parameters:%s", name, missing_pars);
    else
        this->valid_param_object_names.insert(name);

    if (unused_param_names.size() > 0)
        log_warning("%s: Following parameters were not used: %s",
                    name,
                    fmt::to_string(fmt::join(unused_param_names, ", ")));
}

} // namespace godzilla
