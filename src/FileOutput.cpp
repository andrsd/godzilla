#include "Godzilla.h"
#include "CallStack.h"
#include "FileOutput.h"
#include "Problem.h"

namespace godzilla {

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
    std::ostringstream oss;
    internal::fprintf(oss, "%s.%s", this->file_base, this->get_file_ext());
    this->file_name = oss.str();
}

void
FileOutput::set_sequence_file_name(unsigned int stepi)
{
    _F_;
    std::ostringstream oss;
    internal::fprintf(oss, "%s.%d.%s", this->file_base, stepi, this->get_file_ext());
    this->file_name = oss.str();
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
FileOutput::output_step()
{
    _F_;
    PetscErrorCode ierr;

    set_sequence_file_name(this->problem->get_step_num());
    ierr = PetscViewerFileSetName(this->viewer, this->file_name.c_str());
    check_petsc_error(ierr);

    lprintf(9, "Output to file: %s", this->file_name);
    DM dm = this->problem->get_dm();
    output_mesh(dm);
    Vec vec = this->problem->get_solution_vector();
    output_solution(vec);
}

} // namespace godzilla
