#include "FENonlinearProblem.h"
#include "CallStack.h"
#include "Grid.h"
#include "FunctionInterface.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "petscdm.h"
#include "petscdmplex.h"
#include "petscdmlabel.h"


namespace godzilla {

namespace internal {

static PetscErrorCode
zero_fn(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar *u, void *ctx)
{
    u[0] = 0.0;
    return 0;
}

} //internal


InputParameters
FENonlinearProblem::validParams()
{
    InputParameters params = NonlinearProblem::validParams();
    return params;
}

FENonlinearProblem::FENonlinearProblem(const InputParameters & parameters) :
    NonlinearProblem(parameters),
    dim(-1),
    n_fields(0),
    ds(nullptr)
{
    _F_;
}

FENonlinearProblem::~FENonlinearProblem()
{
    _F_;
    for (auto & kv : this->fields) {
        FieldInfo & fi = kv.second;
        PetscFEDestroy(&fi.fe);
    }
}

void
FENonlinearProblem::create()
{
    _F_;
    DMGetDimension(this->grid.getDM(), &this->dim);
    onSetFields();

    NonlinearProblem::create();
}

const std::string &
FENonlinearProblem::getFieldName(PetscInt fid)
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.name;
    else
        godzillaError("Field with id = '", fid, "' does not exist.");
}

PetscInt
FENonlinearProblem::addField(const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    FieldInfo fi = { name, this->n_fields, nullptr, nullptr, nc, k };

    this->fields[fi.id] = fi;
    this->n_fields++;

    return fi.id;
}

void
FENonlinearProblem::addInitialCondition(const InitialCondition *ic)
{
    _F_;
    PetscInt fid = ic->getFieldId();
    const auto & it = this->ics.find(fid);
    if (it == this->ics.end())
        this->ics[fid].ic = ic;
    else
        // TODO: improve this error message
        godzillaError("Initial condition '", ic->getName(), "' is being applied to a field that already has an initial condition.");
}

void
FENonlinearProblem::addBoundaryCondition(const BoundaryCondition *bc)
{
    _F_;
    const std::vector<std::string> & bnd_names = bc->getBoundary();
    for (auto & bname : bnd_names) {
        BCInfo bc_info;
        bc_info.bnd_name = bname;
        bc_info.bc = bc;

        this->bcs.push_back(bc_info);
    }
}

void
FENonlinearProblem::init()
{
    _F_;
    NonlinearProblem::init();

    setupFields();

    const DM & dm = this->getDM();
    PetscErrorCode ierr;
    ierr = DMCreateDS(dm);
    checkPetscError(ierr);
    ierr = DMGetDS(dm, &this->ds);
    checkPetscError(ierr);

    onSetWeakForm();
    setupBoundaryConditions();
}

void
FENonlinearProblem::setupBoundaryConditions()
{
    _F_;
    const DM & dm = getDM();
    PetscErrorCode ierr;

    for (auto & bcinfo : this->bcs) {
        const BoundaryCondition * bc = bcinfo.bc;
        const std::string & bnd_name = bcinfo.bnd_name;

        PetscBool exists = PETSC_FALSE;
        ierr = DMHasLabel(dm, bnd_name.c_str(), &exists);
        checkPetscError(ierr);
        if (exists)
        {
            PetscInt nc = bc->getNumComponents();

            DMLabel label;
            ierr = DMGetLabel(dm, bnd_name.c_str(), &label);
            checkPetscError(ierr);

            IS is;
            ierr = DMGetLabelIdIS(dm, bnd_name.c_str(), &is);
            checkPetscError(ierr);

            PetscInt n_ids;
            ierr = ISGetSize(is, &n_ids);
            checkPetscError(ierr);

            const PetscInt *ids = nullptr;
            ierr = ISGetIndices(is, &ids);
            checkPetscError(ierr);

            ierr = PetscDSAddBoundary(this->ds,
                DM_BC_ESSENTIAL,
                bc->getName().c_str(),
                label,
                n_ids, ids,
                bc->getFieldId(),
                0, NULL,
                (void (*)(void)) &__boundary_condition_function, NULL,
                (void *) bc,
                NULL);

            ierr = ISRestoreIndices(is, &ids);
            checkPetscError(ierr);

        }
        else
            godzillaError("Boundary condition '", bc->getName(), "' is set on boundary '", bnd_name, "' which does not exist in the mesh.");
    }
}

void
FENonlinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = this->getDM();
    ierr = DMPlexSetSNESLocalFEM(dm, this, this, this);
    checkPetscError(ierr);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, NULL, NULL);
    checkPetscError(ierr);
}

void
FENonlinearProblem::setupInitialGuess()
{
    _F_;
    PetscInt n_ics = this->ics.size();
    if (n_ics > 0) {
        if (n_ics != fields.size())
            godzillaError("Provided ", fields.size(), " field(s), but ", n_ics, " initial condition(s).");
        else {
            PetscErrorCode ierr;
            PetscFunc *ic_funcs[n_ics];
            void * ic_ctxs[n_ics];
            for (auto & it : this->ics) {
                PetscInt fid = it.first;
                const ICInfo & ic_info = it.second;
                const InitialCondition * ic = ic_info.ic;

                PetscInt ic_nc = ic->getNumComponents();
                PetscInt field_nc = this->fields[fid].nc;
                if (ic_nc != field_nc)
                    godzillaError("Initial condition '", ic->getName(), "' operates on ", ic_nc, " components, but is set on a field with ", field_nc, " components.");

                ic_funcs[fid] = __initial_condition_function;
                ic_ctxs[fid] = (void *) ic;
            }
            ierr = DMProjectFunction(getDM(), 0.0, ic_funcs, ic_ctxs, INSERT_VALUES, this->x);
            checkPetscError(ierr);
        }
    }
    else {
        // no boundary conditions -> use zero
        PetscErrorCode ierr;
        PetscFunc *initial_guess[1] = { internal::zero_fn };
        ierr = DMProjectFunction(getDM(), 0.0, initial_guess, NULL, INSERT_VALUES, this->x);
        checkPetscError(ierr);
    }

}

PetscErrorCode
FENonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    _F_;
    return 0;
}

PetscErrorCode
FENonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    _F_;
    return 0;
}

void
FENonlinearProblem::setupFields()
{
    _F_;
    PetscErrorCode ierr;
    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;

        // FIXME: determine if the mesh is made of simplex elements
        PetscBool is_simplex = PETSC_FALSE;
        PetscInt qorder = PETSC_DETERMINE;
        ierr = PetscFECreateLagrange(comm(),
            this->dim, fi.nc, is_simplex, fi.k, qorder,
            &fi.fe);
        checkPetscError(ierr);

        ierr = DMSetField(getDM(), fi.id, fi.block, (PetscObject) fi.fe);
        checkPetscError(ierr);
        ierr = PetscFESetName(fi.fe, fi.name.c_str());
        checkPetscError(ierr);
    }
}

void
FENonlinearProblem::setResidualBlock(PetscInt field_id, PetscFEResidualFunc *f0, PetscFEResidualFunc *f1)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetResidual(this->ds, field_id, f0, f1);
    checkPetscError(ierr);
}

void
FENonlinearProblem::setJacobianBlock(PetscInt fid, PetscInt gid, PetscFEJacobianFunc *g0, PetscFEJacobianFunc *g1, PetscFEJacobianFunc *g2, PetscFEJacobianFunc *g3)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetJacobian(this->ds, fid, gid, g0, g1, g2, g3);
    checkPetscError(ierr);
}

}
