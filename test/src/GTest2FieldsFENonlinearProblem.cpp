#include "GTest2FieldsFENonlinearProblem.h"
#include "Godzilla.h"

registerObject(GTest2FieldsFENonlinearProblem);

GTest2FieldsFENonlinearProblem::GTest2FieldsFENonlinearProblem(const InputParameters & params) :
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
