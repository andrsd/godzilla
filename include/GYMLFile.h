#pragma once

#include "yaml-cpp/yaml.h"
#include "InputFile.h"

namespace godzilla {

class App;

/// Generic YML input file
///
class GYMLFile : public InputFile {
public:
    explicit GYMLFile(const App * app);

    void build() override;

protected:
    void build_functions();
    void build_problem();
    void build_partitioner();
    void build_auxiliary_fields();
    void build_initial_conditions();
    void build_boundary_conditions();
    void build_postprocessors();
    void build_ts_adapt(const YAML::Node & problem_node);
};

} // namespace godzilla
