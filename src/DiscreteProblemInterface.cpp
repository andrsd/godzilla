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

DiscreteProblemInterface::DiscreteProblemInterface(Problem * problem, const Parameters & params) :
    problem(problem),
    unstr_mesh(dynamic_cast<const UnstructuredMesh *>(problem->get_mesh())),
    logger(params.get<const App *>("_app")->get_logger()),
    ds(nullptr)
{
}

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
    this->section = this->unstr_mesh->get_local_section();
}

void
DiscreteProblemInterface::create()
{
    _F_;
    assert(this->problem != nullptr);
    assert(this->unstr_mesh != nullptr);

    for (auto & ic : this->ics)
        ic->create();
    for (auto & bc : this->bcs)
        bc->create();
}

void
DiscreteProblemInterface::set_up_initial_conditions()
{
    _F_;

    auto n_ics = this->ics.size();
    if (n_ics == 0)
        return;
    Int n_fields = get_num_fields();
    if (n_ics == n_fields) {
        std::map<Int, InitialCondition *> ics_by_fields;
        for (auto & ic : this->ics) {
            Int fid = ic->get_field_id();
            if (fid == -1)
                continue;
            const auto & it = ics_by_fields.find(fid);
            if (it == ics_by_fields.end()) {
                Int ic_nc = ic->get_num_components();
                Int field_nc = get_field_num_components(fid);
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
DiscreteProblemInterface::set_initial_guess_from_ics()
{
    _F_;
    auto n_ics = this->ics.size();
    PetscFunc * ic_funcs[n_ics];
    void * ic_ctxs[n_ics];
    for (auto & ic : this->ics) {
        Int fid = ic->get_field_id();
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
}

Int
DiscreteProblemInterface::get_field_dof(Int point, Int fid) const
{
    _F_;
    Int offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section, point, fid, &offset));
    return offset;
}

Vector
DiscreteProblemInterface::get_coordinates_local() const
{
    _F_;
    DM dm = this->unstr_mesh->get_dm();
    Vec coord;
    PETSC_CHECK(DMGetCoordinatesLocal(dm, &coord));
    return Vector(coord);
}

void
DiscreteProblemInterface::build_local_solution_vector(const Vector & sln) const
{
    DM dm = this->unstr_mesh->get_dm();
    Real time = this->problem->get_time();
    PETSC_CHECK(DMGlobalToLocal(dm, this->problem->get_solution_vector(), INSERT_VALUES, sln));
    PETSC_CHECK(DMPlexInsertBoundaryValues(dm, PETSC_TRUE, sln, time, nullptr, nullptr, nullptr));
}

void
DiscreteProblemInterface::add_boundary_essential(const std::string & name,
                                                 DMLabel label,
                                                 const std::vector<Int> & ids,
                                                 Int field,
                                                 const std::vector<Int> & components,
                                                 PetscFunc * fn,
                                                 PetscFunc * fn_t,
                                                 void * context) const
{
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_ESSENTIAL,
                                   name.c_str(),
                                   label,
                                   ids.size(),
                                   ids.data(),
                                   field,
                                   components.size(),
                                   components.size() == 0 ? nullptr : components.data(),
                                   (void (*)()) fn,
                                   (void (*)()) fn_t,
                                   context,
                                   nullptr));
}

void
DiscreteProblemInterface::add_boundary_natural(const std::string & name,
                                               DMLabel label,
                                               const std::vector<Int> & ids,
                                               Int field,
                                               const std::vector<Int> & components,
                                               void * context) const
{
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_NATURAL,
                                   name.c_str(),
                                   label,
                                   ids.size(),
                                   ids.data(),
                                   field,
                                   components.size(),
                                   components.size() == 0 ? nullptr : components.data(),
                                   nullptr,
                                   nullptr,
                                   context,
                                   nullptr));
}

void
DiscreteProblemInterface::add_boundary_natural_riemann(const std::string & name,
                                                       DMLabel label,
                                                       const std::vector<Int> & ids,
                                                       Int field,
                                                       const std::vector<Int> & components,
                                                       PetscNaturalRiemannBCFunc * fn,
                                                       PetscNaturalRiemannBCFunc * fn_t,
                                                       void * context) const
{
    _F_;
    PETSC_CHECK(PetscDSAddBoundary(this->ds,
                                   DM_BC_NATURAL_RIEMANN,
                                   name.c_str(),
                                   label,
                                   ids.size(),
                                   ids.data(),
                                   field,
                                   components.size(),
                                   components.size() == 0 ? nullptr : components.data(),
                                   (void (*)()) fn,
                                   (void (*)()) fn_t,
                                   context,
                                   nullptr));
}

} // namespace godzilla
