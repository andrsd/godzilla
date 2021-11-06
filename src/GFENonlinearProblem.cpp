#include "GFENonlinearProblem.h"
#include "CallStack.h"
#include "GGrid.h"
#include "petscdm.h"
#include "petscdmplex.h"
#include "petscdmlabel.h"
#include "petscviewerhdf5.h"


namespace godzilla {

InputParameters
GFENonlinearProblem::validParams()
{
    InputParameters params = GNonlinearProblem::validParams();
    return params;
}

GFENonlinearProblem::GFENonlinearProblem(const InputParameters & parameters) :
    GNonlinearProblem(parameters)
{
    _F_;
}

GFENonlinearProblem::~GFENonlinearProblem()
{
    _F_;
    for (auto & kv : this->fields) {
        FieldInfo & fi = kv.second;
        PetscFEDestroy(&fi.fe);
    }
}

void
GFENonlinearProblem::create()
{
    _F_;
    DMGetDimension(this->grid.getDM(), &this->dim);
    GNonlinearProblem::create();
}

void
GFENonlinearProblem::init()
{
    _F_;
    GNonlinearProblem::init();

    onSetFields();

    const DM & dm = this->getDM();
    PetscErrorCode ierr;
    ierr = DMCreateDS(dm);
    ierr = DMGetDS(dm, &this->ds);

    onSetWeakForm();
    setupBoundaryConditions();
}

void
GFENonlinearProblem::setupBoundaryConditions()
{
    _F_;
}

void
GFENonlinearProblem::setupCallbacks()
{
    _F_;
    PetscErrorCode ierr;
    const DM & dm = this->getDM();
    ierr = DMPlexSetSNESLocalFEM(dm, this, this, this);
    ierr = SNESSetJacobian(this->snes, this->J, this->Jp, NULL, NULL);
}

void
GFENonlinearProblem::setupInitialGuess()
{
    _F_;
    PetscErrorCode ierr;

    onSetInitialConditions();
    // project field ICs if we have them
    if (field_ics.size() > 0)
    {
        if (field_ics.size() != fields.size())
            godzillaError("Provided ", fields.size(), " fields, but ", field_ics.size(), " initial conditions.");
        else {
            std::size_t n = field_ics.size();
            PetscFieldFunc *ic_funcs[n];
            for (unsigned int i = 0; i < fields.size(); i++)
                ic_funcs[i] = field_ics[i];
            ierr = DMProjectField(getDM(), 0.0, this->x, ic_funcs, INSERT_VALUES, this->x);
        }
    }
}

PetscErrorCode
GFENonlinearProblem::computeResidualCallback(Vec x, Vec f)
{
    return 0;
}

PetscErrorCode
GFENonlinearProblem::computeJacobianCallback(Vec x, Mat J, Mat Jp)
{
    return 0;
}

PetscInt
GFENonlinearProblem::addField(const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    PetscErrorCode ierr;

    FieldInfo fi;
    fi.name = name;
    fi.id = this->field_id;
    // entire mesh
    fi.block = NULL;
    fi.nc = nc;
    fi.k = k;

    // FIXME: determine if the mesh is made of simplex elements
    PetscBool is_simplex = PETSC_FALSE;
    PetscInt qorder = PETSC_DETERMINE;
    ierr = PetscFECreateLagrange(comm(),
        this->dim, fi.nc, is_simplex, fi.k, qorder,
        &fi.fe);

    ierr = DMSetField(getDM(), this->field_id, fi.block, (PetscObject) fi.fe);
    ierr = PetscFESetName(fi.fe, fi.name.c_str());

    this->fields[fi.id] = fi;
    this->field_id++;

    return fi.id;
}

void
GFENonlinearProblem::setResidualBlock(PetscInt field_id, PetscFEResidualFunc *f0, PetscFEResidualFunc *f1)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetResidual(this->ds, field_id, f0, f1);
}

void
GFENonlinearProblem::setJacobianBlock(PetscInt fid, PetscInt gid, PetscFEJacobianFunc *g0, PetscFEJacobianFunc *g1, PetscFEJacobianFunc *g2, PetscFEJacobianFunc *g3)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetJacobian(this->ds, fid, gid, g0, g1, g2, g3);
}

void
GFENonlinearProblem::setInitialCondition(PetscFunc *ic)
{
    _F_;
    // TODO: the actual projection should happen in setupInitialGuess()
    // Here we just need to store info and check that this was called only once
    PetscErrorCode ierr;
    PetscFunc *initial_guess[1] = { ic };
    ierr = DMProjectFunction(getDM(), 0.0, initial_guess, NULL, INSERT_VALUES, this->x);
}

void
GFENonlinearProblem::setInitialCondition(PetscInt fid, PetscFieldFunc *ic)
{
    _F_;
    const auto it = this->fields.find(fid);
    if (it == this->fields.end())
        godzillaError("Trying to set initial condition for an non-exiting field (", fid, ").");
    else
        this->field_ics[fid] = ic;
}

}
