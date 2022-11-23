#include "Godzilla.h"
#include "CallStack.h"
#include "FEProblemInterface.h"
#include "UnstructuredMesh.h"
#include "Problem.h"
#include "AuxiliaryField.h"
#include "NaturalBC.h"
#include "Types.h"
#include "App.h"
#include "Logger.h"
#include "PetscFEGodzilla.h"
#include "WeakForm.h"
#include "ResidualFunc.h"
#include "BndResidualFunc.h"
#include "JacobianFunc.h"
#include "BndJacobianFunc.h"
#include <assert.h>
#include <petsc/private/petscfeimpl.h>

namespace godzilla {

FEProblemInterface::FEProblemInterface(Problem * problem, const Parameters & params) :
    DiscreteProblemInterface(problem, params),
    section(nullptr),
    qorder(PETSC_DETERMINE),
    dm_aux(nullptr),
    a(nullptr),
    sln(nullptr),
    ds(nullptr),
    ds_aux(nullptr),
    wf(new WeakForm()),
    dim(-1),
    u(nullptr),
    u_t(nullptr),
    u_x(nullptr),
    u_offset(nullptr),
    u_offset_x(nullptr),
    xyz(nullptr),
    normals(nullptr),
    au(nullptr),
    au_x(nullptr),
    au_offset(nullptr),
    au_offset_x(nullptr),
    time(0.),
    u_t_shift(0.)
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

    VecDestroy(&this->a);
    DMDestroy(&this->dm_aux);

    VecDestroy(&this->sln);
}

void
FEProblemInterface::create()
{
    _F_;
    DiscreteProblemInterface::create();
    for (auto & aux : this->auxs)
        aux->create();
}

void
FEProblemInterface::init()
{
    _F_;
    DiscreteProblemInterface::init();

    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMGetLocalSection(dm, &this->section));
    DM cdm = dm;
    while (cdm) {
        set_up_auxiliary_dm(cdm);
        set_up_field_null_space(cdm);

        PETSC_CHECK(DMCopyDisc(dm, cdm));
        PETSC_CHECK(DMGetCoarseDM(cdm, &cdm));
    }

    set_up_weak_form();
    for (auto & bc : this->bcs) {
        auto * nbc = dynamic_cast<NaturalBC *>(bc);
        if (nbc)
            nbc->set_up_weak_form();
    }
}

void
FEProblemInterface::allocate_objects()
{
    DM dm = this->unstr_mesh->get_dm();
    PETSC_CHECK(DMCreateLocalVector(dm, &this->sln));
}

PetscInt
FEProblemInterface::get_num_fields() const
{
    _F_;
    return this->fields.size();
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

Vec
FEProblemInterface::get_solution_vector_local() const
{
    _F_;
    build_local_solution_vector(this->sln);
    return this->sln;
}

WeakForm *
FEProblemInterface::get_weak_form() const
{
    _F_;
    return this->wf;
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

std::string
FEProblemInterface::get_field_component_name(PetscInt fid, PetscInt component) const
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        const FieldInfo & fi = it->second;
        if (fi.nc == 1)
            return std::string("");
        else {
            assert(component < it->second.nc && component < it->second.component_names.size());
            return it->second.component_names.at(component);
        }
    }
    else
        error("Field with ID = '%d' does not exist.", fid);
}

void
FEProblemInterface::set_field_component_name(PetscInt fid,
                                             PetscInt component,
                                             const std::string name)
{
    _F_;
    const auto & it = this->fields.find(fid);
    if (it != this->fields.end()) {
        if (it->second.nc > 1) {
            assert(component < it->second.nc && component < it->second.component_names.size());
            it->second.component_names[component] = name;
        }
        else
            error("Unable to set component name for single-component field");
    }
    else
        error("Field with ID = '%d' does not exist.", fid);
}

PetscInt
FEProblemInterface::get_field_dof(PetscInt point, PetscInt fid) const
{
    _F_;
    PetscInt offset;
    PETSC_CHECK(PetscSectionGetFieldOffset(this->section, point, fid, &offset));
    return offset;
}

PetscInt
FEProblemInterface::get_num_aux_fields() const
{
    _F_;
    return this->aux_fields.size();
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
        FieldInfo fi = { name, id, nullptr, nullptr, nc, k, {} };
        if (nc > 1) {
            fi.component_names.resize(nc);
            for (unsigned int i = 0; i < nc; i++)
                fi.component_names[i] = fmt::sprintf("%d", i);
        }
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
FEProblemInterface::add_auxiliary_field(AuxiliaryField * aux)
{
    _F_;
    this->auxs.push_back(aux);
}

void
FEProblemInterface::set_up_field_null_space(DM dm)
{
}

void
FEProblemInterface::create_fe(FieldInfo & fi)
{
    _F_;
    const MPI_Comm & comm = this->unstr_mesh->get_comm();
    PetscInt dim = this->problem->get_dimension();
    PetscBool is_simplex = this->unstr_mesh->is_simplex() ? PETSC_TRUE : PETSC_FALSE;
    PETSC_CHECK(internal::create_lagrange_petscfe(comm,
                                                  dim,
                                                  fi.nc,
                                                  is_simplex,
                                                  fi.k,
                                                  this->qorder,
                                                  &fi.fe));
}

void
FEProblemInterface::set_up_ds()
{
    _F_;
    for (auto & it : this->fields)
        create_fe(it.second);
    for (auto & it : this->aux_fields)
        create_fe(it.second);

    set_up_quadrature();

    DM dm = this->unstr_mesh->get_dm();
    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(DMSetField(dm, fi.id, fi.block, (PetscObject) fi.fe));
    }
    PETSC_CHECK(DMCreateDS(dm));
    PETSC_CHECK(DMGetDS(dm, &this->ds));
    for (auto & it : this->fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(PetscDSSetContext(this->ds, fi.id, this));
    }

    //
    this->dim = this->problem->get_dimension();
    PETSC_CHECK(PetscDSGetEvaluationArrays(this->ds, &this->u, &this->u_t, &this->u_x));
    PETSC_CHECK(PetscDSGetComponentOffsets(this->ds, &this->u_offset));
    PETSC_CHECK(PetscDSGetComponentDerivativeOffsets(this->ds, &this->u_offset_x));
    PETSC_CHECK(PetscDSGetWorkspace(this->ds, &this->xyz, nullptr, nullptr, nullptr, nullptr));
}

