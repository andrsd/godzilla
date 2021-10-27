#include "GPetscFeProblem.h"

InputParameters validParams()
{
    InputParameters params = Problem::validParams();
    return params;
}

GPetscFeProblem::GPetscFeProblem(const InputParameters & parameters) :
    Problem(parameters)
{
}

void
GPetscFeProblem::init()
{
}

void
GPetscFeProblem::solve()
{
    std::cerr << "solve" << std::endl;
}

bool
GPetscFeProblem::converged()
{
    return false;
}
