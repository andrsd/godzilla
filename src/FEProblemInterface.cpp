#include "Godzilla.h"
#include "CallStack.h"
#include "FEProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include "AuxiliaryField.h"
#include "FunctionInterface.h"
#include "App.h"
#include "Logger.h"
#include <assert.h>

namespace godzilla {

namespace internal {

static PetscErrorCode
zero_fn(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar * u, void * ctx)
{
    u[0] = 0.0;
    return 0;
}

} // namespace internal

FEProblemInterface::FEProblemInterface(Problem * problem, const InputParameters & params) :
    problem(problem),
    unstr_mesh(dynamic_cast<const UnstructuredMesh *>(problem->get_mesh())),
    logger(params.get<const App *>("_app")->get_logger()),
    qorder(PETSC_DETERMINE),
    ds(nullptr),
    a(nullptr)
{
    assert(this->problem != nullptr);
    assert(this->unstr_mesh != nullptr);
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

    if (this->a)
        VecDestroy(&this->a);
}

void
FEProblemInterface::create()
{
    _F_;
    on_set_fields();

    for (auto & aux : this->auxs)
        aux->create();
    for (auto & ic : this->ics)
        ic->create();
    for (auto & bc : this->bcs)
        bc->create();
}

void
FEProblemInterface::init()
{
    _F_;
    set_up_fes();
    set_up_problem();
}

const UnstructuredMesh *
FEProblemInterface::get_mesh() const
{
    _F_;
    return this->unstr_mesh;
}

std::vector<std::string>
FEProblemInterface::get_field_names() const
{
    _F_;
    std::vector<std::string> infos;
    for (const auto & it : this->fields)
        infos.push_back(it.second.name);

    return infos;
}

const std::string &
FEProblemInterface::get_field_name(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.name;
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_order(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.k;
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_num_components(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end())
        return it->second.nc;
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_id(const std::string & name) const
{
    _F_;
    const auto & it = this->fields_by_name.find(name);
    if (it != this->fields_by_name.end())
        return it->second;
    else
        error("Field '%s' does not exist. Typo?", name);
}

bool
FEProblemInterface::has_field_by_id(PetscInt fid) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    return it != this->fields.end();
}

bool
FEProblemInterface::has_field_by_name(const std::string & name) const
{
    _F_;
    const auto & it = this->fields_by_name.find(name);
    return it != this->fields_by_name.end();
}

const std::string &
FEProblemInterface::get_aux_field_name(PetscInt fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    if (it != this->aux_fields.end())
        return it->second.name;
    else
        error("Auxiliary field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_aux_field_id(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    if (it != this->aux_fields_by_name.end())
        return it->second;
    else
        error("Auxiliary field '%s' does not exist. Typo?", name);
}

bool
FEProblemInterface::has_aux_field_by_id(PetscInt fid) const
{
    _F_;
    const auto & it = this->aux_fields.find(fid);
    return it != this->aux_fields.end();
}

bool
FEProblemInterface::has_aux_field_by_name(const std::string & name) const
{
    _F_;
    const auto & it = this->aux_fields_by_name.find(name);
    return it != this->aux_fields_by_name.end();
}

void
FEProblemInterface::add_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    auto it = this->fields.find(id);
    if (it == this->fields.end()) {
        FieldInfo fi = { name, id, nullptr, nullptr, nc, k };
        this->fields[id] = fi;
        this->fields_by_name[name] = id;
    }
    else
        error("Cannot add field '%s' with ID = %d. ID already exists.", name, id);
}

void
FEProblemInterface::add_aux_fe(PetscInt id, const std::string & name, PetscInt nc, PetscInt k)
{
    _F_;
    auto it = this->aux_fields.find(id);
    if (it == this->aux_fields.end()) {
        FieldInfo fi = { name, id, nullptr, nullptr, nc, k };
        this->aux_fields[id] = fi;
        this->aux_fields_by_name[name] = id;
    }
    else
        error("Cannot add auxiliary field '%s' with ID = %d. ID is already taken.", name, id);
}

void
FEProblemInterface::set_constants(const std::vector<PetscReal> & consts)
{
    _F_;
    this->consts = consts;
}

void
FEProblemInterface::add_initial_condition(InitialCondition * ic)
{
    _F_;
    this->ics.push_back(ic);
}

void
FEProblemInterface::add_boundary_condition(BoundaryCondition * bc)
{
    _F_;
    this->bcs.push_back(bc);
}

void
FEProblemInterface::add_auxiliary_field(AuxiliaryField * aux)
{
    _F_;
    this->auxs.push_back(aux);
}

void
FEProblemInterface::set_up_initial_conditions()
{
    _F_;

    PetscInt n_ics = this->ics.size();
    if (n_ics == 0)
        return;
    if (n_ics == fields.size()) {
        std::map<PetscInt, InitialCondition *> ics_by_fields;
        for (auto & ic : this->ics) {
            PetscInt fid = ic->get_field_id();
            const auto & it = ics_by_fields.find(fid);
            if (it == ics_by_fields.end()) {
                PetscInt ic_nc = ic->get_num_components();
                PetscInt field_nc = this->fields[fid].nc;
                if (ic_nc == field_nc)
                    ics_by_fields[fid] = ic;
                else
                    this->logger->error("Initial condition '%s' operates on %d components, but is "
                                        "set on a field with %d components.",
                                        ic->get_name(),
                                        ic_nc,
                                        field_nc);
            }
            else
                // TODO: improve this error message
                this->logger->error(
                    "Initial condition '%s' is being applied to a field that already "
                    "has an initial condition.",
                    ic->get_name());
        }
    }
    else
        this->logger->error("Provided %d field(s), but %d initial condition(s).",
                            fields.size(),
                            n_ics);
}

void
FEProblemInterface::set_up_boundary_conditions()
{
    _F_;
    /// TODO: refactor this into a method
    bool no_errors = true;
    for (auto & bc : this->bcs) {
        const std::string & bnd_name = bc->get_boundary();
        bool exists = this->unstr_mesh->has_label(bnd_name);
        if (!exists) {
            no_errors = false;
            this->logger->error(
                "Boundary condition '%s' is set on boundary '%s' which does not exist in the mesh.",
                bc->get_name(),
                bnd_name);
        }
    }

    if (no_errors)
        for (auto & bc : this->bcs)
            bc->set_up();
}

void
FEProblemInterface::set_zero_initial_guess()
{
    _F_;
    DM dm = this->unstr_mesh->get_dm();
    PetscFunc * initial_guess[1] = { internal::zero_fn };
    PetscErrorCode ierr = DMProjectFunction(dm,
                                            get_time(),
                                            initial_guess,
                                            NULL,
                                            INSERT_VALUES,
                                            this->problem->get_solution_vector());
    check_petsc_error(ierr);
}

void
FEProblemInterface::set_initial_guess_from_ics()
{
    _F_;
    PetscInt n_ics = this->ics.size();
    PetscFunc * ic_funcs[n_ics];
    void * ic_ctxs[n_ics];
    for (auto & ic : this->ics) {
        PetscInt fid = ic->get_field_id();
        ic_funcs[fid] = __initial_condition_function;
        ic_ctxs[fid] = (void *) ic;
    }

    PetscErrorCode ierr;
    DM dm = this->unstr_mesh->get_dm();
    ierr = DMProjectFunction(dm,
                             get_time(),
                             ic_funcs,
                             ic_ctxs,
                             INSERT_VALUES,
                             this->problem->get_solution_vector());
    check_petsc_error(ierr);
}

void
FEProblemInterface::set_up_initial_guess()
{
    _F_;
    if (this->ics.size() > 0)
        set_initial_guess_from_ics();
    else
        set_zero_initial_guess();
}

void
FEProblemInterface::create_fe(FieldInfo & fi)
{
    _F_;
    PetscErrorCode ierr;

    const MPI_Comm & comm = this->unstr_mesh->get_comm();
    PetscInt dim = this->problem->get_dimension();
    PetscBool is_simplex = this->unstr_mesh->is_simplex() ? PETSC_TRUE : PETSC_FALSE;

    ierr = PetscFECreateLagrange(comm, dim, fi.nc, is_simplex, fi.k, this->qorder, &fi.fe);
    check_petsc_error(ierr);
    ierr = PetscFESetName(fi.fe, fi.name.c_str());
    check_petsc_error(ierr);
}

void
FEProblemInterface::set_up_fes()
{
    _F_;
    for (auto & it : this->fields)
        create_fe(it.second);
    for (auto & it : this->aux_fields)
        create_fe(it.second);
}

void
FEProblemInterface::set_up_problem()
{
    _F_;
    PetscErrorCode ierr;
    DM dm = this->unstr_mesh->get_dm();

    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        ierr = DMSetField(dm, fi.id, fi.block, (PetscObject) fi.fe);
        check_petsc_error(ierr);
    }

    ierr = DMCreateDS(dm);
    check_petsc_error(ierr);

    ierr = DMGetDS(dm, &this->ds);
    check_petsc_error(ierr);

    on_set_weak_form();
    set_up_initial_conditions();
    set_up_boundary_conditions();
    set_up_constants();

    DM cdm = dm;
    while (cdm) {
        set_up_auxiliary_dm(cdm);

        ierr = DMCopyDisc(dm, cdm);
        check_petsc_error(ierr);

        ierr = DMGetCoarseDM(cdm, &cdm);
        check_petsc_error(ierr);
    }
}

void
FEProblemInterface::compute_aux_fields(DM dm_aux, DMLabel label, Vec a)
{
    _F_;
    PetscInt n_auxs = this->auxs.size();
    PetscFunc ** func = new PetscFunc *[n_auxs];
    void ** ctxs = new void *[n_auxs];
    for (std::size_t i = 0; i < n_auxs; i++) {
        auto & aux = this->auxs[i];
        if (aux->get_label() == label)
            func[i] = aux->get_func();
        else
            func[i] = nullptr;
        ctxs[i] = aux;
    }

    PetscErrorCode ierr;
    if (label == nullptr) {
        ierr = DMProjectFunctionLocal(dm_aux, get_time(), func, ctxs, INSERT_ALL_VALUES, a);
        check_petsc_error(ierr);
    }
    else {
        IS is;
        ierr = DMLabelGetValueIS(label, &is);
        check_petsc_error(ierr);

        PetscInt n_ids;
        ierr = ISGetSize(is, &n_ids);
        check_petsc_error(ierr);

        const PetscInt * ids;
        ierr = ISGetIndices(is, &ids);
        check_petsc_error(ierr);

        ierr = DMProjectFunctionLabelLocal(dm_aux,
                                           get_time(),
                                           label,
                                           n_ids,
                                           ids,
                                           PETSC_DETERMINE,
                                           nullptr,
                                           func,
                                           ctxs,
                                           INSERT_ALL_VALUES,
                                           a);
        check_petsc_error(ierr);

        ierr = ISRestoreIndices(is, &ids);
        check_petsc_error(ierr);

        ierr = ISDestroy(&is);
        check_petsc_error(ierr);
    }

    delete[] func;
    delete[] ctxs;
}

void
FEProblemInterface::set_up_auxiliary_dm(DM dm)
{
    _F_;
    PetscErrorCode ierr;

    DM dm_aux;
    ierr = DMClone(dm, &dm_aux);
    check_petsc_error(ierr);

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        ierr = DMSetField(dm_aux, fi.id, fi.block, (PetscObject) fi.fe);
        check_petsc_error(ierr);
    }