void
FEProblemInterface::set_up_quadrature()
{
    _F_;
    assert(this->fields.size() > 0);
    auto first = this->fields.begin();
    FieldInfo & first_fi = first->second;
    for (auto it = ++first; it != this->fields.end(); ++it) {
        FieldInfo & fi = it->second;
        PETSC_CHECK(PetscFECopyQuadrature(first_fi.fe, fi.fe));
    }
    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(PetscFECopyQuadrature(first_fi.fe, fi.fe));
    }
}

void
FEProblemInterface::compute_global_aux_fields(DM dm,
                                              const std::vector<AuxiliaryField *> & auxs,
                                              Vec a)
{
    _F_;
    PetscInt n_auxs = this->aux_fields.size();
    auto ** func = new PetscFunc *[n_auxs];
    void ** ctxs = new void *[n_auxs];
    for (PetscInt i = 0; i < n_auxs; i++) {
        func[i] = nullptr;
        ctxs[i] = nullptr;
    }

    for (const auto & aux : auxs) {
        PetscInt fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
    }

    PETSC_CHECK(
        DMProjectFunctionLocal(dm, this->problem->get_time(), func, ctxs, INSERT_ALL_VALUES, a));

    delete[] func;
    delete[] ctxs;
}

void
FEProblemInterface::compute_label_aux_fields(DM dm,
                                             DMLabel label,
                                             const std::vector<AuxiliaryField *> & auxs,
                                             Vec a)
{
    _F_;
    PetscInt n_auxs = this->aux_fields.size();
    auto ** func = new PetscFunc *[n_auxs];
    void ** ctxs = new void *[n_auxs];
    for (PetscInt i = 0; i < n_auxs; i++) {
        func[i] = nullptr;
        ctxs[i] = nullptr;
    }

    for (const auto & aux : auxs) {
        PetscInt fid = aux->get_field_id();
        func[fid] = aux->get_func();
        ctxs[fid] = aux->get_context();
    }

    IS is;
    PETSC_CHECK(DMLabelGetValueIS(label, &is));
    PetscInt n_ids;
    PETSC_CHECK(ISGetSize(is, &n_ids));
    const PetscInt * ids;
    PETSC_CHECK(ISGetIndices(is, &ids));
    PETSC_CHECK(DMProjectFunctionLabelLocal(dm,
                                            this->problem->get_time(),
                                            label,
                                            n_ids,
                                            ids,
                                            PETSC_DETERMINE,
                                            nullptr,
                                            func,
                                            ctxs,
                                            INSERT_ALL_VALUES,
                                            a));
    PETSC_CHECK(ISRestoreIndices(is, &ids));
    PETSC_CHECK(ISDestroy(&is));

    delete[] func;
    delete[] ctxs;
}

void
FEProblemInterface::compute_aux_fields()
{
    _F_;
    for (const auto & it : this->auxs_by_region) {
        const std::string & region_name = it.first;
        const std::vector<AuxiliaryField *> & auxs = it.second;
        DMLabel label = nullptr;
        if (region_name.length() > 0)
            label = this->unstr_mesh->get_label(region_name);

        if (label == nullptr)
            compute_global_aux_fields(this->dm_aux, auxs, this->a);
        else
            compute_label_aux_fields(this->dm_aux, label, auxs, this->a);
    }
}

