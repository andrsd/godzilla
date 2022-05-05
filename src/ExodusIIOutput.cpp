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
ExodusIIOutput::validParams()
{
    InputParameters params = FileOutput::validParams();
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
ExodusIIOutput::getFileExt() const
{
    _F_;
    return std::string("exo");
}

void
ExodusIIOutput::create()
{
    _F_;
    PetscErrorCode ierr;

    ierr = PetscViewerCreate(comm(), &this->viewer);
    checkPetscError(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWEREXODUSII);
    checkPetscError(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    checkPetscError(ierr);

    PetscInt order = 1;
    ierr = PetscViewerExodusIISetOrder(this->viewer, order);
    checkPetscError(ierr);

    createCellSets();

    this->fepi = dynamic_cast<const FEProblemInterface *>(&this->problem);
}

void
ExodusIIOutput::createCellSets()
{
    _F_;
    PetscErrorCode ierr;

    DM dm = this->problem.getDM();

    PetscInt dim;
    DMGetDimension(dm, &dim);

    DMLabel cs_label = nullptr;
    ierr = DMGetLabel(dm, "Cell Sets", &cs_label);
    checkPetscError(ierr);

    if (cs_label == nullptr) {
        ierr = DMCreateLabel(dm, "Cell Sets");
        checkPetscError(ierr);

        ierr = DMGetLabel(dm, "Cell Sets", &cs_label);
        checkPetscError(ierr);

        // NOTE: Right now we only have a single block in the mesh, unless we load ExodusII mesh.
        // When we have support for multiple blocks, this will need to be changed
        PetscInt block_id = 1;
        ierr = DMLabelAddStratum(cs_label, block_id);
        checkPetscError(ierr);

        // This works for simple meshes
        PetscInt elem_start, elem_end;
        ierr = DMPlexGetDepthStratum(dm, dim, &elem_start, &elem_end);
        PetscInt num_elems = elem_end - elem_start;
        PetscInt * elem_ids = new PetscInt[num_elems];
        for (PetscInt i = 0; i < num_elems; i++)
            elem_ids[i] = i + elem_start;

        IS is;
        ierr = ISCreateGeneral(comm(), num_elems, elem_ids, PETSC_COPY_VALUES, &is);
        checkPetscError(ierr);
        ierr = DMLabelSetStratumIS(cs_label, block_id, is);
        checkPetscError(ierr);
        ierr = ISDestroy(&is);
        checkPetscError(ierr);

        delete[] elem_ids;
    }
}

void
ExodusIIOutput::check()
{
    _F_;
    DM dm = this->problem.getDM();
    PetscInt dim;
    DMGetDimension(dm, &dim);
    if (dim == 1)
        logError("PETSc viewer does not support ExodusII output for 1D problems.");
    if (this->fepi != nullptr)
        logError("ExodusII output does not support finite element problems yet.");
}

void
ExodusIIOutput::outputStep(PetscInt stepi, DM dm, Vec vec)
{
    _F_;
    PetscErrorCode ierr;

    setFileName();

    if (this->file_seq_no == 0)
        outputMesh(dm);

    int exoid;
    ierr = PetscViewerExodusIIGetId(this->viewer, &exoid);
    checkPetscError(ierr);

    if (this->file_seq_no == 0)
        writeVariableInfo(exoid, vec);

    if (stepi == -1) {
        PetscReal time = this->problem.getTime();
        PetscInt num_step = 0;
        ierr = DMSetOutputSequenceNumber(dm, num_step, time);
        checkPetscError(ierr);
    }

    this->file_seq_no++;
}

void
ExodusIIOutput::writeVariableInfo(int exoid, Vec vec)
{
    _F_;
    // PetscErrorCode ierr;
    // DM dm = this->problem.getDM();
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
    // checkPetscError(ierr);
    // ierr = ex_put_variable_names(exoid, EX_NODAL, num_nodal_vars, nodal_var_name);
    // checkPetscError(ierr);
}

} // namespace godzilla