    ierr = DMCreateDS(dm_aux);
    check_petsc_error(ierr);

    bool no_errors = true;
    for (auto & aux : this->auxs) {
        PetscInt fid = aux->get_field_id();
        if (has_aux_field_by_id(fid)) {
            PetscInt aux_nc = aux->get_num_components();
            PetscInt field_nc = this->aux_fields[fid].nc;
            if (aux_nc != field_nc) {
                no_errors = false;
                this->logger->error("Auxiliary field '%s' has %d component(s), but is set on a "
                                    "field with %d component(s).",
                                    aux->get_name(),
                                    aux_nc,
                                    field_nc);
            }
        }
        else {
            no_errors = false;
            this->logger->error("Auxiliary field '%s' is set on auxiliary field with ID '%d', but "
                                "such ID does not exist.",
                                aux->get_name(),
                                fid);
        }
    }
    if (no_errors) {
        if (this->auxs.size() > 0) {
            ierr = DMCreateLocalVector(dm_aux, &this->a);
            check_petsc_error(ierr);
            compute_aux_fields(dm_aux, nullptr, this->a);
            ierr = DMSetAuxiliaryVec(dm, nullptr, 0, 0, this->a);
            check_petsc_error(ierr);
        }
    }

    ierr = DMDestroy(&dm_aux);
}

void
FEProblemInterface::set_up_constants()
{
    _F_;
    if (this->consts.size() == 0)
        return;

    PetscErrorCode ierr;
    ierr = PetscDSSetConstants(this->ds, this->consts.size(), this->consts.data());
    check_petsc_error(ierr);
}

void
FEProblemInterface::set_residual_block(PetscInt field_id,
                                       PetscFEResidualFunc * f0,
                                       PetscFEResidualFunc * f1)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetResidual(this->ds, field_id, f0, f1);
    check_petsc_error(ierr);
}

void
FEProblemInterface::set_jacobian_block(PetscInt fid,
                                       PetscInt gid,
                                       PetscFEJacobianFunc * g0,
                                       PetscFEJacobianFunc * g1,
                                       PetscFEJacobianFunc * g2,
                                       PetscFEJacobianFunc * g3)
{
    _F_;
    PetscErrorCode ierr;
    ierr = PetscDSSetJacobian(this->ds, fid, gid, g0, g1, g2, g3);
    check_petsc_error(ierr);
}

const PetscReal &
FEProblemInterface::get_time() const
{
    return this->problem->get_time();
}

} // namespace godzilla