void
FEProblemInterface::set_up_auxiliary_dm(DM dm)
{
    _F_;
    if (this->aux_fields.size() == 0)
        return;

    PETSC_CHECK(DMClone(dm, &this->dm_aux));

    for (auto & it : this->aux_fields) {
        FieldInfo & fi = it.second;
        PETSC_CHECK(DMSetField(this->dm_aux, fi.id, fi.block, (PetscObject) fi.fe));
    }

    PETSC_CHECK(DMCreateDS(this->dm_aux));

    bool no_errors = true;
    for (auto & aux : this->auxs) {
        PetscInt fid = aux->get_field_id();
        if (has_aux_field_by_id(fid)) {
            PetscInt aux_nc = aux->get_num_components();
            PetscInt field_nc = this->aux_fields[fid].nc;
            if (aux_nc == field_nc) {
                const std::string & region_name = aux->get_region();
                this->auxs_by_region[region_name].push_back(aux);
            }
            else {
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
        PETSC_CHECK(DMCreateLocalVector(this->dm_aux, &this->a));
        PETSC_CHECK(DMSetAuxiliaryVec(dm, nullptr, 0, 0, this->a));

        PETSC_CHECK(DMGetDS(this->dm_aux, &this->ds_aux));
        PETSC_CHECK(PetscDSGetEvaluationArrays(this->ds_aux, &this->au, nullptr, &this->au_x));
        PETSC_CHECK(PetscDSGetComponentOffsets(this->ds_aux, &this->au_offset));
        PETSC_CHECK(PetscDSGetComponentDerivativeOffsets(this->ds_aux, &this->au_offset_x));
    }
}

const PetscInt &
FEProblemInterface::get_spatial_dimension() const
{
    _F_;
    return this->dim;
}

const PetscScalar *
FEProblemInterface::get_field_value(const std::string & field_name) const
{
    _F_;
    if (has_field_by_name(field_name)) {
        PetscInt fid = get_field_id(field_name);
        return this->u + this->u_offset[fid];
    }
    else if (has_aux_field_by_name(field_name)) {
        PetscInt fid = get_aux_field_id(field_name);
        return this->au + this->au_offset[fid];
    }
    else
        error("Field '%s' does not exist. Typo?", field_name);
}

const PetscScalar *
FEProblemInterface::get_field_gradient(const std::string & field_name) const
{
    _F_;
    if (has_field_by_name(field_name)) {
        PetscInt fid = get_field_id(field_name);
        return this->u_x + this->u_offset_x[fid];
    }
    else if (has_aux_field_by_name(field_name)) {
        PetscInt fid = get_aux_field_id(field_name);
        return this->au_x + this->au_offset_x[fid];
    }
    else
        error("Field '%s' does not exist. Typo?", field_name);
}

const PetscScalar *
FEProblemInterface::get_field_dot(const std::string & field_name) const
{
    _F_;
    if (has_field_by_name(field_name)) {
        PetscInt fid = get_field_id(field_name);
        return this->u_t + this->u_offset[fid];
    }
    else if (has_aux_field_by_name(field_name)) {
        return nullptr;
    }
    else
        error("Field '%s' does not exist. Typo?", field_name);
}

const PetscReal &
FEProblemInterface::get_time_shift() const
{
    _F_;
    return this->u_t_shift;
}

const PetscReal &
FEProblemInterface::get_time() const
{
    _F_;
    return this->time;
}

PetscReal * const &
FEProblemInterface::get_normal() const
{
    _F_;
    return this->normals;
}

PetscReal * const &
FEProblemInterface::get_xyz() const
{
    _F_;
    return this->xyz;
}

PetscErrorCode
FEProblemInterface::integrate(PetscDS ds,
                              PetscInt field,
                              PetscInt ne,
                              PetscFEGeom * cgeom,
                              const PetscScalar coefficients[],
                              PetscDS ds_aux,
                              const PetscScalar coefficients_aux[],
                              PetscScalar integral[])
{
    _F_;
    error("Not implemented.");
    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_bnd(PetscDS ds,
                                  PetscInt field,
                                  PetscBdPointFunc obj_func,
                                  PetscInt ne,
                                  PetscFEGeom * fgeom,
                                  const PetscScalar coefficients[],
                                  PetscDS ds_aux,
                                  const PetscScalar coefficients_aux[],
                                  PetscScalar integral[])
{
    _F_;
    error("Not implemented.");
    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_residual(PetscDS ds,
                                       PetscFormKey key,
                                       PetscInt n_elems,
                                       PetscFEGeom * cell_geom,
                                       const PetscScalar coefficients[],
                                       const PetscScalar coefficients_t[],
                                       PetscDS ds_aux,
                                       const PetscScalar coefficients_aux[],
                                       PetscReal t,
                                       PetscScalar elem_vec[])
{
    _F_;
    PetscInt field = key.field;
    const auto & f0_res_fns = this->wf->get(PETSC_WF_F0, key.label, key.value, field, key.part);
    const auto & f1_res_fns = this->wf->get(PETSC_WF_F1, key.label, key.value, field, key.part);
    if ((f0_res_fns.size() == 0) && (f1_res_fns.size() == 0))
        return 0;

    PetscFE & fe = this->fields[field].fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe, &this->dim));
    this->time = t;

    PetscScalar *basis_real, *basis_der_real;
    PETSC_CHECK(PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, nullptr, nullptr));
    PetscScalar *f0, *f1;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, &f0, &f1, nullptr, nullptr, nullptr, nullptr));
    PetscInt f_offset;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field, &f_offset));
    PetscTabulation * T;
    PETSC_CHECK(PetscDSGetTabulation(ds, &T));
    PetscInt tot_dim = 0;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));

    PetscInt tot_dim_aux = 0;
    PetscTabulation * T_aux = nullptr;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
        PETSC_CHECK(PetscDSGetTabulation(ds_aux, &T_aux));
        PetscCheckFalse(
            T[0]->Np != T_aux[0]->Np,
            PETSC_COMM_SELF,
            PETSC_ERR_ARG_WRONG,
            "Number of tabulation points %D != %D number of auxiliary tabulation points",
            T[0]->Np,
            T_aux[0]->Np);
    }

    // FIXME: quad should be a member variable
    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetQuadrature(fe, &quad));
    PetscInt q_dim, q_n_comp, q_n_pts;
    const PetscReal *q_points, *q_weights;
    PETSC_CHECK(PetscQuadratureGetData(quad, &q_dim, &q_n_comp, &q_n_pts, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    PetscInt dim_embed = cell_geom->dimEmbed;
    PetscCheckFalse(cell_geom->dim != q_dim,
                    PETSC_COMM_SELF,
                    PETSC_ERR_ARG_INCOMP,
                    "FEGeom dim %D != %D quadrature dim",
                    cell_geom->dim,
                    q_dim);

    PetscInt n_fields = get_num_fields();
    PetscInt n_fields_aux = get_num_aux_fields();
    PetscInt c_offset = 0;
    PetscInt c_offset_aux = 0;
    for (PetscInt e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom;

        fe_geom.v = this->xyz; /* workspace */

        PETSC_CHECK(PetscArrayzero(f0, q_n_pts * T[field]->Nc));
        PETSC_CHECK(PetscArrayzero(f1, q_n_pts * T[field]->Nc * dim_embed));

        for (PetscInt q = 0; q < q_n_pts; ++q) {
            PETSC_CHECK(
                PetscFEGeomGetPoint(cell_geom, e, q, &q_points[q * cell_geom->dim], &fe_geom));
            PetscReal w = fe_geom.detJ[0] * q_weights[q];

            PETSC_CHECK(evaluate_field_jets(ds,
                                            n_fields,
                                            0,
                                            q,
                                            T,
                                            &fe_geom,
                                            &coefficients[c_offset],
                                            coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                            this->u,
                                            this->u_x,
                                            coefficients_t ? this->u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                0,
                                                q,
                                                T_aux,
                                                &fe_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->au,
                                                this->au_x,
                                                nullptr));
            for (auto & func : f0_res_fns)
                func->evaluate(&f0[q * T[field]->Nc]);
            for (PetscInt c = 0; c < T[field]->Nc; ++c)
                f0[q * T[field]->Nc + c] *= w;

            for (auto & func : f1_res_fns)
                func->evaluate(&f1[q * T[field]->Nc * dim]);
            for (PetscInt c = 0; c < T[field]->Nc; ++c)
                for (PetscInt d = 0; d < this->dim; ++d)
                    f1[(q * T[field]->Nc + c) * dim + d] *= w;
        }

        PETSC_CHECK(update_element_vec(fe,
                                       T[field],
                                       0,
                                       basis_real,
                                       basis_der_real,
                                       e,
                                       cell_geom,
                                       f0,
                                       f1,
                                       &elem_vec[c_offset + f_offset]));

        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
    }

    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_bnd_residual(PetscDS ds,
                                           PetscFormKey key,
                                           PetscInt n_elems,
                                           PetscFEGeom * face_geom,
                                           const PetscScalar coefficients[],
                                           const PetscScalar coefficients_t[],
                                           PetscDS ds_aux,
                                           const PetscScalar coefficients_aux[],
                                           PetscReal t,
                                           PetscScalar elem_vec[])
{
    _F_;
    PetscInt field = key.field;
    const auto & f0_res_fns =
        this->wf->get_bnd(PETSC_WF_BDF0, key.label, key.value, field, key.part);
    const auto & f1_res_fns =
        this->wf->get_bnd(PETSC_WF_BDF1, key.label, key.value, field, key.part);
    if ((f0_res_fns.size() == 0) && (f1_res_fns.size() == 0))
        return 0;

    PetscFE & fe = this->fields[field].fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe, &this->dim));
    this->time = t;

    PetscScalar *basis_real, *basis_der_real;
    PETSC_CHECK(PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, nullptr, nullptr));
    PetscScalar *f0, *f1;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, &f0, &f1, nullptr, nullptr, nullptr, nullptr));
    PetscInt f_offset;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field, &f_offset));
    PetscInt tot_dim = 0;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));
    PetscTabulation * T_face;
    PETSC_CHECK(PetscDSGetFaceTabulation(ds, &T_face));

    PetscInt dim_aux = 0;
    PetscInt tot_dim_aux = 0;
    PetscTabulation * T_face_aux = nullptr;
    PetscBool aux_on_bnd = PETSC_FALSE;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetSpatialDimension(ds_aux, &dim_aux));
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));

        aux_on_bnd = dim_aux < dim ? PETSC_TRUE : PETSC_FALSE;
        if (aux_on_bnd)
            PETSC_CHECK(PetscDSGetTabulation(ds_aux, &T_face_aux));
        else
            PETSC_CHECK(PetscDSGetFaceTabulation(ds_aux, &T_face_aux));

        PetscCheckFalse(
            T_face[0]->Np != T_face_aux[0]->Np,
            PETSC_COMM_SELF,
            PETSC_ERR_ARG_WRONG,
            "Number of tabulation points %D != %D number of auxiliary tabulation points",
            T_face[0]->Np,
            T_face_aux[0]->Np);
    }

    PetscInt n_comp_i = T_face[field]->Nc;

    // FIXME: quad should be a member variable
    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetFaceQuadrature(fe, &quad));
    PetscInt q_dim, q_n_comp, q_n_pts;
    const PetscReal *q_points, *q_weights;
    PETSC_CHECK(PetscQuadratureGetData(quad, &q_dim, &q_n_comp, &q_n_pts, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    PetscInt dim_embed = face_geom->dimEmbed;
    /* TODO FIX THIS */
    face_geom->dim = this->dim - 1;

    PetscCheckFalse(face_geom->dim != q_dim,
                    PETSC_COMM_SELF,
                    PETSC_ERR_ARG_INCOMP,
                    "FEGeom dim %D != %D quadrature dim",
                    face_geom->dim,
                    q_dim);

    PetscInt n_fields = get_num_fields();
    PetscInt n_fields_aux = get_num_aux_fields();
    PetscInt c_offset = 0;
    PetscInt c_offset_aux = 0;
    for (PetscInt e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom, cell_geom;
        const PetscInt face = face_geom->face[e][0];

        fe_geom.v = this->xyz; /* Workspace */
        PETSC_CHECK(PetscArrayzero(f0, q_n_pts * n_comp_i));
        PETSC_CHECK(PetscArrayzero(f1, q_n_pts * n_comp_i * dim_embed));
        for (PetscInt q = 0; q < q_n_pts; ++q) {
            PETSC_CHECK(
                PetscFEGeomGetPoint(face_geom, e, q, &q_points[q * face_geom->dim], &fe_geom));
            PETSC_CHECK(PetscFEGeomGetCellPoint(face_geom, e, q, &cell_geom));
            PetscReal w = fe_geom.detJ[0] * q_weights[q];
            this->normals = fe_geom.n;
            PETSC_CHECK(evaluate_field_jets(ds,
                                            n_fields,
                                            face,
                                            q,
                                            T_face,
                                            &cell_geom,
                                            &coefficients[c_offset],
                                            coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                            this->u,
                                            this->u_x,
                                            coefficients_t ? this->u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                aux_on_bnd ? 0 : face,
                                                q,
                                                T_face_aux,
                                                &cell_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->au,
                                                this->au_x,
                                                nullptr));
            for (auto & func : f0_res_fns)
                func->evaluate(&f0[q * n_comp_i]);
            for (PetscInt c = 0; c < n_comp_i; ++c)
                f0[q * n_comp_i + c] *= w;
            for (auto & func : f1_res_fns)
                func->evaluate(&f1[q * n_comp_i * this->dim]);
            for (PetscInt c = 0; c < n_comp_i; ++c)
                for (PetscInt d = 0; d < this->dim; ++d)
                    f1[(q * n_comp_i + c) * this->dim + d] *= w;
        }
        PETSC_CHECK(update_element_vec(fe,
                                       T_face[field],
                                       face,
                                       basis_real,
                                       basis_der_real,
                                       e,
                                       face_geom,
                                       f0,
                                       f1,
                                       &elem_vec[c_offset + f_offset]));
        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
    }

    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_jacobian(PetscDS ds,
                                       PetscFEJacobianType jtype,
                                       PetscFormKey key,
                                       PetscInt n_elems,
                                       PetscFEGeom * cell_geom,
                                       const PetscScalar coefficients[],
                                       const PetscScalar coefficients_t[],
                                       PetscDS ds_aux,
                                       const PetscScalar coefficients_aux[],
                                       PetscReal t,
                                       PetscReal u_tshift,
                                       PetscScalar elem_mat[])
{
    _F_;
    PetscInt n_fields = get_num_fields();
    PetscInt n_fields_aux = get_num_aux_fields();

    PetscInt field_i = key.field / n_fields;
    PetscInt field_j = key.field % n_fields;

    PetscWeakFormKind kind0, kind1, kind2, kind3;
    switch (jtype) {
    case PETSCFE_JACOBIAN_DYN:
        kind0 = PETSC_WF_GT0;
        kind1 = PETSC_WF_GT1;
        kind2 = PETSC_WF_GT2;
        kind3 = PETSC_WF_GT3;
        break;
    case PETSCFE_JACOBIAN_PRE:
        kind0 = PETSC_WF_GP0;
        kind1 = PETSC_WF_GP1;
        kind2 = PETSC_WF_GP2;
        kind3 = PETSC_WF_GP3;
        break;
    case PETSCFE_JACOBIAN:
        kind0 = PETSC_WF_G0;
        kind1 = PETSC_WF_G1;
        kind2 = PETSC_WF_G2;
        kind3 = PETSC_WF_G3;
        break;
    }

    const auto & g0_jac_fns =
        this->wf->get(kind0, key.label, key.value, field_i, field_j, key.part);
    const auto & g1_jac_fns =
        this->wf->get(kind1, key.label, key.value, field_i, field_j, key.part);
    const auto & g2_jac_fns =
        this->wf->get(kind2, key.label, key.value, field_i, field_j, key.part);
    const auto & g3_jac_fns =
        this->wf->get(kind3, key.label, key.value, field_i, field_j, key.part);
    if ((g0_jac_fns.size() == 0) && (g1_jac_fns.size() == 0) && (g2_jac_fns.size() == 0) &&
        (g3_jac_fns.size() == 0))
        return 0;

    PetscFE & fe_i = this->fields[field_i].fe;
    PetscFE & fe_j = this->fields[field_j].fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe_i, &this->dim));
    this->time = t;
    this->u_t_shift = u_tshift;

    PetscScalar *basis_real, *basis_der_real, *test_real, *test_der_real;
    PETSC_CHECK(
        PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, &test_real, &test_der_real));
    PetscScalar *g0, *g1, *g2, *g3;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, nullptr, nullptr, &g0, &g1, &g2, &g3));
    PetscInt offset_i;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_i, &offset_i));
    PetscInt offset_j;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_j, &offset_j));
    PetscInt tot_dim;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));
    PetscTabulation * T;
    PETSC_CHECK(PetscDSGetTabulation(ds, &T));
    PetscInt n_comp_i = T[field_i]->Nc;
    PetscInt n_comp_j = T[field_j]->Nc;

    PetscTabulation * T_aux = nullptr;
    PetscInt tot_dim_aux = 0;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
        PETSC_CHECK(PetscDSGetTabulation(ds_aux, &T_aux));
        PetscCheckFalse(
            T[0]->Np != T_aux[0]->Np,
            PETSC_COMM_SELF,
            PETSC_ERR_ARG_WRONG,
            "Number of tabulation points %D != %D number of auxiliary tabulation points",
            T[0]->Np,
            T_aux[0]->Np);
    }

    PetscInt n_pts = cell_geom->numPoints;
    PetscInt dim_embed = cell_geom->dimEmbed;
    bool is_affine = cell_geom->isAffine;

    // Initialize here in case the function is not defined
    PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
    PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));

    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetQuadrature(fe_i, &quad));
    PetscInt q_n_comp, q_n_points;
    const PetscReal *q_points, *q_weights;
    PETSC_CHECK(
        PetscQuadratureGetData(quad, nullptr, &q_n_comp, &q_n_points, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    // Offset into elem_mat[] for element e
    PetscInt e_offset = 0;
    // Offset into coefficients[] for element e
    PetscInt c_offset = 0;
    // Offset into coefficientsAux[] for element e
    PetscInt c_offset_aux = 0;
    for (PetscInt e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom;

        fe_geom.dim = cell_geom->dim;
        fe_geom.dimEmbed = cell_geom->dimEmbed;
        if (is_affine) {
            fe_geom.v = this->xyz;
            fe_geom.xi = cell_geom->xi;
            fe_geom.J = &cell_geom->J[e * n_pts * dim_embed * dim_embed];
            fe_geom.invJ = &cell_geom->invJ[e * n_pts * dim_embed * dim_embed];
            fe_geom.detJ = &cell_geom->detJ[e * n_pts];
        }
        for (PetscInt q = 0; q < q_n_points; ++q) {
            if (is_affine) {
                CoordinatesRefToReal(dim_embed,
                                     dim,
                                     fe_geom.xi,
                                     &cell_geom->v[e * n_pts * dim_embed],
                                     fe_geom.J,
                                     &q_points[q * dim],
                                     this->xyz);
            }
            else {
                fe_geom.v = &cell_geom->v[(e * n_pts + q) * dim_embed];
                fe_geom.J = &cell_geom->J[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.invJ = &cell_geom->invJ[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.detJ = &cell_geom->detJ[e * n_pts + q];
            }
            PetscReal w = fe_geom.detJ[0] * q_weights[q];
            if (coefficients)
                PETSC_CHECK(
                    evaluate_field_jets(ds,
                                        n_fields,
                                        0,
                                        q,
                                        T,
                                        &fe_geom,
                                        &coefficients[c_offset],
                                        coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                        this->u,
                                        this->u_x,
                                        coefficients_t ? this->u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                0,
                                                q,
                                                T_aux,
                                                &fe_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->au,
                                                this->au_x,
                                                nullptr));
            if (g0_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
                for (auto & func : g0_jac_fns)
                    func->evaluate(g0);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j; ++c)
                    g0[c] *= w;
            }
            if (g1_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g1_jac_fns)
                    func->evaluate(g1);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j * dim; ++c)
                    g1[c] *= w;
            }
            if (g2_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g2_jac_fns)
                    func->evaluate(g2);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j * dim; ++c)
                    g2[c] *= w;
            }
            if (g3_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));
                for (auto & func : g3_jac_fns)
                    func->evaluate(g3);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j * dim * dim; ++c)
                    g3[c] *= w;
            }

            PETSC_CHECK(update_element_mat(fe_i,
                                           fe_j,
                                           0,
                                           q,
                                           T[field_i],
                                           basis_real,
                                           basis_der_real,
                                           T[field_j],
                                           test_real,
                                           test_der_real,
                                           &fe_geom,
                                           g0,
                                           g1,
                                           g2,
                                           g3,
                                           e_offset,
                                           tot_dim,
                                           offset_i,
                                           offset_j,
                                           elem_mat));
        }
        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
        e_offset += PetscSqr(tot_dim);
    }

    return 0;
}

