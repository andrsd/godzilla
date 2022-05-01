#include "Solution1D.h"
#include "Space.h"
#include <iomanip>

namespace godzilla {

Solution1D::Solution1D(Mesh * mesh, uint num_components) :
    MeshFunction1D(mesh, num_components),
    space(nullptr),
    shfn(nullptr)
{
    _F_;
}

Solution1D::~Solution1D()
{
    _F_;
    free();
}

void
Solution1D::set_active_element(const Element1D * e)
{
    _F_;
    MeshFunction1D::set_active_element(e);
    this->space->get_element_assembly_list(e, &this->al);
    this->shfn->set_active_element(e);
}

void
Solution1D::set_fe_solution(Space * space, Vec vec)
{
    _F_;
    this->space = space;
    this->vec = vec;
    delete this->shfn;
    const Shapeset1D * ss1d = dynamic_cast<const Shapeset1D *>(this->space->get_shapeset());
    this->shfn = new ShapeFunction1D(ss1d);
}

void
Solution1D::free()
{
    _F_;
    free_cur_node();
}

void
Solution1D::precalculate(const uint np, const QPoint1D * pt, uint mask)
{
    _F_;

    mask = FN_DEFAULT;
    Node * node = new_node(mask, np);

    PetscErrorCode ierr;
    Scalar * sln_vec;
    ierr = VecGetArray(this->vec, &sln_vec);
    checkPetscError(ierr);

    for (uint k = 0; k < np; k++)
        node->values[0][FN][k] = 0;
    for (uint i = 0; i < this->al.cnt; i++) {
        this->shfn->set_active_shape(this->al.idx[i]);
        this->shfn->precalculate(np, pt, ShapeFunction1D::FN_DEFAULT);
        PetscInt dof = this->al.dof[i];
        Scalar coef = this->al.coef[i] * (dof >= 0 ? sln_vec[dof] : 1.0);
        Real * shape = this->shfn->get_fn_values();
        for (uint k = 0; k < np; k++)
            node->values[0][FN][k] += shape[k] * coef;
    }

    replace_cur_node(node);

    ierr = VecRestoreArray(this->vec, &sln_vec);
    checkPetscError(ierr);
}

uint
Solution1D::get_order()
{
    _F_;
    return 1;
}

} // namespace godzilla
