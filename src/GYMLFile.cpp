#include "GYMLFile.h"
#include "App.h"
#include "Factory.h"
#include "Mesh.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "Function.h"
#include "AuxiliaryField.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "FEProblemInterface.h"
#include "Postprocessor.h"
#include "Output.h"
#include "CallStack.h"
#include "assert.h"
#include "boost/algorithm/string/join.hpp"
#include "yaml-cpp/node/iterator.h"

template <typename T>
std::string
type_name()
{
    return typeid(T).name();
}

namespace godzilla {

GYMLFile::GYMLFile(const App * app) :
    LoggingInterface(const_cast<App *>(app)->get_logger()),
    app(app),
    mesh(nullptr),
    problem(nullptr)
{
    _F_;
}

bool
GYMLFile::parse(const std::string & file_name)
{
    _F_;
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
GYMLFile::get_mesh()
{
    _F_;
    return this->mesh;
}

Problem *
GYMLFile::get_problem()
{
    _F_;
    return this->problem;
}

void
GYMLFile::create()
{
    _F_;
    for (auto & obj : this->objects)
        obj->create();
}

void
GYMLFile::check()
{
    _F_;
    for (auto & obj : this->objects)
        obj->check();
}

void
GYMLFile::add_object(Object * obj)
{
    _F_;
    if (obj != nullptr) {
        // only add objects with valid parameters
        if (this->valid_param_object_names.count(obj->get_name()) == 1)
            this->objects.push_back(obj);
    }
}

void
GYMLFile::build()
{
    _F_;
    build_mesh();
    build_problem();
    build_functions();
    build_partitioner();
    build_auxiliary_fields();
    build_initial_conditions();
    build_boundary_conditions();
    build_postprocessors();
    build_outputs();
}

void
GYMLFile::build_functions()
{
    _F_;
    YAML::Node funcs_node = this->root["functions"];
    if (!funcs_node)
        return;

    for (const auto & it : funcs_node) {
        YAML::Node fn_node = it.first;
        std::string name = fn_node.as<std::string>();

        InputParameters * params = build_params(funcs_node, name);
        const std::string & class_name = params->get<std::string>("_type");
        auto fn = Factory::create<Function>(class_name, name, params);
        assert(this->problem != nullptr);
        this->problem->add_function(fn);
    }
}

void
GYMLFile::build_mesh()
{
    _F_;
    InputParameters * params = build_params(this->root, "mesh");
    const std::string & class_name = params->get<std::string>("_type");
    this->mesh = Factory::create<Mesh>(class_name, "mesh", params);
    add_object(this->mesh);
}

void
GYMLFile::build_problem()
{
    _F_;
    InputParameters * params = build_params(this->root, "problem");
    const std::string & class_name = params->get<std::string>("_type");
    params->set<const Mesh *>("_mesh") = this->mesh;
    this->problem = Factory::create<Problem>(class_name, "problem", params);
    add_object(this->problem);
}

void
GYMLFile::build_partitioner()
{
    _F_;
    if (!this->mesh)
        return;

    UnstructuredMesh * mesh = dynamic_cast<UnstructuredMesh *>(this->mesh);
    if (!mesh)
        return;

    YAML::Node part_node = this->root["partitioner"];
    if (!part_node)
        return;

    YAML::Node name = part_node["name"];
    if (name)
        mesh->set_partitioner_type(name.as<std::string>());

    YAML::Node overlap = part_node["overlap"];
    if (overlap)
        mesh->set_partition_overlap(overlap.as<PetscInt>());
}

void
GYMLFile::build_auxiliary_fields()
{
    _F_;
    YAML::Node auxs_root_node = this->root["auxs"];
    if (!auxs_root_node)
        return;

    FEProblemInterface * fepface = dynamic_cast<FEProblemInterface *>(this->problem);
    if (fepface == nullptr)
        log_error("Supplied problem type '%s' does not support auxiliary fields.",
                  this->problem->get_type());
    else {
        for (const auto & it : auxs_root_node) {
            YAML::Node aux_node = it.first;
            std::string name = aux_node.as<std::string>();

            InputParameters * params = build_params(auxs_root_node, name);
            params->set<FEProblemInterface *>("_fepi") = fepface;
            const std::string & class_name = params->get<std::string>("_type");
            auto aux = Factory::create<AuxiliaryField>(class_name, name, params);
            fepface->add_auxiliary_field(aux);
        }
    }
}

void
GYMLFile::build_initial_conditions()
{
    _F_;
    YAML::Node ics_root_node = this->root["ics"];
    if (!ics_root_node)
        return;

    FEProblemInterface * fepface = dynamic_cast<FEProblemInterface *>(this->problem);
    if (fepface == nullptr)
        log_error("Supplied problem type '%s' does not support initial conditions.",
                  this->problem->get_type());
    else {
        for (const auto & it : ics_root_node) {
            YAML::Node ic_node = it.first;
            std::string name = ic_node.as<std::string>();

            InputParameters * params = build_params(ics_root_node, name);
            params->set<const FEProblemInterface *>("_fepi") = fepface;
            const std::string & class_name = params->get<std::string>("_type");
            auto ic = Factory::create<InitialCondition>(class_name, name, params);
            fepface->add_initial_condition(ic);
        }
    }
}

void
GYMLFile::build_boundary_conditions()
{
    _F_;
    YAML::Node bcs_root_node = this->root["bcs"];
    if (!bcs_root_node)
        return;

    FEProblemInterface * fepface = dynamic_cast<FEProblemInterface *>(this->problem);
    if (fepface == nullptr)
        log_error("Supplied problem type '%s' does not support boundary conditions.",
                  this->problem->get_type());
    else {
        for (const auto & it : bcs_root_node) {
            YAML::Node bc_node = it.first;
            std::string name = bc_node.as<std::string>();

            InputParameters * params = build_params(bcs_root_node, name);
            params->set<const FEProblemInterface *>("_fepi") = fepface;
            const std::string & class_name = params->get<std::string>("_type");
            auto bc = Factory::create<BoundaryCondition>(class_name, name, params);
            fepface->add_boundary_condition(bc);
        }
    }
}

void
GYMLFile::build_postprocessors()
{
    _F_;
    YAML::Node pps_root_node = this->root["pps"];
    if (!pps_root_node)
        return;

    for (const auto & it : pps_root_node) {
        YAML::Node pps_node = it.first;
        std::string name = pps_node.as<std::string>();

        InputParameters * params = build_params(pps_root_node, name);
        const std::string & class_name = params->get<std::string>("_type");
        params->set<const Problem *>("_problem") = this->problem;
        auto pp = Factory::create<Postprocessor>(class_name, name, params);
        problem->add_postprocessor(pp);
    }
}

void
GYMLFile::build_outputs()
{
    _F_;
    YAML::Node output_root_node = this->root["output"];
    if (!output_root_node)
        return;

    for (const auto & it : output_root_node) {
        YAML::Node output_node = it.first;
        std::string name = output_node.as<std::string>();

        InputParameters * params = build_params(output_root_node, name);
        const std::string & class_name = params->get<std::string>("_type");
        params->set<const Problem *>("_problem") = this->problem;
        auto output = Factory::create<Output>(class_name, name, params);
        assert(this->problem != nullptr);
        this->problem->add_output(output);
    }
}

InputParameters *
GYMLFile::build_params(const YAML::Node & root, const std::string & name)
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

    InputParameters * params = Factory::get_valid_params(class_name);
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
GYMLFile::set_parameter_from_yml(InputParameters * params,
                                 const YAML::Node & node,
                                 const std::string & param_name)
{
    _F_;
    YAML::Node val = node[param_name];
    if (val) {
        const std::string & param_type = params->type(param_name);

        if (param_type == type_name<std::string>())
            params->set<std::string>(param_name) = val.as<std::string>();
        else if (param_type == type_name<PetscReal>())
            params->set<PetscReal>(param_name) = val.as<double>();
        else if (param_type == type_name<int>())
            params->set<int>(param_name) = val.as<int>();
        else if (param_type == type_name<unsigned int>())
            params->set<unsigned int>(param_name) = val.as<unsigned int>();
        // vector values
        else if (param_type == type_name<std::vector<PetscReal>>())
            params->set<std::vector<PetscReal>>(param_name) =
                read_vector_value<double>(param_name, val);
        else if (param_type == type_name<std::vector<int>>())
            params->set<std::vector<int>>(param_name) = read_vector_value<int>(param_name, val);
        else if (param_type == type_name<std::vector<std::string>>())
            params->set<std::vector<std::string>>(param_name) =
                read_vector_value<std::string>(param_name, val);
        // maps
        else if (param_type == type_name<std::map<std::string, std::vector<std::string>>>())
            params->set<std::map<std::string, std::vector<std::string>>>(param_name) =
                read_map_value<std::string, std::vector<std::string>>(param_name, val);
        else if (param_type == type_name<std::map<std::string, PetscReal>>())
            params->set<std::map<std::string, PetscReal>>(param_name) =
                read_map_value<std::string, PetscReal>(param_name, val);
    }
}

template <typename T>
std::vector<T>
GYMLFile::read_vector_value(const std::string & param_name, const YAML::Node & val_node)
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
GYMLFile::read_map_value(const std::string & param_name, const YAML::Node & val_node)
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
GYMLFile::check_params(const InputParameters * params,
                       const std::string & name,
                       std::set<std::string> & unused_param_names)
{
    _F_;
    std::ostringstream oss;

    for (const auto & it : *params) {
        const auto & param_name = it.first;
        if (!params->is_param_valid(param_name) && params->is_param_required(param_name))
            oss << std::endl << "- '" << param_name << "': " << params->get_doc_string(param_name);
    }

    if (!oss.str().empty())
        log_error("%s: Missing required parameters:%s", name, oss.str());
    else
        this->valid_param_object_names.insert(name);

    if (unused_param_names.size() > 0)
        log_warning("%s: Following parameters were not used: %s",
                    name,
                    boost::algorithm::join(unused_param_names, ", "));
}

} // namespace godzilla