PetscErrorCode
FEProblemInterface::integrate_bnd_jacobian(PetscDS ds,
                                           PetscFormKey key,
                                           PetscInt n_elems,
                                           PetscFEGeom * face_geom,
                                           const PetscScalar coefficients[],
                                           const PetscScalar coefficients_t[],
                                           PetscDS ds_aux,
                                           const PetscScalar coefficients_aux[],
                                           PetscReal t,
                                           PetscReal u_tshift,
                                           PetscScalar elem_mat[])
{
    _F_;
    PetscInt n_fields = get_num_fields();
    PetscInt n_fields_aux = get_num_aux_fields();

    PetscInt field_i = key.field / n_fields;
    PetscInt field_j = key.field % n_fields;

    const auto & g0_jac_fns =
        this->wf->get_bnd(PETSC_WF_BDG0, key.label, key.value, field_i, field_j, key.part);
    const auto & g1_jac_fns =
        this->wf->get_bnd(PETSC_WF_BDG1, key.label, key.value, field_i, field_j, key.part);
    const auto & g2_jac_fns =
        this->wf->get_bnd(PETSC_WF_BDG2, key.label, key.value, field_i, field_j, key.part);
    const auto & g3_jac_fns =
        this->wf->get_bnd(PETSC_WF_BDG3, key.label, key.value, field_i, field_j, key.part);
    if ((g0_jac_fns.size() == 0) && (g1_jac_fns.size() == 0) && (g2_jac_fns.size() == 0) &&
        (g3_jac_fns.size() == 0))
        return 0;

    PetscFE & fe_i = this->fields[field_i].fe;
    PetscFE & fe_j = this->fields[field_j].fe;

    PETSC_CHECK(PetscFEGetSpatialDimension(fe_i, &this->dim));
    this->time = t;
    this->u_t_shift = u_tshift;

    PetscScalar *basis_real, *basis_der_real, *test_real, *test_der_real;
    PETSC_CHECK(
        PetscDSGetWorkspace(ds, nullptr, &basis_real, &basis_der_real, &test_real, &test_der_real));
    PetscScalar *g0, *g1, *g2, *g3;
    PETSC_CHECK(PetscDSGetWeakFormArrays(ds, nullptr, nullptr, &g0, &g1, &g2, &g3));
    PetscInt offset_i;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_i, &offset_i));
    PetscInt offset_j;
    PETSC_CHECK(PetscDSGetFieldOffset(ds, field_j, &offset_j));
    PetscInt tot_dim;
    PETSC_CHECK(PetscDSGetTotalDimension(ds, &tot_dim));
    PetscTabulation * T;
    PETSC_CHECK(PetscDSGetFaceTabulation(ds, &T));
    PetscInt n_comp_i = T[field_i]->Nc;
    PetscInt n_comp_j = T[field_j]->Nc;

    PetscTabulation * T_aux = nullptr;
    PetscInt tot_dim_aux = 0;
    if (ds_aux) {
        PETSC_CHECK(PetscDSGetTotalDimension(ds_aux, &tot_dim_aux));
        PETSC_CHECK(PetscDSGetFaceTabulation(ds_aux, &T_aux));
    }

    PetscInt n_pts = face_geom->numPoints;
    PetscInt dim_embed = face_geom->dimEmbed;
    bool is_affine = face_geom->isAffine;

    // Initialize here in case the function is not defined
    PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
    PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
    PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));

    PetscQuadrature quad;
    PETSC_CHECK(PetscFEGetFaceQuadrature(fe_i, &quad));
    PetscInt q_n_comp, q_n_points;
    const PetscReal *q_points, *q_weights;
    PETSC_CHECK(
        PetscQuadratureGetData(quad, nullptr, &q_n_comp, &q_n_points, &q_points, &q_weights));
    PetscCheckFalse(q_n_comp != 1,
                    PETSC_COMM_SELF,
                    PETSC_ERR_SUP,
                    "Only supports scalar quadrature, not %D components",
                    q_n_comp);

    // Offset into elem_mat[] for element e
    PetscInt e_offset = 0;
    // Offset into coefficients[] for element e
    PetscInt c_offset = 0;
    // Offset into coefficients_aux[] for element e
    PetscInt c_offset_aux = 0;
    for (PetscInt e = 0; e < n_elems; ++e) {
        PetscFEGeom fe_geom;
        const PetscInt face = face_geom->face[e][0];
        fe_geom.n = nullptr;
        fe_geom.v = nullptr;
        fe_geom.J = nullptr;
        fe_geom.detJ = nullptr;
        fe_geom.dim = face_geom->dim;
        fe_geom.dimEmbed = face_geom->dimEmbed;

        PetscFEGeom cell_geom;
        cell_geom.dim = face_geom->dim;
        cell_geom.dimEmbed = face_geom->dimEmbed;
        if (is_affine) {
            fe_geom.v = this->xyz;
            fe_geom.xi = face_geom->xi;
            fe_geom.J = &face_geom->J[e * n_pts * dim_embed * dim_embed];
            fe_geom.invJ = &face_geom->invJ[e * n_pts * dim_embed * dim_embed];
            fe_geom.detJ = &face_geom->detJ[e * n_pts];
            fe_geom.n = &face_geom->n[e * n_pts * dim_embed];

            cell_geom.J = &face_geom->suppJ[0][e * n_pts * dim_embed * dim_embed];
            cell_geom.invJ = &face_geom->suppInvJ[0][e * n_pts * dim_embed * dim_embed];
            cell_geom.detJ = &face_geom->suppDetJ[0][e * n_pts];
        }
        for (PetscInt q = 0; q < q_n_points; ++q) {
            if (is_affine) {
                CoordinatesRefToReal(dim_embed,
                                     this->dim - 1,
                                     fe_geom.xi,
                                     &face_geom->v[e * n_pts * dim_embed],
                                     fe_geom.J,
                                     &q_points[q * (this->dim - 1)],
                                     this->xyz);
            }
            else {
                fe_geom.v = &face_geom->v[(e * n_pts + q) * dim_embed];
                fe_geom.J = &face_geom->J[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.invJ = &face_geom->invJ[(e * n_pts + q) * dim_embed * dim_embed];
                fe_geom.detJ = &face_geom->detJ[e * n_pts + q];
                fe_geom.n = &face_geom->n[(e * n_pts + q) * dim_embed];

                cell_geom.J = &face_geom->suppJ[0][(e * n_pts + q) * dim_embed * dim_embed];
                cell_geom.invJ = &face_geom->suppInvJ[0][(e * n_pts + q) * dim_embed * dim_embed];
                cell_geom.detJ = &face_geom->suppDetJ[0][e * n_pts + q];
            }
            this->normals = fe_geom.n;
            PetscReal w = fe_geom.detJ[0] * q_weights[q];
            if (coefficients)
                PETSC_CHECK(
                    evaluate_field_jets(ds,
                                        n_fields,
                                        face,
                                        q,
                                        T,
                                        &cell_geom,
                                        &coefficients[c_offset],
                                        coefficients_t ? &coefficients_t[c_offset] : nullptr,
                                        this->u,
                                        this->u_x,
                                        coefficients_t ? this->u_t : nullptr));
            if (ds_aux)
                PETSC_CHECK(evaluate_field_jets(ds_aux,
                                                n_fields_aux,
                                                face,
                                                q,
                                                T_aux,
                                                &cell_geom,
                                                &coefficients_aux[c_offset_aux],
                                                nullptr,
                                                this->au,
                                                this->au_x,
                                                nullptr));

            if (g0_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g0, n_comp_i * n_comp_j));
                for (auto & func : g0_jac_fns)
                    func->evaluate(g0);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j; ++c)
                    g0[c] *= w;
            }
            if (g1_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g1, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g1_jac_fns)
                    func->evaluate(g1);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j * this->dim; ++c)
                    g1[c] *= w;
            }
            if (g2_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g2, n_comp_i * n_comp_j * dim_embed));
                for (auto & func : g2_jac_fns)
                    func->evaluate(g2);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j * this->dim; ++c)
                    g2[c] *= w;
            }
            if (g3_jac_fns.size() > 0) {
                PETSC_CHECK(PetscArrayzero(g3, n_comp_i * n_comp_j * dim_embed * dim_embed));
                for (auto & func : g3_jac_fns)
                    func->evaluate(g3);
                for (PetscInt c = 0; c < n_comp_i * n_comp_j * this->dim * this->dim; ++c)
                    g3[c] *= w;
            }

            PETSC_CHECK(update_element_mat(fe_i,
                                           fe_j,
                                           face,
                                           q,
                                           T[field_i],
                                           basis_real,
                                           basis_der_real,
                                           T[field_j],
                                           test_real,
                                           test_der_real,
                                           &cell_geom,
                                           g0,
                                           g1,
                                           g2,
                                           g3,
                                           e_offset,
                                           tot_dim,
                                           offset_i,
                                           offset_j,
                                           elem_mat));
        }
        c_offset += tot_dim;
        c_offset_aux += tot_dim_aux;
        e_offset += PetscSqr(tot_dim);
    }

    return 0;
}

