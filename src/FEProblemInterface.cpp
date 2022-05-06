#include "Godzilla.h"
#include "CallStack.h"
#include "FEProblemInterface.h"
#include "Mesh.h"
#include "Problem.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "AuxiliaryField.h"
#include "FunctionInterface.h"
#include "App.h"
#include "Logger.h"

namespace godzilla {

namespace internal {

static PetscErrorCode
zero_fn(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar * u, void * ctx)
{
    u[0] = 0.0;
    return 0;
}

} // namespace internal

FEProblemInterface::FEProblemInterface(Problem & problem, const InputParameters & params) :
    problem(problem),
    logger(const_cast<Logger &>(params.get<const App *>("_app")->getLogger())),
    qorder(PETSC_DETERMINE),
    ds(nullptr)
{
}

FEProblemInterface::~FEProblemInterface()
{
    _F_;
    for (auto & kv : this->fields) {
        FieldInfo & fi = kv.second;
        PetscFEDestroy(&fi.fe);
    }
    for (auto & kv : this->aux_fields) {
        FieldInfo & fi = kv.second;
        PetscFEDestroy(&fi.fe);
    }
}

void
FEProblemInterface::create(DM dm)
{
    _F_;
    onSetFields();

    for (auto & aux : this->auxs)
        aux->create();
    for (auto & it : this->ics)
        it.second.ic->create();
    for (auto & bc : this->bcs)
        bc->create();
}

void
FEProblemInterface::init(DM dm)
{
    _F_;
    setUpFEs(dm);
    setUpProblem(dm);
}

const std::string &
FEProblemInterface::getFieldName(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.name;
    else
        error("Field with ID = '", fid, "' does not exist.");
}

PetscInt
FEProblemInterface::getFieldId(const std::string & name) const
{
    _F_;
    const auto & it = this->fields_by_name.find(name);
    if (it != this->fields_by_name.end())
        return it->second;
    else
        error("Field '", name, "' does not exist. Typo?");
}

bool
FEProblemInterface::hasFieldById(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    return it != this->fields.end();
}

bool
FEProblemInterface::hasFieldByName(const std::string & name) const
{
    _F_;
    const auto & it = this->fields_by_name.find(name);
    return it != this->fields_by_name.end();
}

const std::string &
FEProblemInterface::getAuxFieldName(PetscInt fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        error("Auxiliary field with ID = '", fid, "' does not exist.");
}

PetscInt
FEProblemInterface::getAuxFieldId(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        error("Auxiliary field '", name, "' does not exist. Typo?");
}

bool
FEProblemInterface::hasAuxFieldById(PetscInt fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    return it != this->aux_fields.end();
}

bool
FEProblemInterface::hasAuxFieldByName(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    return it != this->aux_fields_by_name.end();
}

void
FEProblemInterface::addFE(PetscInt id, const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi = { name, id, nullptr, nullptr, nc, k };
        this->fields[id] = fi;
        this->fields_by_name[name] = id;
    }
    else
        error("Cannot add field '", name, "' with ID = ", id, ". ID already exists.");
}

void
FEProblemInterface::addAuxFE(PetscInt id, const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    auto it = this->aux_fields.find(id);
    if (it == this->aux_fields.end()) {
        FieldInfo fi = { name, id, nullptr, nullptr, nc, k };
        this->aux_fields[id] = fi;
        this->aux_fields_by_name[name] = id;
    }
    else
        error("Cannot add auxiliary field '", name, "' with ID = ", id, ". ID is already taken.");
}

void
FEProblemInterface::setConstants(const std::vector<PetscReal> & consts)
{
    _F_;
    this->consts = consts;
}

void
FEProblemInterface::addInitialCondition(InitialCondition * ic)
{
    _F_;
    PetscInt fid = ic->getFieldId();
    const auto & it = this->ics.find(fid);
    if (it == this->ics.end())
        this->ics[fid].ic = ic;
    else
        // TODO: improve this error message
        this->logger.error("Initial condition '",
                           ic->getName(),
                           "' is being applied to a field that already has an initial condition.");
}

void
FEProblemInterface::addBoundaryCondition(BoundaryCondition * bc)
{
    _F_;
    this->bcs.push_back(bc);
}

void
FEProblemInterface::addAuxiliaryField(AuxiliaryField * aux)
{
    _F_;
    this->auxs.push_back(aux);
}

void
FEProblemInterface::setUpBoundaryConditions(DM dm)
{
    _F_;
    /// TODO: refactor this into a method
    bool no_errors = true;
    for (auto & bc : this->bcs) {
        const std::string & bnd_name = bc->getBoundary();
        PetscErrorCode ierr;
        PetscBool exists = PETSC_FALSE;
        ierr = DMHasLabel(dm, bnd_name.c_str(), &exists);
        checkPetscError(ierr);
        if (!exists) {
            no_errors = false;
            this->logger.error("Boundary condition '",
                               bc->getName(),
                               "' is set on boundary '",
                               bnd_name,
                               "' which does not exist in the mesh.");
        }
    }

    if (no_errors)
        for (auto & bc : this->bcs)
            bc->setUp(dm);
}

