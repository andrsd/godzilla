#include "Godzilla.h"
#include "CallStack.h"
#include "HDF5Output.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "petscviewerhdf5.h"

namespace godzilla {

REGISTER_OBJECT(HDF5Output);

static const int MAX_PATH = 1024;

Parameters
HDF5Output::parameters()
{
    Parameters params = FileOutput::parameters();
    return params;
}

HDF5Output::HDF5Output(const Parameters & params) : FileOutput(params), viewer(nullptr)
{
    _F_;
}

HDF5Output::~HDF5Output()
{
    _F_;
    PETSC_CHECK(PetscViewerDestroy(&this->viewer));
}

std::string
HDF5Output::get_file_ext() const
{
    return { "h5" };
}

void
HDF5Output::create()
{
    _F_;
    FileOutput::create();

    PETSC_CHECK(PetscViewerCreate(get_comm(), &this->viewer));
    PETSC_CHECK(PetscViewerSetType(this->viewer, PETSCVIEWERHDF5));
    PETSC_CHECK(PetscViewerFileSetMode(this->viewer, FILE_MODE_WRITE));
}

void
HDF5Output::check()
{
    _F_;
    const auto * mesh = dynamic_cast<const UnstructuredMesh *>(this->problem->get_mesh());
    if (mesh == nullptr)
        log_error("HDF5 output works only with unstructured meshes.");
}

void
HDF5Output::output_step()
{
    _F_;
    set_sequence_file_name(this->problem->get_step_num());
    PETSC_CHECK(PetscViewerFileSetName(this->viewer, this->file_name.c_str()));

    lprintf(9, "Output to file: %s", this->file_name);
    DM dm = this->problem->get_dm();
    PETSC_CHECK(DMView(dm, this->viewer));
    Vec vec = this->problem->get_solution_vector();
    PETSC_CHECK(VecView(vec, this->viewer));
}

} // namespace godzilla
