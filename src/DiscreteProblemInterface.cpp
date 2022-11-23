#include "Godzilla.h"
#include "Parameters.h"
#include "CallStack.h"
#include "App.h"
#include "DiscreteProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "Logger.h"
#include "InitialCondition.h"
#include "BoundaryCondition.h"
#include <cassert>

namespace godzilla {

namespace internal {

static PetscErrorCode
zero_fn(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar * u, void * ctx)
{
    u[0] = 0.0;
    return 0;
}

} // namespace internal

///

DiscreteProblemInterface::DiscreteProblemInterface(Problem * problem, const Parameters & params) :
    problem(problem),
    unstr_mesh(dynamic_cast<const UnstructuredMesh *>(problem->get_mesh())),
    logger(params.get<const App *>("_app")->get_logger())
{
}

DiscreteProblemInterface::~DiscreteProblemInterface() {}

const UnstructuredMesh *
DiscreteProblemInterface::get_mesh() const
{
    _F_;
    return this->unstr_mesh;
}

void
DiscreteProblemInterface::add_initial_condition(InitialCondition * ic)
{
    _F_;
    this->ics.push_back(ic);
}

void
DiscreteProblemInterface::add_boundary_condition(BoundaryCondition * bc)
{
    _F_;
    this->bcs.push_back(bc);
}

void
DiscreteProblemInterface::init()
{
    _F_;
    set_up_ds();
    set_up_initial_conditions();
    set_up_boundary_conditions();
}

void
DiscreteProblemInterface::create()
{
    _F_;
    assert(this->problem != nullptr);
    assert(this->unstr_mesh != nullptr);

    set_up_fields();

    for (auto & ic : this->ics)
        ic->create();
    for (auto & bc : this->bcs)
        bc->create();
}

void
DiscreteProblemInterface::set_up_initial_conditions()
{
    _F_;

    PetscInt n_ics = this->ics.size();
    if (n_ics == 0)
        return;
    PetscInt n_fields = get_num_fields();
    if (n_ics == n_fields) {
        std::map<PetscInt, InitialCondition *> ics_by_fields;
        for (auto & ic : this->ics) {
            PetscInt fid = ic->get_field_id();
            if (fid == -1)
                continue;
            const auto & it = ics_by_fields.find(fid);
            if (it == ics_by_fields.end()) {
                PetscInt ic_nc = ic->get_num_components();
                PetscInt field_nc = get_field_num_components(fid);
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
        this->logger->error("Provided %d field(s), but %d initial condition(s).", n_fields, n_ics);
}

void
DiscreteProblemInterface::set_up_boundary_conditions()
{
    _F_;
    /// TODO: refactor this into a method
    bool no_errors = true;
    for (auto & bc : this->bcs) {
        const std::string & bnd_name = bc->get_boundary();
        bool exists = this->unstr_mesh->has_face_set(bnd_name);
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
DiscreteProblemInterface::set_zero_initial_guess()
{
    _F_;
    DM dm = this->unstr_mesh->get_dm();
    auto n_fields = get_num_fields();
    PetscFunc * initial_guess[n_fields];
    for (PetscInt i = 0; i < n_fields; i++)
        initial_guess[i] = internal::zero_fn;
    PETSC_CHECK(DMProjectFunction(dm,
                                  this->problem->get_time(),
                                  initial_guess,
                                  nullptr,
                                  INSERT_VALUES,
                                  this->problem->get_solution_vector()));
}

void
DiscreteProblemInterface::set_initial_guess_from_ics()
{
    _F_;
    PetscInt n_ics = this->ics.size();
    PetscFunc * ic_funcs[n_ics];
    void * ic_ctxs[n_ics];
    for (auto & ic : this->ics) {
        PetscInt fid = ic->get_field_id();
        ic_funcs[fid] = ic->get_function();
        ic_ctxs[fid] = ic->get_context();
    }

    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMProjectFunction(dm,
                                  this->problem->get_time(),
                                  ic_funcs,
                                  ic_ctxs,
                                  INSERT_VALUES,
                                  this->problem->get_solution_vector()));
}

void
DiscreteProblemInterface::set_up_initial_guess()
{
    _F_;
    if (!this->ics.empty())
        set_initial_guess_from_ics();
    else
        set_zero_initial_guess();
}

Vec
DiscreteProblemInterface::get_coordinates_local() const
{
    _F_;
    DM dm = this->unstr_mesh->get_dm();
    Vec coord;
    PETSC_CHECK(DMGetCoordinatesLocal(dm, &coord));
    return coord;
}

void
DiscreteProblemInterface::build_local_solution_vector(Vec sln) const
{
    DM dm = this->unstr_mesh->get_dm();
    PetscReal time = this->problem->get_time();
    PETSC_CHECK(DMGlobalToLocal(dm, this->problem->get_solution_vector(), INSERT_VALUES, sln));
    PETSC_CHECK(DMPlexInsertBoundaryValues(dm, PETSC_TRUE, sln, time, nullptr, nullptr, nullptr));
}

} // namespace godzilla
