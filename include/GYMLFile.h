#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "InputParameters.h"
#include "PrintInterface.h"
#include "LoggingInterface.h"

namespace godzilla {

class App;
class Factory;
class Grid;
class Problem;
class Function;

/// YML parser for input files
///
class GYMLFile : public PrintInterface, public LoggingInterface {
public:
    GYMLFile(const App & app);

    /// parse the YML file
    virtual void parse(const std::string & file_name);
    /// build the simulation
    virtual void build();

    virtual Grid * getGrid();
    virtual Problem * getProblem();
    const std::vector<Function *> & getFunctions();

    virtual const YAML::Node & getYml();

protected:
    void buildFunctions();
    void buildGrid();
    void buildProblem();
    void buildInitialConditions();
    void buildBoundaryConditions();
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
    /// List of functions defined in the input file
    std::vector<Function *> functions;
};

} // namespace godzilla