// This is a copy of petsc/fe.c, PetscFEUpdateElementVec_Internal
PetscErrorCode
FEProblemInterface::update_element_vec(PetscFE fe,
                                       PetscTabulation tab,
                                       PetscInt r,
                                       PetscScalar tmp_basis[],
                                       PetscScalar tmp_basis_der[],
                                       PetscInt e,
                                       PetscFEGeom * fe_geom,
                                       PetscScalar f0[],
                                       PetscScalar f1[],
                                       PetscScalar elem_vec[])
{
    _F_;
    PetscFEGeom pgeom;
    const PetscInt dEt = tab->cdim;
    const PetscInt dE = fe_geom->dimEmbed;
    const PetscInt Nq = tab->Np;
    const PetscInt Nb = tab->Nb;
    const PetscInt Nc = tab->Nc;
    const PetscReal * basis = &tab->T[0][r * Nq * Nb * Nc];
    const PetscReal * basis_der = &tab->T[1][r * Nq * Nb * Nc * dEt];

    for (PetscInt q = 0; q < Nq; ++q) {
        for (PetscInt b = 0; b < Nb; ++b) {
            for (PetscInt c = 0; c < Nc; ++c) {
                const PetscInt bcidx = b * Nc + c;

                tmp_basis[bcidx] = basis[q * Nb * Nc + bcidx];
                for (PetscInt d = 0; d < dEt; ++d)
                    tmp_basis_der[bcidx * dE + d] = basis_der[q * Nb * Nc * dEt + bcidx * dEt + d];
                for (PetscInt d = dEt; d < dE; ++d)
                    tmp_basis_der[bcidx * dE + d] = 0.0;
            }
        }
        PETSC_CHECK(PetscFEGeomGetCellPoint(fe_geom, e, q, &pgeom));
        PETSC_CHECK(PetscFEPushforward(fe, &pgeom, Nb, tmp_basis));
        PETSC_CHECK(PetscFEPushforwardGradient(fe, &pgeom, Nb, tmp_basis_der));
        for (PetscInt b = 0; b < Nb; ++b) {
            for (PetscInt c = 0; c < Nc; ++c) {
                const PetscInt bcidx = b * Nc + c;
                const PetscInt qcidx = q * Nc + c;

                elem_vec[b] += tmp_basis[bcidx] * f0[qcidx];
                for (PetscInt d = 0; d < dE; ++d)
                    elem_vec[b] += tmp_basis_der[bcidx * dE + d] * f1[qcidx * dE + d];
            }
        }
    }
    return 0;
}

