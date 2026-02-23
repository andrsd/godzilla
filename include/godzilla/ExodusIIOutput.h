// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileOutput.h"
#include "godzilla/DiscreteProblemInterface.h"
#include "godzilla/Types.h"
#include "godzilla/Qtr.h"
#include "exodusIIcpp/exodusIIcpp.h"

namespace godzilla {

class UnstructuredMesh;
class DGProblemInterface;

/// ExodusII output
///
/// This saves the solution into an ExodusII file.
///
/// Input file example:
/// ```
/// output:
///   exo:
///     type: ExodusIIOutput
///     file: 'out'
/// ```
///
/// This output works only with finite element problems
class ExodusIIOutput : public FileOutput, public DiscreteProblemOutputInterface {
public:
    explicit ExodusIIOutput(const Parameters & pars);
    ~ExodusIIOutput() override;

    void create() override;
    void output_mesh();
    void output_step() override;

protected:
    void open_file();
    void write_info();
    void write_all_variable_names();
    void write_elem_variables();
    void write_global_variables();

private:
    String get_file_ext() const override;
    void output_step(const DiscreteProblemInterface & dpi);
    void output_step(const DGProblemInterface & dpi);

    /// Unstructured mesh
    Ref<UnstructuredMesh> mesh;
    /// Variable names to be stored
    std::vector<String> variable_names;
    /// ExodusII file
    Qtr<exodusIIcpp::File> exo;
    /// Step number
    int step_num;
    /// Flag indicating if we need to store mesh during `output_step`
    bool mesh_stored;
    /// List of field variable names to output
    std::vector<String> field_var_names;
    /// List of auxiliary field variable names to output
    std::vector<String> aux_field_var_names;
    /// List of global variable names to output
    std::vector<std::string> global_var_names;
    /// List of nodal variable field IDs
    std::vector<std::pair<FieldID, int>> nodal_var_fids;
    /// List of nodal auxiliary variable field IDs
    std::vector<std::pair<FieldID, int>> nodal_aux_var_fids;
    /// List of elemental variable field IDs
    std::vector<std::pair<FieldID, int>> elem_var_fids;
    /// List of elemental auxiliary variable field IDs
    std::vector<std::pair<FieldID, int>> elem_aux_var_fids;

public:
    static Parameters parameters();
};

} // namespace godzilla
