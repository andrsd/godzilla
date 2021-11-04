#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "utils/InputParameters.h"
#include "base/GPrintInterface.h"

namespace godzilla {

class App;
class Factory;
class GGrid;
class GProblem;
class GExecutioner;

/// YML parser for input files
///
class GYMLFile : public GPrintInterface
{
public:
    GYMLFile(const App & app);

    /// parse the YML file
    virtual void parse(const std::string & file_name);
    /// build the simulation
    virtual void build();

    virtual GGrid * getGrid();
    virtual GProblem * getProblem();
    virtual GExecutioner * getExecutioner();

    virtual const YAML::Node & getYml();

protected:
    void buildGrid();
    void buildProblem();
    void buildExecutioner();
    InputParameters buildParams(const YAML::Node & root, const std::string & name);
    void setParameterFromYML(InputParameters & params, const YAML::Node & node, const std::string & param_name);
    void checkParams(const InputParameters & params, const std::string & name);

    const godzilla::App & app;

    YAML::Node root;

    GGrid * grid;
    GProblem * problem;
    GExecutioner * executioner;
};

}
