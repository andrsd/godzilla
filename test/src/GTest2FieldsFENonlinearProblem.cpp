#include "GTest2FieldsFENonlinearProblem.h"
#include "Godzilla.h"

REGISTER_OBJECT(GTest2FieldsFENonlinearProblem);

GTest2FieldsFENonlinearProblem::GTest2FieldsFENonlinearProblem(const Parameters & params) :
    GTestFENonlinearProblem(params),
    iv(1)
{
}

void
GTest2FieldsFENonlinearProblem::set_up_fields()
{
    GTestFENonlinearProblem::set_up_fields();
    add_fe(this->iv, "v", 1, 1);
}
