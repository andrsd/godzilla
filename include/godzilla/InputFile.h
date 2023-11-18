#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "godzilla/Parameters.h"
#include "godzilla/LoggingInterface.h"
#include "godzilla/PrintInterface.h"

namespace godzilla {

class App;
class Factory;
class Object;
class Mesh;
class Problem;
class Function;

/// Base class for input files
///
class InputFile : public PrintInterface, public LoggingInterface {
public:
    explicit InputFile(App * app);
    virtual ~InputFile() = default;

    /// Get the file name of this input file
    ///
    /// @return File name of this input file
    const std::string & get_file_name() const;

    /// Get application
    ///
    /// @return Application this input file belongs to
    App * get_app() const;

    /// Parse the YML file
    ///
    /// @return `true` if successful, otherwise `false`
    virtual bool parse(const std::string & file_name);

    /// Create objects
    virtual void create_objects();

    /// Check objects
    virtual void check();

    /// build the simulation objects
    virtual void build() = 0;

    /// Get the mesh specified in the input file
    Mesh * get_mesh() const;

    /// Get the problem specified in the input file
    Problem * get_problem() const;

protected:
    /// Representation of a block in the YAML input file
    ///
    /// Block for us has this form:
    /// ```
    /// name:
    ///   parameter_name_1: value_1
    ///   parameter_name_1: value_2
    /// ```
    class Block {
        /// "Parent" YAML node of this block. This typically contains the block name.
        YAML::Node parent_node;
        /// Block parameters as a YAML node
        YAML::Node val_nodes;

    public:
        Block() = default;
        Block(const YAML::Node & parent, const YAML::Node & values);

        /// Get parent YAML node
        const YAML::Node & parent() const;

        /// Get block values as a YAML node
        const YAML::Node & values() const;

        /// Get name of this block
        std::string name() const;

        /// Get parameter by name
        template <typename Key>
        YAML::Node
        operator[](const Key & param_name) const
        {
            return this->val_nodes[param_name];
        }

        /// Get parameter by name
        template <typename Key>
        YAML::Node
        operator[](const Key & param_name)
        {
            return this->val_nodes[param_name];
        }
    };

    void add_object(Object * obj);
    void check_unused_blocks();
    void build_mesh();
    void build_problem();
    void build_outputs();
    Block get_block(const Block & parent, const std::string & name);
    Parameters * build_params(const Block & block);
    void set_parameter_from_yml(Parameters * params,
                                const YAML::Node & node,
                                const std::string & param_name);
    virtual void set_app_defined_param(Parameters * params,
                                       const std::string & name,
                                       const std::string & type,
                                       const YAML::Node & val);
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

    void check_params(const Parameters * params,
                      const std::string & name,
                      std::set<std::string> & unused_param_names);

    /// Application object
    App * app;
    /// Name of this input file
    std::string file_name;
    /// Root node of the YML file
    Block root;
    /// Mesh object
    Mesh * mesh;
    /// Problem object
    Problem * problem;
    /// List of all objects built from the input file
    std::vector<Object *> objs;
    /// Names of object with correct parameters
    std::set<std::string> valid_param_object_names;
    /// Names of used top-level blocks
    std::set<std::string> used_top_block_names;
};

} // namespace godzilla
