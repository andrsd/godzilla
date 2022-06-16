#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "InputParameters.h"
#include "LoggingInterface.h"
#include "PrintInterface.h"

namespace godzilla {

class App;
class Factory;
class Object;
class Mesh;
class Problem;
class Function;

/// YML parser for input files
///
class GYMLFile : public PrintInterface, public LoggingInterface {
public:
    GYMLFile(const App * app);

    /// Parse the YML file
    ///
    /// @return `true` if successful, otherwise `false`
    virtual bool parse(const std::string & file_name);
    /// create
    virtual void create();
    /// check
    virtual void check();
    /// build the simulation
    virtual void build();

    virtual Mesh * get_mesh();
    virtual Problem * get_problem();

protected:
    void add_object(Object * obj);
    void build_functions();
    void build_mesh();
    void build_problem();
    void build_partitioner();
    void build_auxiliary_fields();
    void build_initial_conditions();
    void build_boundary_conditions();
    void build_postprocessors();
    void build_outputs();
    InputParameters * build_params(const YAML::Node & root, const std::string & name);
    void set_parameter_from_yml(InputParameters * params,
                                const YAML::Node & node,
                                const std::string & param_name);

    /// Read a boolean parameter from a YAML file
    bool read_bool_value(const std::string & param_name, const YAML::Node & val_node);

    /// Read a vector-valued parameter from a YAML file
    ///
    /// If users specify a vector-valued parameter as a single value, we read in the single value
    /// but convert it into a vector with one element.
    template <typename T>
    std::vector<T> read_vector_value(const std::string & param_name, const YAML::Node & val_node);

    /// Read a map from a YAML file
    ///
    template <typename K, typename V>
    std::map<K, V> read_map_value(const std::string & param_name, const YAML::Node & val_node);

    void check_params(const InputParameters * params,
                      const std::string & name,
                      std::set<std::string> & unused_param_names);

    /// Application object
    const godzilla::App * app;
    /// Root node of the YML file
    YAML::Node root;
    /// Mesh object
    Mesh * mesh;
    /// Problem object
    Problem * problem;
    /// List of all objects built from the input file
    std::vector<Object *> objects;
    /// Names of object with correct parameters
    std::set<std::string> valid_param_object_names;
};

} // namespace godzilla
