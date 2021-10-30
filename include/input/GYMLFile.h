#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "GPrintInterface.h"

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

    const GodzillaApp & _app;
    Factory & _factory;

    YAML::Node _root;

    std::shared_ptr<GMesh> _mesh;
    std::shared_ptr<GProblem> _problem;
    std::shared_ptr<GExecutioner> _executioner;
};
