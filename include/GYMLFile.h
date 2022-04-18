#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "InputParameters.h"
#include "PrintInterface.h"
#include "LoggingInterface.h"

namespace godzilla {

class App;
class Factory;

/// YML parser for input files
///
class GYMLFile : public PrintInterface, public LoggingInterface {
public:
    GYMLFile(const App & app);

    /// parse the YML file
    virtual void parse(const std::string & file_name);
    /// build the simulation
    virtual void build();

    virtual const YAML::Node & getYml();

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

    void check_arams(const InputParameters & params, const std::string & name);

    const godzilla::App & app;

    YAML::Node root;
};

} // namespace godzilla
