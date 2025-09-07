#include "GTest2FieldsFENonlinearProblem.h"

GTest2FieldsFENonlinearProblem::GTest2FieldsFENonlinearProblem(const Parameters & params) :
    GTestFENonlinearProblem(params),
    iv(1)
{
}

void
GTest2FieldsFENonlinearProblem::set_up_fields()
{
    GTestFENonlinearProblem::set_up_fields();
    set_field(this->iv, "v", 1, Order(1));
}