void
FEProblemInterface::setUpInitialGuess(DM dm, Vec x)
{
    _F_;
    PetscInt n_ics = this->ics.size();
    if (n_ics > 0) {
        if (n_ics != fields.size())
            this->logger.error("Provided ",
                               fields.size(),
                               " field(s), but ",
                               n_ics,
                               " initial condition(s).");
        else {
            bool no_errors = true;
            PetscErrorCode ierr;
            PetscFunc * ic_funcs[n_ics];
            void * ic_ctxs[n_ics];
            for (auto & it : this->ics) {
                PetscInt fid = it.first;
                const ICInfo & ic_info = it.second;
                const InitialCondition * ic = ic_info.ic;

                PetscInt ic_nc = ic->getNumComponents();
                PetscInt field_nc = this->fields[fid].nc;
                if (ic_nc != field_nc) {
                    no_errors = false;
                    this->logger.error("Initial condition '",
                                       ic->getName(),
                                       "' operates on ",
                                       ic_nc,
                                       " components, but is set on a field with ",
                                       field_nc,
                                       " components.");
                }

                ic_funcs[fid] = __initial_condition_function;
                ic_ctxs[fid] = (void *) ic;
            }

            if (no_errors) {
                ierr = DMProjectFunction(dm, getTime(), ic_funcs, ic_ctxs, INSERT_VALUES, x);
                checkPetscError(ierr);
            }
        }
    }
    else {
        // no initial conditions -> use zero
        PetscErrorCode ierr;
        PetscFunc * initial_guess[1] = { internal::zero_fn };
        ierr = DMProjectFunction(dm, getTime(), initial_guess, NULL, INSERT_VALUES, x);
        checkPetscError(ierr);
    }
}

void
FEProblemInterface::setUpFEs(DM dm)
{
    _F_;
    MPI_Comm comm;
    PetscObjectGetComm((PetscObject) dm, &comm);

    PetscErrorCode ierr;

    // FIXME: pull this from UnstructuredMesh
    PetscInt dim;
    DMGetDimension(dm, &dim);

    // FIXME: determine if the mesh is made of simplex elements
    PetscBool is_simplex = PETSC_FALSE;

    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        ierr = PetscFECreateLagrange(comm, dim, fi.nc, is_simplex, fi.k, this->qorder, &fi.fe);
        checkPetscError(ierr);
        ierr = PetscFESetName(fi.fe, fi.name.c_str());
        checkPetscError(ierr);
    }

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        ierr = PetscFECreateLagrange(comm, dim, fi.nc, is_simplex, fi.k, this->qorder, &fi.fe);
        checkPetscError(ierr);
        ierr = PetscFESetName(fi.fe, fi.name.c_str());
        checkPetscError(ierr);
    }
}

void
FEProblemInterface::setUpProblem(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        ierr = DMSetField(dm, fi.id, fi.block, (PetscObject) fi.fe);
        checkPetscError(ierr);
    }

    ierr = DMCreateDS(dm);
    checkPetscError(ierr);

    ierr = DMGetDS(dm, &this->ds);
    checkPetscError(ierr);

    onSetWeakForm();
    setUpBoundaryConditions(dm);
    setUpConstants();

    DM cdm = dm;
    while (cdm) {
        setUpAuxiliaryDM(cdm);

        ierr = DMCopyDisc(dm, cdm);
        checkPetscError(ierr);

        ierr = DMGetCoarseDM(cdm, &cdm);
        checkPetscError(ierr);
    }
}

void
FEProblemInterface::setUpAuxiliaryDM(DM dm)
{
    _F_;
    if (this->aux_fields.size() == 0)
        return;

    PetscErrorCode ierr;

    DM dm_aux;
    ierr = DMClone(dm, &dm_aux);
    checkPetscError(ierr);

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        ierr = DMSetField(dm_aux, fi.id, fi.block, (PetscObject) fi.fe);
        checkPetscError(ierr);
    }

    ierr = DMCreateDS(dm_aux);
    checkPetscError(ierr);

    bool no_errors = true;
    for (auto & aux : this->auxs) {
        PetscInt fid = aux->getFieldId();
        if (hasAuxFieldById(fid)) {
            PetscInt aux_nc = aux->getNumComponents();
            PetscInt field_nc = this->aux_fields[fid].nc;
            if (aux_nc != field_nc) {
                no_errors = false;
                this->logger.error("Auxiliary field '",
                                   aux->getName(),
                                   "' has ",
                                   aux_nc,
                                   " component(s), but is set on a field with ",
                                   field_nc,
                                   " component(s).");
            }
        }
        else {
            no_errors = false;
            this->logger.error("Auxiliary field '",
                               aux->getName(),
                               "' is set on auxiliary field with ID '",
                               fid,
                               "', but such ID does not exist.");
        }
    }
    if (no_errors)
        for (auto & aux : this->auxs) {
            aux->setUp(dm, dm_aux);
        }

    ierr = DMDestroy(&dm_aux);
}

void
FEProblemInterface::setUpConstants()
{
    _F_;
    if (this->consts.size() == 0)
        return;

    PetscErrorCode ierr;
    ierr = PetscDSSetConstants(this->ds, this->consts.size(), this->consts.data());
    checkPetscError(ierr);
}

void
FEProblemInterface::setResidualBlock(PetscInt field_id,
                                     PetscFEResidualFunc * f0,
                                     PetscFEResidualFunc * f1)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetResidual(this->ds, field_id, f0, f1);
    checkPetscError(ierr);
}

void
FEProblemInterface::setJacobianBlock(PetscInt fid,
                                     PetscInt gid,
                                     PetscFEJacobianFunc * g0,
                                     PetscFEJacobianFunc * g1,
                                     PetscFEJacobianFunc * g2,
                                     PetscFEJacobianFunc * g3)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetJacobian(this->ds, fid, gid, g0, g1, g2, g3);
    checkPetscError(ierr);
}

const PetscReal &
FEProblemInterface::getTime() const
{
    return this->problem.getTime();
}

} // namespace godzilla
