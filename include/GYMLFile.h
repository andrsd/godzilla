#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "InputParameters.h"
#include "PrintInterface.h"
#include "LoggingInterface.h"

namespace godzilla {

class App;
class Factory;
class Mesh;
class Problem;

/// YML parser for input files
///
class GYMLFile : public PrintInterface, public LoggingInterface {
public:
    GYMLFile(const App & app);

    /// Retrieve the problem
    virtual Problem * get_problem() const;
    /// Get the YML root node
    const YAML::Node & get_yml() const;
    /// Parse the YML file
    virtual void parse(const std::string & file_name);
    /// Instantiate objects found in the YML file
    /// TODO: needs a better name
    virtual void build();
    /// Call `create` on objects that built up the simulation
    virtual void create();
    /// Call `check` on objects that built up the simulation
    virtual void check();

protected:
    InputParameters & build_params(const YAML::Node & root, const std::string & name);
    void set_parameter_from_yml(InputParameters & params,
                                const YAML::Node & node,
                                const std::string & param_name);

    /// Read a vector-valued parameter from a YAML file
    ///
    /// If users specify a vector-valued parameter as a single value, we read in the single value
    /// but convert it into a vector with one element.
    template <typename T>
    std::vector<T> read_vector_value(const std::string & param_name, const YAML::Node & val_node);

    void check_params(const InputParameters & params, const std::string & name);

    /// Build mesh-derived object
    void build_mesh();

    /// Build Problem-derived object
    void build_problem();

    /// All built objects has to be added by calling this method
    void add_object(Object * obj);

    /// Application this file belongs to
    const godzilla::App & app;
    /// Root node of the YML file
    YAML::Node root;
    /// Mesh
    Mesh * mesh;
    /// Problem
    Problem * problem;
    /// List of all objects built from the input file
    std::vector<Object *> objects;
    /// Names of object with correct parameters
    std::set<std::string> valid_param_object_names;
};

} // namespace godzilla
