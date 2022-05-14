#include "Godzilla.h"
#include "ExodusIIOutput.h"
#include "Problem.h"
#include "FEProblemInterface.h"
#include "petscdmplex.h"
#include "exodusII.h"

namespace godzilla {

registerObject(ExodusIIOutput);

static const int MAX_PATH = 1024;

InputParameters
ExodusIIOutput::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

ExodusIIOutput::ExodusIIOutput(const InputParameters & params) :
    FileOutput(params),
    fepi(nullptr),
    file_seq_no(0)
{
    _F_;
}

std::string
ExodusIIOutput::get_file_ext() const
{
    _F_;
    return std::string("exo");
}

void
ExodusIIOutput::create()
{
    _F_;
    PetscErrorCode ierr;

    ierr = PetscViewerCreate(get_comm(), &this->viewer);
    check_petsc_error(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWEREXODUSII);
    check_petsc_error(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    check_petsc_error(ierr);

    PetscInt order = 1;
    ierr = PetscViewerExodusIISetOrder(this->viewer, order);
    check_petsc_error(ierr);

    create_cell_sets();

    this->fepi = dynamic_cast<const FEProblemInterface *>(&this->problem);
}

void
ExodusIIOutput::create_cell_sets()
{
    _F_;
    PetscErrorCode ierr;

    PetscInt dim = this->problem.get_dimension();
    DM dm = this->problem.get_dm();
    DMLabel cs_label = nullptr;
    ierr = DMGetLabel(dm, "Cell Sets", &cs_label);
    check_petsc_error(ierr);

    if (cs_label == nullptr) {
        ierr = DMCreateLabel(dm, "Cell Sets");
        check_petsc_error(ierr);

        ierr = DMGetLabel(dm, "Cell Sets", &cs_label);
        check_petsc_error(ierr);

        // NOTE: Right now we only have a single block in the mesh, unless we load ExodusII mesh.
        // When we have support for multiple blocks, this will need to be changed
        PetscInt block_id = 1;
        ierr = DMLabelAddStratum(cs_label, block_id);
        check_petsc_error(ierr);

        // This works for simple meshes
        PetscInt elem_start, elem_end;
        ierr = DMPlexGetDepthStratum(dm, dim, &elem_start, &elem_end);
        check_petsc_error(ierr);
        PetscInt num_elems = elem_end - elem_start;
        PetscInt * elem_ids = new PetscInt[num_elems];
        for (PetscInt i = 0; i < num_elems; i++)
            elem_ids[i] = i + elem_start;

        IS is;
        ierr = ISCreateGeneral(get_comm(), num_elems, elem_ids, PETSC_COPY_VALUES, &is);
        check_petsc_error(ierr);
        ierr = DMLabelSetStratumIS(cs_label, block_id, is);
        check_petsc_error(ierr);
        ierr = ISDestroy(&is);
        check_petsc_error(ierr);

        delete[] elem_ids;
    }
}

void
ExodusIIOutput::check()
{
    _F_;
    PetscInt dim = this->problem.get_dimension();
    if (dim == 1)
        log_error("PETSc viewer does not support ExodusII output for 1D problems.");
    if (this->fepi != nullptr)
        log_error("ExodusII output does not support finite element problems yet.");
}

void
ExodusIIOutput::output_step(PetscInt stepi, DM dm, Vec vec)
{
    _F_;
    PetscErrorCode ierr;

    set_file_name();

    if (this->file_seq_no == 0)
        output_mesh(dm);

    int exoid;
    ierr = PetscViewerExodusIIGetId(this->viewer, &exoid);
    check_petsc_error(ierr);

    if (this->file_seq_no == 0)
        write_variable_info(exoid, vec);

    if (stepi == -1) {
        PetscReal time = this->problem.get_time();
        PetscInt num_step = 0;
        ierr = DMSetOutputSequenceNumber(dm, num_step, time);
        check_petsc_error(ierr);
    }

    this->file_seq_no++;
}

void
ExodusIIOutput::write_variable_info(int exoid, Vec vec)
{
    _F_;
    // PetscErrorCode ierr;
    // DM dm = this->problem.get_dm();
    //
    // PetscInt num_nodal_vars = 1;
    // const char * vec_name;
    // PetscObjectGetName((PetscObject) vec, &vec_name);
    //
    // char * nodal_var_name[1];
    // // nodal_var_name[0] = (char *) vec_name;
    // nodal_var_name[0] = (char *) "U";
    //
    // FIXME: this produces a liner error, most likely missing netcdf + exodusII libs in the linker
    // line.
    // ierr = ex_put_variable_param(exoid, EX_NODAL, num_nodal_vars);
    // check_petsc_error(ierr);
    // ierr = ex_put_variable_names(exoid, EX_NODAL, num_nodal_vars, nodal_var_name);
    // check_petsc_error(ierr);
}

} // namespace godzilla
