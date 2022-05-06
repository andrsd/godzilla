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
class Function;

/// YML parser for input files
///
class GYMLFile : public PrintInterface, public LoggingInterface {
public:
    GYMLFile(const App & app);

    /// parse the YML file
    virtual void parse(const std::string & file_name);
    /// create
    virtual void create();
    /// check
    virtual void check();
    /// build the simulation
    virtual void build();

    virtual Mesh * getMesh();
    virtual Problem * getProblem();

    virtual const YAML::Node & getYml();

protected:
    void addObject(Object * obj);
    void buildFunctions();
    void buildMesh();
    void buildProblem();
    void buildPartitioner();
    void buildAuxiliaryFields();
    void buildInitialConditions();
    void buildBoundaryConditions();
    void buildPostprocessors();
    void buildOutputs();
    InputParameters * buildParams(const YAML::Node & root, const std::string & name);
    void setParameterFromYML(InputParameters * params,
                             const YAML::Node & node,
                             const std::string & param_name);

    /// Read a vector-valued parameter from a YAML file
    ///
    /// If users specify a vector-valued parameter as a single value, we read in the single value
    /// but convert it into a vector with one element.
    template <typename T>
    std::vector<T> readVectorValue(const std::string & param_name, const YAML::Node & val_node);

    void checkParams(const InputParameters * params, const std::string & name);

    /// Application object
    const godzilla::App & app;
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
