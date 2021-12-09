#include "Problem.h"
#include "CallStack.h"
#include "Grid.h"
#include "Output.h"

namespace godzilla {

InputParameters
Problem::validParams()
{
    InputParameters params = Object::validParams();
    params.addPrivateParam<Grid *>("_grid");
    return params;
}

Problem::Problem(const InputParameters & parameters) :
    Object(parameters),
    PrintInterface(this),
    grid(*getParam<Grid *>("_grid"))
{
    _F_;
    PetscErrorCode ierr;

    ierr = PetscPartitionerCreate(comm(), &this->partitioner);
    checkPetscError(ierr);
    if (commSize() == 1)
        this->partitioner_info.type = PETSCPARTITIONERSIMPLE;
    else
        this->partitioner_info.type = PETSCPARTITIONERPARMETIS;
    this->partitioner_info.overlap = 0;
}

Problem::~Problem()
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscPartitionerDestroy(&this->partitioner);
    checkPetscError(ierr);
}

void
Problem::check()
{
    _F_;
    for (auto & out : this->outputs) {
        out->check();
    }
}

void
Problem::create()
{
    _F_;
    for (auto & out : this->outputs)
        out->create();
}

void
Problem::addOutput(Output * output)
{
    _F_;
    this->outputs.push_back(output);
}

void
Problem::setPartitionerType(const std::string & type)
{
    _F_;
    this->partitioner_info.type = type;
}

void
Problem::setPartitionOverlap(PetscInt overlap)
{
    _F_;
    this->partitioner_info.overlap = overlap;
}

void
Problem::setUpPartitioning()
{
    _F_;
}

} // namespace godzilla
