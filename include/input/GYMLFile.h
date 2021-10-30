#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "utils/InputParameters.h"
#include "base/GPrintInterface.h"

class GodzillaApp;
class Factory;
class GMesh;
class GProblem;
class GExecutioner;

/// YML parser for input files
///
class GYMLFile : public GPrintInterface
{
public:
    GYMLFile(const GodzillaApp & app, Factory & factory);

    /// parse the YML file
    virtual void parse(const std::string & file_name);
    /// build the simulation
    virtual void build();

    virtual std::shared_ptr<GExecutioner> getExecutioner();

protected:
    void buildMesh();
    void buildProblem();
    void buildExecutioner();
    InputParameters buildParams(const YAML::Node & root, const std::string & name);
    void setParameterFromYML(InputParameters & params, const YAML::Node & node, const std::string & param_name);

    const GodzillaApp & app;
    Factory & factory;

    YAML::Node root;

    std::shared_ptr<GMesh> mesh;
    std::shared_ptr<GProblem> problem;
    std::shared_ptr<GExecutioner> executioner;
};
