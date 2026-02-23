#include "GTestDGLinearProblem.h"

using namespace godzilla;

GTestDGLinearProblem::GTestDGLinearProblem(const Parameters & pars) : ExplicitDGLinearProblem(pars)
{
}

void
GTestDGLinearProblem::set_up_fields()
{
    this->iu = add_field("u", 1, Order(1));
}

void
GTestDGLinearProblem::set_up_weak_form()
{
    // Empty implementation for testing
}

void
GTestDGLinearProblem::set_up_time_scheme()
{
}