// This is a copy of petsc/fe.c, PetscFEUpdateElementMat_Internal
PetscErrorCode
FEProblemInterface::update_element_mat(PetscFE fe_i,
                                       PetscFE fe_j,
                                       PetscInt r,
                                       PetscInt q,
                                       PetscTabulation tab_i,
                                       PetscScalar tmp_basis_i[],
                                       PetscScalar tmp_basis_der_i[],
                                       PetscTabulation tab_j,
                                       PetscScalar tmp_basis_j[],
                                       PetscScalar tmp_basis_der_j[],
                                       PetscFEGeom * fe_geom,
                                       const PetscScalar g0[],
                                       const PetscScalar g1[],
                                       const PetscScalar g2[],
                                       const PetscScalar g3[],
                                       PetscInt e_offset,
                                       PetscInt tot_dim,
                                       PetscInt offset_i,
                                       PetscInt offset_j,
                                       PetscScalar elem_mat[])
{
    _F_;
    const PetscInt dE = tab_i->cdim;
    const PetscInt NqI = tab_i->Np;
    const PetscInt NbI = tab_i->Nb;
    const PetscInt n_comp_i = tab_i->Nc;
    const PetscReal * basis_i = &tab_i->T[0][(r * NqI + q) * NbI * n_comp_i];
    const PetscReal * basis_der_i = &tab_i->T[1][(r * NqI + q) * NbI * n_comp_i * dE];
    const PetscInt NqJ = tab_j->Np;
    const PetscInt NbJ = tab_j->Nb;
    const PetscInt n_comp_j = tab_j->Nc;
    const PetscReal * basis_j = &tab_j->T[0][(r * NqJ + q) * NbJ * n_comp_j];
    const PetscReal * basis_der_j = &tab_j->T[1][(r * NqJ + q) * NbJ * n_comp_j * dE];

    for (PetscInt f = 0; f < NbI; ++f) {
        for (PetscInt fc = 0; fc < n_comp_i; ++fc) {
            // Test function basis index
            const PetscInt fidx = f * n_comp_i + fc;

            tmp_basis_i[fidx] = basis_i[fidx];
            for (PetscInt df = 0; df < dE; ++df)
                tmp_basis_der_i[fidx * dE + df] = basis_der_i[fidx * dE + df];
        }
    }
    PETSC_CHECK(PetscFEPushforward(fe_i, fe_geom, NbI, tmp_basis_i));
    PETSC_CHECK(PetscFEPushforwardGradient(fe_i, fe_geom, NbI, tmp_basis_der_i));
    for (PetscInt g = 0; g < NbJ; ++g) {
        for (PetscInt gc = 0; gc < n_comp_j; ++gc) {
            // Trial function basis index
            const PetscInt gidx = g * n_comp_j + gc;

            tmp_basis_j[gidx] = basis_j[gidx];
            for (PetscInt dg = 0; dg < dE; ++dg)
                tmp_basis_der_j[gidx * dE + dg] = basis_der_j[gidx * dE + dg];
        }
    }
    PETSC_CHECK(PetscFEPushforward(fe_j, fe_geom, NbJ, tmp_basis_j));
    PETSC_CHECK(PetscFEPushforwardGradient(fe_j, fe_geom, NbJ, tmp_basis_der_j));
    for (PetscInt f = 0; f < NbI; ++f) {
        for (PetscInt fc = 0; fc < n_comp_i; ++fc) {
            // Test function basis index
            const PetscInt fidx = f * n_comp_i + fc;
            // Element matrix row
            const PetscInt i = offset_i + f;
            for (PetscInt g = 0; g < NbJ; ++g) {
                for (PetscInt gc = 0; gc < n_comp_j; ++gc) {
                    // Trial function basis index
                    const PetscInt gidx = g * n_comp_j + gc;
                    // Element matrix column
                    const PetscInt j = offset_j + g;
                    const PetscInt f_off = e_offset + i * tot_dim + j;

                    elem_mat[f_off] +=
                        tmp_basis_i[fidx] * g0[fc * n_comp_j + gc] * tmp_basis_j[gidx];
                    for (PetscInt df = 0; df < dE; ++df) {
                        elem_mat[f_off] += tmp_basis_i[fidx] * g1[(fc * n_comp_j + gc) * dE + df] *
                                           tmp_basis_der_j[gidx * dE + df];
                        elem_mat[f_off] += tmp_basis_der_i[fidx * dE + df] *
                                           g2[(fc * n_comp_j + gc) * dE + df] * tmp_basis_j[gidx];
                        for (PetscInt dg = 0; dg < dE; ++dg) {
                            elem_mat[f_off] += tmp_basis_der_i[fidx * dE + df] *
                                               g3[((fc * n_comp_j + gc) * dE + df) * dE + dg] *
                                               tmp_basis_der_j[gidx * dE + dg];
                        }
                    }
                }
            }
        }
    }
    return (0);
}

