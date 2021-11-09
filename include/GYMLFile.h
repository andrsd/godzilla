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
class Executioner;

/// YML parser for input files
///
class GYMLFile : public PrintInterface
{
public:
    GYMLFile(const App & app);

    /// parse the YML file
    virtual void parse(const std::string & file_name);
    /// build the simulation
    virtual void build();

    virtual Grid * getGrid();
    virtual Problem * getProblem();
    virtual Executioner * getExecutioner();

    virtual const YAML::Node & getYml();

protected:
    void buildGrid();
    void buildProblem();
    void buildExecutioner();
    void buildInitialConditons();
    void buildBoundaryConditons();
    void buildOutputs();
    InputParameters buildParams(const YAML::Node & root, const std::string & name);
    void setParameterFromYML(InputParameters & params, const YAML::Node & node, const std::string & param_name);
    void checkParams(const InputParameters & params, const std::string & name);

    const godzilla::App & app;

    YAML::Node root;

    Grid * grid;
    Problem * problem;
    Executioner * executioner;
};

}
