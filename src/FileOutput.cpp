#include "Godzilla.h"
#include "FileOutput.h"
#include "Problem.h"

namespace godzilla {

static const int MAX_PATH = 1024;

InputParameters
FileOutput::valid_params()
{
    InputParameters params = Output::valid_params();
    params.add_required_param<std::string>("file", "The name of the output file.");
    return params;
}

FileOutput::FileOutput(const InputParameters & params) :
    Output(params),
    file_base(get_param<std::string>("file")),
    viewer(nullptr)
{
    _F_;
}

FileOutput::~FileOutput()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscViewerDestroy(&this->viewer);
    check_petsc_error(ierr);
}

const std::string &
FileOutput::get_file_name() const
{
    _F_;
    return this->file_name;
}

void
FileOutput::set_file_name()
{
    _F_;
    char fn[MAX_PATH];
    snprintf(fn, MAX_PATH, "%s.%s", this->file_base.c_str(), this->get_file_ext().c_str());
    this->file_name = std::string(fn);
}

void
FileOutput::set_sequence_file_name(unsigned int stepi)
{
    _F_;
    char fn[MAX_PATH];
    snprintf(fn,
             MAX_PATH,
             "%s.%d.%s",
             this->file_base.c_str(),
             stepi,
             this->get_file_ext().c_str());
    this->file_name = std::string(fn);
}

void
FileOutput::output_mesh(DM dm)
{
    _F_;
    PetscErrorCode ierr;
    ierr = DMView(dm, this->viewer);
    check_petsc_error(ierr);
}

void
FileOutput::output_solution(Vec vec)
{
    _F_;
    PetscErrorCode ierr;
    ierr = VecView(vec, this->viewer);
    check_petsc_error(ierr);
}

void
FileOutput::output_step(PetscInt stepi)
{
    _F_;
    PetscErrorCode ierr;

    if (stepi == -1)
        set_file_name();
    else
        set_sequence_file_name(stepi);
    ierr = PetscViewerFileSetName(this->viewer, this->file_name.c_str());
    check_petsc_error(ierr);

    godzilla_print(9, "Output to file: ", this->file_name);
    DM dm = this->problem->get_dm();
    output_mesh(dm);
    Vec vec = this->problem->get_solution_vector();
    output_solution(vec);
}

} // namespace godzilla
