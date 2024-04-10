#include "GTest2FieldsFENonlinearProblem.h"
#include "godzilla/Godzilla.h"

GTest2FieldsFENonlinearProblem::GTest2FieldsFENonlinearProblem(const Parameters & params) :
    GTestFENonlinearProblem(params),
    iv(1)
{
}

void
GTest2FieldsFENonlinearProblem::set_up_fields()
{
    GTestFENonlinearProblem::set_up_fields();
    set_fe(this->iv, "v", 1, 1);
}
