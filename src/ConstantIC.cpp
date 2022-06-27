#include "Godzilla.h"
#include "ConstantIC.h"
#include "CallStack.h"

namespace godzilla {

REGISTER_OBJECT(ConstantIC);

InputParameters
ConstantIC::valid_params()
{
    InputParameters params = InitialCondition::valid_params();
    params.add_required_param<std::vector<PetscReal>>("value",
                                                      "Constant values for each field component");
    return params;
}

ConstantIC::ConstantIC(const InputParameters & params) :
    InitialCondition(params),
    values(get_param<std::vector<PetscReal>>("value"))
{
    _F_;
}

PetscInt
ConstantIC::get_num_components() const
{
    _F_;
    return this->values.size();
}

void
ConstantIC::evaluate(PetscInt dim,
                     PetscReal time,
                     const PetscReal x[],
                     PetscInt Nc,
                     PetscScalar u[])
{
    _F_;
    for (PetscInt i = 0; i < Nc; i++)
        u[i] = this->values[i];
}

} // namespace godzilla
