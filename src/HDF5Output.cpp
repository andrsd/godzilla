#include "Godzilla.h"
#include "CallStack.h"
#include "HDF5Output.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "petscviewerhdf5.h"

namespace godzilla {

registerObject(HDF5Output);

static const int MAX_PATH = 1024;

InputParameters
HDF5Output::valid_params()
{
    InputParameters params = FileOutput::valid_params();
    return params;
}

HDF5Output::HDF5Output(const InputParameters & params) : FileOutput(params), viewer(nullptr)
{
    _F_;
}

HDF5Output::~HDF5Output()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerDestroy(&this->viewer);
    check_petsc_error(ierr);
}

std::string
HDF5Output::get_file_ext() const
{
    return std::string("h5");
}

void
HDF5Output::create()
{
    _F_;
    FileOutput::create();

    PetscErrorCode ierr;
    ierr = PetscViewerCreate(get_comm(), &this->viewer);
    check_petsc_error(ierr);
    ierr = PetscViewerSetType(this->viewer, PETSCVIEWERHDF5);
    check_petsc_error(ierr);
    ierr = PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE);
    check_petsc_error(ierr);
}

void
HDF5Output::check()
{
    _F_;
    const UnstructuredMesh * mesh =
        dynamic_cast<const UnstructuredMesh *>(this->problem->get_mesh());
    if (mesh == nullptr)
        log_error("HDF5 output works only with unstructured meshes.");
}

void
HDF5Output::output_step()
{
    _F_;
    PetscErrorCode ierr;

    set_sequence_file_name(this->problem->get_step_num());
    ierr = PetscViewerFileSetName(this->viewer, this->file_name.c_str());
    check_petsc_error(ierr);

    lprintf(9, "Output to file: %s", this->file_name);
    DM dm = this->problem->get_dm();
    ierr = DMView(dm, this->viewer);
    check_petsc_error(ierr);
    Vec vec = this->problem->get_solution_vector();
    ierr = VecView(vec, this->viewer);
    check_petsc_error(ierr);
}

} // namespace godzilla
