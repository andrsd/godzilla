// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/InputFile.h"
#include "godzilla/App.h"
#include "godzilla/Factory.h"
#include "godzilla/Mesh.h"
#include "godzilla/Problem.h"
#include "godzilla/Output.h"
#include "godzilla/CallStack.h"
#include "godzilla/Validation.h"
#include "godzilla/Utils.h"
#include <cassert>
#include "fmt/format.h"
#include "yaml-cpp/node/iterator.h"

namespace godzilla {

InputFile::Block::Block(const YAML::Node & parent, const YAML::Node & values) :
    parent_node(parent),
    val_nodes(values)
{
}

const YAML::Node &
InputFile::Block::parent() const
{
    CALL_STACK_MSG();
    return this->parent_node;
}

const YAML::Node &
InputFile::Block::values() const
{
    CALL_STACK_MSG();
    return this->val_nodes;
}

std::string
InputFile::Block::name() const
{
    CALL_STACK_MSG();
    return this->parent_node.as<std::string>();
}

//

InputFile::InputFile(App * app) :
    PrintInterface(app),
    LoggingInterface(app->get_logger()),
    app(app),
    mesh(nullptr),
    problem(nullptr)
{
    CALL_STACK_MSG();
}

const std::string &
InputFile::get_file_name() const
{
    CALL_STACK_MSG();
    return this->file_name;
}

App *
InputFile::get_app() const
{
    return this->app;
}

InputFile::Block &
InputFile::get_root()
{
    CALL_STACK_MSG();
    return this->root;
}

bool
InputFile::parse(const std::string & file_name)
{
    CALL_STACK_MSG();
    lprint(9, "Parsing input file '{}'", file_name);
    try {
        this->root = { YAML::Node(), YAML::LoadFile(file_name) };
        this->file_name = file_name;
        return true;
    }
    catch (YAML::ParserException & e) {
        log_error("Failed to parse the input file: {}", e.what() + 10);
        return false;
    }
}

Mesh *
InputFile::get_mesh() const
{
    CALL_STACK_MSG();
    return this->mesh;
}

Problem *
InputFile::get_problem() const
{
    CALL_STACK_MSG();
    return this->problem;
}

void
InputFile::create_objects()
{
    CALL_STACK_MSG();
    lprint(9, "Creating objects");
    for (auto & obj : this->objs)
        obj->create();
}

void
InputFile::check()
{
    CALL_STACK_MSG();
    for (auto & obj : this->objs)
        obj->check();
    check_unused_blocks();
}

void
InputFile::check_unused_blocks()
{
    CALL_STACK_MSG();
    for (auto it = this->root.values().begin(); it != this->root.values().end(); ++it) {
        auto blk_name = it->first.as<std::string>();
        if (this->used_top_block_names.find(blk_name) == this->used_top_block_names.end())
            log_warning("Unused block '{}' in '{}'.", blk_name, this->file_name);
    }
}

void
InputFile::add_object(Object * obj)
{
    CALL_STACK_MSG();
    if (obj != nullptr) {
        // only add objects with valid parameters
        if (this->valid_param_object_names.count(obj->get_name()) == 1)
            this->objs.push_back(obj);
    }
}

void
InputFile::build_mesh()
{
    CALL_STACK_MSG();
    lprint(9, "- mesh");
    auto node = get_block(this->root, "mesh");
    Parameters * params = build_params(node);
    const auto & class_name = params->get<std::string>("_type");
    this->mesh = this->app->build_object<Mesh>(class_name, "mesh", params);
    add_object(this->mesh);
}

void
InputFile::build_problem()
{
    CALL_STACK_MSG();
    lprint(9, "- problem");
    auto node = get_block(this->root, "problem");
    Parameters * params = build_params(node);
    const auto & class_name = params->get<std::string>("_type");
    params->set<Mesh *>("_mesh") = this->mesh;
    this->problem = this->app->build_object<Problem>(class_name, "problem", params);
    add_object(this->problem);
}

void
InputFile::build_outputs()
{
    CALL_STACK_MSG();
    if (!this->root["output"])
        return;

    lprint(9, "- outputs");
    auto output_block = get_block(this->root, "output");
    for (const auto & it : output_block.values()) {
        Block blk = get_block(output_block, it.first.as<std::string>());
        Parameters * params = build_params(blk);
        const auto & class_name = params->get<std::string>("_type");
        params->set<Problem *>("_problem") = this->problem;
        auto output = this->app->build_object<Output>(class_name, blk.name(), params);
        assert(this->problem != nullptr);
        this->problem->add_output(output);
    }
}

InputFile::Block
InputFile::get_block(const Block & parent, const std::string & name)
{
    CALL_STACK_MSG();
    for (auto it = parent.values().begin(); it != parent.values().end(); ++it) {
        if (it->first.as<std::string>() == name) {
            if (this->root.parent() == parent.parent())
                this->used_top_block_names.insert(name);
            return { it->first, it->second };
        }
    }
    error("Missing '{}' block.", name);
}

Parameters *
InputFile::build_params(const Block & block)
{
    CALL_STACK_MSG();
    std::set<std::string> unused_param_names;
    auto vals = block.values();
    if (vals.IsMap()) {
        for (auto it = vals.begin(); it != vals.end(); ++it)
            unused_param_names.insert(it->first.as<std::string>());
    }

    YAML::Node type = vals["type"];
    if (!type)
        error("{}: No 'type' specified.", block.name());
    const std::string & class_name = type.as<std::string>();
    if (!this->app->get_factory().is_registered(class_name))
        error("{}: Type '{}' is not a registered object.", block.name(), class_name);
    unused_param_names.erase("type");

    Parameters * params = this->app->get_parameters(class_name);
    params->set<std::string>("_type") = class_name;
    params->set<std::string>("_name") = block.name();
    params->set<App *>("_app") = this->app;

    for (auto & kv : *params) {
        const std::string & param_name = kv.first;
        if (!params->is_private(param_name)) {
            set_parameter_from_yml(params, block.values(), param_name);
            unused_param_names.erase(param_name);
        }
    }

    check_params(params, block.name(), unused_param_names);

    return params;
}

void
InputFile::set_parameter_from_yml(Parameters * params,
                                  const YAML::Node & node,
                                  const std::string & param_name)
{
    CALL_STACK_MSG();
    YAML::Node val = node[param_name];
    if (val) {
        const std::string & param_type = params->type(param_name);

        if (param_type == utils::type_name<std::string>())
            params->set<std::string>(param_name) = val.as<std::string>();
        else if (param_type == utils::type_name<Real>())
            params->set<Real>(param_name) = val.as<double>();
        else if (param_type == utils::type_name<Int>())
            params->set<Int>(param_name) = val.as<Int>();
        else if (param_type == utils::type_name<int>())
            params->set<int>(param_name) = val.as<int>();
        else if (param_type == utils::type_name<unsigned int>())
            params->set<unsigned int>(param_name) = val.as<unsigned int>();
        // vector values
        else if (param_type == utils::type_name<std::vector<Real>>())
            params->set<std::vector<Real>>(param_name) = read_vector_value<double>(param_name, val);
        else if (param_type == utils::type_name<std::vector<Int>>())
            params->set<std::vector<Int>>(param_name) = read_vector_value<Int>(param_name, val);
        else if (param_type == utils::type_name<std::vector<int>>())
            params->set<std::vector<int>>(param_name) = read_vector_value<int>(param_name, val);
        else if (param_type == utils::type_name<std::vector<std::string>>())
            params->set<std::vector<std::string>>(param_name) =
                read_vector_value<std::string>(param_name, val);
        // maps
        else if (param_type == utils::type_name<std::map<std::string, std::vector<std::string>>>())
            params->set<std::map<std::string, std::vector<std::string>>>(param_name) =
                read_map_value<std::string, std::vector<std::string>>(param_name, val);
        else if (param_type == utils::type_name<std::map<std::string, Real>>())
            params->set<std::map<std::string, Real>>(param_name) =
                read_map_value<std::string, Real>(param_name, val);
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
    CALL_STACK_MSG();
}

bool
InputFile::read_bool_value(const std::string & param_name, const YAML::Node & val_node)
{
    CALL_STACK_MSG();
    bool val = false;
    if (val_node.IsScalar()) {
        std::string str = utils::to_lower(val_node.as<std::string>());
        if (validation::in(str, { "on", "true", "yes" }))
            val = true;
        else if (validation::in(str, { "off", "false", "no" }))
            val = false;
        else
            log_error("Parameter '{}' must be either 'on', 'off', 'true', 'false', 'yes' or 'no'.",
                      param_name);
    }
    else
        log_error("Parameter '{}' must be either 'on', 'off', 'true', 'false', 'yes' or 'no'.",
                  param_name);

    return val;
}

template <typename T>
std::vector<T>
InputFile::read_vector_value(const std::string & param_name, const YAML::Node & val_node)
{
    CALL_STACK_MSG();
    std::vector<T> vec;
    if (val_node.IsScalar())
        vec.push_back(val_node.as<T>());
    else if (val_node.IsSequence())
        vec = val_node.as<std::vector<T>>();
    else
        log_error("Parameter '{}' must be either a single value or a vector of values.",
                  param_name);

    return vec;
}

template <typename K, typename V>
std::map<K, V>
InputFile::read_map_value(const std::string & param_name, const YAML::Node & val_node)
{
    CALL_STACK_MSG();
    std::map<K, V> map;
    if (val_node.IsMap()) {
        for (YAML::const_iterator it = val_node.begin(); it != val_node.end(); ++it) {
            K key = it->first.as<K>();
            V val = it->second.as<V>();
            map.insert(std::pair<K, V>(key, val));
        }
    }
    else
        log_error("Parameter '{}' must be a map.", param_name);

    return map;
}

void
InputFile::check_params(const Parameters * params,
                        const std::string & name,
                        std::set<std::string> & unused_param_names)
{
    CALL_STACK_MSG();
    std::string missing_pars;

    for (const auto & it : *params) {
        const auto & param_name = it.first;
        if (!params->is_param_valid(param_name) && params->is_param_required(param_name))
            missing_pars +=
                fmt::format("\n- '{}': {}", param_name, params->get_doc_string(param_name));
    }

    if (!missing_pars.empty())
        log_error("{}: Missing required parameters:{}", name, missing_pars);
    else
        this->valid_param_object_names.insert(name);

    if (!unused_param_names.empty())
        log_warning("{}: Following parameters were not used: {}",
                    name,
                    fmt::to_string(fmt::join(unused_param_names, ", ")));
}

} // namespace godzilla
