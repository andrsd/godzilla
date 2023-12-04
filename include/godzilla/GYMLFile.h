// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/InputFile.h"

namespace godzilla {

class App;

/// Generic YML input file
///
class GYMLFile : public InputFile {
public:
    explicit GYMLFile(App * app);

    void build() override;

protected:
    void build_functions();
    void build_problem_adapt();
    void build_partitioner();
    void build_auxiliary_fields();
    void build_initial_conditions();
    void build_boundary_conditions();
    void build_postprocessors();
    void build_ts_adapt(const Block & problem_node);
};

} // namespace godzilla
