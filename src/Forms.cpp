#include "Forms.h"
#include "CallStack.h"
#include "Space.h"
#include "FELinearProblem.h"
#include "PetscMatrix.h"
#include "PetscVector.h"

namespace godzilla {

FormBase::FormBase(const FELinearProblem * fep) : fep(fep), np(0)
{
    _F_;
}

//

BilinearForm::BilinearForm(const FELinearProblem * fep, uint m, uint n) : FormBase(fep), m(m), n(n)
{
    _F_;
}

void
BilinearForm::precalculate()
{
    _F_;
}

void
BilinearForm::assemble(PetscMatrix * matrix, PetscVector * rhs)
{
    _F_;

    const AssemblyList * am = this->fep->get_assembly_list(this->m);
    const AssemblyList * an = this->fep->get_assembly_list(this->n);

    this->np = this->fep->get_num_points();
    this->jxw = this->fep->get_jxw();

    for (int i = 0; i < am->cnt; i++) {
        int k = am->dof[i];
        if (k == Space::DIRICHLET_DOF)
            continue;
        this->grad_v = this->fep->get_grad_v(i);

        for (int j = 0; j < an->cnt; j++) {
            this->grad_u = this->fep->get_grad_u(j);

            Scalar bi = integrate() * an->coef[j] * am->coef[i];
            if (an->dof[j] == Space::DIRICHLET_DOF)
                rhs->add(k, -bi);
            else
                matrix->add(k, an->dof[j], bi);
        }
    }
}

Scalar
BilinearForm::integrate()
{
    PetscScalar integral = 0.0;
    for (uint i = 0; i < this->np; i++)
        integral += this->jxw[i] * (this->grad_u[i] * this->grad_v[i]);
    return integral;
}

// Linear form

LinearForm::LinearForm(const FELinearProblem * fep, uint m) : FormBase(fep), m(m)
{
    _F_;
}

void
LinearForm::precalculate()
{
}

void
LinearForm::assemble(PetscVector * rhs)
{
    _F_;

    const AssemblyList * am = this->fep->get_assembly_list(this->m);

    this->np = this->fep->get_num_points();
    this->jxw = this->fep->get_jxw();

    for (int i = 0; i < am->cnt; i++) {
        if (am->dof[i] == Space::DIRICHLET_DOF)
            continue;
        this->v = this->fep->get_v(i);

        Scalar val = integrate();
        rhs->add(am->dof[i], val * am->coef[i]);
    }
}

Scalar
LinearForm::integrate()
{
    PetscScalar integral = 0.0;
    for (uint qp = 0; qp < this->np; qp++)
        integral += this->jxw[qp] * (-2. * this->v[qp]);
    return integral;
}

} // namespace godzilla