// This is a copy of petsc/fe.c, PetscFEEvaluateFieldJets_Internal
PetscErrorCode
FEProblemInterface::evaluate_field_jets(PetscDS ds,
                                        PetscInt nf,
                                        PetscInt r,
                                        PetscInt q,
                                        PetscTabulation tab[],
                                        PetscFEGeom * fe_geom,
                                        const PetscScalar coefficients[],
                                        const PetscScalar coefficients_t[],
                                        PetscScalar u[],
                                        PetscScalar u_x[],
                                        PetscScalar u_t[])
{
    _F_;
    PetscInt d_offset = 0, f_offset = 0;

    for (PetscInt f = 0; f < nf; ++f) {
        PetscFE fe;
        const PetscInt k = ds->jetDegree[f];
        const PetscInt cdim = tab[f]->cdim;
        const PetscInt n_q = tab[f]->Np;
        const PetscInt n_bf = tab[f]->Nb;
        const PetscInt n_cf = tab[f]->Nc;
        const PetscReal * Bq = &tab[f]->T[0][(r * n_q + q) * n_bf * n_cf];
        const PetscReal * Dq = &tab[f]->T[1][(r * n_q + q) * n_bf * n_cf * cdim];
        const PetscReal * Hq =
            k > 1 ? &tab[f]->T[2][(r * n_q + q) * n_bf * n_cf * cdim * cdim] : nullptr;
        PetscInt h_offset = 0;

        PETSC_CHECK(PetscDSGetDiscretization(ds, f, (PetscObject *) &fe));
        for (PetscInt c = 0; c < n_cf; ++c)
            u[f_offset + c] = 0.0;
        for (PetscInt d = 0; d < cdim * n_cf; ++d)
            u_x[f_offset * cdim + d] = 0.0;
        for (PetscInt b = 0; b < n_bf; ++b) {
            for (PetscInt c = 0; c < n_cf; ++c) {
                const PetscInt cidx = b * n_cf + c;

                u[f_offset + c] += Bq[cidx] * coefficients[d_offset + b];
                for (PetscInt d = 0; d < cdim; ++d)
                    u_x[(f_offset + c) * cdim + d] +=
                        Dq[cidx * cdim + d] * coefficients[d_offset + b];
            }
        }
        if (k > 1) {
            for (PetscInt g = 0; g < nf; ++g)
                h_offset += tab[g]->Nc * cdim;
            for (PetscInt d = 0; d < cdim * cdim * n_cf; ++d)
                u_x[h_offset + f_offset * cdim * cdim + d] = 0.0;
            for (PetscInt b = 0; b < n_bf; ++b) {
                for (PetscInt c = 0; c < n_cf; ++c) {
                    const PetscInt cidx = b * n_cf + c;

                    for (PetscInt d = 0; d < cdim * cdim; ++d)
                        u_x[h_offset + (f_offset + c) * cdim * cdim + d] +=
                            Hq[cidx * cdim * cdim + d] * coefficients[d_offset + b];
                }
            }
            PETSC_CHECK(
                PetscFEPushforwardHessian(fe, fe_geom, 1, &u_x[h_offset + f_offset * cdim * cdim]));
        }
        PETSC_CHECK(PetscFEPushforward(fe, fe_geom, 1, &u[f_offset]));
        PETSC_CHECK(PetscFEPushforwardGradient(fe, fe_geom, 1, &u_x[f_offset * cdim]));
        if (coefficients_t) {
            for (PetscInt c = 0; c < n_cf; ++c)
                u_t[f_offset + c] = 0.0;
            for (PetscInt b = 0; b < n_bf; ++b) {
                for (PetscInt c = 0; c < n_cf; ++c) {
                    const PetscInt cidx = b * n_cf + c;

                    u_t[f_offset + c] += Bq[cidx] * coefficients_t[d_offset + b];
                }
            }
            PETSC_CHECK(PetscFEPushforward(fe, fe_geom, 1, &u_t[f_offset]));
        }
        f_offset += n_cf;
        d_offset += n_bf;
    }
    return 0;
}

} // namespace godzilla
