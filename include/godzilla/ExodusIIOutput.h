// SPDX-FileCopyrightText: 2021 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "godzilla/FileOutput.h"
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
class ExodusIIOutput : public FileOutput {
public:
    explicit ExodusIIOutput(const Parameters & pars);
    ~ExodusIIOutput() override;

    void create() override;
    void output_mesh();
    void output_step() override;

protected:
    void open_file();
    void write_info();
    void write_mesh();
    void write_mesh_continuous();
    void write_mesh_discontinuous();
    void write_coords_continuous(int n_dim);
    void write_coords_discontinuous(int n_dim);
    void write_elements();
    void write_node_sets();
    void write_face_sets();
    void write_all_variable_names();
    void write_variables();
    void write_field_variables();
    void write_nodal_variables_continuous();
    void write_nodal_variables_discontinuous();
    void write_elem_variables();
    void
    write_block_elem_variables(int blk_id, Int n_elems_in_block = 0, const Int * cells = nullptr);
    void write_global_variables();
    void write_block_connectivity_continuous(int blk_id,
                                             PolytopeType polytope_type,
                                             Int n_elems_in_block,
                                             const Int * cells);
    void write_block_connectivity_discontinuous(int blk_id,
                                                PolytopeType polytope_type,
                                                Int n_elems_in_block,
                                                const Int * /*cells*/);
    int get_num_nodes_per_element();

private:
    std::string get_file_ext() const override;

    /// Unstructured mesh
    UnstructuredMesh * mesh;
    bool cont;
    bool discont;
    /// Variable names to be stored
    std::vector<std::string> variable_names;
    /// DG problem interface
    DGProblemInterface * dgpi;
    /// ExodusII file
    Qtr<exodusIIcpp::File> exo;
    /// Step number
    int step_num;
    /// Flag indicating if we need to store mesh during `output_step`
    bool mesh_stored;
    /// List of field variable names to output
    std::vector<std::string> field_var_names;
    /// List of auxiliary field variable names to output
    std::vector<std::string> aux_field_var_names;
    /// List of global variable names to output
    std::vector<std::string> global_var_names;
    /// List of nodal variable field IDs
    std::vector<FieldID> nodal_var_fids;
    /// List of nodal auxiliary variable field IDs
    std::vector<FieldID> nodal_aux_var_fids;
    /// List of elemental variable field IDs
    std::vector<FieldID> elem_var_fids;
    /// List of elemental auxiliary variable field IDs
    std::vector<FieldID> elem_aux_var_fids;

    /// Block ID used in ExodusII file when there are not cell sets
    static const int SINGLE_BLK_ID;

public:
    static Parameters parameters();

    static const char * get_elem_type(PolytopeType elem_type);
    static const Int * get_elem_node_ordering(PolytopeType elem_type);
    static const Int * get_elem_side_ordering(PolytopeType elem_type);
};

} // namespace godzilla
