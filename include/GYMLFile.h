#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "InputParameters.h"
#include "PrintInterface.h"

namespace godzilla {

class App;
class Factory;
class Grid;
class Problem;

/// YML parser for input files
///
class GYMLFile : public PrintInterface {
public:
    GYMLFile(const App & app);

    /// parse the YML file
    virtual void parse(const std::string & file_name);
    /// build the simulation
    virtual void build();

    virtual Grid * getGrid();
    virtual Problem * getProblem();

    virtual const YAML::Node & getYml();

protected:
    void buildGrid();
    void buildProblem();
    void buildInitialConditons();
    void buildBoundaryConditons();
    void buildOutputs();
    InputParameters & buildParams(const YAML::Node & root, const std::string & name);
    void setParameterFromYML(InputParameters & params,
                             const YAML::Node & node,
                             const std::string & param_name);

    /// Read a vector-valued parameter from a YAML file
    ///
    /// If users specify a vector-valued parameter as a single value, we read in the single value
    /// but convert it into a vector with one element.
    template <typename T>
    std::vector<T> readVectorValue(const std::string & param_name, const YAML::Node & val_node);

    void checkParams(const InputParameters & params, const std::string & name);

    const godzilla::App & app;

    YAML::Node root;

    Grid * grid;
    Problem * problem;
};

} // namespace godzilla
