// SPDX-FileCopyrightText: 2023 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileOutput.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Types.h"
#ifdef GODZILLA_WITH_TECIOCPP
    #include "teciocpp/teciocpp.h"
#endif

namespace godzilla {

class UnstructuredMesh;

/// Tecplot output
///
/// This saves the solution into an Tecplot (.szplt) file.
/// This output works only with finite element problems
class TecplotOutput : public FileOutput, public DiscreteProblemOutputInterface {
public:
    explicit TecplotOutput(const Parameters & pars);
    ~TecplotOutput() override;

    void create() override;
    void output_step() override;

protected:
    void open_file();
    void write_zone();
    void write_created_by(int32_t zone);
    void write_coordinates(int32_t zone);
    void write_connectivity(int32_t zone);
    void write_field_variable_values(int32_t zone);
    void write_nodal_field_variable_values(int32_t zone);

private:
    String get_file_ext() const override;

#ifdef GODZILLA_WITH_TECIOCPP
    /// Unstructured mesh
    Ref<UnstructuredMesh> mesh;
    /// File to write into
    teciocpp::File * file;
    /// Variable value locations
    std::vector<teciocpp::ValueLocation> value_locations;
    /// Shared variables
    std::vector<bool> shared_vars;
    /// Number of zones
    Int n_zones;
#endif
    /// Variable names to be stored
    const std::vector<String> variable_names;
    /// List of field variable names to output
    std::vector<String> field_var_names;
    /// List of nodal variable field IDs
    std::vector<FieldID> nodal_var_fids;
    /// List of variable indices
    std::vector<int32_t> nodal_var_idxs;
    /// List of auxiliary field variable names to output
    std::vector<String> aux_field_var_names;
    /// List of nodal auxiliary variable field IDs
    std::vector<FieldID> nodal_aux_var_fids;
    /// List of auxiliary variable indices
    std::vector<int32_t> nodal_aux_var_idxs;

public:
    static Parameters parameters();
};

} // namespace godzilla
