#pragma once

#include "FileOutput.h"

namespace godzilla {

class FEProblemInterface;
class UnstructuredMesh;

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
    ExodusIIOutput(const InputParameters & params);
    virtual ~ExodusIIOutput();

    virtual std::string get_file_ext() const override;
    virtual void check() override;
    virtual void output_step() override;

protected:
    void open_file();
    void write_info();
    void write_mesh();
    void write_coords(int n_dim);
    const char * get_elem_type(DMPolytopeType elem_type) const;
    int get_num_elem_nodes(DMPolytopeType elem_type) const;
    const PetscInt * get_elem_node_ordering(DMPolytopeType elem_type) const;
    void write_elements();
    void write_node_sets();
    void write_face_sets();
    void write_all_variable_names();
    void write_variables();
    void write_field_variables();
    void write_nodal_variables(const PetscScalar * sln);

    /// FE problem interface (convenience pointer)
    const FEProblemInterface * fepi;
    /// Unstructured mesh
    const UnstructuredMesh * mesh;
    /// ExodusII file handle
    int exoid;
    /// Step number
    int step_num;
    /// Flag indicating if we need to store mesh during `output_step`
    bool mesh_stored;
    /// List of nodal variable field IDs
    std::vector<PetscInt> nodal_var_fids;
    /// List of nodal elemental variable field IDs
    std::vector<PetscInt> elem_var_fids;

public:
    static InputParameters valid_params();
};

} // namespace godzilla
